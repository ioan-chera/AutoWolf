//
//  DataFile.h
//  Wolf4SDL
//
//  Created by Ioan on 13.12.2012.
//
//

#ifndef DATAFILE_H_
#define DATAFILE_H_

#include <stdint.h>
#include <stdio.h>

#define FILE_HEADER_LENGTH 8

//
// General info
//
// All data files start with 8-byte header with the file type (identifier)
// The name is defined by the class.
// If called from here, header remains "Unknown"
//

//
// DataFile
//
// Automatic Wolfenstein bot data file
//
class DataFile
{
	friend class DirectoryFile;
protected:
	// name of file (appears as directory entry or outer filename)
	char *filename;
	// length of filename
	size_t filenamelen;
	// file header (as defined by subclasses)
	char header[9];
	// see if initialized
	bool initialized;
	
	// creates a new empty file
	void doInitializeEmpty(const char *fname, size_t nchar = 0);
	// Execute writing to file
	virtual void doWriteToFile(FILE *f) = 0;

public:
	
	
	DataFile();
	virtual ~DataFile()
	{
		delete filename;
	}
	
	// accessor methods
	const char *getFilename() const
	{
		return filename;
	}
	const char *getHeader() const
	{
		return header;
	}
	size_t getFilenameLen() const
	{
		return filenamelen;
	}
	
	// export contents to file
	//virtual char *exportToString() const = 0;
	
	// do that other thing
	void initialize(const char *fname, size_t nchar = 0);
	
	// get file size
	virtual uint64_t getSize() = 0;
};

#endif

