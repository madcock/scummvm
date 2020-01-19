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

#ifndef ULTIMA_GFX_DIALOG_H
#define ULTIMA_GFX_DIALOG_H

#include "ultima/shared/gfx/visual_item.h"

namespace Ultima {
namespace Shared {

class GameBase;

namespace Gfx {

/**
 * Base class for dialog-type views that pop up on top of an existing view
 */
class Dialog : public VisualItem {
protected:
	GameBase *_game;
	VisualItem *_parentView;
public:
	/**
	 * Constructor
	 */
	Dialog(GameBase *game) : VisualItem(nullptr), _game(game) {
	}

	/**
	 * Show the dialog
	 */
	void show();

	/**
	 * Hide the dialog
	 */
	void hide();
};

} // End of namespace Gfx
} // End of namespace Shared
} // End of namespace Ultima

#endif
