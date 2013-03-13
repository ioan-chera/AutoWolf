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
#include "e_hash.h"
#include "Property.h"

class PropertyFilePimpl
{
public:
    EHashTable<Property, EStringHashKey, &Property::_key, &Property::link>
        _propertyTable;
};

//
// PropertyFile::PropertyFile
//
PropertyFile::PropertyFile()// : propertyTable()
{
	strcpy(header, PROPERTY_FILE_HEADER);
    pimpl = new PropertyFilePimpl;
}

//
// PropertyFile::~PropertyFile
//
PropertyFile::~PropertyFile()
{
    delete pimpl;
    Property *prop = NULL;
    
    while((prop = pimpl->_propertyTable.tableIterator(prop)))
    {
        pimpl->_propertyTable.removeObject(prop);
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
        pimpl->_propertyTable.addObject(newProp);
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
    uint32_t numProp = (uint32_t)pimpl->_propertyTable.getNumItems();
    fwrite(&numProp, sizeof(uint32_t), 1, f);
    Property *prop = NULL;
    uint16_t propKeyLen;
    uint32_t propValLen;
    uint8_t btype;
    while((prop = pimpl->_propertyTable.tableIterator(prop)))
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
// PropertyFile::getExplored
//
void PropertyFile::getExplored(void *exploredTarget) 
{
    // File exists. Use its hash table
    Property *prop =
    pimpl->_propertyTable.objectForKey(PROPERTY_KEY_EXPLORED);
    if(prop)
    {
        // Property exists. Get its data (expected is string).
        // Make sure it IS string
        if(prop->type == Property::PStringVal)
        {
            // Type valid. Send the data
            const PString &explorstr = prop->stringValue();
            const uint8_t *explorbuf = (uint8_t *)explorstr.buffer();
            
            {
                int j;
                size_t pos;
                uint8_t mbyte;
                
                boolean *baseaddress = (boolean *)exploredTarget;
                
                for(pos = 0; pos < maparea; pos += 8)
                {
                    mbyte = *explorbuf++;
                    for(j = 7; j >= 0; --j)
                    {
                        *(baseaddress + pos + j) = mbyte & 1;
                        mbyte >>= 1;
                    }
                }
            }
        }
        else
        {
            // it wasn't set as PString. Kill it.
            pimpl->_propertyTable.removeObject(prop);
            delete prop;
        }
    }
}

//
// PropertyFile::putExplored
//
void PropertyFile::putExplored(const void *explored)
{
    // File exists. Look for its Explored property
    Property *prop =
    pimpl->_propertyTable.objectForKey(PROPERTY_KEY_EXPLORED);
    
    if(!prop)
    {
        // Property doesn't exist. Create it.
        prop = new Property(PROPERTY_KEY_EXPLORED);
        pimpl->_propertyTable.addObject(prop);
    }
    
    {
        // Property exists. Change it
        PString dataToWrite(maparea/8);
        prop->type = Property::PStringVal;
        {
            size_t pos;
            uint8_t charac = 0;
            
            boolean *baseaddress = (boolean *)explored;
            
            for(pos = 0; pos < maparea; ++pos)
            {
                charac = (charac << 1) + *(baseaddress + pos);
                if((pos + 1) % 8 == 0)	// reached eight bits
                {
                    dataToWrite.Putc((char)charac);
                    charac = 0;
                }
            }
        }
        // Written.
        prop->setStringValue(dataToWrite);
    }
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
    while ((prop = pimpl->_propertyTable.tableIterator(prop)))
    {
        ret += sizeof(uint16_t);    // key len
        ret += strlen(prop->key());
        ret += sizeof(uint8_t);     // value type
        switch (prop->type)
        {
            case Property::Int32:
                ret += sizeof(int32_t);
                break;
            case Property::PStringVal:
                ret += sizeof(uint32_t) + prop->stringValue().length();
            default:
                // ???
                break;
        }
    }
    return ret;
}