/*
* Copyright (c) 2026 Pebble contributors
*
*   This file is part of the Moddable SDK Tools.
*
*   The Moddable SDK Tools is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   The Moddable SDK Tools is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with the Moddable SDK Tools.  If not, see <http://www.gnu.org/licenses/>.
*
*/

type MusicServiceEvent =
  | "nowPlayingChanged"
  | "playbackStateChanged"
  | "volumeChanged"
  | "trackPositionChanged"
  | "serverConnected"
  | "serverDisconnected";

type MusicServicePlaybackState =
  | "unknown"
  | "playing"
  | "paused"
  | "forwarding"
  | "rewinding";

type MusicServiceCommand =
  | "play"
  | "pause"
  | "togglePlayPause"
  | "nextTrack"
  | "previousTrack"
  | "volumeUp"
  | "volumeDown"
  | "advanceRepeatMode"
  | "advanceShuffleMode"
  | "skipForward"
  | "skipBackward"
  | "like"
  | "dislike"
  | "bookmark";

interface MusicServiceOptions {
  onChange?: (this: MusicService, event: MusicServiceEvent) => void;
}

interface MusicServiceNowPlaying {
  title: string;
  artist: string;
  album: string;
}

interface MusicServiceCapabilities {
  playbackState: boolean;
  progress: boolean;
  volume: boolean;
}

interface MusicServicePlaybackInfo {
  state: MusicServicePlaybackState;
  position: number;
  duration: number;
  playbackRate: number;
  volume: number;
  capabilities: MusicServiceCapabilities;
}

declare class MusicService {
  constructor(options?: MusicServiceOptions);
  close(): void;
  nowPlaying(): MusicServiceNowPlaying | undefined;
  playerName(): string | undefined;
  playbackInfo(): MusicServicePlaybackInfo;
  isCommandSupported(command: MusicServiceCommand): boolean;
  sendCommand(command: MusicServiceCommand): boolean;
}

export default MusicService;
