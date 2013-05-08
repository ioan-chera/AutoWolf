//
// Copyright (C) 1991-1992  id Software
// Copyright (C) 2007-2011  Moritz Kroll
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

#ifndef __ID_PM__
#define __ID_PM__

#ifdef USE_HIRES
#define PMPageSize 16384
#else
#define PMPageSize 4096
#endif

extern int ChunksInFile;
extern int PMSpriteStart;
extern int PMSoundStart;

extern bool PMSoundInfoPagePadded;

// ChunksInFile+1 pointers to page starts.
// The last pointer points one byte after the last page.
extern uint8_t **PMPages;

void PM_Startup();
void PM_Shutdown();

static inline uint32_t PM_GetPageSize(int page)
{
    if(page < 0 || page >= ChunksInFile)
        Quit("PM_GetPageSize: Tried to access illegal page: %i", page);
    return (uint32_t) (PMPages[page + 1] - PMPages[page]);
}

static inline uint8_t *PM_GetPage(int page)
{
    if(page < 0 || page >= ChunksInFile)
        Quit("PM_GetPage: Tried to access illegal page: %i", page);
    return PMPages[page];
}

static inline uint8_t *PM_GetEnd()
{
    return PMPages[ChunksInFile];
}

static inline byte *PM_GetTexture(int wallpic)
{
    return PM_GetPage(wallpic);
}

#include "SODFlag.h"
// IOANCH 20130303: choose whether to remap
static inline uint16_t *PM_GetSprite(int shapenum, boolean remap = true)
{
    // correct alignment is enforced by PM_Startup()
    // IOANCH 20130302: unification
    if(remap)
        shapenum = sprmap[shapenum][SPEAR()];
    return (uint16_t *) (void *) PM_GetPage(PMSpriteStart + shapenum);
}

static inline byte *PM_GetSound(int soundpagenum)
{
    return PM_GetPage(PMSoundStart + soundpagenum);
}

#endif
