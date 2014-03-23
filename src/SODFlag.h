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
namespace SPEAR
{
	extern Boolean8 flag;
	extern const unsigned int gfxvmap[][2];
	extern const unsigned int sprmap[][2];
	extern const unsigned int soundmap[][2];
	extern const char speartitle[], wolftitle[];

    void Initialize(const std::string &basePath);
    inline static unsigned int g(unsigned int value)  {return gfxvmap[value][flag];}
	inline static unsigned int sp(unsigned int value) { return sprmap[value][flag]; }
	inline static unsigned int sd(unsigned int value) { return soundmap[value][flag]; }
	inline static const char *FullTitle() { return flag ? speartitle : wolftitle; }
	
	void SetGlobalValues();
};

#endif
