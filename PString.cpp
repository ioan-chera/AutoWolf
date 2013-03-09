//
// Copyright (C) 2013  Ioan Chera
// Copyright (C) 2004 James Haley
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

#include <stdlib.h>
#include <string.h>

#include "PString.h"

const size_t PString::basesize = PSTRING_LOCAL_LENGTH;

// Many of the functions here have been taken from Eternity Engine's qstring
// class and updated to work on pstrings, thus the copyright notice above.

//
// PString::_unLocalize
//
void PString::_unLocalize(size_t pSize)
{
    if(_isLocal())
    {
        _buffer = (char *)calloc(1, pSize);
        _size   = pSize;
        memcpy(_buffer, _local, _index);
//        strcpy(_buffer, _local);
        
        memset(_local, 0, basesize);
    }
}

//
// PString::PString
//
// Constructor with given cstr and inLength
//
PString::PString(const char *cstr, size_t inLength) : _index(0),
_size(PSTRING_LOCAL_LENGTH)
{
    _buffer = _local;
    memset(_local, 0, sizeof(_local));
    copy(cstr, inLength);
}

//
// PString::clear
//
// Sets the entire PString buffer to zero, and resets the insertion index.
// Does not reallocate the buffer.
//
PString &PString::clear()
{
    memset(_buffer, 0, _size);
    _index = 0;
    
    return *this;
}

//
// PString::concat
//
// Concatenates a C string onto the end of a PString, expanding the buffer if
// necessary.
//
PString &PString::concat(const char *str, size_t inLength)
{
    size_t cursize = _size;
    size_t newsize = _index + inLength;
    
    if(newsize > cursize)
        grow(newsize - cursize);
    memcpy(_buffer + _index, str, inLength);
    
    _index = newsize;
    
    return *this;
}

//
// PString::copy
//
// Copies a C string into the PString. The PString is cleared first,
// and then set to the contents of *str.
//
PString &PString::copy(const char *str, size_t inLength)
{
    if(_index > 0)
        clear();
    
    return concat(str, inLength);
}

//
// PString::grow
//
// Grows the PString's buffer by the indicated amount. This is automatically
// called by other PString methods, so there is generally no need to call it
// yourself.
//
PString &PString::grow(size_t len)
{
    if(len > 0)
    {
        size_t newsize = _size + len;
        
        if(_isLocal()) // are we local?
        {
            if(newsize > basesize) // can we stay local?
                _unLocalize(newsize);
        }
        else
        {
            _buffer = (char *)realloc(_buffer, newsize);
            memset(_buffer + _size, 0, len);
            _size += len;
        }
    }
    
    return *this;
}