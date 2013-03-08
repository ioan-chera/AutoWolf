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


#include "Property.h"

#define NEW_ALLOC_2_RATIO 3

//
// Property::setStringValue
//
// Sets the _stringValue destination to a string
//
#if 0
void Property::setStringValue(const char *newValue)
{
    size_t newAllocLen = strlen(newValue) + 1;
    
    if(newAllocLen > _stringAllocLen || newAllocLen << NEW_ALLOC_2_RATIO < 
       _stringAllocLen)
    {
        delete []_stringValue;
        _stringValue = new char[newAllocLen];
        _stringAllocLen = newAllocLen;
    }
    strcpy(_stringValue, newValue);
}

//
// Property::setKey
//
// Sets the _key destination to a string
//
void setKey(const char *newKey)
{
    // FIXME: this isn't optimized at all
    delete [] _key;
    _key = new char[strlen(newKey) + 1];
    strcpy(_key, newKey);
}
#endif
