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

#include "common/str.h"
#include "common/debug.h"
#include "common/events.h"
#include "common/system.h"
#include "backends/steam/steammanager.h"
#include "backends/steam/steamshim_child.h"

namespace Common {

DECLARE_SINGLETON(Steam::SteamManager);

}

namespace Steam {

SteamManager::SteamManager() {
	_isInBPM = false;
}

SteamManager::~SteamManager() {
	g_system->getEventManager()->getEventDispatcher()->unregisterSource(this);
	STEAMSHIM_deinit();
}

void SteamManager::init() {
	// Set up Steam and events
	if (STEAMSHIM_init()) {
		debug("Steamworks loaded");
		g_system->getEventManager()->getEventDispatcher()->registerSource(this, false);
	}
}

bool SteamManager::isSteamInBigPictureMode() {
	return _isInBPM;
}

bool SteamManager::pollEvent(Common::Event &event) {
	const STEAMSHIM_Event *e;
	while ((e = STEAMSHIM_pump()) != 0) {
		switch (e->type) {
		case SHIMEVENT_ISINBPM:
			_isInBPM = e->okay;
			break;
		default:
			warning("Unknown Steamshim Event %s", e->name);
			break;
		}
	}

	return false;
}

} // End of namespace Steam
