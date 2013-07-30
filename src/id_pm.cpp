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
#include "wl_main.h"

VSwapContainer vSwapData;

//
// Copy constructor
//
VSwapContainer::VSwapContainer(const VSwapContainer &other) :
m_numChunks(other.m_numChunks),
m_spriteStart(other.m_spriteStart),
m_soundStart(other.m_soundStart),
m_soundInfoPagePadded(other.m_soundInfoPagePadded),
m_pageDataSize(other.m_pageDataSize)
{
   m_pages = (uint8_t **) I_CheckedMalloc((m_numChunks + 1) *
                                          sizeof(uint8_t *));
   
   memcpy(m_pages, other.m_pages, (m_numChunks + 1) * sizeof(uint8_t *));
   m_pageData = (uint32_t *) I_CheckedMalloc(m_pageDataSize);
   memcpy(m_pageData, other.m_pageData, m_pageDataSize);
}

//
// VSwapContainer::clear
//
// Clears the contents of the wrapper, freeing data.
//
void VSwapContainer::clear()
{
   m_numChunks = 0;
   m_spriteStart = 0;
   m_soundStart = 0;
   m_soundInfoPagePadded = false;
   free(m_pages);
   free(m_pageData);
   m_pages = NULL;
   m_pageData = NULL;
   m_pageDataSize = 0;
}

//
// VSwapContainer::loadFile
//
// Reads data from a VSWap file. Returns false on error.
// Do it safely: if an error occurs, do not lose current data
//
bool VSwapContainer::loadFile(const char *filename
                              //, PString *errmsg
                              )
{
   VSwapContainer n;
   
   uint32_t *pageOffsets;
   word *pageLengths;
   long fileSize;
   long unpaddedDataSize;
   uint32_t dataStart;
   unsigned u;
   int alignPadding;
   uint32_t offs;
   uint8_t *ptr;
   
   FILE *file = fopen(filename, "rb");
   
   if(!file)
   {
//      if(errmsg)
//      {
//         errmsg->copy("Can't open ").concat(filename).concat(": ").
//         concat(strerror(errno));
//      }
      CA_CannotOpen(filename);
      return false;
   }
   
   // read header
   n.m_numChunks = 0;
   fread(&n.m_numChunks, sizeof(word), 1, file);
   n.m_spriteStart = 0;
   fread(&n.m_spriteStart, sizeof(word), 1, file);
   n.m_soundStart = 0;
   fread(&n.m_soundStart, sizeof(word), 1, file);
   
   pageOffsets = (uint32_t *) I_CheckedMalloc((n.m_numChunks + 1) *
                                              sizeof(int32_t));
   for (u = 0; u <= n.m_numChunks; ++u)
      fread(pageOffsets + u, 1, sizeof(uint32_t), file);
   
   pageLengths = (word *) I_CheckedMalloc(n.m_numChunks * sizeof(word));
   
   for (u = 0; u < n.m_numChunks; ++u)
      fread(pageLengths + u, 1, sizeof(word), file);
   
   fseek(file, 0, SEEK_END);
   fileSize = ftell(file);
   unpaddedDataSize = fileSize - pageOffsets[0];
   if(unpaddedDataSize > (size_t) -1)
   {
//      if(errmsg)
//      {
//         errmsg->copy("The page file \"").concat(filename).concat("\" is too large!");
//      }
      free(pageOffsets);
      free(pageLengths);
      fclose(file);
      Quit(PString("The page file \"").concat(filename).concat("\" is too large!")());
      return false;
   }
   
   pageOffsets[n.m_numChunks] = fileSize;
   
   dataStart = pageOffsets[0];
   
   // Check that all pageOffsets are valid
   for(u = 0; u < n.m_numChunks; ++u)
   {
      if(!pageOffsets[u])
         continue;   // sparse page
      if(pageOffsets[u] < dataStart || pageOffsets[u] >= (size_t) fileSize)
      {
//         if(errmsg)
//         {
//            errmsg->copy("Illegal page offset for page ").concat(u).concat(": ").
//            concat(pageOffsets[u]).concat(" (filesize: ").
//            concat(fileSize).concat(")");
//         }
         Quit(PString("Illegal page offset for page ").concat(u).concat("): ").concat(pageOffsets[u]).concat(" (filesize: ").concat(fileSize).concat(")")());
         free(pageOffsets);
         free(pageLengths);
         fclose(file);
         return false;
         
      }
   }
   
   // Calculate total amount of padding needed for sprites and sound info page
   alignPadding = 0;
   for(u = n.m_spriteStart; u < n.m_soundStart; ++u)
   {
      if(!pageOffsets[u])
         continue;   // sparse page
      offs = pageOffsets[u] - dataStart + alignPadding;
      if(offs & 1)
         alignPadding++;
   }
   
   if((pageOffsets[n.m_numChunks - 1] - dataStart + alignPadding) & 1)
      alignPadding++;
   
   n.m_pageDataSize = (size_t) unpaddedDataSize + alignPadding;
   n.m_pageData = (uint32_t *) I_CheckedMalloc(n.m_pageDataSize);
   n.m_pages = (uint8_t **) I_CheckedMalloc((n.m_numChunks + 1) * sizeof(uint8_t *));
   
   // Load pages and initialize pn.m_Pages pointers
   ptr = (uint8_t *) n.m_pageData;
   for(u = 0; u < n.m_numChunks; u++)
   {
      if((u >= n.m_spriteStart && u < n.m_soundStart) || u == n.m_numChunks - 1)
      {
         size_t offs = ptr - (uint8_t *) n.m_pageData;
         
         // pad with zeros to make it 2-byte aligned
         if(offs & 1)
         {
            *ptr++ = 0;
            if(u == n.m_numChunks - 1)
               n.m_soundInfoPagePadded = true;
         }
      }
      
      n.m_pages[u] = ptr;
      
      if(!pageOffsets[u])
         continue;               // sparse page
      
      // Use specified page length, when next page is sparse page.
      // Otherwise, calculate size from the offset difference between this and the next page.
      uint32_t size;
      if(!pageOffsets[u + 1]) size = pageLengths[u];
      else size = pageOffsets[u + 1] - pageOffsets[u];
      
      fseek(file, pageOffsets[u], SEEK_SET);
      fread(ptr, 1, size, file);
      ptr += size;
   }
   
   // last page points after page buffer
   n.m_pages[n.m_numChunks] = ptr;
   
   free(pageLengths);
   free(pageOffsets);
   fclose(file);
   
   // All is good. So get the data
   clear();
   m_numChunks = n.m_numChunks;
   m_spriteStart = n.m_spriteStart;
   m_soundStart = n.m_soundStart;
   m_soundInfoPagePadded = n.m_soundInfoPagePadded;
   m_pages = n.m_pages;
   m_pageData = n.m_pageData;
   m_pageDataSize = n.m_pageDataSize;
   
   n.m_pages = NULL;
   n.m_pageData = NULL;
   return true;
}
