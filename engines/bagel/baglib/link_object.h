
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

#ifndef BAGEL_BAGLIB_LINK_OBJECT_H
#define BAGEL_BAGLIB_LINK_OBJECT_H

#include "bagel/baglib/object.h"

namespace Bagel {

/**
 * CBagLinkObject is an object that can be place within the slide window.
 */
class CBagLinkObject : public CBagObject {
public:
	enum LINK_TYPE { LINK = 0, CLOSEUP = 1 };

private:
	CBofSize m_xSize;
	int m_nOutline;
	CBofPoint m_xDestLocation;
	CBofPoint m_xSrcLocation;
	LINK_TYPE m_xLinkType;
	int m_nFade;

public:
	CBagLinkObject();
	virtual ~CBagLinkObject();

	// Return true if the Object had members that are properly initialized/de-initialized
	PARSE_CODES setInfo(CBagIfstream &istr);

	CBofRect getRect();

	CBofSize getSize() const {
		return m_xSize;
	}
	CBofPoint GetDstLoc() const {
		return m_xDestLocation;
	}
	CBofPoint GetSrcLoc() const {
		return m_xSrcLocation;
	}

	void setSize(const CBofSize &xSize) {
		m_xSize = xSize;
	}
	void SetDstLoc(CBofPoint xLoc) {
		m_xDestLocation = xLoc;
	}
	void SetSrcLoc(CBofPoint xLoc) {
		m_xSrcLocation = xLoc;
	}
	void SetOutline(int nColor) {
		m_nOutline = nColor;
	}

	virtual bool runObject();
};

} // namespace Bagel

#endif
