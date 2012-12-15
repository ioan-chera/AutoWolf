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
	// file header (as defined by subclasses)
	const char *header;
	// see if initialized
	bool initialized;
	
	// creates a new empty file
	void doInitializeEmpty(const char *fname, const char *hdr);
	// Execute writing to file
	virtual void doWriteToFile(FILE *f) = 0;
	
	const char fileHeader[9] = "Unknown";

public:
	
	
	DataFile() : filename(0), header(0), initialized(false)
	{
	}
	virtual ~DataFile()
	{
		delete filename;
		delete header;
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
	
	// export contents to file
	//virtual char *exportToString() const = 0;
	
	// do that other thing
	void initialize(const char *fname);
	
	// get file size
	virtual uint64_t getSize() = 0;
};

#endif

