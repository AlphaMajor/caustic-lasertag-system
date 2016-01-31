/*
 * game-log.cpp
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: alexey
 */

#include "game/game-log.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "rcsp/operation-codes.hpp"
#include "core/logging.hpp"
#include "string.h"

using namespace GameLog;

const char* BaseStatsCounter::filename = "base-stat.bin";

void BaseStatsCounter::checkAndCreate(PlayerMT2Id player)
{
	auto it = m_results.find(player);
	if (it == m_results.end())
	{
		m_results[player] = PvPDamageResults();
		m_results[player].player = player;
		m_iteratorCorrupted = true;
	}
}

void BaseStatsCounter::registerDamage(PlayerMT2Id player, uint16_t damage)
{
	checkAndCreate(player);
	m_results[player].totalDamage += damage;
}

void BaseStatsCounter::registerHit(PlayerMT2Id player)
{
	checkAndCreate(player);
	m_results[player].hitsCount++;
}

void BaseStatsCounter::registerKill(PlayerMT2Id player)
{
	checkAndCreate(player);
	m_results[player].killsCount++;
}

void BaseStatsCounter::clear()
{
	m_results.clear();
	saveState();
}

void BaseStatsCounter::saveState()
{
	FRESULT res = FR_OK;
	info << "<Save stats> Saving stats";
	res = f_open(&m_file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK)
	{
		error << "<Save stats> Cannot open file to write base stats: " << res;
		return;
	}

	for (auto it = m_results.begin(); it != m_results.end(); it++)
	{
		UINT written = 0;
		res = f_write(&m_file, & (it->second), sizeof(PvPDamageResults), &written);
		if (res != FR_OK)
		{
			error << "<Save stats> Cannot write base stats to file: " << res;
			break;
		}
		if (written != sizeof(PvPDamageResults))
		{
			error << "<Save stats> Wrong size written to file: " << written << " instead of " << sizeof(PvPDamageResults);
			break;
		}
	}
	f_close(&m_file);
}

void BaseStatsCounter::restoreFromFile()
{
	FRESULT res = FR_OK;
	res = f_open(&m_file, filename, FA_READ);
	if (res != FR_OK)
	{
		warning << "<Restore stats> Cannot open file to read base stats: " << res;
		return;
	}

	while (!f_eof(&m_file))
	{
		UINT readed = 0;
		PvPDamageResults tmp;
		res = f_read(&m_file, &tmp, sizeof(PvPDamageResults), &readed);
		if (res != FR_OK)
		{
			error << "<Restore stats> Cannot write base stats from file: " << res;
			break;
		}
		if (readed != sizeof(PvPDamageResults))
		{
			error << "<Restore stats> Wrong size readed from file: " << readed << " instead of " << sizeof(PvPDamageResults) << " or 0";
			break;
		}
		m_results[tmp.player] = tmp;
	}

	f_close(&m_file);
}

void BaseStatsCounter::setStatsReceiver(DeviceAddress addr)
{
	m_statsReceiver = addr;
}

void BaseStatsCounter::interrogate()
{
	switch(m_sendingState)
	{
	case S_READY_TO_TRANSMIT:
		sendNextPackage();
		break;
	case S_WAIT_DELAY_AFTER_CHUNK:
		waitDelay();
		break;
	case S_NOTHING:
	default:
		break;
	}
}

void BaseStatsCounter::sendStats()
{
	prepareTransmission();
}


void BaseStatsCounter::sendNextPackage()
{
	ScopedLock<CritialSection> lock(m_iteratorCheck);
		if (m_iteratorCorrupted)
		{
			prepareTransmission();
			return;
		}
		// Iterator is fine here
		if (m_sendingIterator == m_results.end())
		{
			// All data transmitted
			m_sendingState = S_NOTHING;
			return;
		}

		PvPDamageResults target = m_sendingIterator->second;
		m_sendingIterator++;
		// Now we have valid object and we can send it
	lock.unlock();

	m_sendingState = S_WAITING_FOR_TRANSMISSIO_RESULT;
	RCSPStream::remoteCall(
		m_statsReceiver,
		ConfigCodes::Base::Functions::getPvPResults,
		target,
		true,
		[this](PackageId, bool isSuccess) {
			if (isSuccess)
				onTransmissionSucceeded();
			else
				onTransmissionBroken();
		}
	);

}

void BaseStatsCounter::prepareTransmission()
{
	m_sendingIterator = m_results.begin();
	m_iteratorCorrupted = false;
	m_sendingState = S_READY_TO_TRANSMIT;
}

void BaseStatsCounter::waitDelay()
{
	if (systemClock->getTime() - m_waitingBeginned >= delayAfterChunk)
	{
		m_sendingState = S_READY_TO_TRANSMIT;
	}
}

void BaseStatsCounter::onTransmissionBroken()
{
	m_sendingState = S_NOTHING;
}

void BaseStatsCounter::onTransmissionSucceeded()
{
	m_waitingBeginned = systemClock->getTime();
	m_sendingState = S_WAIT_DELAY_AFTER_CHUNK;
}

Event::Event()
{
	memset(eventParameter, 0, eventParameterSize);
}
