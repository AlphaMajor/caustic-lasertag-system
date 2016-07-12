/*
 * bluetooth-bridge.cpp
 *
 *  Created on: 7 сент. 2015 г.
 *      Author: alexey
 */

#include "bluetooth-bridge/bluetooth-protocol.hpp"
#include "core/device-initializer.hpp"
#include "device/device.hpp"
#include "hal/uart.hpp"
#include "core/os-wrappers.hpp"

struct AnyBuffer
{
	AnyBuffer(uint16_t _size, const void *_data = nullptr);
	uint8_t* data;
	const uint16_t size;
};

class BluetoothBridgePackageTimings
{
public:
	PackageTimings broadcast{false, 1000000, 100000, 100000};
};

extern BluetoothBridgePackageTimings bluetoothBridgePackageTimings;

class BluetoothBridge : public IAnyDevice
{
public:
	void init(const Pinout& pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);

	DeviceConfiguration deviceConfig;

private:
	constexpr static uint16_t bluetoothIncommingBufferSize = 200;
	void receiveNetworkPackage(const DeviceAddress sender, uint8_t* payload, uint16_t payloadLength);
	void receiveBluetoothOneByteISR(uint8_t byte);
	void receiveBluetoothPackageISR(uint8_t* buffer, uint16_t size);

	void sendBluetoothMessage(AnyBuffer* buffer);
	void sendNetworkPackage(AnyBuffer* buffer);

	Bluetooth::MessageCreator m_bluetoothMsgCreator;

	IUARTManager* m_bluetoothPort;

	/// @todo Queues should be as large as possible, so need to increase its size
	Worker m_workerToBluetooth{30};
	Worker m_workerToNetwork{30};
	Bluetooth::MessageReceiver m_receiver;
};