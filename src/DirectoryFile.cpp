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


#include "wl_def.h"
#include "DirectoryFile.h"

//
// DirectoryFile::DirectoryFile
//
DirectoryFile::DirectoryFile() : numberOfFiles(0), addressOfList(0)
{
	strcpy(_header, DIRECTORY_HEADER);
}

//
// DirectoryFile::~DirectoryFile
//
DirectoryFile::~DirectoryFile()
{
    DataFile *file = NULL;

    while((file = fileHash.tableIterator(file)))
    {
        fileHash.removeObject(file);
        delete file;
    }
}

//
// DirectoryFile::addFile
//
// Add file to directory list
// The file has to live through
//
bool DirectoryFile::addFile(DataFile *file)
{
	if(!file || !file->header())	// headerless files are invalid
		return false;
	
    fileHash.addObject(file);
    file->_containerFile = this;
    
	++numberOfFiles;
    
    _updateSize();
	
	return true;
}

//
// DirectoryFile::makeDirectory
//
// create folder if not exist
//
DirectoryFile *DirectoryFile::makeDirectory(const PString &fname)
{
	DataFile *findDir = getFileWithName(fname);
	if(!findDir || (findDir && strcmp(findDir->header(), DIRECTORY_HEADER)))
	{
		// either doesn't exist or is not a folder
		DirectoryFile *newdir = new DirectoryFile;
		newdir->initialize(fname);
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
DataFile *DirectoryFile::getFileWithName(const PString &fname)
{
	// FIXME: Implement faster, proven searching methods than this
    return fileHash.objectForKey(fname);
}

//
// DirectoryFile::_updateSize
//
void DirectoryFile::_updateSize()
{
    _size = FILE_HEADER_LENGTH + sizeof(numberOfFiles) + sizeof(addressOfList);
	
	// content
	DataFile *file = NULL;
    while((file = fileHash.tableIterator(file)))
	{
		// content
		_size += file->size();
		
		// directory entry
		_size += sizeof(uint16_t) + file->filename().length() +
                sizeof(uint64_t);
	}
	
    DataFile::_updateSize();
}

//
// DirectoryFile::doWriteToFile
//
// Execute writing it to file, being given a FILE * variable
//
void DirectoryFile::doWriteToFile(FILE *f)
{
	fwrite(_header, sizeof(char), FILE_HEADER_LENGTH, f);
	fwrite(&numberOfFiles, sizeof(numberOfFiles), 1, f);
	fwrite(&addressOfList, sizeof(addressOfList), 1, f);
	DataFile *file = NULL;
	uint64_t size = 0, addr;
    while((file = fileHash.tableIterator(file)))
	{
		file->doWriteToFile(f);
		size += file->size();	// FIXME: try to cache that
	}
	addr = ftell(f);
	
	fseek(f, -(long)size - (long)sizeof(addressOfList), SEEK_CUR);
	addressOfList = FILE_HEADER_LENGTH + sizeof(numberOfFiles) + sizeof(addressOfList) + size;
	fwrite(&addressOfList, sizeof(addressOfList), 1, f);
	fseek(f, (long)size, SEEK_CUR);
	
	addr = FILE_HEADER_LENGTH + sizeof(numberOfFiles) + sizeof(addressOfList);
	
	uint16_t len;
    for(file = fileHash.tableIterator(NULL); file;
        file = fileHash.tableIterator(file))
	{
		len = (uint16_t)file->filename().length();
		fwrite(&len, sizeof(len), 1, f);
		fwrite(file->filename().buffer(), sizeof(uint8_t), len, f);
		fwrite(&addr, sizeof(addr), 1, f);
		size = file->size();
		addr += size;
	}
}

//
// ExploredArrayFile::doReadFromFile
//
// Execute reading from file
//
bool DirectoryFile::doReadFromFile(FILE *f)
{
	uint64_t baseaddr = ftell(f) - FILE_HEADER_LENGTH;
	uint64_t fileaddr, curaddr;
	// what have we here…
	// 32 number of files
	// 64 address of list. How do?
	
	// assume correctness, master
	uint32_t numFiles;
	fread(&numFiles, sizeof(uint32_t), 1, f);
	fread(&addressOfList, sizeof(uint64_t), 1, f);
	
	int seekresult = fseek(f, (long)(baseaddr + addressOfList), SEEK_SET);
    if(seekresult < 0)  // error
        return false;
	
	uint32_t i;
	uint16_t namelen, oldnamelen = 0;
	char *fname = NULL;
	char filehead[FILE_HEADER_LENGTH + 1];
	filehead[FILE_HEADER_LENGTH] = 0;
	
	DataFile *newFile;
	for(i = 0; i < numFiles; ++i)
	{
		// read length
		fread(&namelen, sizeof(uint16_t), 1, f);
		if(namelen > oldnamelen)
		{
			delete [] fname;
			fname = new char[namelen];
			oldnamelen = namelen;
		}
		fread(fname, sizeof(uint8_t), namelen, f);
		fread(&fileaddr, sizeof(uint64_t), 1, f);
		curaddr = ftell(f);	// address of next entry tag
		
		fseek(f, (long)(baseaddr + fileaddr), SEEK_SET);
		
		// read header
		fread(filehead, sizeof(char), FILE_HEADER_LENGTH, f);
		
		// oh dear
		
        if(!strcmp(filehead, DIRECTORY_HEADER))
		   newFile = new DirectoryFile;
        else if(!strcmp(filehead, PROPERTY_FILE_HEADER))
           newFile = new PropertyFile;
		else// unknown, skip
		{
			fseek(f, (long)curaddr, SEEK_SET);
			continue;
		}
		
		newFile->initialize(PString(fname, namelen));
		
		newFile->doReadFromFile(f);
		
		addFile(newFile);
		
		// now back to business
		fseek(f, (long)curaddr, SEEK_SET);
	}
	
	delete [] fname;
	return true;
}

