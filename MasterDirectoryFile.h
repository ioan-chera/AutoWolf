//
//  MasterDirectoryFile.h
//  Wolf4SDL
//
//  Created by Ioan on 14.12.2012.
//
//

#ifndef MASTERDIRECTORYFILE_H_
#define MASTERDIRECTORYFILE_H_

#include "DirectoryFile.h"

#define MASTERDIR_MAPSDIRECTORY "Maps"
#define MASTERDIR_HEADER "AutoWolf"


//
// MasterDirectoryFile
//
// The Automatic Wolfenstein main directory structure that will get saved to file
//
class MasterDirectoryFile : public DirectoryFile
{
protected:
	const char masterDirectoryFileName[14] = "AutoWolf.data";
	
//	void doWriteToFile(FILE *f);
public:
	MasterDirectoryFile();
	
	static MasterDirectoryFile &MainDir();
	// save it to file
	void saveToFile();
	
	// load it from file
	bool loadFromFile();
};

#endif

