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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/system.h"
#include "common/random.h"
#include "common/debug-channels.h"
#include "common/config-manager.h"
#include "common/memstream.h"
#include "common/compression/installshield_cab.h"
#include "common/serializer.h"

#include "engines/nancy/nancy.h"
#include "engines/nancy/resource.h"
#include "engines/nancy/iff.h"
#include "engines/nancy/input.h"
#include "engines/nancy/sound.h"
#include "engines/nancy/graphics.h"
#include "engines/nancy/console.h"
#include "engines/nancy/util.h"

#include "engines/nancy/action/conversation.h"

#include "engines/nancy/state/logo.h"
#include "engines/nancy/state/scene.h"
#include "engines/nancy/state/help.h"
#include "engines/nancy/state/map.h"
#include "engines/nancy/state/credits.h"
#include "engines/nancy/state/mainmenu.h"
#include "engines/nancy/state/setupmenu.h"
#include "engines/nancy/state/loadsave.h"
#include "engines/nancy/state/savedialog.h"

namespace Nancy {

NancyEngine *g_nancy;

NancyEngine::NancyEngine(OSystem *syst, const NancyGameDescription *gd) :
		Engine(syst),
		_gameDescription(gd),
		_system(syst),
		_datFileMajorVersion(1),
		_datFileMinorVersion(0),
		_false(gd->gameType <= kGameTypeNancy2 ? 1 : 0),
		_true(gd->gameType <= kGameTypeNancy2 ? 2 : 1) {

	g_nancy = this;

	_randomSource = new Common::RandomSource("Nancy");
	_randomSource->setSeed(Common::RandomSource::generateNewSeed());

	_input = new InputManager();
	_sound = new SoundManager();
	_graphicsManager = new GraphicsManager();
	_cursorManager = new CursorManager();

	_resource = nullptr;

	_hasJustSaved = false;
}

NancyEngine::~NancyEngine() {
	destroyState(NancyState::kLogo);
	destroyState(NancyState::kCredits);
	destroyState(NancyState::kMap);
	destroyState(NancyState::kHelp);
	destroyState(NancyState::kScene);
	destroyState(NancyState::kMainMenu);
	destroyState(NancyState::kSetup);
	destroyState(NancyState::kLoadSave);
	destroyState(NancyState::kSaveDialog);

	delete _randomSource;

	delete _graphicsManager;
	delete _cursorManager;
	delete _input;
	delete _sound;

	for (auto &data : _engineData) {
		delete data._value;
	}
}

NancyEngine *NancyEngine::create(GameType type, OSystem *syst, const NancyGameDescription *gd) {
	if (type >= kGameTypeVampire && type <= kGameTypeNancy9) {
		return new NancyEngine(syst, gd);
	}

	error("Unknown GameType");
}

Common::Error NancyEngine::loadGameStream(Common::SeekableReadStream *stream) {
	Common::Serializer ser(stream, nullptr);
	return synchronize(ser);
}

Common::Error NancyEngine::saveGameStream(Common::WriteStream *stream, bool isAutosave) {
	Common::Serializer ser(nullptr, stream);

	return synchronize(ser);
}

bool NancyEngine::canLoadGameStateCurrently() {
	return canSaveGameStateCurrently();
}

bool NancyEngine::canSaveGameStateCurrently() {
	// TODO also disable during secondary movie
	return State::Scene::hasInstance() &&
			NancySceneState._state == State::Scene::kRun &&
			NancySceneState.getActiveConversation() == nullptr;
}

void NancyEngine::secondChance() {
	uint secondChanceSlot = getMetaEngine()->getMaximumSaveSlot();
	saveGameState(secondChanceSlot, "SECOND CHANCE", true);
}

void NancyEngine::errorString(const char *buf_input, char *buf_output, int buf_output_size) {
	if (State::Scene::hasInstance()) {
		if (NancySceneState._state == State::Scene::kLoad) {
			// Error while loading scene
			snprintf(buf_output, buf_output_size, "While loading scene S%u, frame %u, action record %u:\n%s",
				NancySceneState._sceneState.currentScene.sceneID,
				NancySceneState._sceneState.currentScene.frameID,
				NancySceneState._actionManager.getActionRecords().size(),
				buf_input);
		} else {
			// Error while running
			snprintf(buf_output, buf_output_size, "In current scene S%u, frame %u:\n%s",
				NancySceneState._sceneState.currentScene.sceneID,
				NancySceneState._sceneState.currentScene.frameID,
				buf_input);
		}
	} else {
		strncpy(buf_output, buf_input, buf_output_size);
		if (buf_output_size > 0)
			buf_output[buf_output_size - 1] = '\0';
	}
}

bool NancyEngine::hasFeature(EngineFeature f) const {
	return  (f == kSupportsReturnToLauncher) ||
			(f == kSupportsLoadingDuringRuntime) ||
			(f == kSupportsSavingDuringRuntime) ||
			(f == kSupportsChangingOptionsDuringRuntime) ||
			(f == kSupportsSubtitleOptions);
}

const char *NancyEngine::getCopyrightString() const {
	return "Copyright 1989-1997 David P Gray, All Rights Reserved.";
}

uint32 NancyEngine::getGameFlags() const {
	return _gameDescription->desc.flags;
}

const char *NancyEngine::getGameId() const {
	return _gameDescription->desc.gameId;
}

GameType NancyEngine::getGameType() const {
	return _gameDescription->gameType;
}

Common::Platform NancyEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

const StaticData &NancyEngine::getStaticData() const {
	return _staticData;
}

const EngineData *NancyEngine::getEngineData(const Common::String &name) const {
	if (_engineData.contains(name)) {
		return _engineData[name];
	}

	return nullptr;
}

void NancyEngine::setState(NancyState::NancyState state, NancyState::NancyState overridePrevious) {
	// Handle special cases first
	switch (state) {
	case NancyState::kBoot:
		bootGameEngine();
		setState(NancyState::kLogo);
		return;
	case NancyState::kMainMenu: {
		if (!ConfMan.hasKey("original_menus") || ConfMan.getBool("original_menus")) {
			break;
		}

		// Do not use the original engine's menus, call the GMM instead
		openMainMenuDialog();

		if (shouldQuit()) {
			return;
		}

		_input->forceCleanInput();

		return;
	}
	default:
		break;
	}

	if (overridePrevious != NancyState::kNone) {
		_gameFlow.prevState = overridePrevious;
	} else {
		_gameFlow.prevState = _gameFlow.curState;
	}

	_gameFlow.curState = state;
	_gameFlow.changingState = true;
}

void NancyEngine::setToPreviousState() {
	setState(_gameFlow.prevState);
}

void NancyEngine::setMouseEnabled(bool enabled) {
	_cursorManager->showCursor(enabled); _input->setMouseInputEnabled(enabled);
}

void NancyEngine::addDeferredLoader(Common::SharedPtr<DeferredLoader> &loaderPtr) {
	_deferredLoaderObjects.push_back(Common::WeakPtr<DeferredLoader>(loaderPtr));
}

Common::Error NancyEngine::run() {
	setDebugger(new NancyConsole());

	// Boot the engine
	setState(NancyState::kBoot);

	// Check if we need to load a save state from the launcher
	if (ConfMan.hasKey("save_slot")) {
		int saveSlot = ConfMan.getInt("save_slot");
		if (saveSlot >= 0 && saveSlot <= getMetaEngine()->getMaximumSaveSlot()) {
			// Set to Scene but do not do the loading yet
			setState(NancyState::kScene);
		}
	}

	// Main loop
	while (!shouldQuit()) {
		uint32 frameEndTime = _system->getMillis() + 16;

		bool graphicsWereSuppressed = _graphicsManager->_isSuppressed;
		if (!graphicsWereSuppressed) {
			_cursorManager->setCursorType(CursorManager::kNormalArrow);
		}

		_input->processEvents();

		State::State *s;

		if (_gameFlow.changingState) {
			s = getStateObject(_gameFlow.curState);
			if (s) {
				s->onStateEnter(_gameFlow.curState);
			}

			_gameFlow.changingState = false;
		}

		s = getStateObject(_gameFlow.curState);
		if (s) {
			s->process();
		}

		_graphicsManager->draw();

		if (_gameFlow.changingState) {
			_graphicsManager->clearObjects();

			s = getStateObject(_gameFlow.prevState);
			if (s) {
				if(s->onStateExit(_gameFlow.prevState)) {
					destroyState(_gameFlow.prevState);
				}
			}
		}

		_system->updateScreen();

		// In cases where the graphics were not drawn for a frame, we want to make sure the next
		// frame is processed as fast as possible. Thus, we skip deferred loaders and the time
		// delay that normally maintains 60fps
		if (!graphicsWereSuppressed) {
			// Use the spare time until the next frame to load larger data objects
			// Some loading is guaranteed to happen even with no time left, to ensure
			// slower systems won't be stuck waiting forever
			if (_deferredLoaderObjects.size()) {
				uint i = _deferredLoaderObjects.size() - 1;
				int32 timePerObj = (frameEndTime - g_system->getMillis()) / _deferredLoaderObjects.size();

				if (timePerObj < 0) {
					timePerObj = 0;
				}

				for (auto *iter = _deferredLoaderObjects.begin(); iter < _deferredLoaderObjects.end(); ++iter) {
					if (iter->expired()) {
						iter = _deferredLoaderObjects.erase(iter);
					} else {
						auto objectPtr = iter->lock();
						if (objectPtr) {
							if (objectPtr->load(frameEndTime - (i * timePerObj))) {
								iter = _deferredLoaderObjects.erase(iter);
							}
							--i;
						}

						if (_system->getMillis() > frameEndTime) {
							break;
						}
					}
				}
			}

			uint32 frameFinishTime = _system->getMillis();
			if (frameFinishTime < frameEndTime) {
				_system->delayMillis(frameEndTime - frameFinishTime);
			}
		}
	}

	return Common::kNoError;
}

void NancyEngine::pauseEngineIntern(bool pause) {
	State::State *s = getStateObject(_gameFlow.curState);

	if (s) {
		if (pause) {
			s->onStateExit(NancyState::kPause);
		} else {
			s->onStateEnter(NancyState::kPause);
		}
	}

	Engine::pauseEngineIntern(pause);
}

void NancyEngine::bootGameEngine() {
	// Load paths
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "game");
	SearchMan.addSubDirectoryMatching(gameDataDir, "datafiles");
	SearchMan.addSubDirectoryMatching(gameDataDir, "ciftree");
	SearchMan.addSubDirectoryMatching(gameDataDir, "hdsound");
	SearchMan.addSubDirectoryMatching(gameDataDir, "cdsound");
	SearchMan.addSubDirectoryMatching(gameDataDir, "hdvideo");
	SearchMan.addSubDirectoryMatching(gameDataDir, "cdvideo");
	SearchMan.addSubDirectoryMatching(gameDataDir, "iff");
	SearchMan.addSubDirectoryMatching(gameDataDir, "art");
	SearchMan.addSubDirectoryMatching(gameDataDir, "font");

	// Load archive if running a compressed variant
	if (isCompressed()) {
		Common::Archive *cabinet = Common::makeInstallShieldArchive("data");
		if (cabinet) {
			SearchMan.add("data1.cab", cabinet);
		}
	}

	_resource = new ResourceManager();
	_resource->initialize();

	// Read nancy.dat
	readDatFile();

	// Setup mixer
	syncSoundSettings();

	IFF *iff = new IFF("boot");
	if (!iff->load())
		error("Failed to load boot script");

	// Load BOOT chunks data
	Common::SeekableReadStream *chunkStream = nullptr;
	#define LOAD_BOOT_L(t, s) if (chunkStream = iff->getChunkStream(s), chunkStream) {	\
								_engineData.setVal(s, new t(chunkStream));				\
								delete chunkStream;										\
							}
	#define LOAD_BOOT(t) LOAD_BOOT_L(t, #t)

	LOAD_BOOT(BSUM)
	LOAD_BOOT(VIEW)
	LOAD_BOOT(PCAL)
	LOAD_BOOT(INV)
	LOAD_BOOT(TBOX)
	LOAD_BOOT(HELP)
	LOAD_BOOT(CRED)
	LOAD_BOOT(MENU)
	LOAD_BOOT(SET)
	LOAD_BOOT(LOAD)
	LOAD_BOOT(SDLG)
	LOAD_BOOT(MAP)
	LOAD_BOOT(HINT)
	LOAD_BOOT(SPUZ)
	LOAD_BOOT(CLOK)
	LOAD_BOOT(SPEC)
	LOAD_BOOT(RCPR)
	LOAD_BOOT(RCLB)

	LOAD_BOOT_L(ImageChunk, "OB0")
	LOAD_BOOT_L(ImageChunk, "FR0")
	LOAD_BOOT_L(ImageChunk, "LG0")

	chunkStream = iff->getChunkStream("PLG0");
	if (!chunkStream) {
		chunkStream = iff->getChunkStream("PLGO"); // nancy4 and above use an O instead of a zero
	}	

	if (chunkStream) {
		_engineData.setVal("PLG0", new ImageChunk(chunkStream));
	}

	_cursorManager->init(iff->getChunkStream("CURS"));

	_graphicsManager->init();
	_graphicsManager->loadFonts(iff->getChunkStream("FONT"));

	preloadCals();

	_sound->initSoundChannels();
	_sound->loadCommonSounds(iff);

	delete iff;

	// Load convo texts and autotext
	const BSUM *bsum = (const BSUM *)getEngineData("BSUM");
	if (bsum && bsum->conversationTextsFilename.size() && bsum->autotextFilename.size())  {
		iff = new IFF(bsum->conversationTextsFilename);
		if (!iff->load()) {
			error("Could not load CONVO IFF");
		}

		if (chunkStream = iff->getChunkStream("CVTX"), chunkStream) {
			_engineData.setVal("CONVO", new CVTX(chunkStream));
			delete chunkStream;
		}

		delete iff;

		iff = new IFF(bsum->autotextFilename);
		if (!iff->load()) {
			error("Could not load AUTOTEXT IFF");
		}

		if (chunkStream = iff->getChunkStream("CVTX"), chunkStream) {
			_engineData.setVal("AUTOTEXT", new CVTX(chunkStream));
			delete chunkStream;
		}

		delete iff;
	}

	#undef LOAD_BOOT_L
	#undef LOAD_BOOT
}

State::State *NancyEngine::getStateObject(NancyState::NancyState state) const {
	switch (state) {
	case NancyState::kLogo:
		return &State::Logo::instance();
	case NancyState::kCredits:
		return &State::Credits::instance();
	case NancyState::kMap:
		return &State::Map::instance();
	case NancyState::kSetup:
		return &State::SetupMenu::instance();
	case NancyState::kHelp:
		return &State::Help::instance();
	case NancyState::kScene:
		return &State::Scene::instance();
	case NancyState::kMainMenu:
		return &State::MainMenu::instance();
	case NancyState::kLoadSave:
		return &State::LoadSaveMenu::instance();
	case NancyState::kSaveDialog:
		return &State::SaveDialog::instance();
	default:
		return nullptr;
	}
}

void NancyEngine::destroyState(NancyState::NancyState state) const {
	switch (state) {
	case NancyState::kLogo:
		if (State::Logo::hasInstance()) {
			State::Logo::instance().destroy();
		}
		break;
	case NancyState::kCredits:
		if (State::Credits::hasInstance()) {
			State::Credits::instance().destroy();
		}
		break;
	case NancyState::kMap:
		if (State::Map::hasInstance()) {
			State::Map::instance().destroy();
		}
		break;
	case NancyState::kHelp:
		if (State::Help::hasInstance()) {
			State::Help::instance().destroy();
		}
		break;
	case NancyState::kScene:
		if (State::Scene::hasInstance()) {
			State::Scene::instance().destroy();
		}
		break;
	case NancyState::kMainMenu:
		if (State::MainMenu::hasInstance()) {
			State::MainMenu::instance().destroy();
		}
		break;
	case NancyState::kSetup:
		if (State::SetupMenu::hasInstance()) {
			State::SetupMenu::instance().destroy();
		}
		break;
	case NancyState::kLoadSave:
		if (State::LoadSaveMenu::hasInstance()) {
			State::LoadSaveMenu::instance().destroy();
		}
		break;
	case NancyState::kSaveDialog:
		if (State::SaveDialog::hasInstance()) {
			State::SaveDialog::instance().destroy();
		}
		break;
	default:
		break;
	}
}

void NancyEngine::preloadCals() {
	const PCAL *pcal = (const PCAL *)getEngineData("PCAL");
	if (!pcal) {
		// CALs only appeared in nancy2 so a PCAL chunk may not exist
		return;
	}

	for (const Common::String &name : pcal->calNames) {
		if (!_resource->loadCifTree(name, "cal")) {
			error("Failed to preload CAL '%s'", name.c_str());
		}
	}
}

void NancyEngine::readDatFile() {
	Common::SeekableReadStream *datFile = SearchMan.createReadStreamForMember("nancy.dat");
	if (!datFile) {
		error("Unable to find nancy.dat");
	}

	if (datFile->readUint32BE() != MKTAG('N', 'N', 'C', 'Y')) {
		error("nancy.dat is invalid");
	}

	byte major = datFile->readByte();
	byte minor = datFile->readByte();
	if (major != _datFileMajorVersion) {
		error("Incorrect nancy.dat version. Expected '%d.%d', found %d.%d",
			_datFileMajorVersion, _datFileMinorVersion, major, minor);
	} else {
		if (minor != _datFileMinorVersion) {
			warning("Incorrect nancy.dat version. Expected '%d.%d', found %d.%d. Game may still work, but expect bugs",
			_datFileMajorVersion, _datFileMinorVersion, major, minor);
		}
	}

	uint16 numGames = datFile->readUint16LE();
	uint16 gameType = getGameType();
	if (gameType > numGames) {
		// Fallback for when no data is present for the current game:
		// throw a warning and use the last available game data
		warning("Data for game type %d is not in nancy.dat", getGameType());
		gameType = numGames;
	}

	// Seek to offset containing current game
	datFile->skip((gameType - 1) * 4);
	uint32 thisGameOffset = datFile->readUint32LE();
	uint32 nextGameOffset = gameType == numGames ? datFile->size() : datFile->readUint32LE();
	datFile->seek(thisGameOffset);

	_staticData.readData(*datFile, _gameDescription->desc.language, nextGameOffset);
}

Common::Error NancyEngine::synchronize(Common::Serializer &ser) {
	const BSUM *bootSummary = (const BSUM *)getEngineData("BSUM");
	assert(bootSummary);

	// Sync boot summary header, which includes full game title
	ser.syncVersion(kSavegameVersion);
	ser.matchBytes((const char *)bootSummary->header, 90);

	// Sync scene and action records
	NancySceneState.synchronize(ser);
	NancySceneState._actionManager.synchronize(ser);

	return Common::kNoError;
}

bool NancyEngine::isCompressed() {
	return getGameFlags() & GF_COMPRESSED;
}

} // End of namespace Nancy
