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

#include "wl_def.h"
#include "i_system.h"

int pm_ChunksInFile;
int pm_SpriteStart;
int pm_SoundStart;

bool pm_SoundInfoPagePadded = false;

// holds the whole VSWAP
static uint32_t *pm_PageData;
static size_t pm_PageDataSize;

// pm_ChunksInFile+1 pointers to page starts.
// The last pointer points one byte after the last page.
uint8_t **pm_Pages;

//
// PM_Startup
//
void PM_Startup()
{
//    PString fname;
//    fname.copy("VSWAP.").concat(cfg_extension);
   
    // IOANCH 20130726: case insensitive
    PString fname = I_ResolveCaseInsensitivePath(".",
                                         PString("VSWAP.").
                                         concat(cfg_extension)());

    FILE *file = fopen(fname(),"rb");
    if(!file)
        CA_CannotOpen(fname());

    pm_ChunksInFile = 0;
    fread(&pm_ChunksInFile, sizeof(word), 1, file);
    pm_SpriteStart = 0;
    fread(&pm_SpriteStart, sizeof(word), 1, file);
    pm_SoundStart = 0;
    fread(&pm_SoundStart, sizeof(word), 1, file);

    uint32_t *pageOffsets = (uint32_t *) I_CheckedMalloc((pm_ChunksInFile + 1) *
                                                sizeof(int32_t));

   
    fread(pageOffsets, sizeof(uint32_t), pm_ChunksInFile, file);

    word *pageLengths = (word *) I_CheckedMalloc(pm_ChunksInFile * sizeof(word));

    fread(pageLengths, sizeof(word), pm_ChunksInFile, file);

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    long pageDataSize = fileSize - pageOffsets[0];
    if(pageDataSize > (size_t) -1)
        Quit("The page file \"%s\" is too large!", fname());

    pageOffsets[pm_ChunksInFile] = fileSize;

    uint32_t dataStart = pageOffsets[0];
    int i;

    // Check that all pageOffsets are valid
    for(i = 0; i < pm_ChunksInFile; i++)
    {
        if(!pageOffsets[i])
           continue;   // sparse page
        if(pageOffsets[i] < dataStart || pageOffsets[i] >= (size_t) fileSize)
            Quit("Illegal page offset for page %i: %u (filesize: %u)",
                    i, pageOffsets[i], fileSize);
    }

    // Calculate total amount of padding needed for sprites and sound info page
    int alignPadding = 0;
    for(i = pm_SpriteStart; i < pm_SoundStart; i++)
    {
        if(!pageOffsets[i])
           continue;   // sparse page
        uint32_t offs = pageOffsets[i] - dataStart + alignPadding;
        if(offs & 1)
            alignPadding++;
    }

    if((pageOffsets[pm_ChunksInFile - 1] - dataStart + alignPadding) & 1)
        alignPadding++;

    pm_PageDataSize = (size_t) pageDataSize + alignPadding;
    pm_PageData = (uint32_t *) I_CheckedMalloc(pm_PageDataSize);

    pm_Pages = (uint8_t **) I_CheckedMalloc((pm_ChunksInFile + 1) *
                                            sizeof(uint8_t *));

    // Load pages and initialize pm_Pages pointers
    uint8_t *ptr = (uint8_t *) pm_PageData;
    for(i = 0; i < pm_ChunksInFile; i++)
    {
        if((i >= pm_SpriteStart && i < pm_SoundStart) || i == pm_ChunksInFile - 1)
        {
            size_t offs = ptr - (uint8_t *) pm_PageData;

            // pad with zeros to make it 2-byte aligned
            if(offs & 1)
            {
                *ptr++ = 0;
                if(i == pm_ChunksInFile - 1) pm_SoundInfoPagePadded = true;
            }
        }

        pm_Pages[i] = ptr;

        if(!pageOffsets[i])
            continue;               // sparse page

        // Use specified page length, when next page is sparse page.
        // Otherwise, calculate size from the offset difference between this and the next page.
        uint32_t size;
        if(!pageOffsets[i + 1]) size = pageLengths[i];
        else size = pageOffsets[i + 1] - pageOffsets[i];

        fseek(file, pageOffsets[i], SEEK_SET);
        fread(ptr, 1, size, file);
        ptr += size;
    }

    // last page points after page buffer
    pm_Pages[pm_ChunksInFile] = ptr;

    free(pageLengths);
    free(pageOffsets);
    fclose(file);
}

//
// PM_Shutdown
//
void PM_Shutdown()
{
    free(pm_Pages);
    free(pm_PageData);
}
