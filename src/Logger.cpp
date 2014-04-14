//
// Copyright (C) 2014  Ioan Chera
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include <stdarg.h>
#include <stdio.h>
#ifdef __ANDROID__
#include <android/log.h>
#endif
#include "Logger.h"
#include "wl_def.h"

void Logger::Write(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

#ifdef _WIN32
	static bool consoleExists;
	static HANDLE handle;
	DWORD crap;
	char buffer[512];
	buffer[lengthof(buffer) - 1] = 0;
	int sz = vsnprintf(buffer, lengthof(buffer), format, argptr);

	if (!consoleExists)
	{
		consoleExists = true;
		AllocConsole();
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	WriteConsole(handle, buffer, sz < (lengthof(buffer) - 1) ? sz : lengthof(buffer) - 1, &crap, nullptr);
	WriteConsole(handle, "\n", 1, &crap, nullptr);
#elif defined(__ANDROID__)
	__android_log_vprint(ANDROID_LOG_INFO, "AutoWolfNDK", format, argptr);
#else
	vprintf(format, argptr);
	puts("");
#endif
	va_end(argptr);
}