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

#include "wl_def.h"
#include "PropertyFile.h"

//
// PropertyFile::PropertyFile
//
PropertyFile::PropertyFile()// : propertyTable()
{
	strcpy(_header, PROPERTY_FILE_HEADER);
    _size = FILE_HEADER_LENGTH + sizeof(uint32_t);
}

//
// PropertyFile::~PropertyFile
//
PropertyFile::~PropertyFile()
{

    Property *prop = NULL;
    
    while((prop = _propertyTable.tableIterator(prop)))
    {
        _propertyTable.removeObject(prop);
        delete prop;
    }
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
                
            case Property::PStringVal:
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
        _propertyTable.addObject(newProp);
        delete [] keyName;
    }
    
    _updateSize();
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
    fwrite(_header, sizeof(char), FILE_HEADER_LENGTH, f);
    uint32_t numProp = (uint32_t)_propertyTable.getNumItems();
    fwrite(&numProp, sizeof(uint32_t), 1, f);
    Property *prop = NULL;
    uint16_t propKeyLen;
    uint32_t propValLen;
    uint8_t btype;
    while((prop = _propertyTable.tableIterator(prop)))
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
            case Property::PStringVal:
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
// PropertyFile::_makeObjectWithKey
//
Property *PropertyFile::_makeObjectWithKey(const char *key)
{
    Property *prop = _propertyTable.objectForKey(key);
    
    if(!prop)
    {
        // Property doesn't exist. Create it.
        prop = new Property(key);
        _propertyTable.addObject(prop);
    }
    
    return prop;
}

//
// PropertyFile::_updateSize
//
void PropertyFile::_updateSize()
{
    _size = FILE_HEADER_LENGTH + sizeof(uint32_t);
    Property *prop = NULL;
    while ((prop = _propertyTable.tableIterator(prop)))
    {
        _size += sizeof(uint16_t);    // key len
        _size += strlen(prop->key());
        _size += sizeof(uint8_t);     // value type
        switch (prop->type)
        {
            case Property::Int32:
                _size += sizeof(int32_t);
                break;
            case Property::PStringVal:
                _size += sizeof(uint32_t) + prop->stringValue().length();
            default:
                // ???
                break;
        }
    }
    
    DataFile::_updateSize();
}

//
// PropertyFile::hasProperty
//
bool PropertyFile::hasProperty(const char *keyName) const
{
    if (_propertyTable.objectForKey(keyName))
        return true;
    return false;
}

//
// PropertyFile::getIntValue
//
int PropertyFile::getIntValue(const char *keyName) const
{
    Property *prop = _propertyTable.objectForKey(keyName);
    if(prop && prop->type == Property::Int32)
    {
        return (int)prop->intValue;
    }
    return 0;   // default to 0
}

//
// PropertyFile::getStringValue
//
PString PropertyFile::getStringValue(const char *keyName) const
{
    Property *prop = _propertyTable.objectForKey(keyName);
    if(prop && prop->type == Property::PStringVal)
    {
        return prop->stringValue();
    }
    return "";   // default to 0
}

//
// PropertyFile::setIntValue
//
void PropertyFile::setIntValue(const char *keyName, int value)
{
    Property *prop = _makeObjectWithKey(keyName);
    prop->type = Property::Int32;
    prop->intValue = (int32_t)value;
    _updateSize();
}

//
// PropertyFile::setStringValue
//
void PropertyFile::setStringValue(const char *keyName, const PString &value)
{
    Property *prop = _makeObjectWithKey(keyName);
    prop->type = Property::PStringVal;
    prop->setStringValue(value);
    _updateSize();
}