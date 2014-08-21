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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "access/inventory.h"
#include "access/access.h"
#include "access/amazon/amazon_resources.h"
#include "access/martian/martian_resources.h"

namespace Access {

InventoryManager::InventoryManager(AccessEngine *vm) : Manager(vm) {
	_startInvItem = 0;
	_startInvBox = 0;
	_invChangeFlag = true;
	_invRefreshFlag = false;
	_invModeFlag = false;
	_startAboutItem = 0;
	_startTravelItem = 0;

	const char *const *names;
	switch (vm->getGameID()) {
	case GType_Amazon:
		names = Amazon::INVENTORY_NAMES;
		_inv.resize(85);
		break;
	case GType_MartianMemorandum:
		names = Martian::INVENTORY_NAMES;
		_inv.resize(54);
		break;
	default:
		error("Unknown game");
	}

	for (uint i = 0; i < _inv.size(); ++i)
		_names.push_back(names[i]);
}

int &InventoryManager::operator[](int idx) {
	return _inv[idx];
}

int InventoryManager::useItem() { 
	return _vm->_useItem;
}

void InventoryManager::setUseItem(int itemId) { 
	_vm->_useItem = itemId; 
}

void InventoryManager::refreshInventory() {
	if (_vm->_screen->_vesaMode) {
		_invRefreshFlag = true;
		newDisplayInv();
	}
}

int InventoryManager::newDisplayInv() {
	warning("TODO: newDisplayInv");
	return 0;
}


} // End of namespace Access
