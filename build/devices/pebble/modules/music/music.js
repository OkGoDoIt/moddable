/*
 * Copyright (c) 2026 Pebble contributors
 *
 *   This file is part of the Moddable SDK Runtime.
 *
 *   The Moddable SDK Runtime is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   The Moddable SDK Runtime is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

class MusicService extends Native("xs_music_destructor") {
	constructor(options = {}) { super(); native("xs_music").call(this, options); }
	close() { return native("xs_music_close").call(this); }
	nowPlaying() { return native("xs_music_nowPlaying").call(this); }
	playerName() { return native("xs_music_playerName").call(this); }
	playbackInfo() { return native("xs_music_playbackInfo").call(this); }
	isCommandSupported(command) {
		return native("xs_music_isCommandSupported").call(this, command);
	}
	sendCommand(command) { return native("xs_music_sendCommand").call(this, command); }
}

export default MusicService;
