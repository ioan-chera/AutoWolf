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
// DirectoryFile::initialize
//
// Initialize with the Director name
//
void DataFile::initialize(const char *fname)
{
	doInitializeEmpty(fname, fileHeader);
}

//
// DataFile::initializeEmpty
//
// Creates a new structure
//
void DataFile::doInitializeEmpty(const char *fname, const char *hdr)
{
	filename = strdup(fname);
	header = strdup(hdr);
	initialized = true;
}