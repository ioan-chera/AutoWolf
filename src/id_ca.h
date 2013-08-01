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

struct huffnode
{
   word bit0,bit1;       // 0-255 is a character, > is a pointer to a node
};

//
// GraphicLoader
//
// Class to cache graphic chunks from VGAGRAPH
//
class GraphicLoader
{
public:
   struct pictabletype
   {
      int16_t width, height;
   };
private:
   
   FILE *m_file;
   huffnode m_grhuffman[255];
   int32_t  m_grstarts_wl6[NUMCHUNKS_wl6 + 1];
   int32_t  m_grstarts_sod[NUMCHUNKS_sod + 1];
   pictabletype m_pictable[NUMPICS_wl6 > NUMPICS_sod ? NUMPICS_wl6 : NUMPICS_sod];
   byte *m_grsegs[NUMCHUNKS_sod > NUMCHUNKS_wl6 ? NUMCHUNKS_sod : NUMCHUNKS_wl6];
   
   int32_t grFilePos(size_t idx);
   int32_t getChunkLength(int chunk);
   void expandChunk(int chunk, const int32_t *source);
   
   void emptyFields()
   {
      m_file = NULL;
      memset(m_grhuffman, 0, sizeof(m_grhuffman));
      memset(m_grstarts_sod, 0, sizeof(m_grstarts_sod));
      memset(m_grstarts_wl6, 0, sizeof(m_grstarts_wl6));
      memset(m_pictable, 0, sizeof(m_pictable));
      memset(m_grsegs, 0, sizeof(m_grsegs));
   }
   
public:
   GraphicLoader()
   {
      emptyFields();
   }
   void close()
   {
      if(m_file)
         fclose(m_file);
      for (int i = 0; i < (signed int)SPEAR.g(NUMCHUNKS); ++i)
         uncacheChunk(i);
      emptyFields();
   }
   
   void uncacheChunk(int chunk)
   {
      if(m_grsegs[chunk])
      {
         free(m_grsegs[chunk]);
         m_grsegs[chunk] = NULL;
      }
   }

   ~GraphicLoader()
   {
      close();
   }
   void loadFromFile(const char *vgadict, const char *vgahead,
                     const char *vgagraph);
   void cacheChunk(int chunk);
   void cacheScreen(int chunk);
   const byte *operator [](int index) const
   {
      return m_grsegs[index];
   }
   pictabletype sizeAt(int index) const
   {
      return m_pictable[index];
   }
   bool hasPictable() const
   {
      return m_file != NULL;
   }
};
extern GraphicLoader graphSegs;

//
// AudioLoader
//
// for ADLIB and PC SOUND
//
class AudioLoader
{
   FILE *m_file;
   int32_t *m_audiostarts;
   byte *m_audiosegs[NUMSNDCHUNKS_sod > NUMSNDCHUNKS_wl6 ? NUMSNDCHUNKS_sod :
                      NUMSNDCHUNKS_wl6];
   void emptyFields()
   {
      m_file = NULL;
      m_audiostarts = NULL;
      memset(m_audiosegs, 0, sizeof(m_audiosegs));
      m_oldsoundmode = sdm_Off;
   }
   SDMode m_oldsoundmode;
public:
   void uncacheChunk(int chunk)
   {
      if(m_audiosegs[chunk])
      {
         free(m_audiosegs[chunk]);
         m_audiosegs[chunk] = NULL;
      }
   }
   void close();
   AudioLoader()
   {
      emptyFields();
   }
   ~AudioLoader()
   {
      close();
   }
   void loadFromFile(const char *audiohed, const char *audiot);
   int32_t cacheChunk (int chunk);
   void cacheAdlibChunk (int chunk);
   void loadAllSounds (SDMode newMode);
   
   const byte * const* ptr(int index) const
   {
      return &m_audiosegs[index];
   }
   const byte *operator[] (int index) const
   {
      return m_audiosegs[index];
   }
};
extern AudioLoader audioSegs;

//===========================================================================

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

void CA_CannotOpen(const char *name);

#endif
