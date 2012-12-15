//
//  ExploredArrayFile.h
//  Wolf4SDL
//
//  Created by Ioan on 14.12.2012.
//
//

#ifndef EXPLOREDARRAYFILE_H_
#define EXPLOREDARRAYFILE_H_

#include "wl_def.h"
#include "DataFile.h"

#define DATAFILE_EXPLORED_NAME "Explored"

//
// ExploredArrayFile
//
// The map array file
// Has a MD5 checksum name, and contains map data
//
// THE STRUCTURE IN GENERAL
//
// A folder with a MD5 checksum name contains several informations
// Let's now name this simply ExploredFile. Until a release, I can make up my mind how to name them
// to make them future-proof and avoid frequent deprecations

class ExploredArrayFile : public DataFile
{
protected:
	// Execute writing to file
	void doWriteToFile(FILE *f);
	// data stuff
	uint8_t explored[MAPSIZE][MAPSIZE];
	
	const char fileHeader[9] = "Explored";
public:
	ExploredArrayFile()
	{
		memset(explored, 0, sizeof(explored));
	}
	// get file size
	uint64_t getSize();
	
	// copy explored data into target
	void getData(void *putTarget) const;
	
	// inverse
	void putData(const void *getSource);
};

#endif
