//
//  DirectoryFile.h
//  Wolf4SDL
//
//  Created by Ioan on 13.12.2012.
//
//

#ifndef DIRECTORYFILE_H_
#define DIRECTORYFILE_H_

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
//   uint64_t length of contained file
//
class DirectoryFile : public DataFile
{
protected:
	// list of files
	List <DataFile *> fileList;	// FIXME: define a list which owns the referenced components
	// number of files
	uint32_t numberOfFiles;
	// address of list
	uint64_t addressOfList;	// FIXME: really needed?
	
	void doWriteToFile(FILE *f);
	
	const char fileHeader[9] = "Director";
public:
	DirectoryFile() : numberOfFiles(0), addressOfList(0)
	{
		// initialize empty
	}
	~DirectoryFile()
	{
		// TODO: delete all files, then empty the list
	}
	
	// now size
	uint64_t getSize();
	
	// add file to list
	bool addFile(DataFile *file);
	
	// access file with name
	const DataFile *getFileWithName(const char *fname);
};

#endif

