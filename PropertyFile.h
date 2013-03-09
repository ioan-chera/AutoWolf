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

#ifndef PROPERTYFILE_H_
#define PROPERTYFILE_H_

#define PROPERTY_FILE_HEADER "Property"

#include "e_hash.h"
#include "Property.h"
#include "DataFile.h"

// How to declare a hash table
//
// EHashTable<ACSScript, EIntHashKey, &ACSScript::number, 
// &ACSScript::numberLinks> acsScriptsByNumber;

//
// PropertyFile
//
// File containing a hash table of Property objects. Thanks to Quasar for the
// hash table structure.
//
class PropertyFile : public DataFile
{
protected:
    // Execute writing to file
	void doWriteToFile(FILE *f);
	// Execute reading from file
	bool doReadFromFile(FILE *f);
public:
    PropertyFile();
    // the content
    EHashTable<Property, EStringHashKey, &Property::_key, &Property::link>
        propertyTable;
    
    // get file size
	uint64_t getSize();
};

#endif
