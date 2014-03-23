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


#ifndef MASTERDIRECTORYFILE_H_
#define MASTERDIRECTORYFILE_H_

#include "DirectoryFile.h"

#define MASTERDIR_HEADER "AutoWolf"

//
// MasterDirectoryFile
//
// The Automatic Wolfenstein main directory structure that will get saved to file
//
class MasterDirectoryFile : public DirectoryFile
{
	
public:
	MasterDirectoryFile();
	// save it to file
	void saveToFile(const std::string &fpath);
	// load it from file
	bool loadFromFile(const std::string &fpath);
};

extern MasterDirectoryFile masterDir;
#endif

