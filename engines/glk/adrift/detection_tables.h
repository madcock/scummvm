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

#include "engines/game.h"
#include "common/gui_options.h"
#include "common/language.h"

namespace Glk {
namespace Adrift {

/**
 * Game description
 */
struct AdriftGameDescription {
	const char *const _gameId;
	const char *const _extra;
	const char *const _md5;
	size_t _filesize;
	Common::Language _language;
};

const PlainGameDescriptor ADRIFT_GAME_LIST[] = {
	{ "Adrift", "Adrift IF Game" },

	// English Games
	{ "3monkeys", "Three Monkeys, One Cage" },
	{ "akron", "Akron" },
	{ "albridgemanor", "Albridge Manor" },
	{ "asdfa", "A Short Damn Fantasy Adventure" },
	{ "awalkatdusk", "A Walk At Dusk" },
	{ "bariscebik", "Bariscebik" },
	{ "barneysproblem", "Barney's Problem" },
	{ "beanstalk", "Beanstalk the and Jack" },
	{ "beerisntenough", "WHen Beer Isn't Enough" },
	{ "buriedalive", "Buried Alive" },
	{ "caveofwonders", "Cave of Wondors" },
	{ "cbn1", "The Revenge Of Clueless Bob Newbie!" },
	{ "cbn2", "The Revenge Of Clueless Bob Newbie, Part 2" },
	{ "chooseyourown", "Choose Your Own..." },
	{ "circusmenagerie", "Menagerie!" },
	{ "cityInfear", "City In Fear" },
	{ "coloromcadrift", "Color of Milk COffee" },
	{ "compendiumendgame", "The WOodfish Compendium: The Game to End All Games" },
	{ "compendiumforum", "The WOodfish Compendium: Forum" },
	{ "compendiumforum2", "The WOodfish Compendium: Forum2" },
	{ "compendiumimagi", "The WOodfish Compendium: ImagiDroids" },
	{ "compendiumsaffire", "The WOodfish Compendium: Saffire " },
	{ "compendiumtopaz", "The WOodfish Compendium: Topaz" },
	{ "cowboyblues", "Cowboy Blues" },
	{ "crawlersdelight", "Crawler's Delight" },
	{ "crimeadventure", "Crime Adventure" },
	{ "cursed", "Cursed" },
	{ "cyber1", "Cyber Warp 1" },
	{ "cyber2", "Cyber Warp 2" },
	{ "darkness", "Darkness" },
	{ "dayattheoffice", "A Day at the Office" },
	{ "deadreckoningadrift", "Dead Reckoning" },
	{ "dontgoadrift", "Don't Go" },
	{ "doortoutopia", "The Door to Utopia" },
	{ "drwhovortexlust", "Doctor Who and The Vortex of Lust" },
	{ "dragonshrine", "The Curse of DragonShrine" },
	{ "edithscats", "Edith's Cats" },
	{ "enc1", "Encounter 1 : Tim's Mom" },
	{ "enc2", "Encounter 2 : The Study Group" },
	{ "escapetofreedom", "Escape to Freedom" },
	{ "etnyadrift", "Escape to New York" },
	{ "farfromhome", "Far From Home" },
	{ "frustratedinterviewee", "Frustrated Interviewee" },
	{ "gammagals", "The Gamma Gals" },
	{ "ghosttownadrift", "Ghost Town" },
	{ "gmylm", "Give Me Your Lunch Money" },
	{ "goblinhunt", "Goblin Hunt" },
	{ "goldilocksadrift", "Goldilocks is a FOX!" },
	{ "halloweenhijinks", "Halloween Hijinks" },
	{ "houseofthedamned", "House of the Damned" },
	{ "hhofhh", "The Haunted House of Hideous Horror" },
	{ "hcw", "How to Conquer the World" },
	{ "jgrim", "Jonathan Grimshaw: Space Tourist" },
	{ "jimpond", "Jim Pond 1" },
	{ "lairofthevampire", "Lair of the Vampire" },
	{ "legacyofaprincess", "Legacy of a Princess" },
	{ "longjourneyhome", "The Long Journey Home" },
	{ "lostadrift", "Lost" },
	{ "magicshow", "The Magic Show" },
	{ "maroonedadrift", "Marooned" },
	{ "ml256", "Makers Local and the Transdimensional Margarita Blender" },
	{ "monsterage", "Monster Age: Trials of Dustorn" },
	{ "monsters", "Monsters" },
	{ "murdererhadleft", "What The Murderer Had Left" },
	{ "mustescape", "Must Escape!" },
	{ "darkhavenmystery", "The Mystery Of The Darkhaven Caves" },
	{ "neighboursfromhell", "Neighbours From Hell" },
	{ "onnafa", "Oh No, Not Another Fantasy Adventure" },
	{ "overtheedge", "Over the Edge" },
	{ "paintadrift", "Paint!!!" },
	{ "pathwayadrift", "Pathway to Destruction" },
	{ "percytheviking", "The Saga Of Percy The Viking" },
	{ "pestilence", "Pestilence" },
	{ "phonebooth", "Pick Up the Phone Booth and Cry" },
	{ "towerprincess", "The Princess in the Tower" },
	{ "professorvonwitt", "Professor von Witt's Fabulous Flying Machine" },
	{ "provenance", "Provenance" },
	{ "rachelbadday", "Rachel has a bad day" },
	{ "readinginmayadrift", "A Reading in May" },
	{ "sceneofthecrime", "Scene of the Crime" },
	{ "selmaswill", "Selma's Will" },
	{ "seymoursstoopidquest", "Seymour's Stoopid Quest" },
	{ "shardsofmemory", "Shards of Memory" },
	{ "sommerilse", "Sommeril Special Edition" },
	{ "stowaway", "Stowaway" },
	{ "takeone", "Take One" },
	{ "target", "Target" },
	{ "thepkgirl", "The PK Girl" },
	{ "viewisbetter", "The View Is Better Here" },
	{ "thelasthour", "The Last Hour" },
	{ "ticket", "Ticket to No Where" },
	{ "hellinahamper", "To Hell in a Hamper" },
	{ "toomuchexercise", "Too Much Exercise" },
	{ "topaz", "Topaz" },
	{ "twilight", "The Twilight" },
	{ "unauthorizedtermination", "Unauthorised Termination" },
	{ "unfortunately", "Back To Life... Unfortunately" },
	{ "unravelinggod", "Unraveling God" },
	{ "vague", "Vague" },
	{ "veteranknowledge" , "Veteran Knowledge" },
	{ "waxworx", "Wax Worx" },
	{ "campwindylake", "Camp Windy Lake" },
	{ "woodsaredark", "The Woods Are Dark" },
	{ "worstgame", "The Worst Game In The World... Ever!!!" },
	{ "wrecked", "Wrecked" },
	{ "xycanthus", "Doomed Xycanthus" },
	{ "yadfa", "Yet Another Damn Fantasy Adventure" },
	{ "yonastoundingcastle", "Yon Astounding Castle! of some sort" },

	// English Blorb Games
	{ "advbackyard", "An Adventurer's Backyard" },
	{ "allthroughthenight", "All Through the Night" },
	{ "darkhour", "The Dark Hour" },
	{ "jacarandajim", "Jacaranda Jim" },
	{ "jacd", "Just Another Christmas Day" },
	{ "noblecrook1", "Noble Crook, episode 1" },
	{ "noblecrook2", "Noble Crook, episode 2" },
	{ "noblecrook3", "Noble Crook, episode 3" },
	{ "noblecrook4", "Noble Crook, episode 4" },
	{ "sonofcamelot", "Son of Camelot" },

	// Italian Games
	{ "ilgolem", "Il Golem" },

	{ nullptr, nullptr }
};

const AdriftGameDescription ADRIFT_GAMES[] = {
	// English Games
	DT_ENTRY0("3monkeys", "dec34c282511b1eadfe231dbbb49f625", 79286),
	DT_ENTRY0("akron", "2461ceeef3ef1eac15a2051a549b4402", 22258),
	DT_ENTRY0("albridgemanor", "268003e454c4ade042d593af8397a490", 31353),
	DT_ENTRY0("asdfa", "06173b47b4e88c0a494d2263666ad375", 27733),
	DT_ENTRY0("awalkatdusk", "f2e8aca65a17d41804709ee5a420ef36", 20725),
	DT_ENTRY0("bariscebik", "d63d310e2b2fe4ab7ea6bc74c136b8e0", 14634),
	DT_ENTRY0("barneysproblem", "3fe45352a6c0d7bb0bdbf4f9c08afba7", 66055),
	DT_ENTRY0("beanstalk", "fc9c17c355894948dc2e72c66e8d9963", 9837),
	DT_ENTRY0("beerisntenough", "98248a8e65a36f6d0fb98111255a85dc", 14070),
	DT_ENTRY0("buriedalive", "9e9a71babf3012461205dc7aa2cd2a3f", 11808),
	DT_ENTRY0("caveofwonders", "f55f4e63886eb09e1f89bea547574919", 85471),
	DT_ENTRY0("cbn1", "9e27ab68a1f37f5f7591b362c4888526", 7577),
	DT_ENTRY0("cbn2", "983eacedabebf7bbd8fed72ed399bba4", 6137),
	DT_ENTRY0("chooseyourown", "774ae5ba4fea61da0898069df4ed1c81", 54869),
	DT_ENTRY0("circusmenagerie", "3814400e3abb3e6340f055d5ec883c46", 117017),
	DT_ENTRY0("cityInfear", "a6f625f184fa848ee6746e9aa6c94915", 248088),
	DT_ENTRY0("coloromcadrift", "6b8d8f115a89c99f785156a9b5802bc2", 12681),
	DT_ENTRY0("compendiumendgame", "ad76afa2fcdb8c9e2272a115cce6bd4f", 4092),
	DT_ENTRY0("compendiumforum1", "273b73bf6262f603bee198238bc02196", 9128),
	DT_ENTRY0("compendiumforum2", "b4af4a085707f221dbabd2906e411d29", 11172),
	DT_ENTRY0("compendiumimagi", "3cfd6a4dd7ce552afdc52d0923e079cd", 9273),
	DT_ENTRY0("compendiumsaffire", "d8a0413cdb908ba2ac400fe6a880d954", 7486),
	DT_ENTRY0("compendiumtopaz", "5f91c9cd4391b6e44c2c052698d01118", 4866),
	DT_ENTRY0("cowboyblues", "23eabfd5db63ded776dd0323d2abe7ea", 111835),
	DT_ENTRY0("crawlersdelight", "9da704541689f95b3636ad729cfda5f4", 18705),
	DT_ENTRY0("crimeadventure", "965eafa4579caa3bb8382a07b5e1771e", 15073),
	DT_ENTRY0("cursed", "62d2e05e62f1137b25a61bbb46154729", 487990),
	DT_ENTRY1("cursed", "Hints", "4ad13bf274af97ebbfe47c4a852b3a46", 36793),
	DT_ENTRY0("cyber1", "b53f7a302edad4d9e2a3310dc1c66e72", 1786),
	DT_ENTRY0("cyber2", "535c161818ee238e112b1d00bd082b26", 3384),
	DT_ENTRY0("darkness", "f02a5be2f9a2fd59ef10f3d16157ee27", 25238),
	DT_ENTRY0("dayattheoffice", "2d908b5448c6fd2dbc98a7f9d2dda9df", 13824),
	DT_ENTRY0("deadreckoningadrift", "c49f787df29d6a73e77a0a33386614d3", 82279),
	DT_ENTRY0("dontgoadrift", "f192abcc4e1f04787a7f1fde2ad05385", 13789),
	DT_ENTRY0("doortoutopia", "9e727b8ad5b9c62d712d4a302083f6d8", 15432),
	DT_ENTRY0("drwhovortexlust", "e2b76d720096fb6427927456fc80c5a2", 166913),
	DT_ENTRY0("dragonshrine", "604250027cfd8cfd1fb0d231085e40e5", 636932),
	DT_ENTRY0("edithscats", "0c6bbf3d292a427182458bd5332f2348", 18316),
	DT_ENTRY0("enc1", "d297f4376e5ba5b05985ca12a60d8671", 101668),
	DT_ENTRY0("enc2", "4bd8ebd78a0a227510f62f2074e60ee5", 120335),
	DT_ENTRY0("escapetofreedom", "a7edcb289eb8177a143a96912ec55393", 18171),
	DT_ENTRY0("etnyadrift", "990c0e2390d197ebcae1109e5534c433", 59583),
	DT_ENTRY0("farfromhome", "b84df75a6ff647006f1f2783bde114f7", 42118),
	DT_ENTRY0("frustratedinterviewee", "86dcc06a9edcd967183d5448165fd92e", 50578),
	DT_ENTRY0("gammagals", "db813e61ded3e7f642d437ef1e77d0b3", 277834),
	DT_ENTRY0("ghosttownadrift", "60c11a24853dec5e6a292914c6b4f7f2", 30205645),
	DT_ENTRY0("gmylm", "67b61f16ca39d8abc9eb770385ef0e50", 15194757),
	DT_ENTRY0("goblinhunt", "5e36c63feebf77cfba30757042a9fafa", 10891),
	DT_ENTRY0("goldilocksadrift", "12e596065ae94d3f86a67acb4de2aae7", 56539),
	DT_ENTRY0("halloweenhijinks", "14694f7b9cef4baeb0f958500119d2ff", 68052),
	DT_ENTRY0("houseofthedamned", "3b25e7d9e1fd6ff2206ab0555c19e899", 35974),
	DT_ENTRY0("hhofhh", "cff22c2f2af619ac56e075e95385e600", 6541),
	DT_ENTRY0("hcw", "327dfec3d8f0b63ff5b05a552ee946ca", 82659),
	DT_ENTRY0("jgrim", "f9c1b2064edeed5675a3466baeebdefd", 96713),
	DT_ENTRY0("jimpond", "eef1aed7efbd36283b7d39b1514b3933", 50551),
	DT_ENTRY0("lairofthevampire", "33dce0c1c3f9ed2beded0bab931e8d78", 133689),
	DT_ENTRY0("legacyofaprincess", "9db58796cc772b662743e9a65ae1a876", 37003),
	DT_ENTRY0("longjourneyhome", "c16c9e02c4a3248b25cb4f6c1ada0646", 59124),
	DT_ENTRY0("lostadrift", "904b80ebf94df890ad7c26e454b6a612", 42596),
	DT_ENTRY0("magicshow", "29fd03636eec8ffdaea26251a6f2444c", 104201),
	DT_ENTRY0("maroonedadrift", "5e3e0435c98a0a915a98d5b1b080522c", 50516),
	DT_ENTRY0("ml256", "68862c0031f1497e32ce26e0654bb07f", 32397),
	DT_ENTRY0("monsterage", "00b8e89e5e7421a0ea017707466efe17", 16486),
	DT_ENTRY0("monsters", "fde7be359d37c9229fec77244fc38a54", 17168),
	DT_ENTRY0("murdererhadleft", "f95c63f90f28061a15dbe3bdf0cd4048", 13425),
	DT_ENTRY0("mustescape", "e58fd08918fa3d5a38f424f79595fb4e", 17497),
	DT_ENTRY0("darkhavenmystery", "1027e9e3dba08e0fd2796e0e934be855", 27826),
	DT_ENTRY0("neighboursfromhell", "686d8c0995aa085fb056c6a5bd402911", 13520),
	DT_ENTRY0("onnafa", "da4c0cc6fe34a08047de54abc66de038", 167339),
	DT_ENTRY0("overtheedge", "9c7bfb9252ac80597ef125c95d58a54b", 14771),
	DT_ENTRY1("overtheedge", "Gargoyle", "a6bb780b231c31a9e01455d0424acdca", 14937),
	DT_ENTRY0("paintadrift", "1183921d034d3614ec277e18d9112b69", 76493),
	DT_ENTRY0("pathwayadrift", "bcf1f61393c1b8123a98ee4879ffd8a6", 20896),
	DT_ENTRY0("percytheviking", "e995d3a23005914eb92836f141ebe1c4", 5965),
	DT_ENTRY0("pestilence", "2d5ff3fe2d866150689749087d5cee3d", 29021),
	DT_ENTRY0("phonebooth", "25f4977d7b8df28322df945809826f43", 1372),
	DT_ENTRY0("towerprincess", "d745a3f2c4dc40fb10f25d1539a2a1e7", 7181),
	DT_ENTRY0("professorvonwitt", "570e7d90fa42a199ee0cba44a3c95d6b", 31253),
	DT_ENTRY0("provenance", "49ebc8ad4f681eb727c8a2c90c3c6dd7", 532278),
	DT_ENTRY0("rachelbadday", "8940ad383d78b2ba5d1fa9738bf1ea98", 98539),
	DT_ENTRY0("readinginmayadrift", "ba8a12e79520234e31622f141c99cafd", 13434),
	DT_ENTRY0("sceneofthecrime", "a8a094b145523c622e65ab6897727f2d", 68054),
	DT_ENTRY0("selmaswill", "23b7fb4bf976135b8be2740e1fbfb83b", 19559),
	DT_ENTRY0("seymoursstoopidquest", "61f4e155b958c1c652f12ed4216ee10d", 12623),
	DT_ENTRY0("shardsofmemory", "9b75834354aed4b148d4ec2b8cdd66ae", 118604),
	DT_ENTRY0("sommerilse", "1870db33af741482bb5d93ab54200e7c", 22839),
	DT_ENTRY0("stowaway", "6877c3079978ecf74e72b792005fca32", 14231),
	DT_ENTRY0("takeone", "114caad9308b7adf9e15267a11f12632", 9547),
	DT_ENTRY0("target", "c6bbbae5ea3f6407a55435cb4e0c20c3", 43024),
	DT_ENTRY0("thepkgirl", "c9282cf8f91ebfe5a93b136f56d7a1aa", 1645515),
	DT_ENTRY0("viewisbetter", "9334adc4ccd03ceb1684ee56fdb07ab9", 7720),
	DT_ENTRY0("thelasthour", "786d39eb223a64fe836f87b032b60ae1", 10658),
	DT_ENTRY0("ticket", "8c549a6bdc6a5a4895412516be8dce25", 65502),
	DT_ENTRY0("hellinahamper", "810247344fddc9a812bf6283b4826093", 80183),
	DT_ENTRY0("toomuchexercise", "aebb58d94f632c4232bee72b9a1b5e25", 6248),
	DT_ENTRY0("topaz", "0777a97e473b41ae04ab825556748e8d", 5980),
	DT_ENTRY0("twilight", "ab5ddd461c1fb2065d26fcfdf4baa5aa", 71377),
	DT_ENTRY0("unauthorizedtermination", "a7bba6890fe2945bb192c67e395516ac", 107335),
	DT_ENTRY0("unfortunately", "8493ffd877273ef95c88c8f433126473", 55438),
	DT_ENTRY0("unravelinggod", "2d43577a756ddc6b561f447e67270ac4", 45000),
	DT_ENTRY0("vague", "0f5ef8616c668b9c3de078a77b26a512", 122681),
	DT_ENTRY0("veteranknowledge", "409d3d59cb767dc8b812e16db78b2e47", 52248),
	DT_ENTRY0("waxworx", "a10a1e404ae77fd968b4427a13bd7034", 38414),
	DT_ENTRY0("campwindylake", "33ea1f1fea9998472e9c352ea6e1c1d8", 114698),
	DT_ENTRY0("woodsaredark", "4acae86746292076b90fa6fa73ea76d0", 71216),
	DT_ENTRY0("worstgame", "8789e209419529ac22d2be4774620c78", 9858),
	DT_ENTRY0("wrecked", "b1c6c4ef73025fbbe99b612a72e3186a", 88550),
	DT_ENTRY0("xycanthus", "5637cb8221087c5cca094b13138655c3", 116322),
	DT_ENTRY0("yadfa", "5ee5906fc4b889d126cdfd83bd883a43", 77880),
	DT_ENTRY0("yonastoundingcastle", "baf2c0423903a3104f67d3a19bde43df", 2320881),

	// English Blorb Games
	DT_ENTRY0("advbackyard", "73b1171283ffa2b3cf6fb72a0ec31a35", 48990),
	DT_ENTRY0("allthroughthenight", "89dcb84d74f70945780ed9e719401301", 36345976),
	DT_ENTRY0("darkhour", "75bc2ac4d561ad2a2143bd90a19d1b46", 35184),
	DT_ENTRY0("jacarandajim", "78e0de68825eea88995d7490adb6c062", 79146),
	DT_ENTRY0("jacd", "28ae6ee45f1d2980765d3e9645f0b269", 54256),
	DT_ENTRY0("noblecrook1", "334c706261ab79b783403ad3315980fd", 23966),
	DT_ENTRY0("noblecrook2", "9d460ed691ad1b45d5d7c19f1ebf61d9", 22446),
	DT_ENTRY0("noblecrook3", "c9d4a1ba7d5c1625f2d4cad0f067275a", 22088),
	DT_ENTRY0("noblecrook4", "cb26241f8dba982bb0cd357ebd8fac45", 20606),
	DT_ENTRY0("sonofcamelot", "7b3e9e45edc75202b3f586d0ca8dddde", 474380),

	// Italian Games
	DT_ENTRYL0("ilgolem", Common::IT_ITA, "7ab97545be9c30457d0c9e748ab7885f", 6372213),

	DT_END_MARKER
};

} // End of namespace Adrift
} // End of namespace Glk
