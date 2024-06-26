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

#ifndef MTROPOLIS_CORE_H
#define MTROPOLIS_CORE_H

#include "common/scummsys.h"

#ifndef INT32_MIN
#define INT32_MIN (-((int32)(0x7fffffff)) - 1)
#endif

namespace MTropolis {

struct IInterfaceBase {
	virtual ~IInterfaceBase();
};

enum RuntimeVersion {
	kRuntimeVersion100,

	kRuntimeVersion110,
	kRuntimeVersion111,
	kRuntimeVersion112,

	kRuntimeVersion200,
};

} // End of namespace MTropolis

#endif /* MTROPOLIS_CORE_H */
