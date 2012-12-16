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
// DirectoryFile::DirectoryFile
//
DirectoryFile::DirectoryFile() : numberOfFiles(0), addressOfList(0)
{
	strcpy(header, DIRECTORY_HEADER);
}

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
// DirectoryFile::makeDirectory
//
// create folder if not exist
//
DirectoryFile *DirectoryFile::makeDirectory(const char *fname, size_t nchar)
{
	DataFile *findDir = getFileWithName(fname, nchar);
	if(!findDir || (findDir && strcmp(findDir->getHeader(), DIRECTORY_HEADER)))
	{
		// either doesn't exist or is not a folder
		DirectoryFile *newdir = new DirectoryFile;
		newdir->initialize(fname, nchar);
		addFile(newdir);
		
		return newdir;
	}
	
	// exists and is a folder
	return (DirectoryFile *)findDir;
}

//
// DirectoryFile::getFileWithName
//
// access file with name
//
DataFile *DirectoryFile::getFileWithName(const char *fname, size_t nchar)
{
	// FIXME: Implement faster, proven searching methods than this
	DataFile *file;
	for(file = fileList.firstObject(); file; file = fileList.nextObject())
	{
		if(nchar <= 0)
		{
			if(!strcmp(file->getFilename(), fname))
				return file;
		}
		else
		{
			if(!memcmp(file->getFilename(), fname, nchar))
				return file;
		}
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
		ret += sizeof(uint16_t) + file->getFilenameLen() + sizeof(uint64_t) + sizeof(uint64_t);
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
	fputs(header, f);
	fwrite(&numberOfFiles, sizeof(numberOfFiles), 1, f);
	fwrite(&addressOfList, sizeof(addressOfList), 1, f);
	DataFile *file;
	uint64_t size = 0, addr;
	for(file = fileList.firstObject(); file; file = fileList.nextObject())
	{
		file->doWriteToFile(f);
		size += file->getSize();	// FIXME: try to cache that
	}
	addr = ftell(f);
	
	fseek(f, -size - sizeof(addressOfList), SEEK_CUR);
	addressOfList = FILE_HEADER_LENGTH + sizeof(numberOfFiles) + sizeof(addressOfList) + size;
	fwrite(&addressOfList, sizeof(addressOfList), 1, f);
	fseek(f, size, SEEK_CUR);
	
	addr = FILE_HEADER_LENGTH + sizeof(numberOfFiles) + sizeof(addressOfList);
	
	uint16_t len;
	for(file = fileList.firstObject(); file; file = fileList.nextObject())
	{
		len = file->getFilenameLen();
		fwrite(&len, sizeof(len), 1, f);
		fwrite(file->getFilename(), sizeof(uint8_t), len, f);
		fwrite(&addr, sizeof(addr), 1, f);
		size = file->getSize();
		addr += size;
		fwrite(&size, sizeof(size), 1, f);
	}
}

