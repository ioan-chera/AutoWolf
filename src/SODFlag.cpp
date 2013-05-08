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


#include "SODFlag.h"

SODFlag SPEAR;

void SODFlag::Initialize(const PString &basePath)
{
    FILE *f;
    
    f = fopen(basePath.withSubpath("VSWAP.SD3").buffer(), "rb");
    if(!f)
    {
        f = fopen(basePath.withSubpath("VSWAP.SD2").buffer(), "rb");
        if(!f)
        {
            f = fopen(basePath.withSubpath("VSWAP.SD1").buffer(), "rb");
            if(!f)
            {
                f = fopen(basePath.withSubpath("VSWAP.SOD").buffer(), "rb");
                if(!f)
                {
                    this->flag = false;
					return;	// none found: assume Wolf3D
                }
            }
        }
    }
    fclose(f);
	// One of the ifs failed - fall here and return SPEAR() 1
    this->flag = true;
}