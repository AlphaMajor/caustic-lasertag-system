/*
 * game-base-types.hpp
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_GAME_GAME_BASE_TYPES_HPP_
#define INCLUDE_GAME_GAME_BASE_TYPES_HPP_

#include "ir/MT2-base-types.hpp"

namespace GameLog
{
	/**
	 * Description of players damage by one concrete other player
	 */
	struct PvPDamageResults
	{
		PlayerMT2Id player = 0;
		uint16_t killsCount = 0;
		uint16_t woundsCount = 0;
	};

	struct ShotsCounter
	{
		uint16_t count = 0;
	};
}



#endif /* INCLUDE_GAME_GAME_BASE_TYPES_HPP_ */