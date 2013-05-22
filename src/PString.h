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
#define STRING_ASSUMED_DEC_NUMBER_LENGTH 80

//
// PString
//
// "Pascal" string, i.e. string with attached length value, so it can contain
// null characters and whose size can be expected early from disk reading.
//
// Largely based on Quasar's Eternity Engine qstring class
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
    
    friend size_t _PStrReplaceInternal(PString *pstr, char repl);
    
public:
    static const size_t npos;
    static const size_t basesize;
    
    // Statics
    static unsigned int HashCodeCaseStatic(const char *str, size_t inLength);
    static unsigned int HashCodeStatic(const char *str, size_t inLength);
    
    // Constructor
    PString(const char *cstr, size_t inLength);
    PString(const char *cstr);
    PString(const PString &other);
    PString(PString &&other);   // IOANCH 20130522: move constructor
    PString(size_t startSize = 0);
    PString(int number);
    
    // Destructor
    ~PString() { freeBuffer(); }
    
    // Manipulators
    PString &concatExtension(const char *ext, size_t inLength);
    PString &concatExtension(const char *ext);
    PString &concatExtension(const PString &other);
    PString  withExtension(const char *ext, size_t inLength) const;
    PString  withExtension(const char *ext) const;
    PString  withExtension(const PString &other) const;
    PString &clear();
    PString &clearOrCreate(size_t size);
    PString &concat(const char *str, size_t inLength);
    PString &concat(const char *str);
    PString &concat(const PString &src);
    PString &concat(int number);
    PString &copy(const char *str, size_t inLength);
    PString &copy(const char *str);
    PString &copy(const PString &src);
    PString &copy(int number);
    PString &create();
    PString &createSize(size_t size);
    PString &Delc();
    PString &erase(size_t pos, size_t n = npos);
    void     extractFileBase(PString &dest);
    void     freeBuffer();
    PString &grow(size_t len);
    PString &initCreate();
    PString &initCreateSize(size_t size);
    PString &insert(const char *insertstr, size_t inLength, size_t pos);
    PString &insert(const char *insertstr, size_t pos);
    PString &lstrip(char c);
    PString &makeQuoted();
    PString &normalizeSlashes();
    PString &concatSubpath(const char *addend, size_t inLength);
    PString &concatSubpath(const char *addend);
    PString &concatSubpath(const PString &other);
//    PString  withSubpath(const char *addend, size_t inLength) const;
//    PString  withSubpath(const char *addend) const;
//    PString  withSubpath(const PString &add) const;
    PString &Putc(char ch);
    PString &removeFileSpec();
    size_t   replace(const char *filter, size_t inLength, char repl);
    size_t   replace(const char *filter, char repl);
    size_t   replaceNotOf(const char *filter, size_t inLength, char repl);
    size_t   replaceNotOf(const char *filter, char repl);
    PString &rstrip(char c);
    void     swapWith(PString &str2);
    PString &toLower();
    PString &toUpper();
    PString &truncate(size_t pos);
    
    // Accessors
    const char  *buffer() const { return _buffer; }
    char         charAt(size_t idx) const;
    bool         compare(const char *str, size_t inLength) const;
    bool         compare(const char *str) const;
    bool         compare(const PString &other) const;
    void        *copyInto(char *dest, size_t size) const;
    PString     &copyInto(PString &dest) const;
    size_t       find(const char *s, size_t inLength, size_t pos = 0) const;
    size_t       find(const char *s, size_t pos = 0) const;
    size_t       findFirstNotOf(char c) const;
    size_t       findFirstOf(char c) const;
    size_t       findLastOf(char c) const;
    const char  *findSubStr(const char *substr, size_t inLength) const;
    const char  *findSubStr(const char *substr) const;
    unsigned int hashCode() const;      // case-insensitive
    unsigned int hashCodeCase() const;  // case-considering
    size_t       length() const { return _index;  }
    size_t       size() const { return _size;  }
    const char  *strChr(char c) const;
    int          strCmp(const char *str, size_t inLength) const;
    int          strCaseCmp(const char *str, size_t inLength) const;
    int          strCaseCmp(const char *str) const;
    const char  *strRChr(char c) const;
    int          toInt() const;

    // Operators
    bool     operator == (const PString &other) const;
    bool     operator == (const char    *other) const;
    bool     operator != (const PString &other) const;
    bool     operator != (const char    *other) const;
    PString &operator  = (const PString &other);
    PString &operator  = (const char    *other);
    PString &operator  = (int   number);
    PString &operator += (const PString &other);
    PString &operator += (const char    *other);
    PString &operator += (char  ch);
    PString &operator += (int   number);
//    PString  operator +  (const PString &other) const;
//    PString  operator +  (const char    *other) const;
//    PString  operator +  (char  ch) const;
//    PString  operator +  (int   number) const;
    PString &operator << (const PString &other);
    PString &operator << (const char    *other);
    PString &operator << (char   ch);
    PString &operator << (int number);
    
    char       &operator [] (size_t idx);
    const char &operator [] (size_t idx) const;
    
    // Returns string length
//    size_t length() {return _length;}
    
};

PString  operator +  (const char    *first, const PString &second);
PString  operator +  (char  ch,             const PString &second);
PString  operator +  (int   number,         const PString &second);

#endif /* defined(__Wolf4SDL__PString__) */
