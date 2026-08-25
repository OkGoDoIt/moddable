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

#include "applib/peek_widget.h"

static const char *const gResultNames[] = {
	"success",
	"invalidArgs",
	"disabled",
};

//! Copies a string property into buffer (empty when absent/undefined/null).
static void getStringProperty(xsMachine *the, xsSlot object, xsIdentifier property,
		char *buffer, size_t size)
{
	xsSlot tmp;

	buffer[0] = 0;
	if (!xsmcHas(object, property))
		return;

	xsmcGet(tmp, object, property);
	const xsType type = xsmcTypeOf(tmp);
	if ((xsUndefinedType == type) || (xsNullType == type))
		return;

	c_strncpy(buffer, xsmcToString(tmp), size - 1);
	buffer[size - 1] = 0;
}

static uint32_t getIntegerProperty(xsMachine *the, xsSlot object, xsIdentifier property)
{
	xsSlot tmp;

	if (!xsmcHas(object, property))
		return 0;

	xsmcGet(tmp, object, property);
	return (uint32_t)xsmcToInteger(tmp);
}

void xs_peekWidget_publish(xsMachine *the)
{
	// Room for the untruncated strings; the applib wrapper crops to the SDK limits on a
	// UTF-8 codepoint boundary.
	char title[(PEEK_WIDGET_TITLE_MAX_LEN * 4) + 1];
	char subtitle[(PEEK_WIDGET_SUBTITLE_MAX_LEN * 4) + 1];

	if (xsmcArgc < 1)
		xsRangeError("options required");

	getStringProperty(the, xsArg(0), xsID_title, title, sizeof(title));
	getStringProperty(the, xsArg(0), xsID_subtitle, subtitle, sizeof(subtitle));

	const PeekWidgetInfo info = {
		.icon = getIntegerProperty(the, xsArg(0), xsID_icon),
		.title = title,
		.subtitle = subtitle[0] ? subtitle : NULL,
		.launch_code = getIntegerProperty(the, xsArg(0), xsID_launchCode),
		.timeout_s = (uint16_t)getIntegerProperty(the, xsArg(0), xsID_timeout),
	};

	PeekWidgetResult result = peek_widget_publish(&info);
	if ((uint32_t)result >= sizeof(gResultNames) / sizeof(gResultNames[0]))
		result = PEEK_WIDGET_RESULT_INVALID_ARGS;
	xsmcSetString(xsResult, (char *)gResultNames[result]);
}

void xs_peekWidget_withdraw(xsMachine *the)
{
	peek_widget_withdraw();
}
