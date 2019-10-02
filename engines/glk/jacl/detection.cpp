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

#include "glk/jacl/detection.h"
#include "glk/jacl/detection_tables.h"
#include "glk/jacl/common/debug.h"
#include "glk/jacl/common/file.h"
#include "glk/jacl/common/md5.h"
#include "glk/jacl/engines/game.h"

namespace Glk {
namespace JACL {

void JACLMetaEngine::getSupportedGames(PlainGameList &games) {
	for (const PlainGameDescriptor *pd = JACL_GAME_LIST; pd->gameId; ++pd)
		games.push_back(*pd);
}

GameDescriptor JACLMetaEngine::findGame(const char *gameId) {
	for (const PlainGameDescriptor *pd = JACL_GAME_LIST; pd->gameId; ++pd) {
		if (!strcmp(gameId, pd->gameId))
			return *pd;
	}

	return GameDescriptor::empty();
}

bool JACLMetaEngine::detectGames(const Common::FSList &fslist, DetectedGames &gameList) {
	// Loop through the files of the folder
	for (Common::FSList::const_iterator file = fslist.begin(); file != fslist.end(); ++file) {
		// Check for a recognised filename
		if (file->isDirectory())
			continue;

		Common::String filename = file->getName();
		if (!filename.hasSuffixIgnoreCase(".j2"))
			continue;

		Common::File gameFile;
		if (!gameFile.open(*file))
			continue;

		gameFile.seek(0);
		Common::String md5 = Common::computeStreamMD5AsString(gameFile, 5000);
		uint32 filesize = gameFile.size();

		// Scan through the JACL game list for a match
		const GlkDetectionEntry *p = JACL_GAMES;
		while (p->_md5 && p->_filesize != filesize && md5 != p->_md5)
			++p;

		if (!p->_gameId) {
			const PlainGameDescriptor &desc = JACL_GAME_LIST[0];
			gameList.push_back(GlkDetectedGame(desc.gameId, desc.description, filename, md5, filesize));
		} else {
			// Found a match
			PlainGameDescriptor gameDesc = findGame(p->_gameId);
			gameList.push_back(GlkDetectedGame(p->_gameId, gameDesc.description, filename));
		}
	}

	return !gameList.empty();
}

void JACLMetaEngine::detectClashes(Common::StringMap &map) {
	for (const PlainGameDescriptor *pd = JACL_GAME_LIST; pd->gameId; ++pd) {
		if (map.contains(pd->gameId))
			error("Duplicate game Id found - %s", pd->gameId);
		map[pd->gameId] = "";
	}
}

} // End of namespace JACL
} // End of namespace Glk
