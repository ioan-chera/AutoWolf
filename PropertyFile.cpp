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
    
    uint16_t keyLen, valueLen;
    uint8_t valType;
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
        newProp = new Property(keyName);
        switch (valType)
        {
            case 0:
                // int32_t
                
                fread(&newProp->intValue, sizeof(int32_t), 1, f);
                
                break;
                
            default:
                // Pascal string
                fread(&valueLen, sizeof(uint16_t), 1, f);
                valName = new char[valueLen + 1];
                valName[valueLen] = 0;
                fread(valName, sizeof(char), valueLen, f);
                newProp->setStringValue(valName);
                delete [] valName;
                break;
        }
        propertyTable.addObject(newProp);
        delete [] keyName;
    }
    
    return true;
}