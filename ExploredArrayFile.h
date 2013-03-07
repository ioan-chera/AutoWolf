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


#ifndef EXPLOREDARRAYFILE_H_
#define EXPLOREDARRAYFILE_H_

#include "wl_def.h"
#include "DataFile.h"

#define DATAFILE_EXPLORED_NAME "Explored"
#define DATAFILE_EXPLORED_HEADER "Explored"

//
// ExploredArrayFile
//
// The map array file
// Has a MD5 checksum name, and contains map data
//
// THE STRUCTURE IN GENERAL
//
// A folder with a MD5 checksum name contains several informations
// Let's now name this simply ExploredFile. Until a release, I can make up my mind how to name them
// to make them future-proof and avoid frequent deprecations

class ExploredArrayFile : public DataFile
{
protected:
	// Execute writing to file
	void doWriteToFile(FILE *f);
	// data stuff
	boolean explored[MAPSIZE][MAPSIZE];
	// Execute reading from file
	bool doReadFromFile(FILE *f);
public:
	ExploredArrayFile();
	// get file size
	uint64_t getSize();
	
	// copy explored data into target
	void getData(void *putTarget) const;
	
	// inverse
	void putData(const void *getSource);
};

#endif
