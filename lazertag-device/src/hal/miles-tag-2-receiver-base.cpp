/*
 * miles-tag-2-receiver-base.cpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#include "hal/miles-tag-2-base.hpp"
#include "utils.hpp"
#include <stdio.h>

IMilesTag2Receiver* milesTag2Receiver;

/////////////////////
// Receiver
bool MilesTag2ReceiverBase::isCorrect(unsigned int value, unsigned int min, unsigned int max)
{
    return (value > min && value < max);
}


void MilesTag2ReceiverBase::interrogate()
{
    if (getCurrentLength() == 14 && getBit(0) == false) {
        parseAndCallShot();
        resetReceiver();
        // We have shot mesage
    }

    /// @todo determine message type
    /*
    if (!m_dataReady)
        return;

    m_dataReady = false;
    m_shortMessageCallback(m_shortMessageObject, m_data);

    resetReceiver();
    */
}

void MilesTag2ReceiverBase::parseAndCallShot()
{
    unsigned int playerId   = m_data[0] & ~(1 << 7);
    unsigned int teamId     = m_data[1] >> 6;
    unsigned int damageCode = (m_data[1] & 0b00111100) >> 2;
    m_shotCallback(m_shotObject, teamId, playerId, decodeDamage(damageCode));
}

bool MilesTag2ReceiverBase::getBit(unsigned int n)
{
    return m_data[n / 8] & (1 << (7 - n%8));
}

void MilesTag2ReceiverBase::saveBit(bool value)
{
    if (m_pCurrentByte - m_data == MILESTAG2_MAX_MESSAGE_LENGTH)
        return;
    if (value)
        *m_pCurrentByte |= (1 << m_currentBit);
    else
        *m_pCurrentByte &= ~(1 << m_currentBit);

    if (m_currentBit == 0) {
        m_currentBit = 7;
        m_pCurrentByte++;
    } else
        m_currentBit--;
}

int MilesTag2ReceiverBase::getCurrentLength()
{
    return (m_pCurrentByte - m_data)*8 + 7-m_currentBit;
}

uint8_t MilesTag2ReceiverBase::decodeDamage(uint8_t damage)
{
    switch(damage)
    {
    case 0: return 1;
    case 1: return 2;
    case 2: return 4;
    case 3: return 5;
    case 4: return 7;
    case 5: return 10;
    case 6: return 15;
    case 7: return 17;
    case 8: return 20;
    case 9: return 25;
    case 10: return 30;
    case 11: return 35;
    case 12: return 40;
    case 13: return 50;
    case 14: return 75;
    case 15: return 100;
    default: return 0;
    }
}


MilesTag2ReceiverBase::MilesTag2ReceiverBase() :
	m_shotCallback(nullptr),
	m_shotObject(nullptr),
	m_debug(false),
	m_pCurrentByte(m_data),
	m_currentBit(7)
{
}

void MilesTag2ReceiverBase::setShortMessageCallback(MilesTag2ShotCallback callback, void* object)
{
    m_shotCallback = callback;
    m_shotObject = object;
}


void MilesTag2ReceiverBase::enableDebug(bool debug)
{
    m_debug = debug;
}