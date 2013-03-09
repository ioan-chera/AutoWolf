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

#ifndef __Wolf4SDL__PString__
#define __Wolf4SDL__PString__

#include <stdlib.h>

#define PSTRING_LOCAL_LENGTH 32

//
// PString
//
// "Pascal" string, i.e. string with attached length value, so it can contain
// null characters and whose size can be expected early from disk reading.
//
// Largely inspired on Quasar's Eternity Engine qstring class
// IMPORTANT: no strcmp, strcat, strcpy, strlen allowed!
//            no null terminator!
//
class PString
{
protected:
    char _local[PSTRING_LOCAL_LENGTH];
    char *_buffer;  // Buffer of string
    size_t _size; // Length of string
    size_t _index;
    
    bool _isLocal() const { return (_buffer == _local); }
    void _unLocalize(size_t pSize);
    
public:
    static const size_t basesize;
    
    // Constructor
    PString(const char *cstr, size_t inLength);
    
    // Destructor
    ~PString()
    {
        delete [] _buffer;
    }
    
    // Functions
    PString &clear();
    PString &concat(const char *str, size_t inLength);
    PString &copy(const char *str, size_t inLength);
    PString &grow(size_t len);
    
    // Returns string length
//    size_t length() {return _length;}
    
};

#endif /* defined(__Wolf4SDL__PString__) */
