/*
 * RCSP-stream.cpp
 *
 *  Created on: 11 февр. 2015 г.
 *      Author: alexey
 */

#include "rcsp/RCSP-stream.hpp"
#include "core/string-utils.hpp"
#include "fatfs.h"
#include <stdio.h>

RCSPStream::RCSPStream(uint16_t size) :
	m_size(size)
{
	m_stream = new uint8_t[m_size];
	m_freeSpace = m_size;
	memset(m_stream, 0, size*sizeof(uint8_t));
}

RCSPStream::~RCSPStream()
{
	if (m_stream)
		delete[] m_stream;
}

uint8_t* RCSPStream::getStream() const
{
	return m_stream;
}

uint16_t RCSPStream::getSize() const
{
	return m_size;
}

RCSPAggregator::ResultType RCSPStream::addValue(OperationCode code)
{
	//printf("Adding value, code %u\n", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::instance().serializeObject(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

RCSPAggregator::ResultType RCSPStream::addRequest(OperationCode code)
{
	//printf("Adding value, code %u\n", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::instance().serializeObjectRequest(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

RCSPAggregator::ResultType RCSPStream::addCall(OperationCode code)
{
	//printf("Adding value, code %u\n", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::instance().serializeCallRequest(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

RCSPAggregator::ResultType RCSPStream::serializeAnything(OperationCode code, SerializationFunc serializer)
{
	uint8_t *pos = m_stream + m_cursor;
	uint16_t addedSize = 0;
	RCSPAggregator::ResultType result = serializer(pos, code, addedSize);
	if (result.isSuccess)
	{
		m_cursor += addedSize;
	} else {
		//printf("Cannot add %u: %s\n", code, result.errorText);
	}
	return result;
}

PackageId RCSPStream::send(
	DeviceAddress target,
	bool waitForAck,
	PackageSendingDoneCallback doneCallback,
	PackageTimings&& timings
)
{
	return NetworkLayer::instance().send(
		target,
		m_stream,
		m_size,
		waitForAck,
		doneCallback,
		timings
	);
}

void RCSPStream::dispatch()
{
	if (!empty())
	{
		RCSPAggregator::instance().dispatchStream(m_stream, m_cursor);
	}
}


bool RCSPStream::empty()
{
	return (m_cursor == 0);
}

//////////////////////////
// RCSPMultiStream

RCSPMultiStream::RCSPMultiStream()
{
	pushBackStream();
}

void RCSPMultiStream::pushBackStream()
{
	m_streams.push_back(std::shared_ptr<RCSPStream> (new RCSPStream));
}

RCSPAggregator::ResultType RCSPMultiStream::addValue(OperationCode code)
{
	RCSPAggregator::ResultType result = m_streams.back()->addValue(code);
	if (!result.isSuccess && result.details == RCSPAggregator::NOT_ENOUGH_SPACE)
	{
		pushBackStream();
		result = m_streams.back()->addValue(code);
	}
	return result;
}

RCSPAggregator::ResultType RCSPMultiStream::addRequest(OperationCode code)
{
	RCSPAggregator::ResultType result = m_streams.back()->addRequest(code);
	if (!result.isSuccess && result.details == RCSPAggregator::NOT_ENOUGH_SPACE)
	{
		pushBackStream();
		result = m_streams.back()->addRequest(code);
	}
	return result;
}

RCSPAggregator::ResultType RCSPMultiStream::addCall(OperationCode code)
{
	RCSPAggregator::ResultType result = m_streams.back()->addCall(code);
	if (!result.isSuccess && result.details == RCSPAggregator::NOT_ENOUGH_SPACE)
	{
		pushBackStream();
		result = m_streams.back()->addCall(code);
	}
	return result;
}

void RCSPMultiStream::send(
		DeviceAddress target,
		bool waitForAck,
		PackageTimings&& timings
	)
{
	for (auto it=m_streams.begin(); it != m_streams.end(); it++)
	{
		(*it)->send(target, waitForAck, nullptr, std::forward<PackageTimings>(timings));
	}
}

bool RCSPMultiStream::empty()
{
	return m_streams.front()->empty();
}

void RCSPMultiStream::dispatch()
{
	for (auto it=m_streams.begin(); it != m_streams.end(); it++)
	{
		(*it)->dispatch();
	}
}

DetailedResult<FRESULT> RCSPMultiStream::writeToFile(FIL* file)
{
	FRESULT res = FR_OK;
	UINT written = 0;
	for (auto it = m_streams.begin(); it != m_streams.end(); it++)
	{
		res = f_write (file, (*it)->getStream(), (*it)->getSize(), &written);
		if (res != FR_OK)
		{
			return DetailedResult<FRESULT>(res, "Bad file writing result");
		}
		if (written != (*it)->getSize())
		{
			return DetailedResult<FRESULT>(res, "Invalid written bytes count");
		}
	}
}


ReceivePackageCallback RCSPMultiStream::getPackageReceiver()
{
	return [](DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
	{
		RCSPMultiStream answerStream;
		RCSPAggregator::instance().dispatchStream(payload, payloadLength, &answerStream);
		if (!answerStream.empty())
		{
			answerStream.send(sender, true);
		}
	};
}