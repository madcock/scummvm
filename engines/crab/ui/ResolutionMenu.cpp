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

#include "crab/crab.h"
#include "crab/ui/ResolutionMenu.h"

namespace Crab {

using namespace pyrodactyl::image;
using namespace pyrodactyl::text;
using namespace pyrodactyl::ui;

void ResolutionMenu::Load(rapidxml::xml_node<char> *node) {
	cancel.Load(node->first_node("cancel"));
	change.Load(node->first_node("change"));
	custom.Load(node->first_node("custom"));

	info.Load(node->first_node("info"));
	def_info = info.text;

	if (nodeValid("reference", node))
		ref.Load(node->first_node("reference"));

	if (nodeValid("inc", node)) {
		inc.Load(node->first_node("inc"));
		loadNum(columns, "columns", node->first_node("inc"));
	}

	if (nodeValid("options", node)) {
		int count_slot = 0;
		rapidxml::xml_node<char> *resnode = node->first_node("options");
		for (auto n = resnode->first_node("res"); n != NULL; n = n->next_sibling("res"), count_slot++) {
			Dimension d;
			loadNum(d.w, "x", n);
			loadNum(d.h, "y", n);

			if (g_engine->_screenSettings->ValidDimension(d)) {
				dim.push_back(d);
				Button b;
				b.Init(ref, inc.x * (count_slot % columns), inc.y * (count_slot / columns));
				b.caption.text = NumberToString(d.w);
				b.caption.text += " x ";
				b.caption.text += NumberToString(d.h);
				element.push_back(b);
			}
		}
	}

	SetInfo();

	loadBool(use_keyboard, "keyboard", node, false);
	AssignPaths();
}

void ResolutionMenu::Draw() {
	info.Draw();

	switch (state) {
	case STATE_NORMAL:
		change.Draw();
		break;
	case STATE_CHANGE:
		Menu::Draw();
		cancel.Draw();
		custom.Draw();
		break;
	default:
		break;
	}
}

int ResolutionMenu::HandleEvents(const Common::Event &Event) {
	switch (state) {
	case STATE_NORMAL:
		if (change.HandleEvents(Event) == BUAC_LCLICK)
			state = STATE_CHANGE;
		break;
	case STATE_CHANGE: {
		int choice = Menu::HandleEvents(Event);
		if (choice >= 0) {
			g_engine->_screenSettings->cur = dim[choice];
			state = STATE_NORMAL;
			return 1;
		}

		if (custom.HandleEvents(Event) == BUAC_LCLICK) {
			state = STATE_NORMAL;
			return 2;
		}

		if (cancel.HandleEvents(Event) == BUAC_LCLICK)
			state = STATE_NORMAL;
	} break;
	default:
		break;
	}

	return 0;
}

#if 0
int ResolutionMenu::HandleEvents(const SDL_Event &Event) {
	switch (state) {
	case STATE_NORMAL:
		if (change.HandleEvents(Event) == BUAC_LCLICK)
			state = STATE_CHANGE;
		break;
	case STATE_CHANGE: {
		int choice = Menu::HandleEvents(Event);
		if (choice >= 0) {
			g_engine->_screenSettings->cur = dim[choice];
			state = STATE_NORMAL;
			return 1;
		}

		if (custom.HandleEvents(Event) == BUAC_LCLICK) {
			state = STATE_NORMAL;
			return 2;
		}

		if (cancel.HandleEvents(Event) == BUAC_LCLICK)
			state = STATE_NORMAL;
	} break;
	default:
		break;
	}

	return 0;
}
#endif

void ResolutionMenu::SetInfo() {
	info.text = def_info;
	info.text += NumberToString(g_engine->_screenSettings->cur.w);
	info.text += " x ";
	info.text += NumberToString(g_engine->_screenSettings->cur.h);
}

void ResolutionMenu::SetUI() {
	cancel.SetUI();
	change.SetUI();
	custom.SetUI();

	info.SetUI();
	ref.SetUI();
	ButtonMenu::SetUI();
}

} // End of namespace Crab
