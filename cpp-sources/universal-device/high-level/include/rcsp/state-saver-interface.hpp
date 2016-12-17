/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef INCLUDE_RCSP_STATE_SAVER_INTERFACE_HPP_
#define INCLUDE_RCSP_STATE_SAVER_INTERFACE_HPP_

/**
 * Any object that can save its state to disk
 */
class IAnyStateSaver
{
public:
	virtual ~IAnyStateSaver() {}
	virtual void saveState() = 0;
};

#endif /* INCLUDE_RCSP_STATE_SAVER_INTERFACE_HPP_ */