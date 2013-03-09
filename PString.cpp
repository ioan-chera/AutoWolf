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

#include <ctype.h>
#include <string.h>

#include "e_hashkeys.h"

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
// PString::HashCodeCaseStatic
//
// As above, but with case sensitivity.
//
unsigned int PString::HashCodeCaseStatic(const char *str, size_t inLength)
{
    return D_HashTablePKeyCase(str ? str : "", inLength);
}

//
// PString::HashCodeStatic
//
// Static version, for convenience and so that the convention of hashing a
// null pointer to 0 hash code is enforceable without special checks, even
// if the thing being hashed isn't a PString instance.
//
unsigned int PString::HashCodeStatic(const char *str, size_t inLength)
{
    return D_HashTablePKey(str ? str : "", inLength);
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

PString::PString(const PString &other) : _index(0), _size(PSTRING_LOCAL_LENGTH)
{
    _buffer = _local;
    memset(_local, 0, sizeof(_local));
    copy(other);
}

PString::PString(size_t startSize) : _index(0), _size(PSTRING_LOCAL_LENGTH)
{
    _buffer = _local;
    memset(_local, 0, sizeof(_local));
    if(startSize)
        initCreateSize(startSize);
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
// PString::clearOrCreate
//
// Creates a PString, or clears it if it is already valid.
//
PString &PString::clearOrCreate(size_t pSize)
{
    return createSize(pSize);
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

PString &PString::concat(const PString &src)
{
    return concat(src._buffer, src._index);
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

PString &PString::copy(const PString &src)
{
    if(_index > 0)
        clear();
    
    return concat(src);
}

//
// PString::create
//
// Gives the PString a buffer of the default size and initializes
// it to zero. Resets insertion point to zero. This is safe to call
// on an existing qstring to reinitialize it.
//
PString &PString::create()
{
    return createSize(basesize);
}

//
// PString::createSize
//
// Creates a PString with a given initial size, which helps prevent
// unnecessary initial reallocations. Resets insertion point to zero.
// This is safe to call on an existing PString to reinitialize it.
//
PString &PString::createSize(size_t pSize)
{
    // Can remain local?
    if(_isLocal() && pSize <= basesize)
        clear();
    else
    {
        _unLocalize(pSize);
        
        // Don't realloc if not needed
        if(_size < pSize)
        {
            _buffer = (char *)realloc(_buffer, pSize);
            _size   = pSize;
        }
        clear();
    }
    
    return *this;
}

//
// PString::Delc
//
// Deletes a character from the end of the qstring.
//
PString &PString::Delc()
{
    if(_index > 0)
    {
        _index--;
        _buffer[_index] = '\0';
    }
    
    return *this;
}

//
// PString::freeBuffer
//
// Frees the PString object. It should not be used after this,
// unless PString::create is called on it. You don't have to free
// a PString before recreating it, however, since it uses realloc.
//
void PString::freeBuffer()
{
    if(_buffer && !_isLocal())
        free(_buffer);
    
    // return to being local
    _buffer = _local;
    _size   = basesize;
    clear();
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

//
// PString::initCreate
//
// Initializes a PString struct to all zeroes, then calls
// PString::create. This is for safety the first time a PString
// is created (if PString::_buffer is uninitialized, realloc will
// crash).
//
PString &PString::initCreate()
{
    _buffer = _local;
    _size   = basesize;
    clear();
    
    return create();
}

//
// PString::initCreateSize
//
// Initialization and creation with size specification.
//
PString &PString::initCreateSize(size_t pSize)
{
    _buffer = _local;
    _size   = basesize;
    clear();
    
    return createSize(pSize);
}

//
// PString::insert
//
// Inserts a string at a given position in the PString.
//
PString &PString::insert(const char *insertstr, size_t inLength, size_t pos)
{
    char *insertpoint;
    size_t charstomove;
    size_t totalsize    = _index + inLength;
    
    // pos must be between 0 and dest->index - 1
    if(pos >= _index)
    {
        // if beyond, just concatenate it.
        return concat(insertstr, inLength);
    }
    
    // grow the buffer to hold the resulting string if necessary
    if(totalsize > _size)
        grow(totalsize - _size);
    
    insertpoint = _buffer + pos;
    charstomove = _index  - pos;
    
    // use memmove for absolute safety
    memmove(insertpoint + inLength, insertpoint, charstomove);
    memmove(insertpoint, insertstr, inLength);
    
    _index = totalsize;
    
    return *this;
}

//
// PString::Putc
//
// Adds a character to the end of the PString, reallocating via buffer doubling
// if necessary.
//
PString &PString::Putc(char ch)
{
    if(_index >= _size - 1) // leave room for \0
        grow(_size);        // double buffer size
    
    _buffer[_index++] = ch;
    
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

//
// PString::charAt
//
// Indexing function to access a character in a PString.
//
char PString::charAt(size_t idx) const
{
    if(idx >= _size)
        return 0;
    
    // I_Error("qstring::charAt: index out of range\n");
    
    return _buffer[idx];
}

//
// PString::compare
//
// C++ style comparison. True return value means it is equal to the argument.
//
bool PString::compare(const char *str, size_t inLength) const
{
    return !memcmp(_buffer, str, inLength);
}

bool PString::compare(const PString &other) const
{
    return !memcmp(_buffer, other._buffer, _index);
}

//
// PString::hashCode
//
// Calls the standard D_HashTablePKey that is used for the vast majority of
// string hash code computations.
//
unsigned int PString::hashCode() const
{
    return HashCodeStatic(_buffer, _index);
}

//
// PString::hashCodeCase
//
// Returns a hash code computed with the case of characters being treated as
// relevant to the computation.
//
unsigned int PString::hashCodeCase() const
{
    return HashCodeCaseStatic(_buffer, _index);
}

//
// PString::strCmp
//
// C-style string comparison/collation ordering.
//
int PString::strCmp(const char *str, size_t inLength) const
{
    return memcmp(_buffer, str, inLength);
}

//
// _memcasecmp
//
// Case insensitive memcmp
// Copied and developed from:
// http://stackoverflow.com/questions/5017659/implementing-memcmp
//
static int _memcasecmp(const void* buf1, const void* buf2, size_t count)
{
    if(!count)
        return 0;
    
    while(--count && toupper(*(char *)buf1) == toupper(*(char *)buf2 ))
    {
        buf1 = (char *)buf1 + 1;
        buf2 = (char *)buf2 + 1;
    }
    
    return *((unsigned char *)buf1) - *((unsigned char *)buf2);
}

//
// PString::strCaseCmp
//
// Case-insensitive C-style string compare.
//
int PString::strCaseCmp(const char *str, size_t inLength) const
{
    return _memcasecmp(_buffer, str, inLength);
}