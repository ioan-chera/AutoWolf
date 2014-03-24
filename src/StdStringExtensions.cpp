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

#ifdef _WIN32
#include <Windows.h>
#include <Stringapiset.h>
#endif

#include "StdStringExtensions.h"
#include "wl_def.h"

std::string& operator+= (std::string& first, int n)
{
	char buf[33];
	snprintf(buf, sizeof(buf), "%d", n);
	first += buf;
	return first;
}

std::string& operator+= (std::string& first, unsigned n)
{
	char buf[33];
	snprintf(buf, sizeof(buf), "%u", n);
	first += buf;
	return first;
}

std::string& operator+= (std::string& first, unsigned long long n)
{
	char buf[33];
	snprintf(buf, sizeof(buf), "%llu", n);
	first += buf;
	return first;
}

std::string WideCharToUTF8(const std::wstring& source)
{
#ifdef _WIN32
	int length = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, nullptr, 0, nullptr, nullptr);
	char *destination = new char[length];
	WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, destination, length, nullptr, nullptr);
	std::string str(destination);
	delete[] destination;
	return str;
#else
	return "";
#endif
}

std::wstring UTF8ToWideChar(const std::string& source)
{
#ifdef _WIN32
	int length = MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, nullptr, 0);
	wchar_t* destination = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, destination, length);
	std::wstring str(destination);
	delete[] destination;
	return str;
#else
	return L"";
#endif
}

static void normalizeSlashes(std::string& str)
{
	char useSlash = '/'; // The slash type to use for normalization.
	char replaceSlash = '\\'; // The kind of slash to replace.
	bool isUNC = false;

#ifdef _WIN32

	// This is an UNC path; it should use backslashes.
	// NB: We check for both in the event one was changed earlier by mistake.
	if (str.length() > 2 &&
		((str[0] == '\\' || str[0] == '/') && str[0] == str[1]))
	{
		useSlash = '\\';
		replaceSlash = '/';
		isUNC = true;
	}

#endif

	// Convert all replaceSlashes to useSlashes
	size_t i;
	for (i = 0; i < str.length(); ++i)
	{
		if (str[i] == replaceSlash)
			str[i] = useSlash;
	}

	// Remove trailing slashes
	while (str.length() > 1 && str.back() == useSlash)
		str.pop_back();

	std::string doubleSlash;
	doubleSlash += useSlash;
	doubleSlash += useSlash;

	size_t position, oposition;
	while ((position = str.find(doubleSlash, isUNC ? 2 : 0)) != std::string::npos)
	{
		oposition = str.find_first_not_of(useSlash, position);
		if (oposition == std::string::npos)
			str.erase(position);
		else
			str.erase(position + 1, oposition - position - 1);
	}
}

std::string& ConcatSubpath(std::string& source, const std::string& added)
{
	if (source.length() > 0)
		source += "/";
	source += added;
	normalizeSlashes(source);
	return source;
}