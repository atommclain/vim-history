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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#ifndef SIZE_MAX
#define SIZE_MAX ULONG_MAX
#endif

#define SCROLLBAR_WIDTH 16

/* GetMBarHeight() is not very useful */
#define MENUBAR_HEIGHT 20

#define MAX_TEXTEDIT_SIZE 32767L

void panic(const char *format, ...);
void err(short ret, const char *format, ...);
void warnx(const char *format, ...);
void warn(const char *format, ...);
void note(const char *format, ...);

Handle xNewHandle(size_t size);

void debugA(const char *format, ...);

#define STR(x)   #x
#define SHOW_DEFINE(x) if (strcmp(STR(x),#x)!=0) { \
	debugA("%s %s\n", #x,"is defined"); \
} else { \
	debugA("%s %s\n", #x,"is NOT defined"); \
}

#endif
