/* ScummVMDS - Scumm Interpreter DS Port
 * Copyright (C) 2002-2004 The ScummVM project and Neil Millstone
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
 
#ifndef _DS_FS_H
#define _DS_FS_H


#include "stdafx.h"
#include "common/array.h"
#include "common/str.h"

//#include <NDS/ARM9/console.h>
#include "fs.h"
#include "zipreader.h"
#include "ramsave.h"
#include "scummconsole.h"
#include "gba_nds_fat.h"
#include "backends/fs/abstract-fs.h"
//#include "backends/fs/fs.h"

namespace DS {

/**
 * This class is used when a Flash cart is in use.
 */
class DSFileSystemNode : public AbstractFilesystemNode {
protected:
	static ZipFile* _zipFile;

	typedef class Common::String String;

	String _displayName;
	bool _isDirectory;
	bool _isValid;
	String _path;
	int _refCountVal;
	
public:
	DSFileSystemNode();
	DSFileSystemNode(const String &path);
	DSFileSystemNode(const DSFileSystemNode *node);
	DSFileSystemNode(const String& path, bool isDir);
	
	virtual String displayName() const {  return _displayName; }
	virtual String name() const {  return _displayName; }
	virtual bool isValid() const { return _isValid; }
	virtual bool isDirectory() const { return _isDirectory; }
	virtual String path() const { return _path; }
	
	virtual bool listDir(AbstractFSList &list, ListMode mode = FilesystemNode::kListDirectoriesOnly) const;
	virtual AbstractFilesystemNode *parent() const;
	virtual AbstractFilesystemNode *clone() const { return new DSFileSystemNode(this); }
	virtual AbstractFilesystemNode *child(const Common::String& name) const;
	static ZipFile* getZip() { return _zipFile; }
};


/**
 * This class is used when the GBAMP (GBA Movie Player) is used with a CompactFlash card.
 */
class GBAMPFileSystemNode : public AbstractFilesystemNode {
protected:
	typedef class Common::String String;

	String _displayName;
	bool _isDirectory;
	bool _isValid;
	String _path;
	
	int _refCountVal;
	
public:
	GBAMPFileSystemNode();
	GBAMPFileSystemNode(const String &path);
	GBAMPFileSystemNode(const String &path, bool isDirectory);
	GBAMPFileSystemNode(const GBAMPFileSystemNode *node);

	virtual String displayName() const {  return _displayName; }
	virtual String name() const {  return _displayName; }
	
	virtual bool isValid() const { return _isValid; }
	virtual bool isDirectory() const { return _isDirectory; }
	virtual String path() const { return _path; }
	virtual bool listDir(AbstractFSList &list, ListMode mode = FilesystemNode::kListDirectoriesOnly) const;
	virtual AbstractFilesystemNode *parent() const;
	virtual AbstractFilesystemNode *clone() const { return new GBAMPFileSystemNode(this); }
	virtual AbstractFilesystemNode *child(const Common::String& name) const;
	
};


// FIXME: Why is assert redefined ? And why here (this is definitely the wrong place).

#ifdef assert
#undef assert
#endif

#define assert(s) if (!(s)) consolePrintf("Assertion failed: '##s##' at file %s, line %d\n", __FILE__, __LINE__)

}

#endif
