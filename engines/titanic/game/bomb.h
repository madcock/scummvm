/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TITANIC_BOMB_H
#define TITANIC_BOMB_H

#include "titanic/core/background.h"
#include "titanic/messages/messages.h"

namespace Titanic {

class CBomb : public CBackground, CEnterRoomMsgTarget {
private:
	int _fieldE0;
	int _fieldE4;
	int _fieldE8;
	int _fieldEC;
	int _fieldF0;
	int _fieldF4;
	int _fieldF8;
	int _fieldFC;
	int _startingTicks;
	int _field104;
protected:
	virtual bool handleMessage(CEnterRoomMsg &msg);
public:
	CLASSDEF
	CBomb();

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent) const;

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file);
};

} // End of namespace Titanic

#endif /* TITANIC_BOMB_H */
