/*
 * devices-connectors.hpp
 *
 *  Created on: 08 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_DEVICES_CONNECTORS_HPP_
#define LAZERTAG_DEVICE_INCLUDE_DEVICES_CONNECTORS_HPP_

#include <stdint.h>
#include <string.h>
#include "radio.hpp"

using RXConnectorCallback = void (*)(void* object, unsigned int size, void *data, const uint8_t* sourceAddress);
using TXConnectorDoneCallback = void (*)(void* object, const uint8_t* destinationAddress);

const int MaxMessageSize = 20;
template <int MaxMessageSize>
class AnyConnectorBase
{
public:
    AnyConnectorBase() :
		m_incomingCallback(nullptr),
		m_incomingCallbackObject(nullptr),
		m_inDataSize(0),
		m_outDataSize(0),
		m_inputReady(false)
	{}

    virtual ~AnyConnectorBase() {}

    void setIncomingCallback(RXConnectorCallback callback, void* object) {
        m_incomingCallback = callback;
        m_incomingCallbackObject = object;
    }

    virtual void interrogate() {
    	if (m_inputReady && m_incomingCallback)
    		m_incomingCallback(m_incomingCallbackObject, m_inDataSize, m_bufferIn, nullptr);
    	m_inputReady = false;
    }

    virtual void sendData(const uint8_t* data, unsigned int size) {
    	packMessage(data, size);
    	physicalSend();
    }

protected:
    virtual void physicalSend() = 0;
    //virtual void physicalReceive() = 0;

    virtual void packMessage(const uint8_t* data, unsigned int size) {
		// Putting data
		memcpy(m_bufferOut, data, size*sizeof(uint8_t));
		m_outDataSize = size;
	}

    RXConnectorCallback m_incomingCallback;
    void *m_incomingCallbackObject;

    uint8_t m_bufferIn[MaxMessageSize];
    // Data prepared to be physically sent
    uint8_t m_bufferOut[MaxMessageSize];
    unsigned int m_inDataSize;
    unsigned int m_outDataSize;


    bool m_inputReady;
};

template<int AddressLen, int MaxMessageSize>
class AnyAddressableConnector : public AnyConnectorBase<MaxMessageSize>
{
public:
	using AnyConnectorBaseInstance = AnyConnectorBase<MaxMessageSize>;

    AnyAddressableConnector() {}
    virtual ~AnyAddressableConnector() {}

    inline void setAddress(const uint8_t* address) {
        memcpy(m_address, address, AddressLen*sizeof(uint8_t));
    }

    inline const uint8_t* getAddress() { return m_address; }

    inline void setDestinationAddress(const uint8_t* address) {
        memcpy(m_destinationAddress, address, AddressLen*sizeof(const uint8_t));
    }

    inline const uint8_t* getDestinationAddress() { return m_destinationAddress; }

    virtual void interrogate() {
		if (AnyConnectorBaseInstance::m_inputReady && AnyConnectorBaseInstance::m_incomingCallback)
			AnyConnectorBaseInstance::m_incomingCallback(
					AnyConnectorBaseInstance::m_incomingCallbackObject,
					AnyConnectorBaseInstance::m_inDataSize,
					AnyConnectorBaseInstance::m_bufferIn,
					m_incomingAddress);
		AnyConnectorBaseInstance::m_inputReady = false;
	}

protected:
    virtual void packMessage(const uint8_t* data, unsigned int size)
    {

    	// Putting destination address first
    	memcpy(AnyConnectorBaseInstance::m_bufferOut+AddressLen, m_destinationAddress, AddressLen*sizeof(uint8_t));
    	// Putting self address
    	memcpy(AnyConnectorBaseInstance::m_bufferOut, m_address, AddressLen*sizeof(uint8_t));
    	// Putting data
    	memcpy(AnyConnectorBaseInstance::m_bufferOut+2*AddressLen, data, size*sizeof(uint8_t));
    	AnyConnectorBaseInstance::m_outDataSize = 2*AddressLen + size;
    }

    /**
     * @brief Parse incoming message: get target address and source address
     * @param data Incoming raw buffer
     * @param size Incoming raw buffer size
     */
    virtual void unpackMassage(const uint8_t* data, unsigned int size)
    {
    	for (unsigned int i=0; i<AddressLen; i++)
    		if (data[i] != m_address[i])
    			return;
    	// Copying incoming address
    	memcpy(m_incomingAddress, data+AddressLen, AddressLen*sizeof(uint8_t));
    	// Copying data
    	memcpy(AnyConnectorBaseInstance::m_bufferIn, data+2*AddressLen, size*sizeof(uint8_t));
    	AnyConnectorBaseInstance::m_inputReady = true;
    }

private:
    uint8_t m_address[AddressLen];
    uint8_t m_destinationAddress[AddressLen];
    uint8_t m_incomingAddress[AddressLen];
};
/*
template<int AddressLen, int MaxMessageSize>
class NRF24L01Connector : public AnyAddressableConnector<AddressLen, MaxMessageSize>
{
public:
	using AnyAddressableConnectorInstance = AnyAddressableConnector<AddressLen, MaxMessageSize>;
	NRF24L01Connector(NRF24L01Manager& nrf24l01) :
		m_manager(nrf24l01)
	{

	}

private:
	void physicalSend()
	{
	}
	NRF24L01Manager &m_manager;
};
*/
/*
 * void shot(uint8_t damage);
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
 *
 */

enum RifleCommands
{
	// Weapon configuration commands
	RC_CONFIG_RESET_TO_DEFAULT = 0,   // Reset configuration to default (from sd-card or hardcoded)
	RC_CONFIG_SAVE,           // Save current configuration to memory
	RC_CONFIG_SET_DAMAGE,     // Set one shot damage
	RC_CONFIG_SET_TEAM,       // Set player's team
	RC_CONFIG_SET_FIRE_MODE,  // Automaic, semi-automatic, manual reload
	RC_CONFIG_SET_FIRE_RATE,  // Set shots per second
	RC_CONFIG_SET_MAGAZINE_SIZE,         // Bullets per magazine
	RC_CONFIG_SET_MAGAZINES_START_COUNT, // Count of magazines on start

	// Some space reserved

	// On-game commands
	RC_ENABLE = 100,    // Do not change anything, only enable
	RC_DISABLE,         // Disable when dying
	RC_ADD_MAGAZINES,   // Add fixed amount of magazines
	RC_ADD_BULLETS,     // Add fixed amount of bullets
	RC_EMPTY_CURRENT_MAGAZINE, // Emoty current magazine so player need to reload rifle
    RC_JAM,             // Jam gun (what you need to fix it???)
	// Some space reserved

	// Technical commands
	RC_SET_LED_POWER = 150, // Set power level for led

	// Some space reserved

};

class IRifleToMainSensorConnector
{
public:

};

/*
class IRifleToMainSensorConnector : public IAnyConnector
{
public:

};

class IMainSensorToRifleConnector : public IAnyConnector
{
public:

};

class IAdditionalSensorToMainSensorConnector : public IAnyConnector
{
public:

};
*/




#endif /* LAZERTAG_DEVICE_INCLUDE_DEVICES_CONNECTORS_HPP_ */
