//
//  DataFile.cpp
//  Wolf4SDL
//
//  Created by Ioan on 13.12.2012.
//
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