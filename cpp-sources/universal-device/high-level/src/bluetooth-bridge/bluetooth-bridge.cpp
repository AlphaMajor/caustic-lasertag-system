/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/


#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "core/logging.hpp"
#include "core/power-monitor.hpp"
#include "core/string-utils.hpp"
#include "dev/nrf24l01.hpp"
#include "network/broadcast.hpp"
#include "network/network-layer.hpp"

#include <string.h>

using namespace Bluetooth;

BluetoothBridgePackageTimings bluetoothBridgePackageTimings;

BluetoothBridge::BluetoothBridge()
{
}

void BluetoothBridge::initAsSecondaryDevice(const Pinout& pinout, bool isSdcardOk)
{
    UNUSED_ARG(pinout);
    debug << "Bluetooth bridge initialization";

    // default address for bluetooth bridge without SD-card
    deviceConfig.devAddr.address[0] = 50;
    deviceConfig.devAddr.address[1] = 50;
    deviceConfig.devAddr.address[2] = 50;

    if (isSdcardOk)
    {
        if (!m_aggregator->readIni("bb-config.ini"))
        {
            error << "Cannot read config file, so setting default values";
            config.setDefault();
        }
    } else {
        warning << "Bluetooth bridge operate without sd-card, it will use default settings";
    }

    initNetworkClient();
    m_networkClient.connectPackageReceiver(this);

    m_workerToNetwork.setStackSize(512);
    m_workerToNetwork.run();

    m_workerToBluetooth.setStackSize(200);
    m_workerToBluetooth.run();

    configureBluetooth();
}

void BluetoothBridge::init(const Pinout& pinout, bool isSdcardOk)
{
    UNUSED_ARG(pinout);

    // Power monitor should be initialized before configuration reading
    PowerMonitor::instance().init();

    initAsSecondaryDevice(pinout, isSdcardOk);
    initNetwork();
    //NetworkLayer::instance().enableRegularNRFReinit();

	m_tasksPool.add(
			[this] { PowerMonitor::instance().interrogate(); },
			100000
	);

	m_tasksPool.setStackSize(256);
	m_tasksPool.run();
}

void BluetoothBridge::setDefaultPinout(Pinout& pinout)
{
    UNUSED_ARG(pinout);
}

bool BluetoothBridge::checkPinout(const Pinout& pinout)
{
    UNUSED_ARG(pinout);
	return true;
}

void BluetoothBridge::receivePackage(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
{
    debug << "Processing incoming network package";
    m_bluetoothMsgCreator.clear();
    m_bluetoothMsgCreator.setSender(std::move(sender));
    m_bluetoothMsgCreator.addData(payloadLength, payload);
    Bluetooth::Message *msg = new Bluetooth::Message();
    *msg = m_bluetoothMsgCreator.msg();
    trace << "Bluetooth message to be sent: ";
    msg->print();
    m_workerToBluetooth.add(
        [this, msg] ()
        {
            sendBluetoothMessage(msg);
            systemClock->wait_us(1000);
            delete msg;
        }
    );
}

void BluetoothBridge::connectClient(INetworkClient* client)
{
    UNUSED_ARG(client);
}


void BluetoothBridge::configureBluetooth()
{
	m_bluetoothPort = UARTs->get(IUARTSPool::UART2);

	// First, configuring HC-05 bluetooth module to have proper name, UART speed and password
	m_configurator.init(IOPins->getIOPin(IIOPin::PORTA, 1), m_bluetoothPort);

	info << "Determining speed of UART for HC-05";
	HC05Configurator::HC05Result result = m_configurator.selectSpeed();
	if (result != HC05Configurator::HC05Result::ok)
	{
		error << "Cannot determine HC-05 speed. Somebody might have configured "
		         "it by external tool. Bluetooth bridge will not operate! Error: " << HC05Configurator::parseResult(result);
	}

	m_configurator.configure();
	m_configurator.leaveAT();

	// Then we can initialize data receiving system and run worker that put data to bluetooth
	m_bluetoothPort->setBlockSize(1);
	m_bluetoothPort->setRXDoneCallback(
		[this](uint8_t* buffer, uint16_t size)
		{
			UNUSED_ARG(size);
			receiveBluetoothOneByteISR(buffer[0]);
		}
	);

	m_bluetoothPort->init(HC05Configurator::uartTargetSpeed);
}

void BluetoothBridge::receiveBluetoothOneByteISR(uint8_t byte)
{
	m_receiver.readByte(byte);
	if (m_receiver.ready())
	{
		//printf("R\n");
		receiveBluetoothPackageISR(&m_receiver.get());
		m_receiver.reset();
	}
}

void BluetoothBridge::receiveBluetoothPackageISR(Bluetooth::Message* msg)
{
	Bluetooth::Message *msgToSend = new Bluetooth::Message();
	*msgToSend = *msg;
	m_workerToNetwork.addFromISR(
		[this, msgToSend] ()
		{
			info << "Incoming bluetooth: ";
			msgToSend->print();
			//printHex(m->data, msgBuffer->size);
			sendNetworkPackage(msgToSend->address, msgToSend->data, msgToSend->payloadLength());
			delete msgToSend;
		}
	);
}

void BluetoothBridge::sendBluetoothMessage(Bluetooth::Message* msg)
{
	debug << "Sending bluetooth message";
	// Transmitting to bluetooth module and waiting while transmit is done
	m_bluetoothPort->transmit(reinterpret_cast<uint8_t*>(msg), msg->length);
	while (m_bluetoothPort->txBusy()) { }
}

void BluetoothBridge::sendNetworkPackage(DeviceAddress addr, uint8_t* payload, uint16_t size)
{
	debug << "Sending package to network";
	debug << "target: " << ADDRESS_TO_STREAM(addr);
	debug << "payload size: " << size;
	// Sending message body as is
	if (Broadcast::isBroadcast(addr))
	{
		// We need special timings for broadcasts
	    m_networkClient.send(
	    		addr,
                payload,
				size,
                true,
                nullptr,
                bluetoothBridgePackageTimings.broadcast
        );
	} else {
	    // Not broadcast packages are with default timings
	    m_networkClient.send(
	    		addr,
				payload,
				size,
	            true
	    );
	}
}
