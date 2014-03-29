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
////////////////////////////////////////////////////////////////////////////////
//
// Shell character set abstraction (Win32 acting old-fashioned here)
//
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "ShellUnicode.h"
#include "StdStringExtensions.h"

std::string ShellUnicode::getenv(const std::string& name)
{
#ifdef _WIN32
	return WideCharToUTF8(::_wgetenv(UTF8ToWideChar(name)));
#else
	char* env = ::getenv(name.c_str());
	if(!env)
		return "";
	return env;
#endif
}

FILE* ShellUnicode::fopen(const char* fname, const char* finfo)
{
#ifdef _WIN32
	return _wfopen(UTF8ToWideChar(fname), UTF8ToWideChar(finfo));
#else
	return ::fopen(fname, finfo);
#endif
}

int ShellUnicode::remove(const char *fname)
{
#ifdef _WIN32
	return _wremove(UTF8ToWideChar(fname));
#else
	return ::remove(fname);
#endif
}

int ShellUnicode::rename(const char *a, const char *b)
{
#ifdef _WIN32
	return _wrename(UTF8ToWideChar(a), UTF8ToWideChar(b));
#else
	return ::rename(a, b);
#endif
}

int ShellUnicode::unlink(const char *a)
{
#ifdef _WIN32
	return _wunlink(UTF8ToWideChar(a));
#else
	return ::unlink(a);
#endif
}