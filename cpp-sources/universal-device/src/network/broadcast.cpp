/*
 * broadcast.cpp
 *
 *  Created on: 14 апр. 2015 г.
 *      Author: alexey
 */

#include "network/broadcast.hpp"

Broadcast broadcast;

bool Broadcast::isBroadcast(const DeviceAddress& addr)
{
	return addr.address[0] == 255;
}