/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * This code is based on the CRAB engine
 *
 * Copyright (c) Arvind Raja Yadav
 *
 * Licensed under MIT
 *
 */

#include "fightmove.h"
#include "stdafx.h"

using namespace pyrodactyl::anim;

void FightMove::Load(rapidxml::xml_node<char> *node) {
	frames[DIRECTION_DOWN].Load(node->first_node("down"));
	frames[DIRECTION_UP].Load(node->first_node("up"));
	frames[DIRECTION_LEFT].Load(node->first_node("left"));
	frames[DIRECTION_RIGHT].Load(node->first_node("right"));

	if (NodeValid("input", node))
		input.Load(node->first_node("input"));

	if (NodeValid("unlock", node, false))
		unlock.Load(node->first_node("unlock"));

	if (NodeValid("effect", node))
		eff.Load(node->first_node("effect"));

	if (NodeValid("ai", node, false))
		ai.Load(node->first_node("ai"));
}
