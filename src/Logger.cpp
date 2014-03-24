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
#include "Logger.h"
#include "wl_def.h"

#ifdef __APPLE__
#include <asl.h>
#endif

void Logger::Write(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

#ifdef _WIN32
	char buffer[1024];
	buffer[lengthof(buffer) - 1] = 0;
	vsnprintf(buffer, lengthof(buffer), format, argptr);

	OutputDebugStringA(buffer);
	OutputDebugStringA("\n");
#elif defined __APPLE__
	asl_vlog(nullptr, nullptr, ASL_LEVEL_INFO, format, argptr);
#else
#error TODO Linux or others
#endif
	va_end(argptr);
}