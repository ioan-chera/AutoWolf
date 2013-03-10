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


#include <string.h>
#include "wl_def.h"
#include "wl_main.h"
#include "MasterDirectoryFile.h"

char masterDirectoryFilePath[MAX_PATH_LENGTH];

////////////
//
// INSTANCES
//
////////////

MasterDirectoryFile mainDataFile;

////////////
//
// CONSTANTS
//
////////////

//
// DirectoryFile::DirectoryFile
//
MasterDirectoryFile::MasterDirectoryFile()
{
	filename = strdup(masterDirectoryFileName);
	filenamelen = strlen(filename);
	initialized = true;
    

	
	strcpy(header, MASTERDIR_HEADER);
}

//
// MasterDirectoryFile::MainDir
//
// Access the global main object
//
MasterDirectoryFile &MasterDirectoryFile::MainDir()
{
	return mainDataFile;
}

//
// MasterDirectoryFile::initializeConfigLocation
//
void MasterDirectoryFile::initializeConfigLocation()
{
    if(configdir[0] == '\0')
    {
        snprintf(masterDirectoryFilePath, sizeof(masterDirectoryFilePath), "%s"
                 , masterDirectoryFileName);
    }
    else
    {
        if(sizeof(masterDirectoryFileName) + 1 + strlen(configdir) > sizeof
           (masterDirectoryFilePath))
        {
            Quit("Your Autowolf.data directory path is too long. It cannot be used"
                 " for saving bot knowledge.");
        }
        
        snprintf(masterDirectoryFilePath, sizeof(masterDirectoryFilePath),
                 "%s/%s", configdir, masterDirectoryFileName);
    }
}

//
// MasterDirectoryFile::saveToFile
//
// save it to file
//
void MasterDirectoryFile::saveToFile()
{
	FILE *f;
	
	f = fopen(masterDirectoryFilePath, "wb");
	this->MasterDirectoryFile::doWriteToFile(f);
	fclose(f);
}

//
// MasterDirectoryFile::loadFromFile
//
// load it from file
//
// How is it supposed to know:
// As interface command, detect the AutoWolf header. Call the directory (or master directory?)
// internal function. That will read the address and number, and the access each file.
//
// Read each eight-character header, and depending on it, create a new object of each type
// and call that one's reader. When that is finished, go to the next address (remmeber the previous)
//
bool MasterDirectoryFile::loadFromFile()
{
	FILE *f;
	
	f = fopen(masterDirectoryFilePath, "rb");
	if(!f)
		return false;	// no file, no worry

	char getHeader[FILE_HEADER_LENGTH + 1];
	getHeader[FILE_HEADER_LENGTH] = 0;
	
	fread(getHeader, sizeof(char), FILE_HEADER_LENGTH, f);
	
	if(strcmp(getHeader, MASTERDIR_HEADER))
	{
		fclose(f);
		return false;
	}
	
	doReadFromFile(f);
	
	fclose(f);
	
	return true;
}