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

#ifndef SHELLUNICODE_H_
#define SHELLUNICODE_H_

#include <string>

namespace ShellUnicode
{
	std::string getenv(const std::string& name);
	FILE* fopen(const char*, const char*);
	int remove(const char*);
	int rename(const char*, const char*);
	int unlink(const char*);
}

#endif