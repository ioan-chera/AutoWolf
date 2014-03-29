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

#include <time.h>
#include "wl_def.h"
#include "StdStringExtensions.h"
#include "wl_main.h"
#include "MasterDirectoryFile.h"
#include "Config.h"
#include "ShellUnicode.h"

//PString masterDirectoryFilePath;

////////////
//
// INSTANCES
//
////////////

MasterDirectoryFile masterDir;

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
	strcpy(_header, MASTERDIR_HEADER);
}

//
// MasterDirectoryFile::saveToFile
//
// save it to file
//
void MasterDirectoryFile::saveToFile(const std::string &fpath)
{
	FILE *f;
	
	// Only overwrite it when it's over. Till then, write to a temporary file
	std::string transaction(fpath);
	transaction += (unsigned long long)time(nullptr);
	
	f = ShellUnicode::fopen(transaction.c_str(), "wb");
	if(!f)
		return;
	this->doWriteToFile(f);
	fclose(f);
	
	bool success = false;
	if(ShellUnicode::remove(fpath.c_str()) == 0)
	{
		// success removing old fpath
		if(ShellUnicode::rename(transaction.c_str(), fpath.c_str()) == 0)
		{
			// success renaming the temp file to the old file
			success = true;
		}
	}
	if(!success)
		ShellUnicode::remove(transaction.c_str());
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
bool MasterDirectoryFile::loadFromFile(const std::string &fpath)
{
	FILE *f;
	
	f = ShellUnicode::fopen(fpath.c_str(), "rb");
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
	
	if(!DirectoryFile::doReadFromFile(f))
    {
        fclose(f);
        return false;
    }
	
	fclose(f);
	
	return true;
}