// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright(C) 2002 James Haley
// Copyright(C) 2013 Ioan Chera
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//--------------------------------------------------------------------------

// IOANCH 20130308: this is an excerpt from an Eternity module. It has been mo-
//                  dified to use Wolfenstein functions.
#include "wl_def.h"

//
// Shared Hash functions
//

//
// D_HashTableKey
//
// Fairly standard key computation -- this is used for multiple
// tables so there's not much use trying to make it perfect. It'll
// save time anyways.
// 08/28/03: vastly simplified, is now similar to SGI's STL hash
//
unsigned int D_HashTableKey(const char *str)
{
    const char *c = str;
    unsigned int h = 0;
    
    if(!str)
        Quit("D_HashTableKey: cannot hash NULL string!\n");
    
    // note: this needs to be case insensitive for EDF mnemonics
    while(*c)
    {
        h = 5 * h + toupper(*c);
        ++c;
    }
    
    return h;
}

//
// D_HashTablePKey
//
// IOANCH 20130309: updated for PStrings
//
unsigned int D_HashTablePKey(const char *str, size_t inLength)
{
    const char *c = str;
    unsigned int h = 0;
    size_t i;
    
    for(i = 0; i < inLength; ++i)
    {
        h = 5 * h + toupper(*c);
        ++c;
    }
    
    return h;
}

//
// D_HashTableKeyCase
//
// haleyjd 09/09/09: as above, but case-sensitive
//
unsigned int D_HashTableKeyCase(const char *str)
{
    const char *c = str;
    unsigned int h = 0;
    
    if(!str)
        Quit("D_HashTableKeyCase: cannot hash NULL string!\n");
    
    while(*c)
    {
        h = 5 * h + *c;
        ++c;
    }
    
    return h;
}

//
// D_HashTablePKeyCase
//
// IOANCH 20130309: updated for PStrings
//
unsigned int D_HashTablePKeyCase(const char *str, size_t inLength)
{
    const char *c = str;
    unsigned int h = 0;
    size_t i;
    
    for(i = 0; i < inLength; ++i)
    {
        h = 5 * h + *c;
        ++c;
    }
    
    return h;
}