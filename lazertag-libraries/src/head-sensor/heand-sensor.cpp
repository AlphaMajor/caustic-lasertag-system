/*
 * head-sensor.cpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#include <head-sensor/parameters.hpp>
#include "head-sensor/head-sensor.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "dev/console.hpp"
#include "core/scheduler.hpp"
#include "core/logging.hpp"
#include "hal/ext-interrupts.hpp"
#include "dev/sdcard-fs.hpp"

#include <stdio.h>

HeadSensor::HeadSensor()
{
	//m_weapons.insert({1,1,1});
}

void HeadSensor::configure()
{
	ScopedTag tag("head-init");
	IExternalInterruptManager *exti = EXTIS->getEXTI(0);
	exti->init(0);
	exti->turnOn();

	m_mainSensor.setShortMessageCallback(std::bind(&HeadSensor::shotCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	m_mainSensor.init(exti);
	//m_mainSensor.enableDebug(true);

	info << "Mounting sd-card\n";
	if (!SDCardFS::instance().init())
		error << "Error during mounting sd-card!\n";

	RCSPModem::instance().init();

	info << "Creating console commands for head sensor\n";
	Console::instance().registerCommand(
		"die",
		"kill player",
		std::bind(&HeadSensor::testDie, this, std::placeholders::_1)
	);

	info << "Parsing config file\n";
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values\n";
		playerConfig.setDefault();
	}

	info << "Restoring state\n";
	StateSaver::instance().setFilename("state-save");
	// State restoring is always after config reading, so not stored data will be default
	if (StateSaver::instance().tryRestore())
	{
		info << "  restored\n";
	} else {
		error << "  restoring failed\n";
		// setting player state to default
		playerState.reset();
	}

	info << "Initializing visual effects\n";
	m_leds.init(IOPins->getIOPin(1, 0), IOPins->getIOPin(0, 7), IOPins->getIOPin(0, 6));
	m_leds.setColor(getTeamColor());
	m_leds.blink(blinkPatterns.init);


	info << "Other initialization\n";
	RCSPModem::instance().registerBroadcast(broadcast.any);
	RCSPModem::instance().registerBroadcast(broadcast.headSensors);

	StateSaver::instance().runSaver(10000000);

	info << "Head sensor ready to use\n";
}

void HeadSensor::resetToDefaults()
{
	ScopedTag tag("reset");
	info << "Resetting configuration to default\n";
	m_leds.blink(blinkPatterns.anyCommand);
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values\ns";
		playerConfig.setDefault();
	}
	playerState.reset();
	StateSaver::instance().saveState();
}


void HeadSensor::shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	ScopedTag tag("shot-cb");
	info << "** Shot - team: " << teamId << ", player: " << playerId << ", damage: " << damage << "\n";
	if (playerState.isAlive()) {

		if (playerId == playerConfig.plyerMT2Id)
		{
			damage *= playerConfig.selfShotCoeff;
		}
		else if (teamId == playerConfig.teamId)
		{
			damage *= playerConfig.frendlyFireCoeff;
		}

		playerState.damage(damage);
		info << "health: " <<  playerState.healthCurrent << " armor: " << playerState.armorCurrent << "\n";

		// If it was last shoot
		if (!playerState.isAlive())
		{
			dieWeapons();
			StateSaver::instance().saveState();
		} else {
			for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
			{
				RCSPStream stream;
				stream.addValue(ConfigCodes::HeadSensor::State::healthCurrent);
				stream.addValue(ConfigCodes::HeadSensor::State::armorCurrent);
				stream.send(*it, true, nullptr);
			}
		}
	}
	if (!playerState.isAlive()) {
		info << "Player died\n";
		m_leds.blink(blinkPatterns.death);
		// Notifying weapons - do I need it here?
		turnOffWeapons();
	} else {
		m_leds.blink(blinkPatterns.wound);
	}
}

void HeadSensor::playerRespawn()
{
	ScopedTag tag("respawn");
	playerState.respawn();
	m_leds.blink(blinkPatterns.respawn);
	respawnWeapons();
	info << "Player spawned\n";
/*
	std::function<void(void)> respawnFunction = [this] {
		playerState.respawn();
		respawnWeapons();
		printf("Player spawned\n");
	};
	Scheduler::instance().addTask(respawnFunction, true, 0, 0, systemClock->getTime() + playerConfig.postRespawnDelay);
*/
}

void HeadSensor::playerReset()
{
	ScopedTag tag("reset");
	playerState.reset();
	respawnWeapons();
	info << "Player reseted\n";
}

void HeadSensor::playerKill()
{
	ScopedTag tag("kill");
	if (!playerState.isAlive())
		return;
	playerState.kill();
	shotCallback(0, 0, 0);
	dieWeapons();
	info << "Player killed\n";
}

void HeadSensor::dieWeapons()
{
	ScopedTag tag("die-weapons");
	/// Notifying weapons
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Sending kill signal to weapon...\n";
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleDie, false);
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleDie);
	}
}

void HeadSensor::respawnWeapons()
{
	/// Notifying weapons
	ScopedTag tag("reset-weapons");
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Resetting weapon...\n";
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleRespawn);
	}
}

void HeadSensor::turnOffWeapons()
{
	ScopedTag tag("turn-off");
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Turning off weapon...";
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
}

void HeadSensor::registerWeapon(DeviceAddress weaponAddress)
{
	ScopedTag tag("register-weapon");
	info << "Registering weapon\n";
	weaponAddress.print();
	auto it = playerState.weaponsList.weapons.find(weaponAddress);
	if (it == playerState.weaponsList.weapons.end())
	{
		playerState.weaponsList.weapons.insert(weaponAddress);
	}

	RCSPStream stream;
	stream.addValue(ConfigCodes::HeadSensor::Configuration::plyerMT2Id);
	stream.addValue(ConfigCodes::HeadSensor::Configuration::teamId);

	if (playerState.isAlive())
	{
		stream.addCall(ConfigCodes::Rifle::Functions::rifleTurnOn);
	} else {
		stream.addCall(ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
	stream.send(weaponAddress, true);
}

void HeadSensor::setTeam(uint8_t teamId)
{
	ScopedTag tag("set-team");
	info << "Setting team id\n";
	playerConfig.teamId = teamId;
	m_leds.setColor(getTeamColor());
	m_leds.blink(blinkPatterns.anyCommand);
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Changing weapon team id to" << teamId << "\n";
		RCSPStream::remotePullValue(*it, ConfigCodes::HeadSensor::Configuration::teamId);
	}
}

void HeadSensor::addMaxHealth(int16_t delta)
{
	ScopedTag tag("set-team");
	info << "Adding health: " << delta << "\n";
	if (delta < 0 && playerConfig.healthMax < -delta)
	{
		debug << "Max health is " << playerConfig.healthMax << ", so can not add " << delta << "\n";
		return;
	}
	playerConfig.healthMax += delta;
	if (delta < 0 && playerConfig.healthStart < -delta)
	{
		debug << "Start health is " << playerConfig.healthStart << ", so can not add " << delta << "\n";
		return;
	}
	playerConfig.healthStart += delta;
	m_leds.blink(blinkPatterns.anyCommand);
}

uint8_t HeadSensor::getTeamColor()
{
	switch (playerConfig.teamId)
	{
	case 0: return RGBLeds::red;
	case 1: return RGBLeds::blue;
	case 2: return RGBLeds::red | RGBLeds::green;
	case 3: return RGBLeds::green;
	default: return RGBLeds::red | RGBLeds::green | RGBLeds::blue;
	}
}


/////////////////////
// Test functions
void HeadSensor::testDie(const char*)
{
	playerState.healthCurrent = 0;
	shotCallback(0, 0, 0);
}
