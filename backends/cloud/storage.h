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

#ifndef BACKENDS_CLOUD_STORAGE_H
#define BACKENDS_CLOUD_STORAGE_H

#include "backends/cloud/storagefile.h"
#include "backends/cloud/storageinfo.h"
#include "backends/networking/curl/request.h"
#include "backends/networking/curl/curlrequest.h"
#include "common/array.h"
#include "common/stream.h"
#include "common/str.h"
#include "common/callback.h"

namespace Cloud {

/** Struct to represent upload() resulting status. */
struct UploadStatus {
	/** true if Request was interrupted (finished by user with finish()) */
	bool interrupted;
	/** true if Request has failed (bad server response or some other error occurred) */
	bool failed;
	/** Contains uploaded file description (empty if failed) */
	StorageFile file;
	/** Server's original response (empty if not failed) */
	Common::String response;
	/** Server's HTTP response code. */
	long httpResponseCode;

	UploadStatus():
		interrupted(false), failed(false), file(), response(), httpResponseCode(-1) {}

	UploadStatus(bool interrupt, bool failure, StorageFile f, Common::String resp, long code):
		interrupted(interrupt), failed(failure), file(f), response(resp), httpResponseCode(code) {}
};

/** Struct to represent upload() resulting status. */
struct ListDirectoryStatus {
	/** true if Request was interrupted (finished by user with finish()) */
	bool interrupted;
	/** true if Request has failed (bad server response or some other error occurred) */
	bool failed;
	/** Contains listed files (might be incomplete if failed or interrupted) */
	Common::Array<StorageFile> &files;
	/** Server's original response (empty if not failed) */
	Common::String response;
	/** Server's HTTP response code. */
	long httpResponseCode;

	ListDirectoryStatus(Common::Array<StorageFile> &f) :
		interrupted(false), failed(false), files(f), response(), httpResponseCode(-1) {}

	ListDirectoryStatus(bool interrupt, bool failure, Common::Array<StorageFile> &f, Common::String resp, long code) :
		interrupted(interrupt), failed(failure), files(f), response(resp), httpResponseCode(code) {}
};

class Storage {
public:
	typedef Networking::Response<Common::Array<StorageFile>&> FileArrayResponse;
	typedef Networking::Response<StorageInfo> StorageInfoResponse;
	typedef Networking::Response<bool> BoolResponse;
	typedef Networking::Response<UploadStatus> UploadResponse;
	typedef Networking::Response<ListDirectoryStatus> ListDirectoryResponse;

	typedef Common::BaseCallback<FileArrayResponse> *FileArrayCallback;
	typedef Common::BaseCallback<StorageInfoResponse> *StorageInfoCallback;
	typedef Common::BaseCallback<BoolResponse> *BoolCallback;
	typedef Common::BaseCallback<UploadResponse> *UploadCallback;
	typedef Common::BaseCallback<ListDirectoryResponse> *ListDirectoryCallback;

	Storage() {}
	virtual ~Storage() {}

	/**
	 * Storage methods, which are used by CloudManager to save
	 * storage in configuration file.
	 */

	/**
	 * Save storage data using ConfMan.
	 * @param keyPrefix all saved keys must start with this prefix.
	 * @note every Storage must write keyPrefix + "type" key
	 *       with common value (e.g. "Dropbox").
	 */
	virtual void saveConfig(Common::String keyPrefix) = 0;

	/**
	 * Public Cloud API comes down there.
	 *
	 * All Cloud API methods return Networking::Request *, which
	 * might be used to control request. All methods also accept
	 * a callback, which is called, when request is complete.
	 */

	/** Returns ListDirectoryStatus struct with list of files. */
	virtual Networking::Request *listDirectory(Common::String path, ListDirectoryCallback callback, bool recursive = false) = 0;
	
	/** Returns UploadStatus struct with info about uploaded file. */
	virtual Networking::Request *upload(Common::String path, Common::SeekableReadStream *contents, UploadCallback callback) = 0;
	virtual Networking::Request *upload(Common::String remotePath, Common::String localPath, UploadCallback callback) = 0;

	/** Returns pointer to Networking::NetworkReadStream. */
	virtual Networking::Request *streamFile(Common::String path, Networking::NetworkReadStreamCallback callback) = 0;

	/** Calls the callback when finished. */
	virtual Networking::Request *download(Common::String remotePath, Common::String localPath, BoolCallback callback) = 0;

	/** Returns Common::Array<StorageFile> with list of files, which were not downloaded. */
	virtual Networking::Request *downloadFolder(Common::String remotePath, Common::String localPath, FileArrayCallback callback, bool recursive = false) = 0;

	/** Calls the callback when finished. */
	virtual Networking::Request *remove(Common::String path, BoolCallback callback) = 0;

	/** Calls the callback when finished. */
	virtual Networking::Request *syncSaves(BoolCallback callback) = 0;

	/** Calls the callback when finished. */
	virtual Networking::Request *createDirectory(Common::String path, BoolCallback callback) = 0;

	/** Calls the callback when finished. */
	virtual Networking::Request *touch(Common::String path, BoolCallback callback) = 0;

	/** Returns the StorageInfo struct. */
	virtual Networking::Request *info(StorageInfoCallback callback) = 0;

	/** Returns whether saves sync process is running. */
	virtual bool isSyncing() = 0;

	/** Returns whether there are any requests running. */
	virtual bool isWorking() = 0;
};

} // End of namespace Cloud

#endif
