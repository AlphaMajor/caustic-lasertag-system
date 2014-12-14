/*
 * miles-tag-2-base.hpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_HAL_MILES_TAG_2_BASE_HPP_
#define LAZERTAG_DEVICE_INCLUDE_HAL_MILES_TAG_2_BASE_HPP_

#include "miles-tag-2.hpp"

#define MILESTAG2_MAX_MESSAGE_LENGTH    40

class MilesTag2TransmitterBase : public IMilesTag2Transmitter
{
public:
    MilesTag2TransmitterBase();
    virtual ~MilesTag2TransmitterBase() {}

    void setPlayerId(uint8_t playerId);
    void setTeamId(uint8_t teamId);

    // Standard commands
    void shot(uint8_t damage);
    void addHealth(uint8_t value);
    void addRounds(uint8_t value);
    void adminKill();
    void pauseOrUnpause();
    void startGame();
    void restoreDefaults();
    void respawn();
    void newGameImmediate();
    void fullAmmo();
    void endGame();
    void resetClock();
    void initializePlayer();
    void explodePlayer();
    void newGameReady();
    void fullHealth();
    void fullArmor();
    void clearScores();
    void testSensors();
    void stunPlayer();
    void disarmPlayer();

    void ammoPickup(uint8_t ammoBoxId);     // 0x00-0x0F
    void healthPickup(uint8_t healthBoxId); // 0x00-0x0F
    void flagPickup(uint8_t flagBoxId);     // 0x00-0x0F

protected:
    bool nextBit();
    void cursorToStart();
    virtual void beginTransmission() = 0;
    uint8_t *m_pCurrentByte;
    uint8_t m_currentBit;
    uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];

    unsigned int m_currentLength;

    unsigned int m_length;

private:
    void sendCommand(uint8_t commandCode);
    uint8_t encodeDamage(uint8_t damage);
    uint8_t m_playerId;
    uint8_t m_teamId;
};

class MilesTag2ReceiverBase : public IMilesTag2Receiver
{
public:
	MilesTag2ReceiverBase();
    virtual ~MilesTag2ReceiverBase() {}
    void setShortMessageCallback(MilesTag2ShotCallback callback, void* object);
    void interrogate();
    virtual void init(unsigned int channel)  = 0;

    void enableDebug(bool debug);

protected:
    uint8_t decodeDamage(uint8_t damage);
	void saveBit(bool value);

	bool isCorrect(unsigned int value, unsigned int min, unsigned int max);
	int getCurrentLength();
	bool getBit(unsigned int n);
	void parseAndCallShot();

	virtual void resetReceiver() = 0;

    MilesTag2ShotCallback m_shotCallback;
	void* m_shotObject;
	unsigned int m_lastTime;
	bool m_dataReady;
	bool m_debug;

	uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];
	uint8_t *m_pCurrentByte;
	uint8_t m_currentBit;
};

#endif /* LAZERTAG_DEVICE_INCLUDE_HAL_MILES_TAG_2_BASE_HPP_ */