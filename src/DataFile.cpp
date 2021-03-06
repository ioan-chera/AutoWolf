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


#include "wl_def.h"
#include "DataFile.h"

//
// DataFile::DataFile
//
DataFile::DataFile() : _initialized(false), _containerFile(NULL)
{
	strcpy(_header, "Unknown");
}

//
// DirectoryFile::initialize
//
// Initialize with the Director name
//
void DataFile::initialize(const std::string &fname)
{
	doInitializeEmpty(fname);
}

//
// DataFile::initializeEmpty
//
// Creates a new structure
//
void DataFile::doInitializeEmpty(const std::string &fname)
{
    _filename = fname.c_str();
	_initialized = true;
}

//
// DataFile::_updateSize
//
void DataFile::_updateSize()
{
    if(_containerFile)
        _containerFile->_updateSize();
}