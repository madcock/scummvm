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

// Resource library

#include "common/archive.h"
#include "common/file.h"
#include "common/textconsole.h"
#include "common/memstream.h"
#include "sci/resource.h"
#include "sci/resource_intern.h"
#include "sci/util.h"

namespace Sci {

AudioVolumeResourceSource::AudioVolumeResourceSource(ResourceManager *resMan, const Common::String &name, ResourceSource *map, int volNum)
	: VolumeResourceSource(name, map, volNum, kSourceAudioVolume) {

	_audioCompressionType = 0;
	_audioCompressionOffsetMapping = NULL;

	/*
	 * Check if this audio volume got compressed by our tool. If that is the
	 * case, set _audioCompressionType and read in the offset translation
	 * table for later usage.
	 */

	Common::SeekableReadStream *fileStream = getVolumeFile(resMan, 0);
	if (!fileStream)
		return;

	fileStream->seek(0, SEEK_SET);
	uint32 compressionType = fileStream->readUint32BE();
	switch (compressionType) {
	case MKTAG('M','P','3',' '):
	case MKTAG('O','G','G',' '):
	case MKTAG('F','L','A','C'):
		// Detected a compressed audio volume
		_audioCompressionType = compressionType;
		// Now read the whole offset mapping table for later usage
		int32 recordCount = fileStream->readUint32LE();
		if (!recordCount)
			error("compressed audio volume doesn't contain any entries");
		int32 *offsetMapping = new int32[(recordCount + 1) * 2];
		_audioCompressionOffsetMapping = offsetMapping;
		for (int recordNo = 0; recordNo < recordCount; recordNo++) {
			*offsetMapping++ = fileStream->readUint32LE();
			*offsetMapping++ = fileStream->readUint32LE();
		}
		// Put ending zero
		*offsetMapping++ = 0;
		*offsetMapping++ = fileStream->size();
	}

	if (_resourceFile)
		delete fileStream;
}

AudioVolumeResourceSource::~AudioVolumeResourceSource() {
	delete[] _audioCompressionOffsetMapping;
}

bool Resource::loadFromWaveFile(Common::SeekableReadStream *file) {
	byte *ptr = new byte[_size];
	_data = ptr;

	uint32 bytesRead = file->read(ptr, _size);
	if (bytesRead != _size)
		error("Read %d bytes from %s but expected %u", bytesRead, _id.toString().c_str(), _size);

	_status = kResStatusAllocated;
	return true;
}

bool Resource::loadFromAudioVolumeSCI11(Common::SeekableReadStream *file) {
	// Check for WAVE files here
	uint32 riffTag = file->readUint32BE();
	if (riffTag == MKTAG('R','I','F','F')) {
		_headerSize = 0;
		_size = file->readUint32LE() + 8;
		file->seek(-8, SEEK_CUR);
		return loadFromWaveFile(file);
	}
	file->seek(-4, SEEK_CUR);

	// Rave-resources (King's Quest 6) don't have any header at all
	if (getType() != kResourceTypeRave) {
		ResourceType type = _resMan->convertResType(file->readByte());
		if (((getType() == kResourceTypeAudio || getType() == kResourceTypeAudio36) && (type != kResourceTypeAudio))
			|| ((getType() == kResourceTypeSync || getType() == kResourceTypeSync36) && (type != kResourceTypeSync))) {
			warning("Resource type mismatch loading %s", _id.toString().c_str());
			unalloc();
			return false;
		}

		_headerSize = file->readByte();

		if (type == kResourceTypeAudio) {
			if (_headerSize != 7 && _headerSize != 11 && _headerSize != 12) {
				warning("Unsupported audio header");
				unalloc();
				return false;
			}

			if (_headerSize != 7) { // Size is defined already from the map
				// Load sample size
				file->seek(7, SEEK_CUR);
				_size = file->readUint32LE();
				assert(!file->err() && !file->eos());
				// Adjust offset to point at the header data again
				file->seek(-11, SEEK_CUR);
			}
		}
	}
	return loadPatch(file);
}

bool Resource::loadFromAudioVolumeSCI1(Common::SeekableReadStream *file) {
	byte *ptr = new byte[size()];
	_data = ptr;

	if (!ptr) {
		error("Can't allocate %u bytes needed for loading %s", _size, _id.toString().c_str());
	}

	uint32 bytesRead = file->read(ptr, size());
	if (bytesRead != size())
		warning("Read %d bytes from %s but expected %u", bytesRead, _id.toString().c_str(), _size);

	_status = kResStatusAllocated;
	return true;
}

void ResourceManager::addNewGMPatch(SciGameId gameId) {
	Common::String gmPatchFile;

	switch (gameId) {
	case GID_ECOQUEST:
		gmPatchFile = "ECO1GM.PAT";
		break;
	case GID_HOYLE3:
		gmPatchFile = "HOY3GM.PAT";
		break;
	case GID_LSL1:
		gmPatchFile = "LL1_GM.PAT";
		break;
	case GID_LSL5:
		gmPatchFile = "LL5_GM.PAT";
		break;
	case GID_LONGBOW:
		gmPatchFile = "ROBNGM.PAT";
		break;
	case GID_SQ1:
		gmPatchFile = "SQ1_GM.PAT";
		break;
	case GID_SQ4:
		gmPatchFile = "SQ4_GM.PAT";
		break;
	case GID_FAIRYTALES:
		gmPatchFile = "TALEGM.PAT";
		break;
	default:
		break;
	}

	if (!gmPatchFile.empty() && Common::File::exists(gmPatchFile)) {
		ResourceSource *psrcPatch = new PatchResourceSource(gmPatchFile);
		processPatch(psrcPatch, kResourceTypePatch, 4);
	}
}

void ResourceManager::processWavePatch(ResourceId resourceId, Common::String name) {
	ResourceSource *resSrc = new WaveResourceSource(name);
	Common::File file;
	file.open(name);

	updateResource(resourceId, resSrc, file.size());
	_sources.push_back(resSrc);

	debugC(1, kDebugLevelResMan, "Patching %s - OK", name.c_str());
}

void ResourceManager::readWaveAudioPatches() {
	// Here we do check for SCI1.1+ so we can patch wav files in as audio resources
	Common::ArchiveMemberList files;
	SearchMan.listMatchingMembers(files, "*.wav");

	for (Common::ArchiveMemberList::const_iterator x = files.begin(); x != files.end(); ++x) {
		Common::String name = (*x)->getName();

		if (Common::isDigit(name[0]))
			processWavePatch(ResourceId(kResourceTypeAudio, atoi(name.c_str())), name);
	}
}

void ResourceManager::removeAudioResource(ResourceId resId) {
	// Remove resource, unless it was loaded from a patch
	if (_resMap.contains(resId)) {
		Resource *res = _resMap.getVal(resId);

		if (res->_source->getSourceType() == kSourceAudioVolume) {
			if (res->_status == kResStatusLocked) {
				warning("Failed to remove resource %s (still in use)", resId.toString().c_str());
			} else {
				if (res->_status == kResStatusEnqueued)
					removeFromLRU(res);

				_resMap.erase(resId);
				delete res;
			}
		}
	}
}

// Early SCI1.1 65535.MAP structure (uses RESOURCE.AUD):
// =========
// 6-byte entries:
// w nEntry
// dw offset

// Late SCI1.1 65535.MAP structure (uses RESOURCE.SFX):
// =========
// 5-byte entries:
// w nEntry
// tb offset (cumulative)

// QFG3 Demo 0.MAP structure:
// =========
// 10-byte entries:
// w nEntry
// dw offset
// dw size

// LB2 Floppy/Mother Goose SCI1.1 0.MAP structure:
// =========
// 8-byte entries:
// w nEntry
// w 0xffff
// dw offset

// Early SCI1.1 MAP structure:
// ===============
// 10-byte entries:
// b noun
// b verb
// b cond
// b seq
// dw offset
// w syncSize + syncAscSize

// Late SCI1.1 MAP structure:
// ===============
// Header:
// dw baseOffset
// Followed by 7 or 11-byte entries:
// b noun
// b verb
// b cond
// b seq
// tb cOffset (cumulative offset)
// w syncSize (iff seq has bit 7 set)
// w syncAscSize (iff seq has bit 6 set)

int ResourceManager::readAudioMapSCI11(IntMapResourceSource *map) {
#ifndef ENABLE_SCI32
	// SCI32 support is not built in. Check if this is a SCI32 game
	// and if it is abort here.
	if (_volVersion >= kResVersionSci2)
		return SCI_ERROR_RESMAP_NOT_FOUND;
#endif

	uint32 offset = 0;
	Resource *mapRes = findResource(ResourceId(kResourceTypeMap, map->_mapNumber), false);

	if (!mapRes) {
		warning("Failed to open %i.MAP", map->_mapNumber);
		return SCI_ERROR_RESMAP_NOT_FOUND;
	}

	ResourceSource *src = findVolume(map, map->_volumeNumber);

	if (!src) {
		warning("Failed to find volume for %i.MAP", map->_mapNumber);
		return SCI_ERROR_NO_RESOURCE_FILES_FOUND;
	}

	const uint32 srcSize = getVolumeFile(src)->size();

	SciSpan<const byte>::const_iterator ptr = mapRes->cbegin();

	// Heuristic to detect entry size
	uint32 entrySize = 0;
	for (int i = mapRes->size() - 1; i >= 0; --i) {
		if (ptr[i] == 0xff)
			entrySize++;
		else
			break;
	}

	if (map->_mapNumber == 65535) {
		while (ptr != mapRes->cend()) {
			uint16 n = ptr.getUint16LE();
			ptr += 2;

			if (n == 0xffff)
				break;

			if (entrySize == 6) {
				offset = ptr.getUint32LE();
				ptr += 4;
			} else {
				offset += ptr.getUint24LE();
				ptr += 3;
			}

			assert(offset < srcSize);
			addResource(ResourceId(kResourceTypeAudio, n), src, offset);
		}
	} else if (map->_mapNumber == 0 && entrySize == 10 && ptr[3] == 0) {
		// QFG3 demo format
		// ptr[3] would be 'seq' in the normal format and cannot possibly be 0
		while (ptr != mapRes->cend()) {
			uint16 n = ptr.getUint16BE();
			ptr += 2;

			if (n == 0xffff)
				break;

			offset = ptr.getUint32LE();
			ptr += 4;
			uint32 size = ptr.getUint32LE();
			ptr += 4;

			assert(offset + size <= srcSize);
			addResource(ResourceId(kResourceTypeAudio, n), src, offset, size);
		}
	} else if (map->_mapNumber == 0 && entrySize == 8 && (ptr + 2).getUint16LE() == 0xffff) {
		// LB2 Floppy/Mother Goose SCI1.1 format
		Common::SeekableReadStream *stream = getVolumeFile(src);

		while (ptr != mapRes->cend()) {
			uint16 n = ptr.getUint16LE();
			ptr += 4;

			if (n == 0xffff)
				break;

			offset = ptr.getUint32LE();
			ptr += 4;

			// The size is not stored in the map and the entries have no order.
			// We need to dig into the audio resource in the volume to get the size.
			stream->seek(offset + 1);
			byte headerSize = stream->readByte();
			assert(headerSize == 11 || headerSize == 12);

			stream->skip(7);
			uint32 size = stream->readUint32LE() + headerSize + 2;

			assert(offset + size <= srcSize);
			addResource(ResourceId(kResourceTypeAudio, n), src, offset, size);
		}
	} else {
		bool isEarly = (entrySize != 11);

		if (!isEarly) {
			offset = ptr.getUint32LE();
			ptr += 4;
		}

		while (ptr != mapRes->cend()) {
			uint32 n = ptr.getUint32BE();
			int syncSize = 0;
			ptr += 4;

			if (n == 0xffffffff)
				break;

			if (isEarly) {
				offset = ptr.getUint32LE();
				ptr += 4;
			} else {
				offset += ptr.getUint24LE();
				ptr += 3;
			}

			if (isEarly || (n & 0x80)) {
				syncSize = ptr.getUint16LE();
				ptr += 2;

				// FIXME: The sync36 resource seems to be two bytes too big in KQ6CD
				// (bytes taken from the RAVE resource right after it)
				if (syncSize > 0) {
					assert(offset + syncSize <= srcSize);
					addResource(ResourceId(kResourceTypeSync36, map->_mapNumber, n & 0xffffff3f), src, offset, syncSize);
				}
			}

			// Checking for this 0x40 flag breaks at least Laura Bow 2 CD 1.1
			// map 448
			if (g_sci->getGameId() == GID_KQ6 && (n & 0x40)) {
				// This seems to define the size of raw lipsync data (at least
				// in KQ6 CD Windows).
				int kq6HiresSyncSize = ptr.getUint16LE();
				ptr += 2;

				if (kq6HiresSyncSize > 0) {
					assert(offset + syncSize + kq6HiresSyncSize <= srcSize);
					addResource(ResourceId(kResourceTypeRave, map->_mapNumber, n & 0xffffff3f), src, offset + syncSize, kq6HiresSyncSize);
					syncSize += kq6HiresSyncSize;
				}
			}

			const ResourceId id = ResourceId(kResourceTypeAudio36, map->_mapNumber, n & 0xffffff3f);

			// At least version 1.00 of GK2 has multiple invalid audio36 map
			// entries on CD 6
			if (g_sci->getGameId() == GID_GK2 &&
				map->_volumeNumber == 6 &&
				offset + syncSize >= srcSize) {

				debugC(kDebugLevelResMan, "Invalid offset %u for %s in map %d for disc %d", offset + syncSize, id.toPatchNameBase36().c_str(), map->_mapNumber, map->_volumeNumber);
				continue;
			}

			assert(offset + syncSize < srcSize);
			addResource(id, src, offset + syncSize);
		}
	}

	return 0;
}

// AUDIOnnn.MAP contains 10-byte entries:
// Early format:
// w 5 bits resource type and 11 bits resource number
// dw 7 bits volume number and 25 bits offset
// dw size
// Later format:
// w nEntry
// dw offset+volume (as in resource.map)
// dw size
// ending with 10 0xFFs
int ResourceManager::readAudioMapSCI1(ResourceSource *map, bool unload) {
	Common::File file;

	if (!file.open(map->getLocationName()))
		return SCI_ERROR_RESMAP_NOT_FOUND;

	bool oldFormat = (file.readUint16LE() >> 11) == kResourceTypeAudio;
	file.seek(0);

	uint32 volumeSize;
	int lastVolumeNo = -1;

	for (;;) {
		uint16 n = file.readUint16LE();
		uint32 offset = file.readUint32LE();
		uint32 size = file.readUint32LE();

		if (file.eos() || file.err()) {
			warning("Error while reading %s", map->getLocationName().c_str());
			return SCI_ERROR_RESMAP_NOT_FOUND;
		}

		if (n == 0xffff)
			break;

		byte volume_nr;

		if (oldFormat) {
			n &= 0x07ff; // Mask out resource type
			volume_nr = offset >> 25; // most significant 7 bits
			offset &= 0x01ffffff; // least significant 25 bits
		} else {
			volume_nr = offset >> 28; // most significant 4 bits
			offset &= 0x0fffffff; // least significant 28 bits
		}

		ResourceSource *src = findVolume(map, volume_nr);

		if (src) {
			if (volume_nr != lastVolumeNo) {
				volumeSize = getVolumeFile(src)->size();
				lastVolumeNo = volume_nr;
			}

			if (unload)
				removeAudioResource(ResourceId(kResourceTypeAudio, n));
			else {
				assert(offset + size <= volumeSize);
				addResource(ResourceId(kResourceTypeAudio, n), src, offset, size);
			}
		} else {
			warning("Failed to find audio volume %i", volume_nr);
		}
	}

	return 0;
}

void ResourceManager::setAudioLanguage(int language) {
	if (_audioMapSCI1) {
		if (_audioMapSCI1->_volumeNumber == language) {
			// This language is already loaded
			return;
		}

		// We already have a map loaded, so we unload it first
		readAudioMapSCI1(_audioMapSCI1, true);

		// Remove all volumes that use this map from the source list
		Common::List<ResourceSource *>::iterator it = _sources.begin();
		while (it != _sources.end()) {
			ResourceSource *src = *it;
			if (src->findVolume(_audioMapSCI1, src->_volumeNumber)) {
				it = _sources.erase(it);
				delete src;
			} else {
				++it;
			}
		}

		// Remove the map itself from the source list
		_sources.remove(_audioMapSCI1);
		delete _audioMapSCI1;

		_audioMapSCI1 = NULL;
	}

	Common::String filename = Common::String::format("AUDIO%03d", language);
	Common::String fullname = filename + ".MAP";
	if (!Common::File::exists(fullname)) {
		warning("No audio map found for language %i", language);
		return;
	}

	_audioMapSCI1 = addSource(new ExtAudioMapResourceSource(fullname, language));

	// Search for audio volumes for this language and add them to the source list
	Common::ArchiveMemberList files;
	SearchMan.listMatchingMembers(files, filename + ".0??");
	for (Common::ArchiveMemberList::const_iterator x = files.begin(); x != files.end(); ++x) {
		const Common::String name = (*x)->getName();
		const char *dot = strrchr(name.c_str(), '.');
		int number = atoi(dot + 1);

		addSource(new AudioVolumeResourceSource(this, name, _audioMapSCI1, number));
	}

	scanNewSources();
}

int ResourceManager::getAudioLanguage() const {
	return (_audioMapSCI1 ? _audioMapSCI1->_volumeNumber : 0);
}

bool ResourceManager::isGMTrackIncluded() {
	// This check only makes sense for SCI1 and newer games
	if (getSciVersion() < SCI_VERSION_1_EARLY)
		return false;

	// SCI2 and newer games always have GM tracks
	if (getSciVersion() >= SCI_VERSION_2)
		return true;

	// For the leftover games, we can safely use SCI_VERSION_1_EARLY for the soundVersion
	const SciVersion soundVersion = SCI_VERSION_1_EARLY;

	// Read the first song and check if it has a GM track
	bool result = false;
	Common::List<ResourceId> resources = listResources(kResourceTypeSound, -1);
	Common::sort(resources.begin(), resources.end());
	Common::List<ResourceId>::iterator itr = resources.begin();
	int firstSongId = itr->getNumber();

	SoundResource *song1 = new SoundResource(firstSongId, this, soundVersion);
	if (!song1) {
		warning("ResourceManager::isGMTrackIncluded: track 1 not found");
		return false;
	}

	SoundResource::Track *gmTrack = song1->getTrackByType(0x07);
	if (gmTrack)
		result = true;

	delete song1;

	return result;
}

SoundResource::SoundResource(uint32 resourceNr, ResourceManager *resMan, SciVersion soundVersion) : _resMan(resMan), _soundVersion(soundVersion) {
	Resource *resource = _resMan->findResource(ResourceId(kResourceTypeSound, resourceNr), true);
	int trackNr, channelNr;
	if (!resource)
		return;

	_innerResource = resource;
	_soundPriority = 0xFF;

	Channel *channel, *sampleChannel;

	switch (_soundVersion) {
	case SCI_VERSION_0_EARLY:
	case SCI_VERSION_0_LATE:
		// SCI0 only has a header of 0x11/0x21 byte length and the actual midi track follows afterwards
		_trackCount = 1;
		_tracks = new Track[_trackCount];
		_tracks->digitalChannelNr = -1;
		_tracks->type = 0; // Not used for SCI0
		_tracks->channelCount = 1;
		// Digital sample data included? -> Add an additional channel
		if (resource->getUint8At(0) == 2)
			_tracks->channelCount++;
		_tracks->channels = new Channel[_tracks->channelCount];
		channel = &_tracks->channels[0];
		channel->flags |= 2; // don't remap (SCI0 doesn't have remapping)
		if (_soundVersion == SCI_VERSION_0_EARLY) {
			channel->data = resource->subspan(0x11);
		} else {
			channel->data = resource->subspan(0x21);
		}
		if (_tracks->channelCount == 2) {
			// Digital sample data included
			_tracks->digitalChannelNr = 1;
			sampleChannel = &_tracks->channels[1];
			// we need to find 0xFC (channel terminator) within the data
			SciSpan<const byte>::const_iterator it = channel->data.cbegin();
			while (it != channel->data.cend() && *it != 0xfc)
				it++;
			// Skip any following 0xFCs as well
			while (it != channel->data.cend() && *it == 0xfc)
				it++;
			// Now adjust channels accordingly
			sampleChannel->data = channel->data.subspan(it - channel->data.cbegin());
			channel->data = channel->data.subspan(0, it - channel->data.cbegin());
			// Read sample header information
			//Offset 14 in the header contains the frequency as a short integer. Offset 32 contains the sample length, also as a short integer.
			_tracks->digitalSampleRate = sampleChannel->data.getUint16LEAt(14);
			_tracks->digitalSampleSize = sampleChannel->data.getUint16LEAt(32);
			_tracks->digitalSampleStart = 0;
			_tracks->digitalSampleEnd = 0;
			sampleChannel->data += 44; // Skip over header
		}
		break;

	case SCI_VERSION_1_EARLY:
	case SCI_VERSION_1_LATE:
	case SCI_VERSION_2_1_EARLY: {
		SciSpan<const byte> data = *resource;
		// Count # of tracks
		_trackCount = 0;
		while ((*data++) != 0xFF) {
			_trackCount++;
			while (*data != 0xFF)
				data += 6;
			++data;
		}
		_tracks = new Track[_trackCount];
		data = *resource;

		byte channelCount;

		for (trackNr = 0; trackNr < _trackCount; trackNr++) {
			// Track info starts with track type:BYTE
			// Then the channel information gets appended Unknown:WORD, ChannelOffset:WORD, ChannelSize:WORD
			// 0xFF:BYTE as terminator to end that track and begin with another track type
			// Track type 0xFF is the marker signifying the end of the tracks

			_tracks[trackNr].type = *data++;
			// Counting # of channels used
			SciSpan<const byte> data2 = data;
			channelCount = 0;
			while (*data2 != 0xFF) {
				data2 += 6;
				channelCount++;
				_tracks[trackNr].channelCount++;
			}
			_tracks[trackNr].channels = new Channel[channelCount];
			_tracks[trackNr].channelCount = 0;
			_tracks[trackNr].digitalChannelNr = -1; // No digital sound associated
			_tracks[trackNr].digitalSampleRate = 0;
			_tracks[trackNr].digitalSampleSize = 0;
			_tracks[trackNr].digitalSampleStart = 0;
			_tracks[trackNr].digitalSampleEnd = 0;
			if (_tracks[trackNr].type != 0xF0) { // Digital track marker - not supported currently
				channelNr = 0;
				while (channelCount--) {
					channel = &_tracks[trackNr].channels[channelNr];
					const uint16 dataOffset = data.getUint16LEAt(2);

					if (dataOffset >= resource->size()) {
						warning("Invalid offset inside sound resource %d: track %d, channel %d", resourceNr, trackNr, channelNr);
						data += 6;
						continue;
					}

					uint16 size = data.getUint16LEAt(4);

					if (dataOffset + size > resource->size()) {
						warning("Invalid size inside sound resource %d: track %d, channel %d", resourceNr, trackNr, channelNr);
						size = resource->size() - dataOffset;
					}

					channel->data = resource->subspan(dataOffset, size);

					channel->curPos = 0;
					channel->number = channel->data[0];

					channel->poly = channel->data[1] & 0x0F;
					channel->prio = channel->data[1] >> 4;
					channel->time = channel->prev = 0;
					channel->data += 2; // skip over header
					if (channel->number == 0xFE) { // Digital channel
						_tracks[trackNr].digitalChannelNr = channelNr;
						_tracks[trackNr].digitalSampleRate = channel->data.getUint16LEAt(0);
						_tracks[trackNr].digitalSampleSize = channel->data.getUint16LEAt(2);
						_tracks[trackNr].digitalSampleStart = channel->data.getUint16LEAt(4);
						_tracks[trackNr].digitalSampleEnd = channel->data.getUint16LEAt(6);
						channel->data += 8; // Skip over header
						channel->flags = 0;
					} else {
						channel->flags = channel->number >> 4;
						channel->number = channel->number & 0x0F;

						// 0x20 is set on rhythm channels to prevent remapping
						// CHECKME: Which SCI versions need that set manually?
						if (channel->number == 9)
							channel->flags |= 2;
						// Note: flag 1: channel start offset is 0 instead of 10
						//               (currently: everything 0)
						//               also: don't map the channel to device
						//       flag 2: don't remap
						//       flag 4: start muted
						// QfG2 lacks flags 2 and 4, and uses (flags >= 1) as
						// the condition for starting offset 0, without the "don't map"
					}
					_tracks[trackNr].channelCount++;
					channelNr++;
					data += 6;
				}
			} else {
				// The first byte of the 0xF0 track's channel list is priority
				_soundPriority = *data;

				// Skip over digital track
				data += 6;
			}
			++data; // Skipping 0xFF that closes channels list
		}
		break;
	}

	default:
		error("SoundResource: SCI version %d is unsupported", _soundVersion);
	}
}

SoundResource::~SoundResource() {
	for (int trackNr = 0; trackNr < _trackCount; trackNr++)
		delete[] _tracks[trackNr].channels;
	delete[] _tracks;

	_resMan->unlockResource(_innerResource);
}

#if 0
SoundResource::Track* SoundResource::getTrackByNumber(uint16 number) {
	if (_soundVersion <= SCI_VERSION_0_LATE)
		return &_tracks[0];

	if (/*number >= 0 &&*/number < _trackCount)
		return &_tracks[number];
	return NULL;
}
#endif

SoundResource::Track *SoundResource::getTrackByType(byte type) {
	if (_soundVersion <= SCI_VERSION_0_LATE)
		return &_tracks[0];

	for (int trackNr = 0; trackNr < _trackCount; trackNr++) {
		if (_tracks[trackNr].type == type)
			return &_tracks[trackNr];
	}
	return NULL;
}

SoundResource::Track *SoundResource::getDigitalTrack() {
	for (int trackNr = 0; trackNr < _trackCount; trackNr++) {
		if (_tracks[trackNr].digitalChannelNr != -1)
			return &_tracks[trackNr];
	}
	return NULL;
}

// Gets the filter mask for SCI0 sound resources
int SoundResource::getChannelFilterMask(int hardwareMask, bool wantsRhythm) {
	SciSpan<const byte> data = *_innerResource;
	int channelMask = 0;

	if (_soundVersion > SCI_VERSION_0_LATE)
		return 0;

	++data; // Skip over digital sample flag

	for (int channelNr = 0; channelNr < 16; channelNr++) {
		channelMask = channelMask >> 1;

		byte flags;

		if (_soundVersion == SCI_VERSION_0_EARLY) {
			// Each channel is specified by a single byte
			// Upper 4 bits of the byte is a voices count
			// Lower 4 bits -> bit 0 set: use for AdLib
			//				   bit 1 set: use for PCjr
			//				   bit 2 set: use for PC speaker
			//				   bit 3 set and bit 0 clear: control channel (15)
			//				   bit 3 set and bit 0 set: rhythm channel (9)
			// Note: control channel is dynamically assigned inside the drivers,
			// but seems to be fixed at 15 in the song data.
			flags = *data++;

			// Get device bits
			flags &= 0x7;
		} else {
			// Each channel is specified by 2 bytes
			// 1st byte is voices count
			// 2nd byte is play mask, which specifies if the channel is supposed to be played
			// by the corresponding hardware

			// Skip voice count
			++data;

			flags = *data++;
		}

		bool play;
		switch (channelNr) {
		case 15:
			// Always play control channel
			play = true;
			break;
		case 9:
			// Play rhythm channel when requested
			play = wantsRhythm;
			break;
		default:
			// Otherwise check for flag
			play = flags & hardwareMask;
		}

		if (play) {
			// This Channel is supposed to be played by the hardware
			channelMask |= 0x8000;
		}
	}

	return channelMask;
}

byte SoundResource::getInitialVoiceCount(byte channel) {
	if (_soundVersion > SCI_VERSION_0_LATE)
		return 0; // TODO

	// Skip over digital sample flag
	SciSpan<const byte> data = _innerResource->subspan(1);

	if (_soundVersion == SCI_VERSION_0_EARLY)
		return data[channel] >> 4;
	else
		return data[channel * 2];
}

void WaveResourceSource::loadResource(ResourceManager *resMan, Resource *res) {
	Common::SeekableReadStream *fileStream = getVolumeFile(resMan, res);
	if (!fileStream)
		return;

	assert(fileStream->size() == -1 || res->_fileOffset < fileStream->size());
	fileStream->seek(res->_fileOffset, SEEK_SET);
	res->loadFromWaveFile(fileStream);
	if (_resourceFile)
		delete fileStream;
}

void AudioVolumeResourceSource::loadResource(ResourceManager *resMan, Resource *res) {
	Common::SeekableReadStream *fileStream = getVolumeFile(resMan, res);
	if (!fileStream)
		return;

	if (_audioCompressionType) {
		// this file is compressed, so lookup our offset in the offset-translation table and get the new offset
		//  also calculate the compressed size by using the next offset
		int32 *mappingTable = _audioCompressionOffsetMapping;
		int32 compressedOffset = 0;

		do {
			if (*mappingTable == res->_fileOffset) {
				mappingTable++;
				compressedOffset = *mappingTable;
				// Go to next compressed offset and use that to calculate size of compressed sample
				switch (res->getType()) {
				case kResourceTypeSync:
				case kResourceTypeSync36:
				case kResourceTypeRave:
					// we should already have a (valid) size
					break;
				default:
					mappingTable += 2;
					res->_size = *mappingTable - compressedOffset;
				}
				break;
			}
			mappingTable += 2;
		} while (*mappingTable);

		if (!compressedOffset)
			error("could not translate offset to compressed offset in audio volume");
		fileStream->seek(compressedOffset, SEEK_SET);

		switch (res->getType()) {
		case kResourceTypeAudio:
		case kResourceTypeAudio36:
			// Directly read the stream, compressed audio wont have resource type id and header size for SCI1.1
			res->loadFromAudioVolumeSCI1(fileStream);
			if (_resourceFile)
				delete fileStream;
			return;
		default:
			break;
		}
	} else {
		assert(fileStream->size() == -1 || res->_fileOffset < fileStream->size());
		// original file, directly seek to given offset and get SCI1/SCI1.1 audio resource
		fileStream->seek(res->_fileOffset, SEEK_SET);
	}
	if (getSciVersion() < SCI_VERSION_1_1)
		res->loadFromAudioVolumeSCI1(fileStream);
	else
		res->loadFromAudioVolumeSCI11(fileStream);

	if (_resourceFile)
		delete fileStream;
}

bool ResourceManager::addAudioSources() {
#ifdef ENABLE_SCI32
	// Multi-disc audio is added during addAppropriateSources for those titles
	// that require it
	if (_multiDiscAudio) {
		return true;
	}
#endif

	Common::List<ResourceId> resources = listResources(kResourceTypeMap);
	Common::List<ResourceId>::iterator itr;

	for (itr = resources.begin(); itr != resources.end(); ++itr) {
		ResourceSource *src = addSource(new IntMapResourceSource("MAP", 0, itr->getNumber()));

		if (itr->getNumber() == 65535 && Common::File::exists("RESOURCE.SFX"))
			addSource(new AudioVolumeResourceSource(this, "RESOURCE.SFX", src, 0));
		else if (Common::File::exists("RESOURCE.AUD"))
			addSource(new AudioVolumeResourceSource(this, "RESOURCE.AUD", src, 0));
		else
			return false;
	}

	return true;
}

void ResourceManager::changeAudioDirectory(const Common::String &path) {
	// Resources must be cleared before ResourceSources because the destructor
	// of a Resource accesses its own ResourceSource
	ResourceMap::iterator resIt = _resMap.begin();
	while (resIt != _resMap.end()) {
		Resource *resource = resIt->_value;
		ResourceType type = resource->getType();
		if (type == kResourceTypeMap ||
			type == kResourceTypeAudio36 ||
			type == kResourceTypeSync36) {

			if (type == kResourceTypeMap && resource->getNumber() == 65535) {
				++resIt;
				continue;
			}

			if (resource->_status == kResStatusLocked) {
				resource->_lockers = 1;
				unlockResource(resource);
			}
			if (resource->_status == kResStatusEnqueued) {
				removeFromLRU(resource);
			}
			delete resource;
			_resMap.erase(resIt);
		}

		++resIt;
	}

	Common::List<ResourceSource *>::iterator sourceIt = _sources.begin();
	while (sourceIt != _sources.end()) {
		ResourceSource *source = *sourceIt;
		ResSourceType sourceType = source->getSourceType();
		if ((sourceType == kSourceIntMap && source->_volumeNumber != 65535) ||
			(sourceType == kSourceAudioVolume && source->getLocationName() != "RESOURCE.SFX")) {

			sourceIt = _sources.erase(sourceIt);
			delete source;
		} else {
			++sourceIt;
		}
	}

	Common::String mapName = "MAP";
	Common::String audioResourceName = "RESOURCE.AUD";
	if (!path.empty()) {
		mapName = Common::String::format("%s/MAP", path.c_str());
		audioResourceName = Common::String::format("%s/RESOURCE.AUD", path.c_str());
	}

	Common::List<ResourceId> resources = listResources(kResourceTypeMap);
	Common::List<ResourceId>::iterator it;
	for (it = resources.begin(); it != resources.end(); ++it) {
		// Don't readd 65535.map or resource.sfx
		if (it->getNumber() == 65535)
			continue;

		ResourceSource *src = addSource(new IntMapResourceSource(mapName, 0, it->getNumber()));
		addSource(new AudioVolumeResourceSource(this, audioResourceName, src, 0));
	}

	scanNewSources();
}

} // End of namespace Sci
