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

#include "mm/mm1/views/combat.h"
#include "mm/mm1/game/encounter.h"
#include "mm/mm1/globals.h"
#include "mm/mm1/mm1.h"
#include "mm/mm1/sound.h"

namespace MM {
namespace MM1 {
namespace Views {

Combat::Combat() : TextView("Combat"),
		_monsterList(g_globals->_encounters._monsterList) {
}

bool Combat::msgFocus(const FocusMessage &msg) {
	_mode = BATTLE;

	// Clear combat data
	clear();

	_monstersCount = _monsterList.size();
	_party.clear();
	for (uint i = 0; i < g_globals->_party.size(); ++i)
		_party.push_back(&g_globals->_party[i]);

	loadArrays();
	setupCanAttacks();
	setupHandicap();

	// TODO: Replace with correct selection of starting char
	g_globals->_currCharacter = &g_globals->_party[0];

	return true;
}

void Combat::draw() {
	//Game::Encounter &enc = g_globals->_encounters;

	clearSurface();
	writeStaticContent();
	writeHandicap();
	writeRound();
	writePartyNumbers();
	writeMonsters();
	writeParty();

	writeOptions();
}

void Combat::timeout() {
	redraw();
}

bool Combat::msgKeypress(const KeypressMessage &msg) {
	return true;
}

void Combat::writeOptions() {
	resetBottom();
	writeString(0, 20, STRING["dialogs.combat.options_for"]);
	writeString(0, 22, g_globals->_currCharacter->_name);

	switch (_currentChar) {
	case 0:
		writeChar(3, 3, (unsigned char)'\xB1');
		break;
	case 1:
		writeChar(7, 3, (unsigned char)'\xB2');
		break;
	case 2:
		writeChar(3, 4, (unsigned char)'\xB3');
		break;
	case 3:
		writeChar(7, 4, (unsigned char)'\xB4');
		break;
	case 4:
		writeChar(3, 5, (unsigned char)'\xB5');
		break;
	case 5:
		writeChar(7, 5, (unsigned char)'\xB6');
		break;
	default:
		break;
	}

	bool canAttack = _canAttack[_currentChar];
	if (canAttack) {
		writeAttackOptions();
		_val5 = 'A';
		_val2 = 'F';

		// Archers can always attack
		canAttack = g_globals->_currCharacter->_class == ARCHER;
	}
	if (canAttack && g_globals->_currCharacter->_v6a) {
		_val3 = 'S';
		writeShootOption();
	}

	if (g_globals->_currCharacter->_sp._current) {
		writeCastOption();
		_val4 = 'C';
	}

	writeString(16, 22, STRING["dialogs.combat.exchange_use"]);
	writeString(16, 23, STRING["dialogs.combat.retreat_block"]);
}

void Combat::writeAttackOptions() {
	writeString(16, 20, STRING["dialogs.combat.attack"]);
	writeString(16, 21, STRING["dialogs.combat.fight"]);
}

void Combat::writeCastOption() {
	writeString(30, 21, STRING["dialogs.combat.cast"]);
}

void Combat::writeShootOption() {
	writeString(30, 20, STRING["dialogs.combat.shoot"]);
}

void Combat::resetBottom() {
	clearLines(20, 24);
	_val2 = _val3 = _val4 = _val5 = ' ';
}

void Combat::writeStaticContent() {
	writeString(0, 0, STRING["dialogs.combat.combat"]);
	writeString(0, 1, STRING["dialogs.combat.round"]);
	writeString(0, 7, STRING["dialogs.combat.delay"]);
	writeString(0, 8, STRING["dialogs.combat.protect"]);
	writeString(0, 9, STRING["dialogs.combat.quickref"]);
	writeString(0, 10, STRING["dialogs.combat.view_char"]);
	writeString(0, 12, STRING["dialogs.combat.handicap"]);

	_textPos = Common::Point(0, 15);
	for (int i = 0; i < 40; ++i)
		writeChar('-');
}

void Combat::writeHandicap() {
	writeString(0, 13, "          ");
	_textPos.x = 0;

	switch (_handicap) {
	case HANDICAP_EVEN:
		writeString(STRING["dialogs.combat.even"]);
		break;
	case HANDICAP_PARTY:
		writeString(STRING["dialogs.combat.party_plus"]);
		writeNumber(_handicap4);
		break;
	case HANDICAP_MONSTER:
		writeString(STRING["dialogs.combat.monster_plus"]);
		writeNumber(_handicap4);
		break;
	}
}

void Combat::writeRound() {
	writeNumber(7, 1, _roundNum);
}

void Combat::writePartyNumbers() {
	for (uint i = 0; i < g_globals->_party.size(); ++i) {
		writeChar(2 + 4 * (i % 2), 3 + (i / 2),
			_canAttack[i] ? '+' : ' ');
		writeChar('1' + i);
	}
}

void Combat::writeMonsters() {
	if (_monsterList.empty()) {
		_textPos = Common::Point(10, 0);
		writeSpaces(30);
	} else {
		for (int i = 0; i < (int)_monsterList.size(); ++i) {
			_textPos = Common::Point(11, i);
			writeChar(i < _attackerVal ? '+' : ' ');
			writeChar('A' + i);
			writeString(") ");
			writeString(_monsterList[i]._name);
			writeMonsterStatus(i);
		}
	}

	for (; _textPos.y < 15; _textPos.y++) {
		_textPos.x = 10;
		writeSpaces(30);
	}
}

void Combat::writeMonsterStatus(int monsterNum) {
	monsterSetPtr(monsterNum);
	byte statusBits = _monsterStatus[monsterNum];

	if (statusBits) {
		writeDots();

		int status;
		if (statusBits == 0xff) {
			status = MON_DEAD;
		} else {	
			for (status = MON_PARALYZED; !(statusBits & 0x80);
					++status, statusBits <<= 1) {
			}
		}

		writeString(STRING[Common::String::format("dialogs.combat.status.%d",
			status)]);
	} else if (_arr1[monsterNum] != _monsterP->_field11) {
		writeDots();
		writeString(STRING["dialogs.combat.status.wounded"]);
	} else {
		writeSpaces(40 - _textPos.x);
	}
}

void Combat::writeDots() {
	while (_textPos.x < 30)
		writeChar('.');
}

void Combat::writeParty() {
	clearPartyArea();

	for (uint i = 0; i < g_globals->_party.size(); ++i) {
		writeChar(1 + 21 * (i % 2), 16 + (i / 2), '1' + i);
		writeString(") ");
		writeString(g_globals->_party[i]._name);
	}
}

void Combat::clearPartyArea() {
	clearLines(16, 18);
}

} // namespace Views
} // namespace MM1
} // namespace MM
