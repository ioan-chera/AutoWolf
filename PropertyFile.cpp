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

#include "PropertyFile.h"

//
// PropertyFile::PropertyFile
//
PropertyFile::PropertyFile() : propertyTable()
{
	strcpy(header, PROPERTY_FILE_HEADER);
    this->propertyTable = new EHashTable<Property, EStringHashKey,
        &Property::_key, &Property::link>;
}

//
// PropertyFile::~PropertyFile
//
PropertyFile::~PropertyFile()
{
    delete this->propertyTable;
}

//
// PropertyFile::doReadFromFile
//
bool PropertyFile::doReadFromFile(FILE *f)
{
    //
    // Naive, basic structure
    // - Number of properties
    // - Pascal string (16-bit uint length)
    // - type specifier (byte)
    // - value:
    //   0: int32_t
    //   1: Pascal string (uint16_t length)
    //
    uint32_t i, numReads;
    
    fread(&numReads, sizeof(uint32_t), 1, f);
    
    uint16_t keyLen;
    uint32_t valueLen;
    uint8_t valType;
    Property::Type ptype;
    char *keyName, *valName;
    Property *newProp;
    // FIXME: don't assume correct file
    for(i = 0; i < numReads; ++i)
    {
        fread(&keyLen, sizeof(uint16_t), 1, f);
        keyName = new char[keyLen + 1];
        keyName[keyLen]=0;
        fread(keyName, sizeof(char), keyLen, f);
        fread(&valType, sizeof(uint8_t), 1, f);
        ptype = (Property::Type)valType;
        newProp = new Property(keyName);
        newProp->type = ptype;
        switch (ptype)
        {
            case Property::Int32:
                // int32_t
                
                fread(&newProp->intValue, sizeof(int32_t), 1, f);
                break;
                
            case Property::PString:
                // Pascal string
                fread(&valueLen, sizeof(uint32_t), 1, f);
                valName = new char[valueLen + 1];
                valName[valueLen] = 0;
                fread(valName, sizeof(char), valueLen, f);
                newProp->setStringValue(PString(valName, (size_t)valueLen));
                delete [] valName;
                break;
            default:
                // ???
                break;
        }
        propertyTable->addObject(newProp);
        delete [] keyName;
    }
    
    return true;
}

//
// PropertyFile::doWriteToFile
//
void PropertyFile::doWriteToFile(FILE *f)
{
    //
    // Naive, basic structure
    // - Number of properties
    // - Pascal string (16-bit uint length)
    // - type specifier (byte)
    // - value:
    //   0: int32_t
    //   1: Pascal string (uint16_t length)
    //
    fwrite(header, sizeof(char), FILE_HEADER_LENGTH, f);
    uint32_t numProp = (uint32_t)propertyTable->getNumItems();
    fwrite(&numProp, sizeof(uint32_t), 1, f);
    Property *prop = NULL;
    uint16_t propKeyLen;
    uint32_t propValLen;
    uint8_t btype;
    while((prop = propertyTable->tableIterator(prop)))
    {
        propKeyLen = (uint16_t)strlen(prop->key());
        fwrite(&propKeyLen, sizeof(uint16_t), 1, f);
        fwrite(prop->key(), sizeof(char), (size_t)propKeyLen, f);
        // how know type???
        btype = (uint8_t)prop->type;
        fwrite(&btype, sizeof(uint8_t), 1, f);
        switch (prop->type)
        {
            case Property::Int32:
                fwrite(&prop->intValue, sizeof(int32_t), 1, f);
                break;
            case Property::PString:
                propValLen = (uint32_t)prop->stringValue().length();
                fwrite(&propValLen, sizeof(uint32_t), 1, f);
                fwrite(prop->stringValue().buffer(), sizeof(char), propValLen,
                       f);
            default:
                // ???
                break;
        }
    }
}

//
// PropertyFile::initialize
//
void PropertyFile::initialize(const char *fname, size_t nchar)
{
    DataFile::initialize(fname, nchar);
//    propertyTable.initialize(127);
}

//
// PropertyFile::getSize
//
// This really should be part of reading and react to changes.
//
uint64_t PropertyFile::getSize()
{
    uint64_t ret = FILE_HEADER_LENGTH + sizeof(uint32_t);
    Property *prop = NULL;
    while ((prop = propertyTable->tableIterator(prop)))
    {
        ret += sizeof(uint16_t);    // key len
        ret += strlen(prop->key());
        ret += sizeof(uint8_t);     // value type
        switch (prop->type)
        {
            case Property::Int32:
                ret += sizeof(int32_t);
                break;
            case Property::PString:
                ret += sizeof(uint32_t) + prop->stringValue().length();
            default:
                // ???
                break;
        }
    }
    return ret;
}