/*
 * rifle.cpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#include "rifle/resources.hpp"
#include "rifle/rifle.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "rcsp/broadcast.hpp"
#include "rcsp/RCSP-state-saver.hpp"
#include "core/string-utils.hpp"
#include "core/logging.hpp"
#include "dev/miles-tag-2.hpp"
#include "dev/wav-player.hpp"
#include "dev/io-pins-utils.hpp"
#include "hal/system-clock.hpp"


#include <stdio.h>
#include <string>
#include <utility>

PlayerDisplayableData::PlayerDisplayableData(const DeviceAddress& headSensorAddress) :
	m_headSensorAddress(&headSensorAddress)
{
	healthMax = 0;
	armorMax = 0;

	healthCurrent = 0;
	armorCurrent = 0;

	lifesCountCurrent = 0;
	pointsCount = 0;
	killsCount = 0;
	deathsCount = 0;
}

void PlayerDisplayableData::syncAll()
{
	RCSPMultiStream stream;
	stream.addRequest(ConfigCodes::HeadSensor::Configuration::healthMax);
	stream.addRequest(ConfigCodes::HeadSensor::Configuration::armorMax);
	stream.addRequest(ConfigCodes::HeadSensor::State::healthCurrent);
	stream.addRequest(ConfigCodes::HeadSensor::State::armorCurrent);
	stream.addRequest(ConfigCodes::HeadSensor::State::lifesCountCurrent);
	stream.addRequest(ConfigCodes::HeadSensor::State::pointsCount);
	stream.addRequest(ConfigCodes::HeadSensor::State::killsCount);
	stream.addRequest(ConfigCodes::HeadSensor::State::deathsCount);

	stream.send(*m_headSensorAddress, false);
}

void PlayerDisplayableData::print()
{
	printf("\nCurrent player's state:\n");
	constexpr uint8_t barLength = 10;
	if (healthMax != 0)
	{
		printf("Health:  ");
		printBar(barLength, barLength * healthCurrent / healthMax);
		printf(" %u/%u\n", healthCurrent, healthMax);
	}
	if (armorMax != 0)
	{
		printf("Armor:   ");
		printBar(barLength, barLength * armorCurrent / armorMax);
		printf(" %u/%u\n", armorCurrent, armorMax);
	}
	printf("Lifes:  %u\n", lifesCountCurrent);
	printf("Points: %u\n", pointsCount);
	printf("Kills:  %u\n", killsCount);
	printf("Deaths: %u\n", deathsCount);
}

RifleConfiguration::RifleConfiguration()
{
	setDefault();
}

bool RifleConfiguration::isAutoReloading()
{
	return (!reloadIsMagazineSmart
			&& !reloadNeedMagDisconnect
			&& !reloadNeedMagChange
			&& !reloadNeedBolt);
}

bool RifleConfiguration::isReloadingByDistortingTheBolt()
{
	return (!reloadNeedMagDisconnect
			&& !reloadNeedMagChange
			&& reloadNeedBolt);
}

void RifleConfiguration::setDefault()
{
	slot = 1;
	weightInSlot = 0;

	damageMin = 25;
	damageMax = 25;
	firePeriod = 100000;
	shotDelay = 0;

	jamProb = 0;
	criticalProb = 0;
	criticalCoeff = 1;

	semiAutomaticAllowed = true;
	automaticAllowed = true;

	reloadIsMagazineSmart = false;
	reloadNeedMagDisconnect = false;
	reloadNeedMagChange = false;
	reloadNeedBolt = false;
	reloadPlaySound = true;

	magazinesCountMax = 10;
	magazinesCountStart = magazinesCountMax;

	bulletsInMagazineMax = 30;
	bulletsInMagazineStart = bulletsInMagazineMax;
	reloadingTime = 3000000;

	heatPerShot = 0;
	heatLossPerSec = 0;

	fireFlashPeriod = 100000;
	fireVibroPeriod = 100000;
}

RifleState::RifleState(RifleConfiguration* config) :
	m_config(config)
{
	if (config)
		reset();
}

void RifleState::reset()
{
	bulletsInMagazineCurrent = m_config->bulletsInMagazineStart;
	magazinesCountCurrent = m_config->magazinesCountStart;
	heatnessCurrent = 0;
	lastReloadTime = 0;
}

Rifle::Rifle()
{
	m_tasksPool.setStackSize(256);
}

void Rifle::setDafaultPinout(Pinout& pinout)
{
	pinout.set(PinoutTexts::trigger, 0, 0);
	pinout.set(PinoutTexts::reload, 1, 2);
	pinout.set(PinoutTexts::automatic, 2, 7);
	pinout.set(PinoutTexts::semiAutomatic, 1, 9);
	pinout.set(PinoutTexts::mag1Sensor, 0, 11);
	pinout.set(PinoutTexts::mag2Sensor, 0, 12);
	pinout.set(PinoutTexts::flash, 1, 11);
	pinout.set(PinoutTexts::vibro, 0, 1);
}

bool Rifle::checkPinout(const Pinout& pinout)
{
	bool result = true;
	if (!pinout[PinoutTexts::trigger].exists())
	{
		error << "Trigger pin is not set";
		result = false;
	}

	if (!pinout[PinoutTexts::reload].exists())
	{
		error << "Reload pin is not set";
		result = false;
	}


	if (!pinout[PinoutTexts::automatic].exists())
	{
		error << "Aitomatic switch pin is not set";
		result = false;
	}


	if (!pinout[PinoutTexts::semiAutomatic].exists())
	{
		error << "Semi-automatic switch pin is not set";
		result = false;
	}

	return result;
}


void Rifle::init(const Pinout& pinout)
{

	ScopedTag tag("rifle-configure");

	info << "Wav player initialization";
	WavPlayer::instance().init();

	info << "Loading default config";
	RCSPAggregator::instance().readIni("config.ini");

	info << "Restoring state";
	StateSaver::instance().setFilename("state-save");
	/// @todo Chack that rife is turned on/off correctly anway
	if (StateSaver::instance().tryRestore())
	{
		info  << "  restored";
	} else {
		warning << "  restoring failed";
		rifleReset();
	}
/*
	m_updateDisplayTask.setStackSize(256);
	m_updateDisplayTask.setTask(std::bind(&PlayerDisplayableData::print, &playerDisplayable));
	m_updateDisplayTask.run(1000000, 3000000, 3000000);*/

	//Scheduler::instance().addTask(std::bind(&Rifle::updatePlayerState, this), false, 3000000, 0, 1000000);

	// Line for DEBUG purpose only
	//rifleTurnOn();

	info << "Configuring buttons";
	info << "Current pinout:";
	pinout.printPinout();
	m_fireButton = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::trigger].port,
			pinout[PinoutTexts::trigger].pin
	);
	m_fireButton->useEXTI(true);
	m_fireButton->setAutoRepeat(config.automaticAllowed);
	m_fireButton->setRepeatPeriod(config.firePeriod);
	m_fireButton->setCallback(std::bind(&Rifle::makeShot, this, std::placeholders::_1));
	m_fireButton->turnOn();

	m_buttonsInterrogator.registerObject(m_fireButton);

	m_reloadButton = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::reload].port,
			pinout[PinoutTexts::reload].pin
	);
	m_reloadButton->setAutoRepeat(false);
	m_reloadButton->setRepeatPeriod(2*config.firePeriod);
	m_reloadButton->setCallback(std::bind(&Rifle::distortBolt, this, std::placeholders::_1));
	m_reloadButton->turnOn();

	m_buttonsInterrogator.registerObject(m_reloadButton);

	m_automaticFireSwitch = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::automatic].port,
			pinout[PinoutTexts::automatic].pin
	);
	m_automaticFireSwitch->turnOff();

	m_semiAutomaticFireSwitch = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::semiAutomatic].port,
			pinout[PinoutTexts::semiAutomatic].pin
	);
	m_semiAutomaticFireSwitch->turnOff();

	if (pinout[PinoutTexts::flash].exists())
	{
		m_fireFlash = IOPins->getIOPin(pinout[PinoutTexts::flash].port, pinout[PinoutTexts::flash].pin);
		m_fireFlash->switchToOutput();
		m_fireFlash->reset();
	}
	if (pinout[PinoutTexts::vibro].exists())
	{
		m_vibroEngine = IOPins->getIOPin(pinout[PinoutTexts::vibro].port, pinout[PinoutTexts::vibro].pin);
		m_vibroEngine->switchToOutput();
		m_vibroEngine->reset();
	}

	if (!config.isAutoReloading() && !config.isReloadingByDistortingTheBolt())
	{
		m_magazine1Sensor = ButtonsPool::instance().getButtonManager(
				pinout[PinoutTexts::mag1Sensor].port,
				pinout[PinoutTexts::mag1Sensor].pin
		);
		m_magazine1Sensor->setAutoRepeat(false);
		m_magazine1Sensor->setRepeatPeriod(config.firePeriod);
		m_magazine1Sensor->setCallback(std::bind(&Rifle::magazineSensor, this, true, 1, std::placeholders::_1));
		m_magazine1Sensor->setDepressCallback(std::bind(&Rifle::magazineSensor, this, false, 1, true));
		m_magazine1Sensor->turnOn();
		m_buttonsInterrogator.registerObject(m_magazine1Sensor);

		m_magazine2Sensor = ButtonsPool::instance().getButtonManager(
				pinout[PinoutTexts::mag2Sensor].port,
				pinout[PinoutTexts::mag2Sensor].pin
		);
		m_magazine2Sensor->setAutoRepeat(false);
		m_magazine2Sensor->setRepeatPeriod(config.firePeriod);
		m_magazine2Sensor->setCallback(std::bind(&Rifle::magazineSensor, this, true, 2, std::placeholders::_1));
		m_magazine2Sensor->setDepressCallback(std::bind(&Rifle::magazineSensor, this, false, 2, true));
		m_magazine2Sensor->turnOn();
		m_buttonsInterrogator.registerObject(m_magazine2Sensor);
	}
	detectRifleState();

	m_buttonsInterrogator.setStackSize(256);
	m_buttonsInterrogator.run(10);


	info << "Configuring MT2 transmitter";
	m_mt2Transmitter.init();
	m_mt2Transmitter.setPlayerIdReference(rifleOwner.plyerMT2Id);
	m_mt2Transmitter.setTeamIdReference(rifleOwner.teamId);
	m_mt2Transmitter.setChannel(3);

	info << "RCSP modem initialization";
	RCSPModem::instance().init();

	info << "Looking for sound files...";
	initSounds();

	info << "Other initialization";
	RCSPModem::instance().registerBroadcast(broadcast.any);
	RCSPModem::instance().registerBroadcast(broadcast.rifles);

	m_tasksPool.run();

	rifleTurnOff();
	// Registering at head sensor's weapons list


	registerWeapon();

	StateSaver::instance().runSaver(10000000);

	updatePlayerState();

	info << "Rifle ready to use\n";
}

void Rifle::detectRifleState()
{
	if (config.isAutoReloading() || config.isReloadingByDistortingTheBolt())
	{
		m_state = WeaponState::ready;
	} else {
		m_currentMagazineNumber = getCurrentMagazineNumber();
		info << "Magazine inserted: " << m_currentMagazineNumber;
		if (m_currentMagazineNumber != 0)
		{
			m_state = WeaponState::ready;
		} else {
			m_state = WeaponState::magazineRemoved;
			m_fireButton->setAutoRepeat(false);
		}
	}
}

void Rifle::loadConfig()
{
	IniParcer *parcer = new IniParcer;
	parcer->setCallback([](const char* key, const char* val) { printf("k = %s, v = %s\n", key, val); });
	Result res = parcer->parseFile("default-config.ini");
	if (!res.isSuccess)
		printf("Error: %s\n", res.errorText);
	delete parcer;
}


void Rifle::initSounds()
{
	m_shootingSound.readVariants("sound/shoot-", ".wav");
	m_reloadingSound.readVariants("sound/reload-", ".wav");
	m_noAmmoSound.readVariants("sound/no-ammo-", ".wav");
	m_noMagazines.readVariants("sound/no-magazines-", ".wav");
	m_respawnSound.readVariants("sound/respawn-", ".wav", 1);
	m_dieSound.readVariants("sound/die-", ".wav", 1);
	m_enemyDamaged.readVariants("sound/enemy-injured-", ".wav", 1);
	m_enemyKilled.readVariants("sound/enemy-killed-", ".wav", 1);
	m_friendDamaged.readVariants("sound/friend-injured-", ".wav", 1);
}

void Rifle::makeShot(bool isFirst)
{
	ScopedTag tag("shot");
	switch(m_state)
	{
	case WeaponState::magazineReturned:
	case WeaponState::ready:
		if (isSafeSwitchSelected())
			break;

		if (!isFirst && !config.automaticAllowed)
			break;

		if (state.bulletsInMagazineCurrent != 0)
		{
			state.bulletsInMagazineCurrent--;
			info << "<----<< Sending bullet with damage " << config.damageMin;
			m_mt2Transmitter.shot(config.damageMin);
			if (m_fireFlash)
			{
				m_fireFlash->set();
				delayedSwitchPin(m_tasksPool, m_fireFlash, false, config.fireFlashPeriod);
			}
			if (m_vibroEngine)
			{
				m_vibroEngine->set();
				delayedSwitchPin(m_tasksPool, m_vibroEngine, false, config.fireVibroPeriod);
			}
			m_shootingSound.play();
			if (state.bulletsInMagazineCurrent == 0)
			{
				m_state = WeaponState::magazineEmpty;
				m_fireButton->setAutoRepeat(false);
				if (config.isAutoReloading())
					reloadAndPlay();
				break;
			}
		} else {
			m_noAmmoSound.play();
			info << "Magazine is empty\n";
			break;
		}

		if (m_semiAutomaticFireSwitch->state() && config.semiAutomaticAllowed)
			m_fireButton->setAutoRepeat(false);

		if (m_automaticFireSwitch->state() && config.automaticAllowed)
			m_fireButton->setAutoRepeat(true);
		break;

	case WeaponState::magazineRemoved:
	case WeaponState::otherMagazineInserted:
	case WeaponState::magazineEmpty:
		m_noAmmoSound.play();
		info << "Magazine is empty\n";
		break;

	case WeaponState::reloading:
	default:
		break;
	}
	trace << "State: " << m_state;
}

void Rifle::distortBolt(bool)
{
	ScopedTag tag("reload");
	switch(m_state)
	{
	case WeaponState::magazineEmpty:
		if (config.isReloadingByDistortingTheBolt())
		{
			reloadAndPlay();
		}
		break;
	case WeaponState::magazineReturned:
		if (!config.reloadNeedMagChange)
		{
			reloadAndPlay();
		}
		break;
	case WeaponState::otherMagazineInserted:
		reloadAndPlay();
		break;
	case WeaponState::ready:
		if (config.isReloadingByDistortingTheBolt())
		{
			reloadAndPlay();
		} else {
			/// @todo Add here removing of one shell
		}
		break;
	case WeaponState::reloading:
	default:
		break;

	}
	trace << "State: " << m_state;
}

void Rifle::magazineSensor(bool isConnected, uint8_t sensorNumber, bool isFirst)
{
	info << "Sensor cb for  " << sensorNumber;
	if (!isConnected)
	{
		m_state = WeaponState::magazineRemoved;
		m_fireButton->setAutoRepeat(false);
	} else {
		switch(m_state)
		{

		case WeaponState::magazineRemoved:
			if (config.reloadNeedMagChange)
			{
				if (sensorNumber != m_currentMagazineNumber) // even if m_currentMagazineNumber == 0
				{
					// So NEW magazine was inserted
					m_state = WeaponState::otherMagazineInserted;
					if (!config.reloadNeedBolt)
					{
						m_currentMagazineNumber = sensorNumber;
						reloadAndPlay();
					}
				} else {
					// Old magazine was inserted
					m_state = WeaponState::magazineReturned;
					m_fireButton->setAutoRepeat(true);
				}
			} else {
				m_state = WeaponState::otherMagazineInserted;
				if (!config.reloadNeedBolt)
				{
					m_currentMagazineNumber = sensorNumber;
					reloadAndPlay();
				}
			}
			break;
		case WeaponState::magazineReturned:
		case WeaponState::magazineEmpty:
		case WeaponState::otherMagazineInserted:
		case WeaponState::ready:
		case WeaponState::reloading:
			warning << "Double magazine inserting...\n";
			break;
		default:
			break;
		}
	}
	info << "State: " << m_state;
}

uint8_t Rifle::getCurrentMagazineNumber()
{
	if (m_magazine1Sensor->state())
		return 1;
	else if (m_magazine2Sensor->state())
		return 2;
	else
		return 0;
}

void Rifle::reloadAndPlay()
{
	if (state.magazinesCountCurrent == 0)
	{
		info << "No more magazines!\n";
		/// @todo Add sound "no magazines"
		return;
	}
	m_state = WeaponState::reloading;
	m_tasksPool.addOnce(
		[this]() {
			m_state = WeaponState::ready;
			m_fireButton->setAutoRepeat(true);
			state.magazinesCountCurrent--;
			state.bulletsInMagazineCurrent = config.bulletsInMagazineMax;
		},
		config.reloadingTime
	);
	info << "reloading\n";
	if (config.reloadPlaySound)
		m_reloadingSound.play();
}

bool Rifle::isSafeSwitchSelected()
{
	return !(m_automaticFireSwitch->state() || m_semiAutomaticFireSwitch->state());
}

/*
bool Rifle::isReloading()
{
	return (systemClock->getTime() - state.lastReloadTime < config.reloadingTime);
}*/

void Rifle::updatePlayerState()
{
	playerDisplayable.syncAll();
	/*
	if (isEnabled && playerDisplayable.healthCurrent == 0)
		rifleTurnOff();

	if (!isEnabled && playerDisplayable.healthCurrent != 0)
		rifleTurnOn();
*/
	//playerDisplayable.print();
}

void Rifle::rifleTurnOff()
{
	ScopedTag tag("rifle-turn-off");
	info << "Rifle turned OFF\n";
	m_isEnabled = false;
	m_fireButton->turnOff();
	m_reloadButton->turnOff();
}

void Rifle::rifleTurnOn()
{
	ScopedTag tag("rifle-turn-on");
	info << "Rifle turned ON\n";
	m_isEnabled = true;
	m_fireButton->turnOn();
	m_reloadButton->turnOn();
	detectRifleState();
}

void Rifle::rifleReset()
{
	ScopedTag tag("rifle-reset");
	state.reset();
	detectRifleState();
	info << "Rifle resetted\n";
}

void Rifle::rifleRespawn()
{
	ScopedTag tag("rifle-respawn");
	rifleReset();
	rifleTurnOn();
	updatePlayerState();
	m_respawnSound.play();
	info << "Rifle respawned\n";
}

void Rifle::rifleDie()
{
	ScopedTag tag("rifle-die");
	info << "\'Rifle die\' activated\n";
	if (!m_isEnabled)
		return;

	rifleTurnOff();
	m_dieSound.play();
}


void Rifle::playDamagerNotification(uint8_t state)
{
/*
	if (WavPlayer::instance().isPlaying())
	{
		/// @todo Fix it when sound mixing will be done
		//scheduleDamageNotification(state);
		return;
	}*/

	switch (state)
	{
	default:
	case NotificationSoundCase::enemyInjured:
		m_enemyDamaged.play();
		break;
	case NotificationSoundCase::enemyKilled:
		m_enemyKilled.play();
		break;
	case NotificationSoundCase::friendInjured:
		m_friendDamaged.play();
		break;
	}
}

void Rifle::scheduleDamageNotification(uint8_t state)
{
	/*
	Scheduler::instance().addTask(
			std::bind(&Rifle::playDamagerNotification, this, state),
			true,
			0,
			0,
			100000
	);*/
}


void Rifle::riflePlayEnemyDamaged(uint8_t state)
{
	ScopedTag tag("notify-damaged");
	info << "Player damaged with state: " << state;
	playDamagerNotification(state);
}

void Rifle::registerWeapon()
{
	ScopedTag tag("rifle-register");

	if (m_registerWeaponPAckageId != 0)
		return;

	info << "Registering weapon...\n";
	//config.headSensorAddr.print();
	m_registerWeaponPAckageId = RCSPStream::remoteCall(
			config.headSensorAddr,
			ConfigCodes::HeadSensor::Functions::registerWeapon,
			RCSPModem::instance().devAddr,
			true,
			[this](PackageId packageId, bool result) -> void
			{
				m_registerWeaponPAckageId = 0;
			},
			std::forward<PackageTimings>(riflePackageTimings.registration)
	);
}
