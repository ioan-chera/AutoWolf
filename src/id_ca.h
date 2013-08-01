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

// IOANCH 20130801: use classes

//
// MapLoader
//
// Loads maps, caching them on demand to a mapsegs array
//
class MapLoader
{
   struct maptype
   {
      int32_t planestart[3];
      word    planelength[3];
      word    width,height;
      char    name[16];
   };
   
   FILE *m_file;    // file handle
   word m_RLEWtag;  // RLEW compression tag
   maptype  m_mapheaderseg[NUMMAPS];
   word     m_mapsegs[MAPPLANES][maparea];
   int m_mapon;
   
public:
   MapLoader() : m_file(NULL), m_RLEWtag(0), m_mapon(-1)
   {
      memset(m_mapheaderseg, 0, sizeof(m_mapheaderseg));
      memset(m_mapsegs, 0, sizeof(m_mapsegs));
   }
   void close()
   {
      if(m_file)
      {
         fclose(m_file);
         m_file = NULL;
      }
   }

   ~MapLoader()
   {
      close();
   }
   
   void loadFromFile(const char *maphead, const char *gamemaps);
   void cacheMap(int mapnum, short episode = 0);
   
   word operator()(int plane, int x, int y) const
   {
      return m_mapsegs[plane][(y << mapshift) + x];
   }
   word *ptr(int plane, int x, int y)
   {
      return &m_mapsegs[plane][(y << mapshift) + x];
   }
   const word *cptr(int plane, int x, int y) const
   {
      return &m_mapsegs[plane][(y << mapshift) + x];
   }
   const word *operator[] (int plane) const
   {
      return m_mapsegs[plane];
   }
   word *operator[] (int plane)
   {
      return m_mapsegs[plane];
   }
   void setAt(int plane, int x, int y, word value)
   {
      m_mapsegs[plane][(y << mapshift) + x] = value;
   }
   int map() const
   {
      return m_mapon;
   }
};
extern MapLoader mapSegs;
//===========================================================================


//===========================================================================

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

void CA_CacheScreen (int chunk);

void CA_CannotOpen(const char *name);

#endif
