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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#pragma pack(1)
#include "e_hashkeys.h"

#include "PString.h"
#include "Exception.h"

#ifdef _WIN32
    #define strcasecmp stricmp
    #define strncasecmp strnicmp
    #define snprintf _snprintf
#endif

#if defined(_WIN32) || defined(APPLE_NON_CPP11) || defined(MEMMEM_NOT_DEFINED)
//
// memmem
//
// Missing from Windows; copied and edited from:
// http://stackoverflow.com/questions/2188914/c-searching-for-a-string-in-a-file
//
void *memmem(const void *str, size_t hlen, const void *substr, size_t nlen)
{
    int needle_first;
    const char *p = (const char *)str;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = *(unsigned char *)substr;

    while (plen >= nlen && (p = (const char *)memchr(p, needle_first, 
        plen - nlen + 1)))
    {
        if (!memcmp(p, substr, nlen))
            return (void *)p;

        p++;
        plen = hlen - (p - (const char *)str);
    }

    return NULL;
}

#endif

const size_t PString::npos     = ((size_t)-1);
const size_t PString::basesize = PSTRING_LOCAL_LENGTH;

#define PSTRING_REPLACE_LENGTH 256

static unsigned char pstr_repltable[PSTRING_REPLACE_LENGTH];

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

PString::PString(const char *cstr) : _index(0), _size(PSTRING_LOCAL_LENGTH)
{
    _buffer = _local;
    memset(_local, 0, sizeof(_local));
    copy(cstr);
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

PString::PString(int number) : _index(0), _size(PSTRING_LOCAL_LENGTH)
{
    _buffer = _local;
    memset(_local, 0, sizeof(_local));
    copy(number);
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

PString &PString::concat(const char *str)
{
    return concat(str, strlen(str));
}

PString &PString::concat(const PString &src)
{
    return concat(src._buffer, src._index);
}

PString &PString::concat(int number)
{
    static char buf[STRING_ASSUMED_DEC_NUMBER_LENGTH];
    int nchar = snprintf(buf, STRING_ASSUMED_DEC_NUMBER_LENGTH, "%d", number);
    if(nchar >= STRING_ASSUMED_DEC_NUMBER_LENGTH)
        throw Exception("PString number too large");
    return concat(buf, nchar);
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

PString &PString::copy(const char *str)
{
    if(_index > 0)
        clear();
    return concat(str);
}

PString &PString::copy(const PString &src)
{
    if(_index > 0)
        clear();
    
    return concat(src);
}

PString &PString::copy(int number)
{
    if(_index > 0)
        clear();
    return concat(number);
}

////////////////////////////////////////////////////////////////////////////////

//
// PString::addDefaultExtension
//
// Note: an empty string will not be modified.
//
PString &PString::addDefaultExtension(const char *ext, size_t inLength)
{
    char *p = _buffer;
    
    if(_index > 0)
    {
        p = p + _index - 1;
        while(p-- > _buffer && *p != '/' && *p != '\\')
        {
            if(*p == '.')
                return *this; // has an extension already.
        }
        if(*ext != '.') // need a dot?
            *this += '.';
        concat(ext, inLength);   // add the extension
    }
    
    return *this;
}

PString &PString::addDefaultExtension(const char *ext)
{
    return addDefaultExtension(ext, strlen(ext));
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
// PString::erase
//
// std::string-compatible erasure function.
//
PString &PString::erase(size_t pos, size_t n)
{
    // truncate handles the case of n == qstring::npos
    if(!n)
        return *this;
    else if(n == npos)
        return truncate(pos);
    
    // pos must be between 0 and qstr->index - 1
    if(pos >= _index)
        return *this;
    
    size_t endPos = pos + n;
    if(endPos > _index)
        endPos = _index;
    
    char *to   = _buffer + pos;
    char *from = _buffer + endPos;
    char *end  = _buffer + _index;
    
    while(to != end)
    {
        *to = *from;
        ++to;
        if(from != end)
            ++from;
    }
    
    _index -= (endPos - pos);
    return *this;
}

//
// PString::extractFileBase
//
// This one is not limited to 8 character file names, and will include any
// file extension, however, so it is not strictly equivalent.
//
void PString::extractFileBase(PString &dest)
{
    const char *src = _buffer + _index - 1;
    dest.copy("", 0);
    
    // back up until a \ or the start

    while(src != _buffer &&
          *(src - 1) != ':' &&
          *(src - 1) != '\\' &&
          *(src - 1) != '/')
    {

        --src;
    }
    
    dest.copy(src, _index - (size_t)src);
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
PString &PString::insert(const char *insertstr, size_t pos)
{
    return insert(insertstr, strlen(insertstr), pos);
}

//
// PString::lstrip
//
// Removes occurrences of a specified character at the beginning of a PString.
//
PString &PString::lstrip(char c)
{
    size_t i   = 0;
    size_t len = _index;
    
    while(i < _index && _buffer[i] == c)
        ++i;
    
    if(i)
    {
        if((len -= i) == 0)
            clear();
        else
        {
            memmove(_buffer, _buffer + i, len);
            memset(_buffer + len, 0, _size - len);
            _index -= i;
        }
    }
    
    return *this;
}

//
// PString::makeQuoted
//
// Adds quotation marks to the PString.
//
PString &PString::makeQuoted()
{
    // if the string is empty, make it "", else add quotes around the contents
    if(_index == 0)
        return concat("\"\"", 2);
    else
    {
        insert("\"", 1, 0);
        return Putc('\"');
    }
}

//
// _NormalizeSlashes
//
// Remove trailing slashes, translate backslashes to slashes
// The string to normalize is passed and returned in str
//
// killough 11/98: rewritten
// IOANCH 20130309: copied from Eternity
//
static void _NormalizeSlashes(char *str, size_t inLength)
{
    char *p;
    char useSlash      = '/'; // The slash type to use for normalization.
    char replaceSlash = '\\'; // The kind of slash to replace.
    bool isUNC = false;
    
#ifdef _WIN32

    // This is an UNC path; it should use backslashes.
    // NB: We check for both in the event one was changed earlier by mistake.
    if(strlen(str) > 2 &&
       ((str[0] == '\\' || str[0] == '/') && str[0] == str[1]))
    {
        useSlash = '\\';
        replaceSlash = '/';
        isUNC = true;
    }

#endif
    
    // Convert all replaceSlashes to useSlashes
    size_t i;
    for(p = str, i = 0; i < inLength; p++, i++)
    {
        if(*p == replaceSlash)
            *p = useSlash;
    }
    
    // Remove trailing slashes
    while(p > str && *--p == useSlash)
        *p = 0;
    
    // Collapse multiple slashes
    const char *endpos = str + inLength;
    // IOANCH: I hope it works...
    for(p = str + (isUNC ? 2 : 0); (*str++ = *p), str != endpos; )
        if(*p++ == useSlash)
            while(*p == useSlash)
                p++;
}

//
// PString::normalizeSlashes
//
// Calls M_NormalizeSlashes on a PString, which replaces \ characters with /
// and eliminates any duplicate slashes. This isn't simply a convenience
// method, as the PString structure requires a fix-up after this function is
// used on it, in order to keep the string length correct.
//
PString &PString::normalizeSlashes()
{
    _NormalizeSlashes(_buffer, _index);
    _index = strlen(_buffer);
    
    return *this;
}

//
// PString::pathConcatenate
//
// Concatenate a C string assuming the PString's current contents are a file
// path. Slashes will be normalized.
//
PString &PString::pathConcatenate(const char *addend, size_t inLength)
{
    // Only add a slash if this is not the initial path component.
    if(_index > 0)
        *this += '/';
    
    concat(addend, inLength);
    normalizeSlashes();
    
    return *this;
}
PString &PString::pathConcatenate(const char *addend)
{
    return pathConcatenate(addend, strlen(addend));
}

//
// PString::Putc
//
// Adds a character to the end of the PString, reallocating via buffer doubling
// if necessary.
//
PString &PString::Putc(char ch)
{
    if(_index >= _size)     // leave room for \0
        grow(_size);        // double buffer size
    
    _buffer[_index++] = ch;
    
    return *this;
}

//
// PString::removeFileSpec
//
// Removes a filespec from a path.
// If called on a path without a file, the last path component is removed.
//
PString &PString::removeFileSpec()
{
    size_t lastSlash;
    
    lastSlash = findLastOf('/');
    if(lastSlash == npos)
        lastSlash = findLastOf('\\');
    if(lastSlash != npos)
        truncate(lastSlash);
    
    return *this;
}

//
// _PStrReplaceInternal
//
// Static routine for replacement functions.
//
size_t _PStrReplaceInternal(PString *pstr, char repl)
{
    size_t repcount = 0;
    unsigned char *rptr = (unsigned char *)(pstr->_buffer);
    
    // now scan through the qstring buffer and replace any characters that
    // match characters in the filter table.
    size_t i;
    for(i = 0; i < pstr->_index; ++i)
    {
        if(pstr_repltable[*rptr])
        {
            *rptr = (unsigned char)repl;
            ++repcount; // count characters replaced
        }
        ++rptr;
    }
    
    return repcount;
}

//
// PString::replace
//
// Replaces characters in the PString that match any character in the filter
// string with the character specified by the final parameter.
//
size_t PString::replace(const char *filter, size_t inLength, char repl)
{
    const unsigned char *fptr = (unsigned char *)filter;
    
    memset(pstr_repltable, 0, sizeof(pstr_repltable));
    
    // first scan the filter string and build the replacement filter table
    size_t i;
    for(i = 0; i < inLength; ++i)
        pstr_repltable[*fptr++] = 1;
    
    return _PStrReplaceInternal(this, repl);
}
size_t PString::replace(const char *filter, char repl)
{
    return replace(filter, strlen(filter), repl);
}

//
// PString::replaceNotOf
//
// As above, but replaces all characters NOT in the filter string.
//
size_t PString::replaceNotOf(const char *filter, size_t inLength, char repl)
{
    const unsigned char *fptr = (unsigned char *)filter;
    
    memset(pstr_repltable, 1, sizeof(pstr_repltable));
    
    // first scan the filter string and build the replacement filter table
    size_t i;
    for(i = 0; i < inLength; ++i)
        pstr_repltable[*fptr++] = 0;
    
    return _PStrReplaceInternal(this, repl);
}
size_t PString::replaceNotOf(const char *filter, char repl)
{
    return replaceNotOf(filter, strlen(filter), repl);
}

//
// PString::rstrip
//
// Removes occurrences of a specified character at the end of a PString.
//
PString &PString::rstrip(char c)
{
    while(_index && _buffer[_index - 1] == c)
        Delc();
    
    return *this;
}

//
// PString::swapWith
//
// Exchanges the contents of two PStrings.
//
void PString::swapWith(PString &str2)
{
    char   *tmpbuffer;
    size_t  tmpsize;
    size_t  tmpindex;
    
    // Both must be unlocalized.
    _unLocalize(_size);
    str2._unLocalize(str2._size);
    
    tmpbuffer = this->_buffer; // make a shallow copy
    tmpsize   = this->_size;
    tmpindex  = this->_index;
    
    this->_buffer = str2._buffer;
    this->_size   = str2._size;
    this->_index  = str2._index;
    
    str2._buffer = tmpbuffer;
    str2._size   = tmpsize;
    str2._index  = tmpindex;
}

//
// _memlwr
//
// IOANCH 20130309: portable strlwr function, copied from Eternity
//
char *_memlwr(char *string, size_t inLength)
{
    char *s = string;
    size_t i;
    
    for(i = 0; i < inLength; ++i)
    {
        char c = *s;
        *s++ = tolower(c);
    }
    
    return string;
}

//
// PString::toLower
//
// Converts the string to lowercase.
//
PString &PString::toLower()
{
    _memlwr(_buffer, _index);
    return *this;
}

//
// _memupr
//
// IOANCH 20130309: portable strupr function, copied from Eternity
//
char *_memupr(char *string, size_t inLength)
{
    char *s = string;
    size_t i;
    
    for(i = 0; i < inLength; ++i)
    {
        char c = *s;
        *s++ = toupper(c);
    }
    
    return string;
}

//
// PString::toUpper
//
// Converts the string to uppercase.
//
PString &PString::toUpper()
{
    _memupr(_buffer, _index);
    return *this;
}

//
// PString::truncate
//
// Truncates the PString to the indicated position.
//
PString &PString::truncate(size_t pos)
{
    // pos must be between 0 and qstr->index - 1
    if(pos >= _index)
        return *this;
    
    memset(_buffer + pos, 0, _index - pos);
    _index = pos;
    
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
    return !memcmp(_buffer, str, _index < inLength ? _index : inLength);
}

bool PString::compare(const char *str) const
{
    return !strcmp(_buffer, str);
}

bool PString::compare(const PString &other) const
{
    return !memcmp(_buffer, other._buffer, _index < other._index ? _index :
                   other._index);
}

//
// PString::copyInto
//
// Copies the PString into a C string buffer.
//
void *PString::copyInto(char *dest, size_t pSize) const
{
    return memcpy(dest, _buffer, pSize);
}

PString &PString::copyInto(PString &dest) const
{
    if(dest._index > 0)
        dest.clear();
    
    return dest.concat(*this);
}

//
// PString::find
//
// std::string-compatible find function.
//
size_t PString::find(const char *s, size_t inLength, size_t pos) const
{
    // pos must be between 0 and index - 1
    if(pos >= _index)
        return npos;
    
    char *base   = _buffer + pos;
    char *substr =  (char *)memmem(base, _index - pos, s, inLength);
    
    return substr ? substr - _buffer : npos;
}
size_t PString::find(const char *s, size_t pos) const
{
    // pos must be between 0 and index - 1
    if(pos >= _index)
        return npos;
    
    char *base   = _buffer + pos;
    char *substr =  strstr(base, s);
    
    return substr ? substr - _buffer : npos;
}

//
// PString::findFirstNotOf
//
// Finds the first occurance of a character in the PString which does not
// match the provided character. Returns PString::npos if not found.
//
size_t PString::findFirstNotOf(char c) const
{
    const char *rover = _buffer, *endbuf = _buffer + _index;
    bool found = false;
    
    while(rover != endbuf)
    {
        if(*rover != c)
        {
            found = true;
            break;
        }
        ++rover;
    }
    
    return found ? rover - _buffer : npos;
}

//
// PString::findFirstOf
//
// Finds the first occurance of a character in the PString and returns its
// position. Returns PString::npos if not found.
//
size_t PString::findFirstOf(char c) const
{
    const char *rover = _buffer, *endbuf = _buffer + _index;
    bool found = false;
    
    while(rover != endbuf)
    {
        if(*rover == c)
        {
            found = true;
            break;
        }
        ++rover;
    }
    
    return found ? rover - _buffer : npos;
}

//
// PString::findLastOf
//
// Find the last occurrance of a character in the PString which matches
// the provided character. Returns PString::npos if not found.
//
size_t PString::findLastOf(char c) const
{
    const char *rover;
    bool found = false;
    
    if(!_index)
        return npos;
    
    rover = _buffer + _index - 1;
    do
    {
        if(*rover == c)
        {
            found = true;
            break;
        }
    }
    while((rover == _buffer) ? false : (--rover, true));
    
    return found ? rover - _buffer : npos;
}

//
// PString::findSubStr
//
// Calls strstr on the PString. If the passed-in string is found, then the
// return value points to the location of the first instance of that substring.
//
const char *PString::findSubStr(const char *substr, size_t inLength) const
{
    return (const char *)memmem(_buffer, _index, substr, inLength);
}
const char *PString::findSubStr(const char *substr) const
{
    return (const char *)strstr(_buffer, substr);
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
// PString::strChr
//
// Calls strchr on the PString ("find first of", C-style).
//
const char *PString::strChr(char c) const
{
    return (const char *)memchr(_buffer, c, _index);
}

//
// PString::strCmp
//
// C-style string comparison/collation ordering.
//
int PString::strCmp(const char *str, size_t inLength) const
{
    return memcmp(_buffer, str, _index < inLength ? _index : inLength);
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
    return _memcasecmp(_buffer, str, _index < inLength ? _index : inLength);
}
int PString::strCaseCmp(const char *str) const
{
    return strcasecmp(_buffer, str);
}

//
// _memrchr
//
char *_memrchr(const char *s, char c, size_t n)
{
    // Look right to left for a character. If found, return its address. If not,
    // return NULL
    if(n == 0)
        return  NULL;
    const char *e = s + n - 1;
    do
    {
        if(*e == c)
            return (char *)e;
    }
    while (e-- != s);
    return NULL;
}

//
// PString::strRChr
//
// Calls strrchr on the PString ("find last of", C-style)
//
const char *PString::strRChr(char c) const
{
    return _memrchr(_buffer, c, _index);
}

//
// PString::toInt
//
// Returns the PString converted to an integer via atoi.
//
int PString::toInt() const
{
    return atoi(_buffer);
}


////////////////////////////////////////////////////////////////////////////////

//
// PString::operator ==
//
// Overloaded comparison operator for PString &
//
bool PString::operator == (const PString &other) const
{
    return !memcmp(_buffer, other._buffer, _index < other._index ? _index :
                   other._index);
}
bool PString::operator == (const char *other) const
{
   return !strcmp(_buffer, other);
}

//
// PString::operator !=
//
// Overloaded comparison operator for PString &
//
bool PString::operator != (const PString &other) const
{
    return memcmp(_buffer, other._buffer, _index < other._index ? _index :
                   other._index) != 0;
}
bool PString::operator != (const char *other) const
{
    return strcmp(_buffer, other) != 0;
}

//
// PString::operator =
//
// Assignment from a PString &
//
PString &PString::operator = (const PString &other)
{
    return copy(other);
}
PString &PString::operator = (const char *other)
{
    return copy(other);
}
PString &PString::operator = (int number)
{
    return copy(number);
}

//
// PString::operator +=
//
// Overloaded += for PString &
//
PString &PString::operator += (const PString &other)
{
    return concat(other);
}
PString &PString::operator += (const char *other)
{
    return concat(other);
}
PString &PString::operator += (char ch)
{
    return Putc(ch);
}
PString &PString::operator += (int number)
{
    return concat(number);
}

//
// PString::operator +=
//
// Overloaded + for PString &
//
PString PString::operator + (const PString &other) const
{
    return PString(*this).concat(other);
}
PString PString::operator + (const char *other) const
{
    return PString(*this).concat(other);
}
PString PString::operator + (char ch) const
{
    return PString(*this).Putc(ch);
}
PString PString::operator + (int number) const
{
    return PString(*this).concat(number);
}
PString  operator +  (const char    *first, const PString &second)
{
    return PString(first).concat(second);
}
PString  operator +  (char  ch,             const PString &second)
{
    return PString().Putc(ch).concat(second);
}
PString  operator +  (int   number,         const PString &second)
{
    return PString(number).concat(second);
}


//
// Stream Insertion Operators
//

PString &PString::operator << (const PString &other)
{
    return concat(other);
}
PString &PString::operator << (const char *other)
{
    return concat(other);
}

PString &PString::operator << (char ch)
{
    return Putc(ch);
}

//
// PString::operator []
//
// Read-write variant.
//
char &PString::operator [] (size_t idx)
{
    return _buffer[idx];
}

//
// PString::operator []
//
// Read-only variant.
//
const char &PString::operator [] (size_t idx) const
{
    return _buffer[idx];
}