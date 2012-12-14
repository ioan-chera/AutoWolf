//
//  DirectoryFile.cpp
//  Wolf4SDL
//
//  Created by Ioan on 13.12.2012.
//
//

#include <string.h>
#include "DirectoryFile.h"

//
// DirectoryFile::addFile
//
// Add file to directory list
// The file has to live through
//
bool DirectoryFile::addFile(DataFile *file)
{
	if(!file || !file->getHeader())	// headerless files are invalid
		return false;
	
	fileList.add(file);
	++numberOfFiles;
	// that has to be it?
	
	return true;
}

//
// DirectoryFile::getFileWithName
//
// access file with name
//
const DataFile *DirectoryFile::getFileWithName(const char *fname)
{
	// FIXME: Implement faster, proven searching methods than this
	const DataFile *file;
	for(file = fileList.firstObject(); file; file = fileList.nextObject())
	{
		if(!strcasecmp(file->getFilename(), fname))
			return file;
	}
	return NULL;
}

//
// DirectoryFile::getSize
//
// calculate the size
//
uint64_t DirectoryFile::getSize()
{
	// size of:
	// 8 header
	// 4 number of files
	// 8 address of file list
	// length of each file
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
	//   uint64_t length of contained file
	
	// FIXME: get cache from doWriteToFile
	
	// header length + number of entries + address of entry list
	uint64_t ret = FILE_HEADER_LENGTH + sizeof(numberOfFiles) + sizeof(addressOfList);
	
	// content
	DataFile *file;
	for(file = fileList.firstObject(); file; file = fileList.nextObject())
	{
		// content
		ret += file->getSize();
		
		// directory entry
		ret += sizeof(uint16_t) + strlen(file->getFilename()) + sizeof(uint64_t) + sizeof(uint64_t);
	}
	
	return ret;
}

//
// DirectoryFile::doWriteToFile
//
// Execute writing it to file, being given a FILE * variable
//
void DirectoryFile::doWriteToFile(FILE *f)
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