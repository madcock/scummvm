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

#include "ac/global_dynamicsprite.h"
#include "util/wgt2allg.h" // Allegro RGB, PALETTE
#include "ac/draw.h"
#include "ac/dynamicsprite.h"
#include "ac/path_helper.h"
#include "ac/spritecache.h"
#include "ac/runtime_defines.h" //MAX_PATH
#include "gfx/graphicsdriver.h"
#include "gfx/bitmap.h"

using namespace AGS::Common;
using namespace AGS::Engine;

extern SpriteCache spriteset;
extern IGraphicsDriver *gfxDriver;

int LoadImageFile(const char *filename) {
	ResolvedPath rp;
	if (!ResolveScriptPath(filename, true, rp))
		return 0;

	Bitmap *loadedFile = BitmapHelper::LoadFromFile(rp.FullPath);
	if (!loadedFile && !rp.AltPath.IsEmpty() && rp.AltPath.Compare(rp.FullPath) != 0)
		loadedFile = BitmapHelper::LoadFromFile(rp.AltPath);
	if (!loadedFile)
		return 0;

	int gotSlot = spriteset.GetFreeIndex();
	if (gotSlot <= 0)
		return 0;

	add_dynamic_sprite(gotSlot, ReplaceBitmapWithSupportedFormat(loadedFile));

	return gotSlot;
}
