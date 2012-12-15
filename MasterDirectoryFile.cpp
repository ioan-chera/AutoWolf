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
// MasterDirectoryFile::MainDir
//
// Access the global main object
//
MasterDirectoryFile &MasterDirectoryFile::MainDir()
{
	return mainDataFile;
}

//
// DirectoryFile::doWriteToFile
//
// Execute writing it to file, being given a FILE * variable
//
/*
void MasterDirectoryFile::doWriteToFile(FILE *f)
{
	fputs(fileHeader, f);
	fwrite(&numberOfFiles, sizeof(numberOfFiles), 1, f);
	fwrite(&addressOfList, sizeof(addressOfList), 1, f);
	DataFile *file;
	for(file = fileList.firstObject(); file; file = fileList.nextObject())
	{
		file->doWriteToFile(f);
	}
	uint64_t addr = FILE_HEADER_LENGTH + sizeof(numberOfFiles) + sizeof(addressOfList);
	uint64_t size;
	uint16_t len;
	for(file = fileList.firstObject(); file; file = fileList.nextObject())
	{
		len = strlen(file->getFilename());
		fwrite(&len, sizeof(len), 1, f);
		fputs(file->getFilename(), f);
		fwrite(&addr, sizeof(addr), 1, f);
		addr += file->getSize();
		size = file->getSize();
		fwrite(&size, sizeof(size), 1, f);
	}
}
*/
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