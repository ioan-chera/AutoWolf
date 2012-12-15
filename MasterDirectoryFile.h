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


//
// MasterDirectoryFile
//
// The Automatic Wolfenstein main directory structure that will get saved to file
//
class MasterDirectoryFile : public DirectoryFile
{
protected:
	const char fileHeader[9] = "AutoWolf";
	const char masterDirectoryFileName[14] = "AutoWolf.data";
	
//	void doWriteToFile(FILE *f);
public:
	
	static MasterDirectoryFile &MainDir();
	// save it to file
	void saveToFile();
};

#endif

