/*
 * package-sender.cpp
 *
 *  Created on: 28 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/package-sender.hpp"

#include "core/scheduler.hpp"

#include <stdio.h>
#include <string.h>

void PackageSender::init()
{
	printf("Package sender initialization...\n");
	nrf.setTXDoneCallback(std::bind(&PackageSender::TXDoneCallback, this));
	nrf.setDataReceiveCallback(std::bind(&PackageSender::RXCallback, this, std::placeholders::_1, std::placeholders::_2));
	nrf.init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		nullptr,
		SPIs->getSPI(1)
	);
	nrf.printStatus();
	Scheduler::instance().addTask(std::bind(&PackageSender::interrogate, this), false, 10000);
}

uint16_t PackageSender::generatePackageId()
{
	uint16_t id = (systemClock->getTime()) & 0x3FFF;
	if (id == 0)
		id = 1;
	return id;
}


uint16_t PackageSender::send(DeviceAddress target, uint8_t* data, uint16_t size, bool waitForAck, PackageSendingDoneCallback doneCallback)
{
	if (size>Package::payloadLength)
		return 0;
	PackageIdAndTTL idAndTTL;
	idAndTTL.packageId = generatePackageId();

	if (waitForAck)
	{
		Time time = systemClock->getTime();
		m_packages[idAndTTL.packageId] = WaitingPackage();
		m_packages[idAndTTL.packageId].wasCreated = time;
		m_packages[idAndTTL.packageId].nextTransmission = time;
		m_packages[idAndTTL.packageId].callback = doneCallback;
		m_packages[idAndTTL.packageId].package.sender = self;
		m_packages[idAndTTL.packageId].package.target = target;
		m_packages[idAndTTL.packageId].package.idAndTTL = idAndTTL;
		return idAndTTL.packageId;
	} else {
		m_packagesNoAck.push_back(Package());
		m_packagesNoAck.back().sender = self;
		m_packagesNoAck.back().target = target;
		m_packagesNoAck.back().idAndTTL = idAndTTL;
		memcpy(m_packagesNoAck.back().payload, data, size);
		if (size<Package::payloadLength)
			memset(m_packagesNoAck.back().payload+size, 0, size-Package::payloadLength);
		return 0;
	}
}

void PackageSender::TXDoneCallback()
{
	isSendingNow = false;
}

void PackageSender::RXCallback(uint8_t channel, uint8_t* data)
{
	Package received;
	memcpy(&received, data, sizeof(Package));

	// Skipping packages for other devices
	if (received.target != self)
		return;

	// Dispatching if this is acknoledgement
	AckPayload *ackDispatcher = reinterpret_cast<AckPayload *>(received.payload);
	if (ackDispatcher->isAck())
	{
		printf("Ack package received for id=%u\n", ackDispatcher->packageId);
		auto it = m_packages.find(ackDispatcher->packageId);
		PackageSendingDoneCallback callback = it->second.callback;
		if (it != m_packages.end())
			m_packages.erase(it);
		printf("Package removed from queue\n");
		if (callback)
			callback(ackDispatcher->packageId, true);
		return;
	}

	printf("Received package with id=%u\n", received.idAndTTL.packageId);
	// Generating acknledgement

	// Forming payload for ack package
	AckPayload ackPayload;
	ackPayload.packageId = received.idAndTTL.packageId;
	// Forming ack package
	Package ack;
	ack.target = received.sender;
	ack.sender = self;
	ack.idAndTTL.packageId = generatePackageId();
	memcpy(&ack.payload, &ackPayload, sizeof(ackPayload));
	// Adding ack package to list for sending
	m_packagesNoAck.push_back(ack);

	// Putting received package to list
	m_incoming.push_back(received);
}

void PackageSender::interrogate()
{
	if (!isSendingNow) sendNext();
	while (!m_incoming.empty())
	{
		ConfigsAggregator::instance().dispatchStream(m_incoming.front().payload, m_incoming.front().payloadLength);
		m_incoming.pop_front();
	}
}

void PackageSender::sendNext()
{
	// First, sending packages without response
	if (!m_packagesNoAck.empty())
	{
		printf("Sending package without ack needed\n");
		nrf.sendData(Package::packageLength, (uint8_t*) &(m_packagesNoAck.front()));
		m_packagesNoAck.pop_front();
		isSendingNow = true;
		// Tell that now we are sending no-response package
		currentlySendingPackageId = 0;
		return;
	}
	Time time = systemClock->getTime();
	for (auto it=m_packages.begin(); it!=m_packages.end(); it++)
	{
		// If timeout
		if (time - it->second.wasCreated > timeout)
		{
			PackageSendingDoneCallback callback = it->second.callback;
			uint16_t timeoutedPackageId = it->second.package.idAndTTL.packageId;
			m_packages.erase(it);
			if (callback)
				callback(timeoutedPackageId, false);

			/// @todo Improve code somehow and remove this return
			// Return because iterators are bad now
			return;
		}
		// If it is time to (re)send package
		if (it->second.nextTransmission < time)
		{
			printf("Sending package with ack needed\n");
			currentlySendingPackageId = it->first;
			isSendingNow = true;
			it->second.nextTransmission = time+resendTime;
			nrf.sendData(Package::packageLength, (uint8_t*) &(it->second.package));
			return;
		}
	}
}
