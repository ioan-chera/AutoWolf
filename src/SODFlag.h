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


#include "PString.h"

// IOANCH 20130301: unification culling
#include "gfxvabstract.h"
// IOANCH: added sprite header
#include "sprabstract.h"
// IOANCH 20130301: unification
#include "audioabstract.h"


#ifndef SODFLAG_H_
#define SODFLAG_H_
//
// SODFlag
//
// Class for Spear master flag
//
class SODFlag
{
    
    Boolean8 flag;
    static const unsigned int gfxvmap[][2];
    static const unsigned int sprmap[][2];
    static const unsigned int soundmap[][2];
    static const char wolftitle[];
    static const char speartitle[];
public:
    void Initialize(const PString &basePath);
    SODFlag() : flag(false)                  {}
    Boolean8             operator()()   const {return flag;}
    unsigned int g(unsigned int value) const {return gfxvmap[value][flag];}
    unsigned int sp(unsigned int value)const {return sprmap[value][flag];}
    unsigned int sd(unsigned int value)const {return soundmap[value][flag];}
    const char *FullTitle() const {return flag ? speartitle : wolftitle;}
};
extern SODFlag SPEAR;

#endif
