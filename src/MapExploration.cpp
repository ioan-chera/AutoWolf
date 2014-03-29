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
#include "MapExploration.h"

//
// PropertyFile::_PackBoolean8Array
//
PString MapExploration::PackBoolean8Array() const
{
    size_t pos;
    uint8_t charac = 0;
    
    size_t arraySize = maparea;
    
    PString dataToWrite(arraySize / 8);
    
    Boolean8 *baseaddress = const_cast<Boolean8*>(explored[0]);
    
    for(pos = 0; pos < arraySize; ++pos)
    {
        charac = (charac << 1) + *(baseaddress + pos);
        if((pos + 1) % 8 == 0)	// reached eight bits
        {
            dataToWrite.Putc((char)charac);
            charac = 0;
        }
    }
    
    return dataToWrite;
}

//
// PropertyFile::_UnpackBoolean8Array
//
void MapExploration::UnpackBoolean8Array(const PString &source)
{
    const PString &explorstr = source;
    auto explorbuf = reinterpret_cast<const uint8_t *>(explorstr.buffer());
    
    int j;
    size_t pos, targetSize = maparea;
    uint8_t mbyte;
    
    if (source.length() < targetSize / 8) 
    {
        return;
    }
    
    Boolean8 *baseaddress = const_cast<Boolean8*>(explored[0]);
    
    for(pos = 0; pos < targetSize; pos += 8)
    {
        mbyte = *explorbuf++;
        for(j = 7; j >= 0; --j)
        {
            *(baseaddress + pos + j) = mbyte & 1;
            mbyte >>= 1;
        }
    }
}