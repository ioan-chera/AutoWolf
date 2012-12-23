//
//  DictionaryFile.h
//  Wolf4SDL
//
//  Created by Ioan on 23.12.2012.
//
//

#ifndef DICTIONARYFILE_H_
#define DICTIONARYFILE_H_

#include "DataFile.h"
#include "Dictionary.h"

//
// DictionaryFile
//
// A file that stores key/value pairs
//
class DictionaryFile : public DataFile
{
protected:
	// Execute writing to file
	void doWriteToFile(FILE *f);
	// Execute reading from file
	bool doReadFromFile(FILE *f);
	
	// Data content
	Dictionary dictionary;
	
public:
	// get file size
	uint64_t getSize() = 0;
};

#endif
