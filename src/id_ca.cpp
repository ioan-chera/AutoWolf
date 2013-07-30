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

// ID_CA.C

// this has been customized for WOLF

////////////////////////////////////////////////////////////////////////////////
//
// Id Software Caching Manager
//
// Must be started BEFORE the memory manager, because it needs to get the 
// headers
// loaded into the data segment
//
////////////////////////////////////////////////////////////////////////////////

// IOANCH 20121223: Cleaned up this file

#include <sys/types.h>
#if defined _WIN32
    #include <io.h>
#elif defined _arch_dreamcast
    #include <unistd.h>
#else
    #include <sys/uio.h>
    #include <unistd.h>
#endif

#include "wl_def.h"
#include "wl_main.h"
#pragma hdrstop
#include "Config.h"
#include "i_system.h"
#include "i_video.h"
#include "PString.h"

#define THREEBYTEGRSTARTS

////////////////////////////////////////////////////////////////////////////////
//
//                             LOCAL CONSTANTS
//
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    word bit0,bit1;       // 0-255 is a character, > is a pointer to a node
} huffnode;

// IOANCH: moved structure into a local scope, only used there

////////////////////////////////////////////////////////////////////////////////
//
//                             GLOBAL VARIABLES
//
////////////////////////////////////////////////////////////////////////////////

#define BUFFERSIZE 0x1000
static int32_t ca_bufferseg[BUFFERSIZE/4];

int     mapon;

word    *mapsegs[MAPPLANES];
static maptype* ca_mapheaderseg[NUMMAPS];
// IOANCH 20130301: unification
byte    *ca_audiosegs[NUMSNDCHUNKS_sod > NUMSNDCHUNKS_wl6 ? NUMSNDCHUNKS_sod :
				   NUMSNDCHUNKS_wl6];
byte    *ca_grsegs[NUMCHUNKS_sod > NUMCHUNKS_wl6 ? NUMCHUNKS_sod : NUMCHUNKS_wl6];

word    ca_RLEWtag;

int     menu_missingep = 0;

////////////////////////////////////////////////////////////////////////////////
//
//                             LOCAL VARIABLES
//
////////////////////////////////////////////////////////////////////////////////

PString cfg_extension; // Need a string, not constant to change cache files
PString cfg_graphext;
PString cfg_audioext;
static const char ca_gheadname[] = "VGAHEAD.";
static const char ca_gfilename[] = "VGAGRAPH.";
static const char ca_gdictname[] = "VGADICT.";
static const char ca_mheadname[] = "MAPHEAD.";
static const char ca_mfilename[] = "maptemp.";
static const char ca_aheadname[] = "AUDIOHED.";
static const char ca_afilename[] = "AUDIOT.";

static int32_t  ca_grstarts_wl6[NUMCHUNKS_wl6 + 1];
static int32_t  ca_grstarts_sod[NUMCHUNKS_sod + 1];

// static int32_t  ca_grstarts[NUMCHUNKS_sod > NUMCHUNKS_wl6 ? NUMCHUNKS_sod + 1 : NUMCHUNKS_wl6 + 1];
static int32_t* ca_audiostarts; // array of offsets in audio / AUDIOT

#ifdef GRHEADERLINKED
huffnode *ca_grhuffman;
#else
huffnode ca_grhuffman[255];
#endif

int    ca_grhandle = -1;               // handle to EGAGRAPH
int    ca_maphandle = -1;              // handle to MAPTEMP / GAMEMAPS
int    ca_audiohandle = -1;            // handle to AUDIOT / AUDIO

int32_t   ca_chunkcomplen,ca_chunkexplen;

SDMode ca_oldsoundmode;


//
// GRFILEPOS
//
static int32_t GRFILEPOS(const size_t idx)
{
    if(SPEAR())
        assert(idx < lengthof(ca_grstarts_sod));
    else
        assert(idx < lengthof(ca_grstarts_wl6));
	return IMPALE(ca_grstarts)[idx];
}

////////////////////////////////////////////////////////////////////////////////
//
//                            LOW LEVEL ROUTINES
//
////////////////////////////////////////////////////////////////////////////////

//
// CAL_GetGrChunkLength
//
// Gets the length of an explicit length chunk (not tiles)
// The file pointer is positioned so the compressed data can be read in next.
//
static void CAL_GetGrChunkLength(int chunk)
{
    lseek(ca_grhandle, GRFILEPOS(chunk), SEEK_SET);
    read(ca_grhandle, &ca_chunkexplen, sizeof(ca_chunkexplen));
    ca_chunkcomplen = GRFILEPOS(chunk + 1) - GRFILEPOS(chunk) - 4;
}

//
// CA_WriteFile
//
// Writes a file from a memory buffer
//
Boolean CA_WriteFile(const char *filename, void *ptr, int32_t length)
{
    const int handle = open(filename, O_CREAT | O_WRONLY | O_BINARY, 0644);
    if(handle == -1)
        return false;

    if(!write(handle, ptr, length))
    {
        close(handle);
        return false;
    }
    close(handle);
    return true;
}

//
// CA_LoadFile
//
// Allocate space for and load a file
//
Boolean CA_LoadFile(const char *filename, memptr *ptr)
{
    int32_t size;

    const int handle = open(filename, O_RDONLY | O_BINARY);
    if(handle == -1)
        return false;

    size = lseek(handle, 0, SEEK_END);
    lseek(handle, 0, SEEK_SET);
   *ptr = I_CheckedMalloc(size);
    if(!read(handle, *ptr, size))
    {
        close(handle);
        return false;
    }

    close(handle);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
//                COMPRESSION routines, see JHUFF.C for more
//
////////////////////////////////////////////////////////////////////////////////

//
// CAL_HuffExpand
//
static void CAL_HuffExpand(byte *source, byte *dest, int32_t length,
                           huffnode *hufftable)
{
    byte *end;
    huffnode *headptr, *huffptr;

    if(!length || !dest)
    {
        Quit("length or dest is null!");
        return;
    }

    headptr = hufftable + 254;        // head node is always node 254

    int written = 0;

    end = dest + length;

    byte val = *source++;
    byte mask = 1;
    word nodeval;
    huffptr = headptr;
    while(1)
    {
        if(!(val & mask))
            nodeval = huffptr->bit0;
        else
            nodeval = huffptr->bit1;

        if(mask == 0x80)
        {
            val = *source++;
            mask = 1;
        }
        else mask <<= 1;

        if(nodeval < 256)
        {
            *dest++ = (byte)nodeval;
            written++;
            huffptr = headptr;
            if(dest >= end) break;
        }
        else
        {
            huffptr = hufftable + (nodeval - 256);
        }
    }
}

//
// CAL_CarmackExpand
//
// Length is the length of the EXPANDED data
//
#define NEARTAG 0xa7
#define FARTAG  0xa8

static void CAL_CarmackExpand(byte *source, word *dest, int length)
{
    word ch, chhigh, count, offset;
    byte *inptr;
    word *copyptr, *outptr;

    length /= 2;

    inptr = (byte *)source;
    outptr = dest;

    while(length > 0)
    {
        ch = READWORD(inptr);
        chhigh = ch >> 8;
        if(chhigh == NEARTAG)
        {
            count = ch & 0xff;
            if(!count)
            {                               // have to insert a word containing the tag byte
                ch |= *inptr++;
                *outptr++ = ch;
                length--;
            }
            else
            {
                offset = *inptr++;
                copyptr = outptr - offset;
                length -= count;
                if(length < 0) 
					return;

                while (count--)
                    *outptr++ = *copyptr++;
            }
        }
        else if(chhigh == FARTAG)
        {
            count = ch & 0xff;
            if(!count)
            {                               // have to insert a word containing the tag byte
                ch |= *inptr++;
                *outptr++ = ch;
                length --;
            }
            else
            {
                offset = READWORD(inptr);
                copyptr = dest + offset;
                length -= count;
                if(length<0) 
					return;

                while(count--)
                    *outptr++ = *copyptr++;
            }
        }
        else
        {
            *outptr++ = ch;
            length--;
        }
    }
}

//
// CAL_RLEWexpand
//
// length is EXPANDED length
//
static void CAL_RLEWexpand(word *source, word *dest, int32_t length, word rlewtag)
{
    word value, count, i;
    word *end = dest + length / 2;

//
// expand it
//
    do
    {
        value = *source++;
        if (value != rlewtag)
		{
            //
            // uncompressed
            //
            *dest++ = value;
		}
        else
        {
            //
            // compressed string
            //
            count = *source++;
            value = *source++;
            for (i = 1; i <= count; i++)
                *dest++ = value;
        }
    } while(dest < end);
}



////////////////////////////////////////////////////////////////////////////////
//
//                                         CACHE MANAGER ROUTINES
//
////////////////////////////////////////////////////////////////////////////////

//
// CAL_SafeOpen
//
// IOANCH: added a safe opening routine, as it's fairly largely used in this
// code (20130729)
//
static int CAL_SafeOpen(const char *path, int oflag)
{
   int ret = open(path, oflag);
   if(ret == -1)
      CA_CannotOpen(path);
   return ret;
}

//
// CAL_SetupGrFile
//
static void CAL_SetupGrFile ()
{
    PString fname;
    int handle;
    byte *compseg;

#ifdef GRHEADERLINKED

    ca_grhuffman = (huffnode *)&EGAdict;
    if(SPEAR())
        ca_grstarts_sod = (int32_t _seg *)FP_SEG(&EGAhead);
    else
        ca_grstarts_wl6 = (int32_t _seg *)FP_SEG(&EGAhead);

#else	//  !defined(GRHEADERLINKED)

//
// load ???dict.ext (huffman dictionary for graphics files)
//

   // IOANCH 20130726: case insensitive
   fname = I_ResolveCaseInsensitivePath(".",
                                        PString(ca_gdictname).
                                        concat(cfg_graphext)());

//    fname.copy(ca_gdictname).concat(cfg_graphext);  // IOANCH 20130509: don't
                                            // withExtension yet
   handle = CAL_SafeOpen(fname(), O_RDONLY | O_BINARY);

    read(handle, ca_grhuffman, sizeof(ca_grhuffman));
    close(handle);

    // load the data offsets from ???head.ext
   
   fname = I_ResolveCaseInsensitivePath(".",
                                        PString(ca_gheadname).
                                        concat(cfg_graphext)());

//    fname.copy(ca_gheadname).concat(cfg_graphext);

   handle = CAL_SafeOpen(fname(), O_RDONLY | O_BINARY);

    long headersize = lseek(handle, 0, SEEK_END);
    lseek(handle, 0, SEEK_SET);

    // IOANCH 20130301: unification culling
//    int testexp = sizeof(ca_grstarts_wl6);
	int expectedsize;
    
    if(SPEAR())
        expectedsize = lengthof(ca_grstarts_sod) - menu_missingep;
    else
        expectedsize = lengthof(ca_grstarts_wl6) - menu_missingep;

    if(!cfg_ignorenumchunks && headersize / 3 != (long) expectedsize)	// IOANCH 20130116: changed name
        Quit((PString("AutoWolf was not compiled for these data files:\n")<<fname<<
            " contains a wrong number of offsets ("<<int(headersize/3)<<
             " instead of "<<expectedsize<<")!\n\n"
            "Please check whether you are using the right executable!\n"
            "(For mod developers: perhaps you forgot to update NUMCHUNKS?)")());

    if(SPEAR())
    {
        byte data[lengthof(ca_grstarts_sod) * 3];
        read(handle, data, sizeof(data));
        close(handle);

        const byte* d = data;
        for (int32_t* i = ca_grstarts_sod; i != endof(ca_grstarts_sod); ++i)
        {
            const int32_t val = d[0] | d[1] << 8 | d[2] << 16;
            *i = (val == 0x00FFFFFF ? -1 : val);
            d += 3;
        }
    }
    else
    {
        byte data[lengthof(ca_grstarts_wl6) * 3];
        read(handle, data, sizeof(data));
        close(handle);
        
        const byte* d = data;
        for (int32_t* i = ca_grstarts_wl6; i != endof(ca_grstarts_wl6); ++i)
        {
            const int32_t val = d[0] | d[1] << 8 | d[2] << 16;
            *i = (val == 0x00FFFFFF ? -1 : val);
            d += 3;
        }
    }
#endif	//  !defined(GRHEADERLINKED)

//
// Open the graphics file, leaving it open until the game is finished
//
   fname = I_ResolveCaseInsensitivePath(".",
                                        PString(ca_gfilename).
                                        concat(cfg_graphext)());

//    fname.copy(ca_gfilename).concat(cfg_graphext);

   ca_grhandle = CAL_SafeOpen(fname(), O_RDONLY | O_BINARY);

//
// load the pic and sprite headers into the arrays in the data segment
//
   
   pictable = (pictabletype *)I_CheckedMalloc(SPEAR.g(NUMPICS)*
                                              sizeof(pictabletype));
   
    CAL_GetGrChunkLength(SPEAR.g(STRUCTPIC));        // position file pointer
   compseg = (byte *)I_CheckedMalloc(ca_chunkcomplen);
    read (ca_grhandle,compseg,ca_chunkcomplen);
    CAL_HuffExpand(compseg, (byte*)pictable, SPEAR.g(NUMPICS) *
                   sizeof(pictabletype), ca_grhuffman);
    free(compseg);
}

//==========================================================================

//
// CAL_SetupMapFile
//
static void CAL_SetupMapFile ()
{
    int     i;
    int handle;
    int32_t length,pos;
    PString fname;
   // IOANCH: moved struct here, only used here
   struct mapfiletype
   {
      word ca_RLEWtag;
      int32_t headeroffsets[100];
   } tinf; // IOANCH: created on stack

//
// load maphead.ext (offsets and tileinfo for map file)
//
   fname = I_ResolveCaseInsensitivePath(".",
                                        PString(ca_mheadname).
                                        concat(cfg_extension)());

//    fname.copy(ca_mheadname).concat(cfg_extension);

   handle = CAL_SafeOpen(fname(), O_RDONLY | O_BINARY);

    length = NUMMAPS*4+2; // used to be "filelength(handle);"
//   tinf = (mapfiletype *)I_CheckedMalloc(sizeof(mapfiletype));
    read(handle, &tinf, length);
    close(handle);

    ca_RLEWtag=tinf.ca_RLEWtag;

//
// open the data file
//
    // IOANCH 20130301: unification culling
   fname = I_ResolveCaseInsensitivePath(".",
                                        PString("GAMEMAPS.").
                                        concat(cfg_extension)());

//    fname.copy("GAMEMAPS.").concat(cfg_extension);

   ca_maphandle = CAL_SafeOpen(fname(), O_RDONLY | O_BINARY);

//
// load all map header
//
    for (i=0;i<NUMMAPS;i++)
    {
        pos = tinf.headeroffsets[i];
        if (pos<0)                          // $FFFFFFFF start is a sparse map
            continue;

       ca_mapheaderseg[i] = (maptype *)I_CheckedMalloc(sizeof(maptype));
        lseek(ca_maphandle,pos,SEEK_SET);
        read (ca_maphandle,(memptr)ca_mapheaderseg[i],sizeof(maptype));
    }

//    free(tinf);

//
// allocate space for 3 64*64 planes
//
    for (i=0;i<MAPPLANES;i++)
    {
       mapsegs[i] = (word *)I_CheckedMalloc(maparea * 2);
    }
}


//==========================================================================

//
// CAL_SetupAudioFile
//
static void CAL_SetupAudioFile ()
{
    PString fname;

//
// load AUDIOHED.ext (offsets for audio file)
//
   // IOANCH 20130726: case insensitive
   fname = I_ResolveCaseInsensitivePath(".",
                                        PString(ca_aheadname).
                                        concat(cfg_audioext)());
   // fname.copy(ca_aheadname).concat(cfg_audioext);

    void* ptr;
    if (!CA_LoadFile(fname(), &ptr))
        CA_CannotOpen(fname());
    ca_audiostarts = (int32_t*)ptr;

//
// open the data file
//
   fname = I_ResolveCaseInsensitivePath(".",
                                        PString(ca_afilename).
                                        concat(cfg_audioext)());

//   fname.copy(ca_afilename).concat(cfg_audioext);

   ca_audiohandle = CAL_SafeOpen(fname(), O_RDONLY | O_BINARY);
}

//==========================================================================

//
// CA_Startup
//
// Open all files and load in headers
//
void CA_Startup ()
{
#ifdef PROFILE
    unlink ("PROFILE.TXT");
    profilehandle = open("PROFILE.TXT", O_CREAT | O_WRONLY | O_TEXT);
#endif

    CAL_SetupMapFile ();
    CAL_SetupGrFile ();
    CAL_SetupAudioFile ();

    mapon = -1;
}

//==========================================================================

//
// CA_Shutdown
//
// Closes all files
//
void CA_Shutdown ()
{
    int i,start;

    if(ca_maphandle != -1)
        close(ca_maphandle);
    if(ca_grhandle != -1)
        close(ca_grhandle);
    if(ca_audiohandle != -1)
        close(ca_audiohandle);

    for(i=0; i<(signed int)SPEAR.g(NUMCHUNKS); i++)
        UNCACHEGRCHUNK(i);
    free(pictable);

    switch(ca_oldsoundmode)
    {
        case sdm_Off:
            return;
        case sdm_PC:
			// IOANCH 20130301: unification
            start = IMPALE(STARTPCSOUNDS);
            break;
        case sdm_AdLib:
            start = IMPALE((int)STARTADLIBSOUNDS);
            break;
    }

	// IOANCH 20130301: unification
	unsigned int NUMSOUNDS_cur = IMPALE((unsigned int)NUMSOUNDS);
    for(i=0; i<(signed int)NUMSOUNDS_cur; i++,start++)
        UNCACHEAUDIOCHUNK(start);
}

//===========================================================================

//
// CA_CacheAudioChunk
//
int32_t CA_CacheAudioChunk (int chunk)
{
    int32_t pos = ca_audiostarts[chunk];
    int32_t size = ca_audiostarts[chunk+1]-pos;

    if (ca_audiosegs[chunk])
        return size;                        // already in memory

    ca_audiosegs[chunk]=(byte *) I_CheckedMalloc(size);

    lseek(ca_audiohandle,pos,SEEK_SET);
    read(ca_audiohandle,ca_audiosegs[chunk],size);

    return size;
}

//
// CAL_CacheAdlibSoundChunk
//
static void CAL_CacheAdlibSoundChunk (int chunk)
{
    int32_t pos = ca_audiostarts[chunk];
    int32_t size = ca_audiostarts[chunk+1]-pos;

    if (ca_audiosegs[chunk])
        return;                        // already in memory

    lseek(ca_audiohandle, pos, SEEK_SET);
    read(ca_audiohandle, ca_bufferseg, ORIG_ADLIBSOUND_SIZE - 1);   // without data[1]

    AdLibSound *sound = (AdLibSound *) I_CheckedMalloc(size +
                                                       sizeof(AdLibSound) -
                                                       ORIG_ADLIBSOUND_SIZE);

    byte *ptr = (byte *) ca_bufferseg;
    sound->common.length = READLONGWORD(ptr);
    sound->common.priority = READWORD(ptr);
    sound->inst.mChar = *ptr++;
    sound->inst.cChar = *ptr++;
    sound->inst.mScale = *ptr++;
    sound->inst.cScale = *ptr++;
    sound->inst.mAttack = *ptr++;
    sound->inst.cAttack = *ptr++;
    sound->inst.mSus = *ptr++;
    sound->inst.cSus = *ptr++;
    sound->inst.mWave = *ptr++;
    sound->inst.cWave = *ptr++;
    sound->inst.nConn = *ptr++;
    sound->inst.voice = *ptr++;
    sound->inst.mode = *ptr++;
    sound->inst.unused[0] = *ptr++;
    sound->inst.unused[1] = *ptr++;
    sound->inst.unused[2] = *ptr++;
    sound->block = *ptr++;

    read(ca_audiohandle, sound->data, size - ORIG_ADLIBSOUND_SIZE + 1);  // + 1 because of byte data[1]

    ca_audiosegs[chunk]=(byte *) sound;
}

//===========================================================================

//
// CA_LoadAllSounds
//
// Purges all sounds, then loads all new ones (mode switch)
//
void CA_LoadAllSounds ()
{
    unsigned start,i;
    unsigned char cachein = 0;
    // IOANCH 20130303: don't use label use variable

    switch (ca_oldsoundmode)
    {
        case sdm_Off:
            cachein = 1;
            break;
        case sdm_PC:
			// IOANCH 20130301: unification
            start = IMPALE(STARTPCSOUNDS);
            break;
        case sdm_AdLib:
            start = IMPALE((unsigned)STARTADLIBSOUNDS);
            break;
    }

	// IOANCH 20130301: unification
	unsigned int NUMSOUNDS_cur = IMPALE((unsigned int)NUMSOUNDS);
    
    if(!cachein)
        for (i=0;i<NUMSOUNDS_cur;i++,start++)
            UNCACHEAUDIOCHUNK(start);

    ca_oldsoundmode = SoundMode;

    switch (SoundMode)
    {
        case sdm_Off:
			// IOANCH 20130301: unification
            start = IMPALE((unsigned)STARTADLIBSOUNDS);
			// needed for priorities...
            break;
        case sdm_PC:
            start = IMPALE(STARTPCSOUNDS);
            break;
        case sdm_AdLib:
            start = IMPALE((unsigned)STARTADLIBSOUNDS);
            break;
    }

	// IOANCH 20130301: unification
    if(start == IMPALE((unsigned)STARTADLIBSOUNDS))
    {
        for (i=0;i<NUMSOUNDS_cur;i++,start++)
            CAL_CacheAdlibSoundChunk(start);
    }
    else
    {
        for (i=0;i<NUMSOUNDS_cur;i++,start++)
            CA_CacheAudioChunk(start);
    }
}

//===========================================================================

//
// CAL_ExpandGrChunk
//
// Does whatever is needed with a pointer to a compressed chunk
//
static void CAL_ExpandGrChunk (int chunk, int32_t *source)
{
    int32_t    expanded;

    if (chunk >= (signed int)SPEAR.g(STARTTILE8) && chunk < (signed int)SPEAR.g(STARTEXTERNS))
    {
        //
        // expanded sizes of tile8/16/32 are implicit
        //

#define BLOCK           64
#define MASKBLOCK       128

        if (chunk<(signed int)SPEAR.g(STARTTILE8M))          // tile 8s are all in one chunk!
            expanded = BLOCK*SPEAR.g(NUMTILE8);
        else if (chunk<(signed int)SPEAR.g(STARTTILE16))
            expanded = MASKBLOCK*SPEAR.g(NUMTILE8M);
        else if (chunk<(signed int)SPEAR.g(STARTTILE16M))    // all other tiles are one/chunk
            expanded = BLOCK*4;
        else if (chunk<(signed int)SPEAR.g(STARTTILE32))
            expanded = MASKBLOCK*4;
        else if (chunk<(signed int)SPEAR.g(STARTTILE32M))
            expanded = BLOCK*16;
        else
            expanded = MASKBLOCK*16;
    }
    else
    {
        //
        // everything else has an explicit size longword
        //
        expanded = *source++;
    }

    //
    // allocate final space, decompress it, and free bigbuffer
    // Sprites need to have shifts made and various other junk
    //
    ca_grsegs[chunk]=(byte *) I_CheckedMalloc(expanded);

    CAL_HuffExpand((byte *) source, ca_grsegs[chunk], expanded, ca_grhuffman);
}

//
// CA_CacheGrChunk
//
// Makes sure a given chunk is in memory, loadiing it if needed
//
void CA_CacheGrChunk (int chunk)
{
    int32_t pos,compressed;
    int32_t *source;
    int  next;

    if (ca_grsegs[chunk])
        return;                             // already in memory

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
    pos = GRFILEPOS(chunk);
    if (pos<0)                              // $FFFFFFFF start is a sparse tile
        return;

    next = chunk +1;
    while (GRFILEPOS(next) == -1)           // skip past any sparse tiles
        next++;

    compressed = GRFILEPOS(next)-pos;

    lseek(ca_grhandle,pos,SEEK_SET);

    if (compressed<=BUFFERSIZE)
    {
        read(ca_grhandle,ca_bufferseg,compressed);
        source = ca_bufferseg;
    }
    else
    {
        source = (int32_t *) I_CheckedMalloc(compressed);

        read(ca_grhandle,source,compressed);
    }

    CAL_ExpandGrChunk (chunk,source);

    if (compressed>BUFFERSIZE)
        free(source);
}



//==========================================================================

//
// CA_CacheScreen
//
// Decompresses a chunk from disk straight onto the screen
//
void CA_CacheScreen (int chunk)
{
    int32_t    pos,compressed,expanded;
    memptr  bigbufferseg;
    int32_t    *source;
    int             next;
    byte *pic, *vbuf;
    int x, y, scx, scy;
    unsigned i, j;

//
// load the chunk into a buffer
//
    pos = GRFILEPOS(chunk);
    next = chunk +1;
    while (GRFILEPOS(next) == -1)           // skip past any sparse tiles
        next++;
    compressed = GRFILEPOS(next)-pos;

    lseek(ca_grhandle,pos,SEEK_SET);

    bigbufferseg=I_CheckedMalloc(compressed);

    read(ca_grhandle,bigbufferseg,compressed);
    source = (int32_t *) bigbufferseg;

    expanded = *source++;

//
// allocate final space, decompress it, and free bigbuffer
// Sprites need to have shifts made and various other junk
//
    pic = (byte *) I_CheckedMalloc(64000);

    CAL_HuffExpand((byte *) source, pic, expanded, ca_grhuffman);

    vbuf = I_LockBuffer();
    if(vbuf != NULL)
    {
        for(y = 0, scy = 0; y < 200; y++, scy += vid_scaleFactor)
        {
            for(x = 0, scx = 0; x < 320; x++, scx += vid_scaleFactor)
            {
                byte col = pic[(y * 80 + (x >> 2)) + (x & 3) * 80 * 200];
                for(i = 0; i < vid_scaleFactor; i++)
                    for(j = 0; j < vid_scaleFactor; j++)
                        vbuf[(scy + i) * vid_bufferPitch + scx + j] = col;
            }
        }
        I_UnlockBuffer();
    }
    free(pic);
    free(bigbufferseg);
}

//==========================================================================

//
// CA_CacheMap
//
// WOLF: This is specialized for a 64*64 map size
//
void CA_CacheMap (int mapnum)
{
    int32_t   pos,compressed;
    int       plane;
    word     *dest;
    memptr    bigbufferseg;
    unsigned  size;
    word     *source;
    // IOANCH 20130301: unification culling
    word     *buffer2seg;
    int32_t   expanded;

    mapon = mapnum;

//
// load the planes into the allready allocated buffers
//
    size = maparea*2;

    for (plane = 0; plane<MAPPLANES; plane++)
    {
        pos = ca_mapheaderseg[mapnum]->planestart[plane];
        compressed = ca_mapheaderseg[mapnum]->planelength[plane];

        dest = mapsegs[plane];

        lseek(ca_maphandle,pos,SEEK_SET);
        if (compressed<=BUFFERSIZE)
            source = (word *) ca_bufferseg;
        else
        {
            bigbufferseg=I_CheckedMalloc(compressed);

            source = (word *) bigbufferseg;
        }

        read(ca_maphandle,source,compressed);
        // IOANCH 20130301: unification culling
        //
        // unhuffman, then unRLEW
        // The huffman'd chunk has a two byte expanded length first
        // The resulting RLEW chunk also does, even though it's not really
        // needed
        //
        expanded = *source;
        source++;
        buffer2seg = (word *) I_CheckedMalloc(expanded);

        CAL_CarmackExpand((byte *) source, buffer2seg,expanded);
        CAL_RLEWexpand(buffer2seg+1,dest,size,ca_RLEWtag);
        free(buffer2seg);


        if (compressed>BUFFERSIZE)
            free(bigbufferseg);
    }
}

//===========================================================================

//
// CA_CannotOpen
//
void CA_CannotOpen(const char *string)
{
    // IOANCH 20130510: don't use a statically allocated char array.
    Quit ((PString("Can't open ") << string << "!\n")());
}
