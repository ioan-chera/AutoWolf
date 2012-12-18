//
//  DirectoryFile.h
//  Wolf4SDL
//
//  Created by Ioan on 13.12.2012.
//
//

#ifndef DIRECTORYFILE_H_
#define DIRECTORYFILE_H_

#define DIRECTORY_HEADER "Director"

#include "DataFile.h"
#include "List.h"


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
class DirectoryFile : public DataFile
{
protected:
	// list of files
	List <DataFile *> fileList;	// FIXME: define a list which owns the referenced components
	// number of files
	uint32_t numberOfFiles;
	// address of list
	uint64_t addressOfList;
	
	void doWriteToFile(FILE *f);
	// Execute reading from file
	bool doReadFromFile(FILE *f);
public:
	DirectoryFile();
	~DirectoryFile()
	{
		DataFile *file, *next;
		for(file = fileList.firstObject(); file; file = next)
		{
			next = fileList.nextObject();
			delete file;
		}
	}
	
	// now size
	uint64_t getSize();
	
	// add file to list
	bool addFile(DataFile *file);
	
	// access file with name
	DataFile *getFileWithName(const char *fname, size_t nchar = 0);
	
	// create folder if not exist
	DirectoryFile *makeDirectory(const char *fname, size_t nchar = 0);
};

#endif

