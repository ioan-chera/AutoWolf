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


#ifndef DIRECTORYFILE_H_
#define DIRECTORYFILE_H_

#define DIRECTORY_HEADER "Director"

#include "DataFile.h"


//
// DirectoryFile
//
// Contains a directory entry, towards subfiles
//
// Structure:
//  uint32_t number of files
//  uint64_t address of file list
//  Contents of each file
//  File list
//
// File list:
//  entries of:
//   uint16_t length of file name
//   non-null-string file name
//   uint64_t address of file within container
//
class DirectoryFilePimpl;

class DirectoryFile : public DataFile
{
private:
	// list of files
    DirectoryFilePimpl *pimpl;

protected:
	// number of files
	uint32_t numberOfFiles;
	// address of list
	uint64_t addressOfList;
	
	void doWriteToFile(FILE *f);
	// Execute reading from file
	bool doReadFromFile(FILE *f);
public:
	DirectoryFile();
	virtual ~DirectoryFile();
	
	// now size
	uint64_t getSize();
	
	// add file to list
	bool addFile(DataFile *file);
	
	// access file with name
	DataFile *getFileWithName(const PString &fname);
	
	// create folder if not exist
	DirectoryFile *makeDirectory(const PString &fname);
};

#endif

