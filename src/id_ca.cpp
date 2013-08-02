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

#include "wl_def.h"
#include "wl_main.h"
#include "Config.h"
#include "i_system.h"
#include "i_video.h"
#include "id_ca.h"
#include "m_swap.h"
#include "PString.h"

MapLoader mapSegs;
GraphicLoader graphSegs;
AudioLoader audioSegs;


//
// MapLoader::loadFromFile
//
// IOANCH: use classes
// Gets addresses from file to prepare for caching
//
void MapLoader::loadFromFile(const char *maphead, const char *gamemaps)
{
   int     i;
   int32_t pos;
   // IOANCH: moved struct here, only used here
   
   int32_t headeroffsets[NUMMAPS];   // IOANCH: used to be 100
   
   //
   // load maphead.ext (offsets and tileinfo for map file)
   //
   InBuffer header;
   if(!header.openFile(maphead, BufferedFileBase::LENDIAN))
      CA_CannotOpen(maphead);
   
//   length = NUMMAPS * sizeof(int32_t); // used to be "filelength(handle);"
   header.readUint16(m_RLEWtag);
   header.readSint32Array(headeroffsets, NUMMAPS);
   header.Close();
   
   //
   // open the data file
   //
   // IOANCH 20130301: unification culling
   
   if(!m_filebuf.openFile(gamemaps, BufferedFileBase::LENDIAN))
      CA_CannotOpen(gamemaps);
   
   //
   // load all map header
   //
   for (i = 0; i < NUMMAPS; i++)
   {
      pos = headeroffsets[i];
      if (pos < 0)                          // $FFFFFFFF start is a sparse map
         continue;
      
      m_filebuf.seek(pos, SEEK_SET);
      maptype &m = m_mapheaderseg[i];
      m_filebuf.readSint32Array(m.planestart, lengthof(m.planestart));
      m_filebuf.readUint16Array(m.planelength, lengthof(m.planelength));
      m_filebuf.readUint16(m.width);
      m_filebuf.readUint16(m.height);
      m_filebuf.read(m.name, lengthof(m.name));
   }                                                           // pointer
   m_mapon = -1;
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
         {                     // have to insert a word containing the tag byte
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
         {                     // have to insert a word containing the tag byte
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
// IOANCH: possibly added endian swapping, but commented out, since it may be
// the output of Carmack expansion, which already does swapping
//
static void CAL_RLEWexpand(word *source, word *dest, int32_t length, word rlewtag)
{
   word value, count, i;
   word *end = dest + length / sizeof(word);
   
   //
   // expand it
   //
   do
   {
      value = /*SwapUShort(*/*source++/*)*/;
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
         count = /*SwapUShort(*/*source++/*)*/;
         value = /*SwapUShort(*/*source++/*)*/;
         for (i = 1; i <= count; i++)
            *dest++ = value;
      }
   } while(dest < end);
}


//
// MapLoader::cacheMap
//
// WOLF: This is specialized for a 64*64 map size
//
#define BUFFERSIZE 0x1000
void MapLoader::cacheMap (int mapnum, short episode)
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
   
   m_mapon = mapnum - 10 * episode;
   
   int32_t bufferseg[BUFFERSIZE/4];
   
   //
   // load the planes into the allready allocated buffers
   //
   size = maparea*2;
   
   for (plane = 0; plane<MAPPLANES; plane++)
   {
      pos = m_mapheaderseg[mapnum].planestart[plane];
      compressed = m_mapheaderseg[mapnum].planelength[plane];
      
      dest = m_mapsegs[plane];
      
      m_filebuf.seek(pos, SEEK_SET);
      if (compressed <= BUFFERSIZE)
         source = (word *) bufferseg;
      else
      {
         bigbufferseg = I_CheckedMalloc(compressed);
         
         source = (word *) bigbufferseg;
      }
      
      m_filebuf.read(source, compressed);
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
      CAL_RLEWexpand(buffer2seg+1,dest,size,m_RLEWtag);
      free(buffer2seg);
      
      if (compressed > BUFFERSIZE)
         free(bigbufferseg);
   }
}

////////////////////////////////////////////////////////////////////////////////

int     menu_missingep = 0;

//
// CAL_HuffExpand
//
static void CAL_HuffExpand(const byte *source, byte *dest, int32_t length,
                           const huffnode *hufftable)
{
   const byte *end;
   const huffnode *headptr, *huffptr;
   
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
// GRFILEPOS
//
int32_t GraphicLoader::grFilePos( size_t idx)
{
   if(SPEAR())
      assert(idx < lengthof(m_grstarts_sod));
   else
      assert(idx < lengthof(m_grstarts_wl6));
	return IMPALE(m_grstarts)[idx];
}

//
// GraphicLoader::getChunkLength
//
// Obtains length of chunk
//
int32_t GraphicLoader::getChunkLength(int chunk)
{
//   fseek(m_file, grFilePos(chunk), SEEK_SET);
   return grFilePos(chunk + 1) - grFilePos(chunk) - 4;
}

//
// GraphicLoader::loadFromFile
//
// Setups the graphic loader with Wolf3D VGA graphic files
//
void GraphicLoader::loadFromFile(const char *vgadict, const char *vgahead,
                  const char *vgagraph)
{
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
   
   //    fname.copy(ca_gdictname).concat(cfg_graphext);  // IOANCH 20130509: don't
   // withExtension yet

   InBuffer dfile;
   if(!dfile.openFile(vgadict, BufferedFileBase::LENDIAN))
      CA_CannotOpen(vgadict);
   
   for (size_t u = 0; u < lengthof(m_grhuffman); ++u)
   {
      dfile.readUint16(m_grhuffman[u].bit0);
      dfile.readUint16(m_grhuffman[u].bit1);
   }

   dfile.Close();
   
   // load the data offsets from ???head.ext
   
   if(!dfile.openFile(vgahead, BufferedFileBase::LENDIAN))
      CA_CannotOpen(vgahead);
   
   dfile.seek(0, SEEK_END);
   long headersize = dfile.Tell();
   dfile.seek(0, SEEK_SET);
   
   // IOANCH 20130301: unification culling
   //    int testexp = sizeof(ca_grstarts_wl6);
	int expectedsize;
   
   if(SPEAR())
      expectedsize = lengthof(m_grstarts_sod) - menu_missingep;
   else
      expectedsize = lengthof(m_grstarts_wl6) - menu_missingep;
   
   if(!cfg_ignorenumchunks && headersize / 3 != (long) expectedsize)	// IOANCH 20130116: changed name
      Quit((PString("AutoWolf was not compiled for these data files:\n")<<vgahead<<
            " contains a wrong number of offsets ("<<int(headersize/3)<<
            " instead of "<<expectedsize<<")!\n\n"
            "Please check whether you are using the right executable!\n"
            "(For mod developers: perhaps you forgot to update NUMCHUNKS?)")());
   
   if(SPEAR())
   {
      byte data[lengthof(m_grstarts_sod) * 3];
      dfile.read(data, sizeof(data));
      dfile.Close();
      
      const byte* d = data;
      for (int32_t* i = m_grstarts_sod; i != endof(m_grstarts_sod); ++i)
      {
         const int32_t val = d[0] | d[1] << 8 | d[2] << 16;
         *i = (val == 0x00FFFFFF ? -1 : val);
         d += 3;
      }
   }
   else
   {
      byte data[lengthof(m_grstarts_wl6) * 3];
      dfile.read(data, sizeof(data));
      dfile.Close();
      
      const byte* d = data;
      for (int32_t* i = m_grstarts_wl6; i != endof(m_grstarts_wl6); ++i)
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
   
   //    fname.copy(ca_gfilename).concat(cfg_graphext);
   
   if(!m_filebuf.openFile(vgagraph, BufferedFileBase::LENDIAN))
      CA_CannotOpen(vgagraph);
   
   //
   // load the pic and sprite headers into the arrays in the data segment
   //
   
//   m_pictable = (pictabletype *)I_CheckedMalloc(SPEAR.g(NUMPICS)*
//                                                 sizeof(pictabletype));
   int32_t complen = getChunkLength(SPEAR.g(STRUCTPIC));
   int32_t explen;
   m_filebuf.seek(grFilePos(SPEAR.g(STRUCTPIC)), SEEK_SET);
   m_filebuf.readSint32(explen);
   compseg = (byte *)I_CheckedMalloc(complen);
   m_filebuf.read(compseg, complen);
   CAL_HuffExpand(compseg, (byte*)m_pictable, SPEAR.g(NUMPICS) *
                  sizeof(pictabletype), m_grhuffman);
   m_pictableloaded = true;
   free(compseg);
}

//
// GraphicLoader::expandChunk
//
// Does whatever is needed with a pointer to a compressed chunk
//
void GraphicLoader::expandChunk (int chunk, const int32_t *source)
{
   int32_t    expanded;
   
   if (chunk >= (signed int)SPEAR.g(STARTTILE8) &&
       chunk < (signed int)SPEAR.g(STARTEXTERNS))
   {
      //
      // expanded sizes of tile8/16/32 are implicit
      //
      
#define BLOCK           64
#define MASKBLOCK       128
      
      if (chunk<(signed int)SPEAR.g(STARTTILE8M)) // tile 8s are all in one chunk!
         expanded = BLOCK*SPEAR.g(NUMTILE8);
      else if (chunk<(signed int)SPEAR.g(STARTTILE16))
         expanded = MASKBLOCK*SPEAR.g(NUMTILE8M);
      else if (chunk<(signed int)SPEAR.g(STARTTILE16M))// all other tiles are one/chunk
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
   m_grsegs[chunk]=(byte *) I_CheckedMalloc(expanded);
   
   CAL_HuffExpand((byte *) source, m_grsegs[chunk], expanded, m_grhuffman);
}


//
// GraphicLoader::cacheChunk
//
// Makes sure a given chunk is in memory, loadiing it if needed
//
void GraphicLoader::cacheChunk (int chunk)
{
   int32_t pos,compressed;
   int32_t *source;
   int  next;
   
   int32_t bufferseg[BUFFERSIZE/4];
   
   if (m_grsegs[chunk])
      return;                             // already in memory
   
   //
   // load the chunk into a buffer, either the miscbuffer if it fits, or allocate
   // a larger buffer
   //
   pos = grFilePos(chunk);
   if (pos<0)                              // $FFFFFFFF start is a sparse tile
      return;
   
   next = chunk +1;
   while (grFilePos(next) == -1)           // skip past any sparse tiles
      next++;
   
   compressed = grFilePos(next)-pos;
   m_filebuf.seek(pos, SEEK_SET);
   
   if (compressed<=BUFFERSIZE)
   {
      m_filebuf.read(bufferseg, compressed);
      source = bufferseg;
   }
   else
   {
      source = (int32_t *) I_CheckedMalloc(compressed);
      m_filebuf.read(source, compressed);
   }
   
   expandChunk(chunk, source);
   
   if (compressed>BUFFERSIZE)
      free(source);
}

//
// GraphicLoader::cacheScreen
//
// Decompresses a chunk from disk straight onto the screen
//
void GraphicLoader::cacheScreen (int chunk)
{
   int32_t    pos,compressed,expanded;
   memptr  bigbufferseg;
   const int32_t    *source;
   int             next;
   byte *pic, *vbuf;
   int x, y, scx, scy;
   unsigned i, j;
   
   //
   // load the chunk into a buffer
   //
   pos = grFilePos(chunk);
   next = chunk +1;
   while (grFilePos(next) == -1)           // skip past any sparse tiles
      next++;
   compressed = grFilePos(next)-pos;
   
   m_filebuf.seek(pos, SEEK_SET);
   
   bigbufferseg=I_CheckedMalloc(compressed);
   
   m_filebuf.read(bigbufferseg, compressed);

   source = (const int32_t *) bigbufferseg;
   
   expanded = *source++;
   
   //
   // allocate final space, decompress it, and free bigbuffer
   // Sprites need to have shifts made and various other junk
   //
   pic = (byte *) I_CheckedMalloc(64000);
   
   CAL_HuffExpand((const byte *) source, pic, expanded, m_grhuffman);
   
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

//
// AudioLoader::loadFromFile
//
void AudioLoader::loadFromFile (const char *audiohed, const char *audiot)
{
   //
   // load AUDIOHED.ext (offsets for audio file)
   //
   
   void* ptr;
   if (!CA_LoadFile(audiohed, &ptr))
      CA_CannotOpen(audiohed);
   m_audiostarts = (int32_t*)ptr;
   
   //
   // open the data file
   //
   if(!m_filebuf.openFile(audiot, BufferedFileBase::LENDIAN))
      CA_CannotOpen(audiot);
}

//
// AudioLoader::cacheChunk
//
int32_t AudioLoader::cacheChunk (int chunk)
{
   int32_t pos = m_audiostarts[chunk];
   int32_t size = m_audiostarts[chunk+1]-pos;
   
   if (m_audiosegs[chunk])
      return size;                        // already in memory
   
   m_audiosegs[chunk]=(byte *) I_CheckedMalloc(size);
   
   m_filebuf.seek(pos, SEEK_SET);
   m_filebuf.read(m_audiosegs[chunk], size);
   
   return size;
}

//
// AudioLoader::cacheAdlibChunk
//
void AudioLoader::cacheAdlibChunk (int chunk)
{
   int32_t pos = m_audiostarts[chunk];
   int32_t size = m_audiostarts[chunk+1]-pos;
   
   if (m_audiosegs[chunk])
      return;                        // already in memory
   
   int32_t bufferseg[BUFFERSIZE/4];
   
   m_filebuf.seek(pos, SEEK_SET);
   m_filebuf.read(bufferseg, ORIG_ADLIBSOUND_SIZE - 1);
   
   // without data[1]
   
   AdLibSound *sound = (AdLibSound *) I_CheckedMalloc(size +
                                                      sizeof(AdLibSound) -
                                                      ORIG_ADLIBSOUND_SIZE);
   
   byte *ptr = (byte *) bufferseg;
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
   
   m_filebuf.read(sound->data, size - ORIG_ADLIBSOUND_SIZE + 1);
   // + 1 because of byte data[1]
   
   m_audiosegs[chunk]=(byte *) sound;
}

//
// AudioLoader::loadAllSounds
//
// Purges all sounds, then loads all new ones (mode switch)
//
void AudioLoader::loadAllSounds (SDMode newMode)
{
   unsigned start,i;
   unsigned char cachein = 0;
   // IOANCH 20130303: don't use label use variable
   
   switch (m_oldsoundmode)
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
	const unsigned int NUMSOUNDS_cur = IMPALE((unsigned int)NUMSOUNDS);
   
   if(!cachein)
      for (i=0;i<NUMSOUNDS_cur;i++,start++)
         uncacheChunk(start);
   
   m_oldsoundmode = newMode;
   
   switch (newMode)
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
      for (i = 0; i < NUMSOUNDS_cur; i++, start++)
         cacheAdlibChunk(start);
   }
   else
   {
      for (i = 0; i < NUMSOUNDS_cur; i++, start++)
         cacheChunk(start);
   }
}

//
// AudioLoader::close
//
// Cleaner
//
void AudioLoader::close()
{
   m_filebuf.Close();
   free(m_audiostarts);
   int start = -1;
   switch (m_oldsoundmode)
   {
      case sdm_Off:
         break;
      case sdm_PC:
         start = IMPALE(STARTPCSOUNDS);
         break;
      case sdm_AdLib:
         start = IMPALE((int)STARTADLIBSOUNDS);
         break;
   }
   if(start > -1)
   {
      unsigned int NUMSOUNDS_cur = IMPALE((unsigned int)NUMSOUNDS);
      for(int i=0; i<(signed int)NUMSOUNDS_cur; i++,start++)
         uncacheChunk(start);
   }
   emptyFields();
}

////////////////////////////////////////////////////////////////////////////////
//
//                             LOCAL CONSTANTS
//
////////////////////////////////////////////////////////////////////////////////
// IOANCH: moved structure into a local scope, only used there

////////////////////////////////////////////////////////////////////////////////
//
//                             GLOBAL VARIABLES
//
////////////////////////////////////////////////////////////////////////////////

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



#ifdef GRHEADERLINKED
static huffnode *ca_grhuffman;
#else
#endif

////////////////////////////////////////////////////////////////////////////////
//
//                            LOW LEVEL ROUTINES
//
////////////////////////////////////////////////////////////////////////////////

//
// CA_WriteFile
//
// Writes a file from a memory buffer
//
Boolean8 CA_WriteFile(const char *filename, void *ptr, int32_t length)
{
   FILE *f = fopen(filename, "wb");
//    const int handle = open(filename, O_CREAT | O_WRONLY | O_BINARY, 0644);
    if(!f)
        return false;

    if(fwrite(ptr, 1, length, f) < length)
    {
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}

//
// CA_LoadFile
//
// Allocate space for and load a file
// IOANCH: uses fopen now
//
Boolean8 CA_LoadFile(const char *filename, memptr *ptr)
{
    int32_t size;

   InBuffer file;
   
   //FILE *file = fopen(filename, "rb");
   if(!file.openFile(filename, BufferedFileBase::LENDIAN))
        return false;

   file.seek(0, SEEK_END);
   size = file.Tell();
   file.seek(0, SEEK_SET);
   *ptr = I_CheckedMalloc(size);
   if(file.read(*ptr, size) < size)
    {
       file.Close();
        return false;
    }
   file.Close();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
//                                         CACHE MANAGER ROUTINES
//
////////////////////////////////////////////////////////////////////////////////

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

   // IOANCH: use C++ classes
   mapSegs.loadFromFile(I_ResolveCaseInsensitivePath(".",
                                                     PString(ca_mheadname).
                                                     concat(cfg_extension)())(),
                        I_ResolveCaseInsensitivePath(".",
                                                     PString("GAMEMAPS.").
                                                     concat(cfg_extension)())());
   
   graphSegs.loadFromFile(I_ResolveCaseInsensitivePath(".",
                                                      PString(ca_gdictname).
                                                      concat(cfg_graphext)())(),
                          I_ResolveCaseInsensitivePath(".",
                                                      PString(ca_gheadname).
                                                       concat(cfg_graphext)())(),
                          I_ResolveCaseInsensitivePath(".",
                                                       PString(ca_gfilename).
                                                       concat(cfg_graphext)())());

   audioSegs.loadFromFile(I_ResolveCaseInsensitivePath(".",
                                                       PString(ca_aheadname).
                                                       concat(cfg_audioext)())(),
                          I_ResolveCaseInsensitivePath(".",
                                                       PString(ca_afilename).
                                                       concat(cfg_audioext)())());
}

//==========================================================================

//
// CA_Shutdown
//
// Closes all files
//
void CA_Shutdown ()
{
   mapSegs.close();
   graphSegs.close();
   audioSegs.close();
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
