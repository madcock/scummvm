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

#ifndef TITANIC_SCREEN_MANAGER_H
#define TITANIC_SCREEN_MANAGER_H

#include "common/scummsys.h"
#include "common/array.h"
#include "titanic/support/direct_draw.h"
#include "titanic/support/font.h"
#include "titanic/input_handler.h"
#include "titanic/support/mouse_cursor.h"
#include "titanic/support/text_cursor.h"
#include "titanic/support/video_surface.h"
#include "titanic/core/resource_key.h"

namespace Titanic {

/**
 * The original used page flipping with one primary and one back buffer.
 * Since we don't need that in ScummVM, the back buffer number below is
 * remapped to the primary surface
 */
enum SurfaceNum {
	SURFACE_PRIMARY = -1,
	SURFACE_BACKBUFFER = -1
};

class TitanicEngine;

class CScreenManager {
	struct VideoSurfaceEntry {
		CVideoSurface *_surface;
		Rect _bounds;
	};
protected:
	TitanicEngine *_vm;
public:
	static CScreenManager *_screenManagerPtr;
	static CScreenManager *_currentScreenManagerPtr;

	/**
	 * Set the current screen manager
	 */
	static CScreenManager *setCurrent();
public:
	Common::Array<VideoSurfaceEntry> _backSurfaces;
	CVideoSurface *_frontRenderSurface;
	CMouseCursor *_mouseCursor;
	CTextCursor *_textCursor;
	CInputHandler *_inputHandler;
	int _fontNumber;
public:
	CScreenManager(TitanicEngine *vm);
	virtual ~CScreenManager();

	void fn1() {}
	void fn2() {}

	virtual void setWindowHandle(int v);
	virtual bool resetWindowHandle(int v);
	
	/**
	 * Sets the video mode
	 */
	virtual void setMode(int width, int height, int bpp, uint numBackSurfaces, bool flag2) = 0;

	/**
	 * Handles drawing the cursors
	 */
	virtual void drawCursors() = 0;
	
	virtual void proc6() = 0;
	virtual void proc7() = 0;
	virtual CVideoSurface *getSurface(SurfaceNum surfaceNum) const = 0;

	/**
	 * Return the front render surface
	 */
	virtual CVideoSurface *getFrontRenderSurface() const = 0;
	
	/**
	 * Fill an area with a specific color
	 */
	virtual void fillRect(SurfaceNum surfaceNum, Rect *rect, byte r, byte g, byte b) = 0;

	/**
	 * Blits a surface onto one of the screen surfaces
	 */
	virtual void blitFrom(SurfaceNum surfaceNum, CVideoSurface *src, const Point *destPos = nullptr,
		const Rect *srcRect = nullptr) = 0;

	virtual void proc12() = 0;
	
	/**
	 * Write a string
	 * @param surfaceNum	Destination surface
	 * @param destRect		Bounds within dest surface
	 * @param yOffset		Y offset for drawing, to allow for parts of
	 *						the text to be scrolled off-screen
	 * @param str			Line or lines to write
	 * @param textCursor	Optional text cursor pointer
	 */
	virtual int writeString(int surfaceNum, const Rect &destRect,
		int yOffset, const CString &str, CTextCursor *textCursor) = 0;

	virtual void proc14() = 0;

	/**
	 * Set the font color
	 */
	virtual void setFontColor(byte r, byte g, byte b) = 0;

	/**
	 * Get the text area a string will fit into
	 * @param str		String
	 * @param maxWidth	Maximum width in pixels
	 * @param sizeOut	Optional pointer to output size
	 * @returns			Required height
	 */
	virtual int getTextBounds(const CString &str, int maxWidth, Point *sizeOut = nullptr) const = 0;

	/**
	 * Get the current font height
	 */
	virtual int getFontHeight() const = 0;

	/**
	 * Returns the width of a given string in pixels
	 */
	virtual int stringWidth(const CString &str) = 0;

	virtual void proc19() = 0;

	/**
	 * Clear a portion of a specified surface
	 */
	virtual void clearSurface(SurfaceNum surfaceNum, Rect *_bounds) = 0;

	/**
	 * Resize the passed surface
	 */
	virtual void resizeSurface(CVideoSurface *surface, int width, int height) = 0;

	/**
	 * Creates a surface of a given size
	 */
	virtual CVideoSurface *createSurface(int w, int h) = 0;
	
	/**
	 * Creates a surface from a specified resource
	 */
	virtual CVideoSurface *createSurface(const CResourceKey &key) = 0;
	
	virtual void proc24() = 0;
	virtual void proc25() = 0;
	virtual void showCursor() = 0;
	virtual void hideCursor() = 0;

	/**
	 * Set drawing bounds for a specified surface
	 */
	void setSurfaceBounds(SurfaceNum surfaceNum, const Rect &r);

	/**
	 * Set the current font number
	 */
	int setFontNumber(int fontNumber);
};

class OSScreenManager: CScreenManager {
private:
	DirectDrawManager _directDrawManager;

	/**
	 * Frees any surface buffers
	 */
	void destroyFrontAndBackBuffers();

	/**
	 * Load game cursors
	 */
	void loadCursors();

	/**
	 * Gets an underlying surface
	 */
	DirectDrawSurface *getDDSurface(SurfaceNum surfaceNum);
public:
	int _field48;
	int _field4C;
	int _field50;
	int _field54;
	STFont _fonts[4];
public:
	OSScreenManager(TitanicEngine *vm);
	virtual ~OSScreenManager();

	/**
	 * Sets the video mode
	 */
	virtual void setMode(int width, int height, int bpp, uint numBackSurfaces, bool flag2);

	/**
	 * Handles drawing the cursors
	 */
	virtual void drawCursors();

	virtual void proc6();
	virtual void proc7();
	virtual CVideoSurface *getSurface(SurfaceNum surfaceNum) const;

	/**
	 * Return the front render surface
	 */
	virtual CVideoSurface *getFrontRenderSurface() const {
		return _frontRenderSurface;
	}


	/**
	 * Fill an area with a specific color
	 */
	virtual void fillRect(SurfaceNum surfaceNum, Rect *rect, byte r, byte g, byte b);

	/**
	 * Blits a surface onto one of the screen surfaces
	 */
	virtual void blitFrom(SurfaceNum surfaceNum, CVideoSurface *src, const Point *destPos,
		const Rect *srcRect = nullptr);

	virtual void proc12();

	/**
	 * Write a string
	 * @param surfaceNum	Destination surface
	 * @param destRect		Bounds within dest surface
	 * @param yOffset		Y offset for drawing, to allow for parts of
	 *						the text to be scrolled off-screen
	 * @param str			Line or lines to write
	 * @param textCursor	Optional text cursor pointer
	 */
	virtual int writeString(int surfaceNum, const Rect &destRect,
		int yOffset, const CString &str, CTextCursor *textCursor);

	virtual void proc14();

	/**
	 * Set the font color
	 */
	virtual void setFontColor(byte r, byte g, byte b);

	/**
	 * Get the text area a string will fit into
	 * @param str		String
	 * @param maxWidth	Maximum width in pixels
	 * @param sizeOut	Optional pointer to output size
	 * @returns			Required height
	 */
	virtual int getTextBounds(const CString &str, int maxWidth, Point *sizeOut = nullptr) const;

	/**
	 * Get the current font height
	 */
	virtual int getFontHeight() const;

	/**
	 * Returns the width of a given string in pixels
	 */
	virtual int stringWidth(const CString &str);

	virtual void proc19();

	/**
	 * Clear a portion of the screen surface
	 */
	virtual void clearSurface(SurfaceNum surfaceNum, Rect *bounds);

	/**
	 * Resize the passed surface
	 */
	virtual void resizeSurface(CVideoSurface *surface, int width, int height);

	/**
	 * Creates a surface of a given size
	 */
	virtual CVideoSurface *createSurface(int w, int h);
	
	/**
	 * Creates a surface from a specified resource
	 */
	virtual CVideoSurface *createSurface(const CResourceKey &key);

	virtual void proc23();
	virtual void proc24();
	virtual void proc25();
	virtual void showCursor();
	virtual void hideCursor();
};

} // End of namespace Titanic

#endif /* TITANIC_SCREEN_MANAGER_H */
