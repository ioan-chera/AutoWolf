//
//  MasterDirectoryFile.cpp
//  Wolf4SDL
//
//  Created by Ioan on 14.12.2012.
//
//

#include <string.h>
#include "MasterDirectoryFile.h"

////////////
//
// INSTANCES
//
////////////

MasterDirectoryFile mainDataFile;

////////////
//
// CONSTANTS
//
////////////

//
// DirectoryFile::DirectoryFile
//
MasterDirectoryFile::MasterDirectoryFile()
{
	filename = strdup(masterDirectoryFileName);
	filenamelen = strlen(filename);
	initialized = true;
	
	strcpy(header, "AutoWolf");
}

//
// MasterDirectoryFile::MainDir
//
// Access the global main object
//
MasterDirectoryFile &MasterDirectoryFile::MainDir()
{
	return mainDataFile;
}

//
// MasterDirectoryFile::saveToFile
//
// save it to file
//
void MasterDirectoryFile::saveToFile()
{
	FILE *f;
	
	f = fopen(masterDirectoryFileName, "wb");
	this->MasterDirectoryFile::doWriteToFile(f);
	fclose(f);
}