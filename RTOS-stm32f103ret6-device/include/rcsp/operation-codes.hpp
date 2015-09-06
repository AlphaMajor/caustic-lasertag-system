/*
 * config-codes.h
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LOGIC_CONFIGS_H_
#define LOGIC_CONFIGS_H_

#include "rcsp/RCSP-aggregator.hpp"

// Includes that contains types which might be used as parameters types
#include "rcsp/RCSP-base-types.hpp"
#include "rifle/rifle-base-types.hpp"
#include "head-sensor/head-sensor-base-types.hpp"
#include "dev/MT2-base-types.hpp"

#include "utils/macro.hpp"

#define PAR_CODE(type, parameterName, value)  constexpr uint16_t parameterName = RCSPAggregator::SetObjectOC(value); \
                                              constexpr const char parameterName##Text[] = STRINGIFICATE(variable); \
                                              using parameterName##Type = type;


#define FUNC_CODE_NP(function, value)  constexpr uint16_t function = RCSPAggregator::SetCallRequestOC(value);

#define FUNC_CODE_1P(function, argType, value)  constexpr uint16_t function = RCSPAggregator::SetCallRequestOC(value); \
                                                using function##Arg1Type = argType;


/*
 *
 * Variables naming guideline:
 *    variableMax
 *    variableMin
 *    variableStart
 *    variableCurrent
 */


using UintParameter = uint16_t;
using IntParameter = int16_t;
using TimeInterval = uint32_t;
using FloatParameter = float;
using PlayerId = uint8_t[3];

namespace ConfigCodes
{
	constexpr uint16_t noOperation = 0;
	constexpr uint16_t acknoledgement = RCSPAggregator::SetCallRequestOC(0xFFFF);
	namespace AnyDevice
	{
		namespace Configuration
		{
			PAR_CODE(DeviceAddress, devAddr,           2000)
		}

		namespace Functions
		{
			FUNC_CODE_NP(resetToDefaults,   2100)
		}
	}
	namespace Rifle
	{
		namespace Configuration
		{
			PAR_CODE(UintParameter, slot,           1)
			PAR_CODE(UintParameter, weightInSlot,   2)
			PAR_CODE(UintParameter, damageMin,      5)
			PAR_CODE(UintParameter, damageMax,      6)
			PAR_CODE(TimeInterval, firePeriod,      7)
			PAR_CODE(TimeInterval, shotDelay,       8)
			PAR_CODE(FloatParameter, jamProb,       9)
			PAR_CODE(FloatParameter, criticalProb,  10)
			PAR_CODE(FloatParameter, criticalCoeff, 11)

			/// Are these bools need to be grouped to one variable with bit fields?
			PAR_CODE(bool, semiAutomaticAllowed, 12) ///< Is no need in bolt disorting between shots
			PAR_CODE(bool, automaticAllowed,     13)

			// Reload cycle control
			PAR_CODE(bool, reloadIsMagazineSmart,   15) ///< Is magazine smart (with MCU)?

			PAR_CODE(bool, reloadNeedMagDisconnect, 16) ///< 1. Should the magazine be disconnected?
			PAR_CODE(bool, reloadNeedMagChange,     17) ///< 2. Should the magazine be changed?
			PAR_CODE(bool, reloadNeedBolt,          18) ///< 3. Should the bolt be distorted?

			PAR_CODE(bool, reloadPlaySound,         20) ///< Should reloading sound be played?

			PAR_CODE(UintParameter, magazinesCountStart,      30)
			PAR_CODE(UintParameter, magazinesCountMax,        31)

			PAR_CODE(UintParameter, bulletsInMagazineStart,   32)
			PAR_CODE(UintParameter, bulletsInMagazineMax,     33)

			PAR_CODE(UintParameter, reloadingTime,            34)

			PAR_CODE(UintParameter, heatPerShot,              40)
			PAR_CODE(UintParameter, heatLossPerSec,           41)

			PAR_CODE(TimeInterval, fireFlashPeriod,           50)
			PAR_CODE(TimeInterval, fireVibroPeriod,           51)

			PAR_CODE(DeviceAddress, headSensorAddr,           80)

		}

		namespace State
		{
			// Rifle state
			PAR_CODE(UintParameter, bulletsInMagazineCurrent, 101)
			PAR_CODE(UintParameter, magazinesCountCurrent,    102)

			PAR_CODE(UintParameter, heatnessCurrent,    110)
		}

		namespace Functions
		{
			FUNC_CODE_NP(rifleTurnOff,       201)
			FUNC_CODE_NP(rifleTurnOn,        202)
			FUNC_CODE_NP(rifleReset,         203)
			FUNC_CODE_NP(rifleRespawn,       204)
			FUNC_CODE_NP(rifleDie,           205)
			FUNC_CODE_NP(rifleWound,         206)
			FUNC_CODE_1P(rifleShock, TimeInterval,        207)

			FUNC_CODE_1P(riflePlayEnemyDamaged, uint8_t,  210)
			FUNC_CODE_NP(headSensorToRifleHeartbeat,      220)
		}
	}

	namespace HeadSensor
	{
		namespace Configuration
		{
			/// Player configuration
			PAR_CODE(UintParameter, healthMax,   1000)
			PAR_CODE(UintParameter, armorMax,    1001)
			PAR_CODE(UintParameter, healthStart, 1003)
			PAR_CODE(UintParameter, armorStart,  1004)

			PAR_CODE(FloatParameter, isHealable, 1010)

			PAR_CODE(UintParameter, lifesCount,  1011)

			/// Effectivity of armor
			PAR_CODE(FloatParameter, armorCoeffStart,  1012)

			/// Simply multiplied by damage to player
			PAR_CODE(FloatParameter, damageCoeffStart, 1013)

			/// Multiplied by player's shutting damage
			PAR_CODE(FloatParameter, shotsCoeffStart,  1014)

			/// Multiplied by friendly fire
			PAR_CODE(FloatParameter, frendlyFireCoeff, 1015)

			PAR_CODE(FloatParameter, selfShotCoeff,    1016)


			PAR_CODE(TimeInterval, preRespawnDelay,    1021)
			PAR_CODE(TimeInterval, postRespawnDelay,   1022)
			PAR_CODE(bool,         autoRespawn,        1023)
			PAR_CODE(TimeInterval, shockDelayImmortal, 1024)
			PAR_CODE(TimeInterval, shockDelayInactive, 1025)


			PAR_CODE(UintParameter, plyerId,        1030)
			PAR_CODE(PlayerMT2Id,   plyerMT2Id,     1031)
			PAR_CODE(TeamMT2Id,     teamId,         1032)

			PAR_CODE(UintParameter, slot1MaxWeight,    1041)
			PAR_CODE(UintParameter, slot2MaxWeight,    1042)
			PAR_CODE(UintParameter, slot3MaxWeight,    1043)
			PAR_CODE(UintParameter, slot4MaxWeight,    1044)
			PAR_CODE(UintParameter, slot5MaxWeight,    1045)


			PAR_CODE(FloatParameter, zone1DamageCoeff,    1500)
			PAR_CODE(FloatParameter, zone2DamageCoeff,    1501)
			PAR_CODE(FloatParameter, zone3DamageCoeff,    1502)
			PAR_CODE(FloatParameter, zone4DamageCoeff,    1503)
			PAR_CODE(FloatParameter, zone5DamageCoeff,    1504)
			PAR_CODE(FloatParameter, zone6DamageCoeff,    1505)
		}

		namespace State
		{
			PAR_CODE(UintParameter, healthCurrent,      1100)
			PAR_CODE(UintParameter, armorCurrent,       1101)
			PAR_CODE(FloatParameter, armorCoeffCurrent,  1102)
			PAR_CODE(FloatParameter, damageCoeffCurrent, 1103)
			PAR_CODE(FloatParameter, shotsCoeffCurrent,  1104)

			PAR_CODE(UintParameter, lifesCountCurrent,  1005)

			PAR_CODE(UintParameter, pointsCount, 1110)
			PAR_CODE(UintParameter, killsCount,  1111)
			PAR_CODE(UintParameter, deathsCount, 1112)

			PAR_CODE(ConnectedWeaponsList, weaponsList, 1200)
		}

		namespace Functions
		{
			FUNC_CODE_NP(playerTurnOff,      1201)
			FUNC_CODE_NP(playerTurnOn,       1202)
			FUNC_CODE_NP(playerReset,        1203)
			FUNC_CODE_NP(playerRespawn,      1204)
			FUNC_CODE_NP(playerKill,         1205)

			FUNC_CODE_1P(addMaxHealth, IntParameter, 1220)
			FUNC_CODE_1P(setTeam,      uint8_t,      1221)

			FUNC_CODE_1P(registerWeapon,  DeviceAddress,      1300)
			FUNC_CODE_1P(notifyIsDamager, DamageNotification, 1400)

			FUNC_CODE_NP(rifleToHeadSensorHeartbeat, 1500)
		}
	}
}


#endif /* LOGIC_CONFIGS_H_ */