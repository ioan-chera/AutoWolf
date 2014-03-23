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

#include <Windows.h>
#include <Stringapiset.h>

#include "StdStringExtensions.h"

std::string& operator+= (std::string& first, int n)
{
	char buf[33];
	_snprintf(buf, sizeof(buf), "%d", n);
	first += buf;
	return first;
}

std::string& operator+= (std::string& first, unsigned n)
{
	char buf[33];
	_snprintf(buf, sizeof(buf), "%u", n);
	first += buf;
	return first;
}

std::string WideCharToUTF8(const std::wstring& source)
{
	int length = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, nullptr, 0, nullptr, nullptr);
	char *destination = new char[length];
	WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, destination, length, nullptr, nullptr);
	std::string str(destination);
	delete[] destination;
	return std::move(str);
}

std::wstring UTF8ToWideChar(const std::string& source)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, nullptr, 0);
	wchar_t* destination = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, destination, length);
	std::wstring str(destination);
	delete[] destination;
	return std::move(str);
}