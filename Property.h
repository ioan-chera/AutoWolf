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


#ifndef __Wolf4SDL__Property__
#define __Wolf4SDL__Property__

#include <stdlib.h>

#include "m_dllist.h"

//
// Meant to be part of a PropertyFile, also addressed by a hash table
//
// Retrieving properties from disk file:
// - Free all properties from file (if any, unlikely)
// - Read serialized data.
// - Create Property objects from serialized data (key and value)
// - Add them to hash table of the file
//
// Accessing a property with a given name
// - Use objectforkey on hash table of file, choose string/number value
//
// Changing property in a file:
// - directly change number value, or set string value from accessor
//
// Adding a property to a file:
// - Create new property with new key and value
// - Link it to hash table
//
// Removing property from a file:
// - Unlink property from hash table
// - Free it
//
// Writing property to file:
// - access each object from hash table
// - serialize and write its contents

//
// Property
//
// Keyed string/number value
//

class Property
{
protected:
    // Length of string value (for quick changing)
    size_t _stringAllocLen;
    
    // Pointer to string value
    char *_stringValue;
public:
    // Key string for hashing
    char *_key;    
    
    // link of itself in hash table
    DLListItem <Property> link;
    
    // Integer value
    int32_t intValue;
    
    // Constructor
    Property(const char *key);
    // Destructor
    ~Property()
    {
        delete [] _key;
        delete [] _stringValue;
    }
    
    // Getter of string value
    const char *stringValue() const
    {
        return _stringValue;
    }
    
    // Getter of key
    const char *key() const
    {
        return _key;
    }
    
    // Setter of key
    void setKey(const char *newKey);
    
    // Setter to string value
    void setStringValue(const char *newValue);
};


#endif /* defined(__Wolf4SDL__Property__) */
