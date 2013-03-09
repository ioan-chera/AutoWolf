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
// ALSO: no errors allowed.
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
    
    // Statics
    static unsigned int HashCodeCaseStatic(const char *str, size_t inLength);
    static unsigned int HashCodeStatic(const char *str, size_t inLength);
    
    // Constructor
    PString(const char *cstr, size_t inLength);
    PString(const PString &other);
    PString(size_t startSize = 0);
    
    // Destructor
    ~PString() { freeBuffer(); }
    
    // Manipulators
    PString &clear();
    PString &clearOrCreate(size_t size);
    PString &concat(const char *str, size_t inLength);
    PString &concat(const PString &src);
    PString &copy(const char *str, size_t inLength);
    PString &copy(const PString &src);
    PString &create();
    PString &createSize(size_t size);
    PString &Delc();
    void     freeBuffer();
    PString &grow(size_t len);
    PString &initCreate();
    PString &initCreateSize(size_t size);
    PString &insert(const char *insertstr, size_t inLength, size_t pos);
    PString &Putc(char ch);
    
    // Accessors
    const char *buffer() const { return _buffer; }
    char        charAt(size_t idx) const;
    bool        compare(const char *str, size_t inLength) const;
    bool        compare(const PString &other) const;
    unsigned int hashCode() const;      // case-insensitive
    unsigned int hashCodeCase() const;  // case-considering
    size_t      length() const { return _index;  }
    size_t      size() const { return _size;  }
    int         strCmp(const char *str, size_t inLength) const;
    int         strCaseCmp(const char *str, size_t inLength) const;

    
    // Returns string length
//    size_t length() {return _length;}
    
};

#endif /* defined(__Wolf4SDL__PString__) */
