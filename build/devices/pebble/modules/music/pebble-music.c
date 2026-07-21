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

#include "xsmc.h"
#include "xsHost.h"
#include "mc.xs.h"
#include "moddableAppState.h"

#include "builtinCommon.h"

#include "applib/music_service.h"

typedef struct {
	xsMachine	*the;
	xsSlot		obj;
	xsSlot		*onChange;
} PebbleMusicRecord, *PebbleMusic;

static void musicEventHandler(MusicServiceEventType eventType);

static const char *const gCommandNames[] = {
	"play",
	"pause",
	"togglePlayPause",
	"nextTrack",
	"previousTrack",
	"volumeUp",
	"volumeDown",
	"advanceRepeatMode",
	"advanceShuffleMode",
	"skipForward",
	"skipBackward",
	"like",
	"dislike",
	"bookmark",
};

static const char *const gEventNames[] = {
	"nowPlayingChanged",
	"playbackStateChanged",
	"volumeChanged",
	"trackPositionChanged",
	"serverConnected",
	"serverDisconnected",
};

static const char *const gPlaybackStateNames[] = {
	"unknown",
	"playing",
	"paused",
	"forwarding",
	"rewinding",
};

static MusicServiceCommand commandFromSlot(xsMachine *the, xsSlot slot)
{
	const char *name = xsmcToString(slot);

	for (uint32_t i = 0; i < sizeof(gCommandNames) / sizeof(gCommandNames[0]); i++) {
		if (0 == c_strcmp(name, gCommandNames[i]))
			return (MusicServiceCommand)i;
	}

	xsRangeError("unknown music command");
	return MusicServiceCommandPlay;
}

static void setStringProperty(xsMachine *the, xsSlot object, xsIdentifier property,
		const char *value)
{
	xsSlot tmp;
	xsmcSetString(tmp, (char *)value);
	xsmcSet(object, property, tmp);
}

void xs_music_destructor(void *data)
{
	PebbleMusic music = data;
	if (!music)
		return;

	if (music->onChange)
		music_service_unsubscribe();

	setModdableAppState(music, C_NULL);
	c_free(music);
}

void xs_music_mark(xsMachine *the, void *it, xsMarkRoot markRoot)
{
	PebbleMusic music = it;

	if (music->onChange)
		(*markRoot)(the, music->onChange);
}

static const xsHostHooks xsMusicHooks = {
	xs_music_destructor,
	xs_music_mark,
	NULL
};

void xs_music(xsMachine *the)
{
	if (getModdableAppState(music))
		xsUnknownError("only one");

	builtinInitializeTarget(the);

	PebbleMusic music = c_calloc(1, sizeof(PebbleMusicRecord));
	if (!music)
		xsRangeError("no memory");

	music->the = the;
	music->obj = xsThis;
	music->onChange = builtinGetCallback(the, xsID_onChange);

	xsRemember(music->obj);
	setModdableAppState(music, music);
	xsmcSetHostData(xsThis, music);
	xsSetHostHooks(xsThis, (xsHostHooks *)&xsMusicHooks);

	if (music->onChange)
		music_service_subscribe(musicEventHandler);
}

void xs_music_close(xsMachine *the)
{
	PebbleMusic music = xsmcGetHostData(xsThis);

	if (music && xsmcGetHostDataValidate(xsThis, (void *)&xsMusicHooks)) {
		xsForget(music->obj);
		xs_music_destructor(music);
		xsmcSetHostData(xsThis, NULL);
		xsmcSetHostDestructor(xsThis, NULL);
	}
}

void xs_music_nowPlaying(xsMachine *the)
{
	char title[MUSIC_SERVICE_BUFFER_LENGTH];
	char artist[MUSIC_SERVICE_BUFFER_LENGTH];
	char album[MUSIC_SERVICE_BUFFER_LENGTH];

	xsmcGetHostDataValidate(xsThis, (void *)&xsMusicHooks);
	if (!music_service_has_now_playing())
		return;

	music_service_get_now_playing(title, artist, album);
	xsmcSetNewObject(xsResult);
	setStringProperty(the, xsResult, xsID_title, title);
	setStringProperty(the, xsResult, xsID_artist, artist);
	setStringProperty(the, xsResult, xsID_album, album);
}

void xs_music_playerName(xsMachine *the)
{
	char playerName[MUSIC_SERVICE_BUFFER_LENGTH];

	xsmcGetHostDataValidate(xsThis, (void *)&xsMusicHooks);
	if (music_service_get_player_name(playerName))
		xsmcSetString(xsResult, playerName);
}

void xs_music_playbackInfo(xsMachine *the)
{
	MusicServicePlaybackInfo info;
	xsSlot tmp;

	xsmcGetHostDataValidate(xsThis, (void *)&xsMusicHooks);
	music_service_get_playback_info(&info);

	xsmcSetNewObject(xsResult);
	MusicServicePlaybackState state = info.playback_state;
	if ((uint32_t)state >= sizeof(gPlaybackStateNames) / sizeof(gPlaybackStateNames[0]))
		state = MusicServicePlaybackStateUnknown;
	setStringProperty(the, xsResult, xsID_state, gPlaybackStateNames[state]);

	xsmcSetNumber(tmp, info.position_ms);
	xsmcSet(xsResult, xsID_position, tmp);
	xsmcSetNumber(tmp, info.duration_ms);
	xsmcSet(xsResult, xsID_duration, tmp);
	xsmcSetInteger(tmp, info.playback_rate_percent);
	xsmcSet(xsResult, xsID_playbackRate, tmp);
	xsmcSetInteger(tmp, info.volume_percent);
	xsmcSet(xsResult, xsID_volume, tmp);

	xsmcSetNewObject(tmp);
	xsmcSet(xsResult, xsID_capabilities, tmp);
	xsSlot supported;
	xsmcSetBoolean(supported, info.capabilities & MusicServiceCapabilityPlaybackState);
	xsmcSet(tmp, xsID_playbackState, supported);
	xsmcSetBoolean(supported, info.capabilities & MusicServiceCapabilityProgress);
	xsmcSet(tmp, xsID_progress, supported);
	xsmcSetBoolean(supported, info.capabilities & MusicServiceCapabilityVolume);
	xsmcSet(tmp, xsID_volume, supported);
}

void xs_music_isCommandSupported(xsMachine *the)
{
	xsmcGetHostDataValidate(xsThis, (void *)&xsMusicHooks);
	MusicServiceCommand command = commandFromSlot(the, xsArg(0));
	xsmcSetBoolean(xsResult, music_service_is_command_supported(command));
}

void xs_music_sendCommand(xsMachine *the)
{
	xsmcGetHostDataValidate(xsThis, (void *)&xsMusicHooks);
	MusicServiceCommand command = commandFromSlot(the, xsArg(0));
	xsmcSetBoolean(xsResult, music_service_send_command(command));
}

static void musicEventHandler(MusicServiceEventType eventType)
{
	PebbleMusic music = getModdableAppState(music);
	if (!music || !music->onChange)
		return;

	if ((uint32_t)eventType >= sizeof(gEventNames) / sizeof(gEventNames[0]))
		return;

	xsMachine *the = music->the;
	xsBeginHost(the);
		xsmcSetString(xsResult, (char *)gEventNames[eventType]);
		xsCallFunction1(xsReference(music->onChange), music->obj, xsResult);
	xsEndHost(the);
}
