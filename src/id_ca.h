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

#ifndef __ID_CA__
#define __ID_CA__

// IOANCH 20121223: Cleaned up this file

//===========================================================================

#define NUMMAPS         60
#ifdef USE_FLOORCEILINGTEX
    #define MAPPLANES       3
#else
    #define MAPPLANES       2
#endif

#define UNCACHEGRCHUNK(chunk) {if(ca_grsegs[chunk]) {free(ca_grsegs[chunk]); ca_grsegs[chunk]=NULL;}}
#define UNCACHEAUDIOCHUNK(chunk) {if(ca_audiosegs[chunk]) {free(ca_audiosegs[chunk]); ca_audiosegs[chunk]=NULL;}}

//===========================================================================

#pragma pack(push, 1)
struct maptype
{
    int32_t planestart[3];
    word    planelength[3];
    word    width,height;
    char    name[16];
};
#pragma pack(pop)

//===========================================================================

extern  int   mapon;

extern  word *mapsegs[MAPPLANES];
// IOANCH 20130301: unification
extern  byte *ca_audiosegs[NUMSNDCHUNKS_sod > NUMSNDCHUNKS_wl6 ? NUMSNDCHUNKS_sod :
						NUMSNDCHUNKS_wl6];
extern  byte *ca_grsegs[NUMCHUNKS_sod > NUMCHUNKS_wl6 ? NUMCHUNKS_sod : NUMCHUNKS_wl6];

class PString;
extern  PString  cfg_extension;
extern  PString  cfg_graphext;
extern  PString  cfg_audioext;

//===========================================================================

Boolean CA_LoadFile (const char *filename, memptr *ptr);
Boolean CA_WriteFile (const char *filename, void *ptr, int32_t length);

// int32_t CA_RLEWCompress (word *source, int32_t length, word *dest, word rlewtag);

// void CA_RLEWexpand (word *source, word *dest, int32_t length, word rlewtag);

void CA_Startup ();
void CA_Shutdown ();

int32_t CA_CacheAudioChunk (int chunk);
void CA_LoadAllSounds ();

void CA_CacheGrChunk (int chunk);
void CA_CacheMap (int mapnum);

void CA_CacheScreen (int chunk);

void CA_CannotOpen(const char *name);

#endif
