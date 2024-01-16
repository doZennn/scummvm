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

#ifndef STEAM_STEAMMANAGER_H
#define STEAM_STEAMMANAGER_H

#include "common/array.h"
#include "common/singleton.h"
#include "common/str-array.h"
#include "common/events.h"

namespace Steam {

class SteamManager : public Common::Singleton<SteamManager>, public Common::EventSource {
private:
	bool _isInBPM = false;
	bool pollEvent(Common::Event &event) override;

public:
	SteamManager();
	~SteamManager() override;

	/**
	 * @note It's called once on startup in scummvm_main().
	 */
	void init();

	/** Returns whether there are any requests running. */
	bool isWorking() const;

	bool isSteamInBigPictureMode();
};

/** Shortcut for accessing the connection manager. */
#define SteamMan        Steam::SteamManager::instance()

} // End of namespace Steam

#endif
