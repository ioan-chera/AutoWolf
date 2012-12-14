//
//  MasterDirectoryFile.cpp
//  Wolf4SDL
//
//  Created by Ioan on 14.12.2012.
//
//

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
	doWriteToFile(f);
	fclose(f);
}