//
// Copyright (C) 2013  Ioan Chera
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


#include <string.h>
#include "DataFile.h"

////////////
//
// CONSTANTS
//
////////////

////////////
//
// INSTANCES
//
////////////

////////////
//
// METHODS
//
////////////

//
// DataFile::DataFile
//
DataFile::DataFile() : filename(0), initialized(false)
{
	strcpy(header, "Unknown");
}

//
// DirectoryFile::initialize
//
// Initialize with the Director name
//
void DataFile::initialize(const char *fname, size_t nchar)
{
	doInitializeEmpty(fname, nchar);
}

//
// DataFile::initializeEmpty
//
// Creates a new structure
//
void DataFile::doInitializeEmpty(const char *fname, size_t nchar)
{
	if(nchar <= 0)	// text string
	{
		filename = strdup(fname);
		filenamelen = strlen(filename);
	}
	else			// arbitrary char array (may have null)
	{
		filename = new char[nchar + 1];
		filename[nchar] = 0;
		memcpy(filename, fname, nchar);
		filenamelen = nchar;
	}
	
	initialized = true;
}