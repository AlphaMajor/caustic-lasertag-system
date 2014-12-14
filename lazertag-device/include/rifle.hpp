/*
 * rifle.hpp
 *
 *  Created on: 13 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_RIFLE_HPP_
#define LAZERTAG_DEVICE_INCLUDE_RIFLE_HPP_

#include <stdint.h>

/*
enum RifleMagazinesCountMode
{
	RMU_FIXED_COUNT = 0,       // Fixed count when respawn
	RMU_UNLIMITED,             // Unlimited
};
*/
enum RifleReloadMode
{
	RRM_ONLY_SHUTTER,          // Disort the shutter
	RRM_MAGAZINE_AND_SHUTTER,  // Changing magazine (depends on RifleMagazineType) and disort the shutter
};

enum RifleAutoReload
{
	RAR_DISABLED = 0,
	RAR_ENABLED
};

enum RifleMagazineType
{
	RMT_UNCHANGABLE = 0,    // Magazine does not really exists
	RMT_ONE_REPLACEABLE,    // One magazine that can be disconnected
	RMT_TWO_REPLACEABLE,    // Two magazines that should be swapped
	RMT_INTELLECTUAL,       // Magazines contains MCU and interface
};


class Rifle
{
public:
	class Configuration
	{
	public:
		Configuration();

		void setFallback();
		//void readFromFile(const char* filename);

		uint8_t slot;
		uint8_t weightInSlot;

		uint8_t damage;
		uint32_t firePeriod; // Time between shots in us

		RifleMagazineType magazineType;
		RifleReloadMode magazinesReloadMode;
		RifleAutoReload autoReload;
		uint32_t magazinesCount;
		uint32_t bulletsPerMagazine;
	};

	Rifle();
	void init();
	void mainLoopBody();

	Configuration config;
private:

};





#endif /* LAZERTAG_DEVICE_INCLUDE_RIFLE_HPP_ */
