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


#ifndef DATAFILE_H_
#define DATAFILE_H_

#include <stdint.h>
#include <stdio.h>
#include "PString.h"
#include "m_dllist.h"

#define FILE_HEADER_LENGTH 8

//
// General info
//
// All data files start with 8-byte header with the file type (identifier)
// The name is defined by the class.
// If called from here, header remains "Unknown"
//

//
// DataFile
//
// Automatic Wolfenstein bot data file
//
class DataFile
{
	friend class DirectoryFile;
protected:

	// file header (as defined by subclasses)
	char header[FILE_HEADER_LENGTH + 1];
	// see if initialized
	bool initialized;
	
	// creates a new empty file
	void doInitializeEmpty(const PString &fname);
	// Execute writing to file
	virtual void doWriteToFile(FILE *f) = 0;
	// Execute reading from file
	virtual bool doReadFromFile(FILE *f) = 0;
    
public:
	// name of file (appears as directory entry or outer filename)
    PString _filename;
    DLListItem <DataFile> hashLink;
	
	DataFile();
	virtual ~DataFile() {}
	
	// accessor methods
	const PString &getFilename() const
	{
		return _filename;
	}
	const char *getHeader() const
	{
		return header;
	}
	
	// do that other thing
	void initialize(const PString &fname);
	
	// get file size
	virtual uint64_t getSize() = 0;
};

#endif

