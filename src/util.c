/*
 * Copyright (c) 2020-2022 joshua stein <jcs@jcs.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Dialogs.h>
#include <TextUtils.h>


#include "util.h"

/* ALRT resources */
#define ASK_ALERT_ID 130

#define ERROR_STRING_SIZE	1024
static char err_str[ERROR_STRING_SIZE];

/* basic DITL with an ok button (1), text area (2), and icon (3) */
#define ALERT_DITL_ICON		3
static const char alert_ditl[] = {
	0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
	0x00, 0xE6, 0x00, 0x5A, 0x01, 0x20, 0x04, 0x02,
	0x4F, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x00, 0x32, 0x00, 0x40, 0x01, 0x21, 0x08, 0x02,
	0x5E, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x00, 0x0A, 0x00, 0x2A, 0x00, 0x2A, 0xA0, 0x02,
	0x00, 0x02
};
static Handle alert_ditl_h = NULL;

/* DITL with a Yes button (1), No button (2), text (3), and icon (4) */
static const char ask_ditl[] = {
	0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
	0x00, 0xE6, 0x00, 0x5A, 0x01, 0x20, 0x04, 0x03,
	0x59, 0x65, 0x73, 0x21, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x46, 0x00, 0xA0, 0x00, 0x5A, 0x00, 0xDA,
	0x04, 0x02, 0x4E, 0x6F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0A, 0x00, 0x32, 0x00, 0x41, 0x01, 0x22,
	0x08, 0x02, 0x5E, 0x30, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0A, 0x00, 0x0A, 0x00, 0x2A, 0x00, 0x2A,
	0xA0, 0x02, 0x00, 0x01
};
static Handle ask_ditl_h = NULL;

/* DITL with just a text view */
static const char progress_ditl[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14,
	0x00, 0x1E, 0x00, 0x32, 0x01, 0x3B, 0x08, 0x02,
	0x5E, 0x30
};
static Handle progress_ditl_h = NULL;
static DialogPtr progress_dialog = NULL;

enum {
	STOP_ALERT,
	CAUTION_ALERT,
	NOTE_ALERT
};

static TEHandle track_control_te = NULL;

void vwarn(short alert_func, const char *format, va_list ap);

/*
 * BSD err(3) and warn(3) functions
 */

void
vwarn(short alert_func, const char *format, va_list ap)
{
	Rect bounds, irect;
	short quit = 0, height, width, hit;
	WindowPtr win, dialog;
	OSType itype;
	Handle ihandle;
	
	GetPort(&win);
	vsprintf(err_str, format, ap);

	width = 300;
	height = 100;
	bounds.left = (qd.screenBits.bounds.right - width) / 2;
	bounds.right = bounds.left + width;
	bounds.top = GetMBarHeight() +
	  ((qd.screenBits.bounds.bottom - height) / 2.5);
	bounds.bottom = bounds.top + height;
	
	ParamText(c2pstr(err_str), "\p", "\p", "\p");

	alert_ditl_h = xNewHandle(sizeof(alert_ditl));
	HLock(alert_ditl_h);
	memcpy(*alert_ditl_h, alert_ditl, sizeof(alert_ditl));
	HUnlock(alert_ditl_h);

	dialog = NewDialog(nil, &bounds, "\p", false, dBoxProc,
	  (WindowPtr)-1L, false, 0, alert_ditl_h);
	  
#if 0
	/* XXX: why doesn't changing this work? */
	GetDItem(dialog, ALERT_DITL_ICON, &itype, &ihandle, &irect);
	switch (alert_func) {
	case CAUTION_ALERT:
		ihandle = GetIcon(cautionIcon);
		break;
	case NOTE_ALERT:
		ihandle = GetIcon(noteIcon);
		break;
	default:
		ihandle = GetIcon(stopIcon);
	}
	ihandle = GetIcon(cautionIcon);
	SetDItem(dialog, ALERT_DITL_ICON, itype, ihandle, &irect);
#endif

	ShowWindow(dialog);
	for (;;) {
		ModalDialog(0, &hit);
		if (hit == ok)
			break;
	}
	DisposeDialog(dialog);
	DisposeHandle(alert_ditl_h);

	SetPort(win);
	
	if (quit)
		ExitToShell();
}

void
panic(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vwarn(STOP_ALERT, format, ap);
	va_end(ap);
	
	ExitToShell();
}

void
err(short ret, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vwarn(STOP_ALERT, format, ap);
	va_end(ap);
	
	ExitToShell();
}

void
warn(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vwarn(CAUTION_ALERT, format, ap);
	va_end(ap);
}

void
warnx(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vwarn(CAUTION_ALERT, format, ap);
	va_end(ap);
}

void
note(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vwarn(NOTE_ALERT, format, ap);
	va_end(ap);
}

/*
 * Error checking wrappers for Mac toolkit functions
 */
 
Handle
xNewHandle(size_t size)
{
	Handle h;
	
	if (size == 0)
		panic("Zero xNewHandle size");

	h = NewHandle(size);
	if (h == NULL)
		panic("Failed to NewHandle(%lu)", size);
	
	return h;
}

void
debugA(const char *format, ...)
{
	va_list ap;
	
	long	gestalt_rc;
	Boolean hasColor = false;
    if (Gestalt(gestaltQuickdrawFeatures, &gestalt_rc) == noErr) {
		if (BitTst(&gestalt_rc,31-gestaltHasColor)) {
	    	hasColor = true;
		}
    }

	va_start(ap, format);
	vsprintf(err_str, format, ap);
	if (hasColor) {
		warn(err_str);
	} else {
		DebugStr(c2pstr(err_str));
	}
	
	
	va_end(ap);
}
