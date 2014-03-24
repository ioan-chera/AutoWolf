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
// STL String extensions
//
////////////////////////////////////////////////////////////////////////////////

#ifndef STDSTRINGEXTENSIONS_H_
#define STDSTRINGEXTENSIONS_H_

#include <string>
#include "StdStringExtensions.h"
#include "wl_def.h"

std::string& operator+= (std::string& first, int n);
std::string& operator+= (std::string& first, unsigned n);
std::string& operator+= (std::string& first, unsigned long long n);

std::string WideCharToUTF8(const std::wstring& source);
std::wstring UTF8ToWideChar(const std::string& source);

inline static const char* SafeCString(const char* str)
{
	return str ? str : "";
}

inline static const wchar_t* SafeCString(const wchar_t* str)
{
	return str ? str : L"";
}

inline static int strcasecmp(const std::string& str1, const char* str2)
{
	return strcasecmp(str1.c_str(), str2);
}

inline static std::wstring _wgetenv(const std::wstring& varname)
{
#ifdef _WIN32
	return SafeCString(_wgetenv(varname.c_str()));
#else
	return L"";
#endif
}

std::string& ConcatSubpath(std::string& source, const std::string& added);

#endif