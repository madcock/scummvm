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

#include "chewy/defines.h"
#include "chewy/events.h"
#include "chewy/global.h"
#include "chewy/ani_dat.h"
#include "chewy/room.h"
#include "chewy/rooms/room52.h"

namespace Chewy {
namespace Rooms {

void Room52::entry() {
	SetUpScreenFunc = setup_func;
	spieler_mi[P_HOWARD].Mode = true;
	if (_G(spieler).R52HotDogOk && !_G(spieler).R52KakerWeg)
		plot_armee(0);
	if (_G(spieler).R52KakerWeg)
		det->stop_detail(0);
	if (!flags.LoadGame) {
		det->show_static_spr(4);
		_G(spieler).R52TuerAuf = true;
		set_person_pos(20, 50, P_HOWARD, P_LEFT);
		set_person_pos(35, 74, P_CHEWY, P_RIGHT);
		auto_move(2, P_CHEWY);
		_G(spieler).R52TuerAuf = false;
		det->hide_static_spr(4);
		check_shad(2, 1);
	}
}

void Room52::xit() {
	if (_G(spieler).PersonRoomNr[P_HOWARD] == 52) {
		_G(spieler).PersonRoomNr[P_HOWARD] = 51;
		spieler_mi[P_HOWARD].Mode = false;
	}
}

void Room52::gedAction(int index) {
	if (index == 1)
		kaker_platt();
}

int16 Room52::use_hot_dog() {
	int16 i;
	int16 action_ret = false;
	hide_cur();
	if (is_cur_inventar(BURGER_INV)) {
		action_ret = true;
		auto_move(3, P_CHEWY);
		start_spz_wait(CH_ROCK_GET1, 1, ANI_VOR, P_CHEWY);
		det->show_static_spr(0);
		del_inventar(_G(spieler).AkInvent);
		auto_move(4, P_CHEWY);
		_G(spieler).R52HotDogOk = true;
		plot_armee(20);
		atds->set_ats_str(341, 1, ATS_DATEI);
		auto_move(2, P_CHEWY);
		set_person_spr(P_LEFT, P_CHEWY);
		start_aad_wait(288, -1);
	} else if (is_cur_inventar(KILLER_INV)) {
		action_ret = true;
		auto_move(5, P_CHEWY);
		_G(spieler).PersonHide[P_CHEWY] = true;
		start_detail_wait(7, 1, ANI_VOR);
		det->start_detail(8, 255, ANI_VOR);
		for (i = 0; i < 5; i++) {
			wait_show_screen(20);
			det->stop_detail(2 + i);
		}
		det->stop_detail(0);
		det->stop_detail(8);
		start_detail_wait(7, 1, ANI_RUECK);
		_G(spieler).PersonHide[P_CHEWY] = false;
		atds->set_steuer_bit(341, ATS_AKTIV_BIT, ATS_DATEI);
		start_aad_wait(303, -1);
		atds->set_ats_str(KILLER_INV, 1, INV_ATS_DATEI);
		_G(spieler).R52KakerWeg = true;
	}
	show_cur();
	return action_ret;
}

void Room52::plot_armee(int16 frame) {
	int16 i;
	for (i = 0; i < 5; i++) {
		wait_show_screen(frame);
		det->start_detail(2 + i, 255, ANI_VOR);
	}
}

void Room52::kaker_platt() {
	if (!_G(spieler).R52KakerJoke &&
		_G(spieler).R52HotDogOk &&
		!_G(spieler).R52KakerWeg &&
		!flags.ExitMov) {
		_G(spieler).R52KakerJoke = true;
		stop_person(P_CHEWY);
		start_aad_wait(289, -1);
	}
}

void Room52::setup_func() {
	int16 x, y;
	int16 ch_y;
	if (_G(spieler).PersonRoomNr[P_HOWARD] == 52) {
		calc_person_look();
		x = 1;
		ch_y = spieler_vector[P_CHEWY].Xypos[1];
		if (ch_y < 97) {
			y = 44;
		} else {
			y = 87;
		}
		go_auto_xy(x, y, P_HOWARD, ANI_GO);
	}
}

} // namespace Rooms
} // namespace Chewy
