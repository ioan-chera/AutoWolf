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

struct t_compshape;

//
// VSwapContainer
//
// IOANCH:
// Wrapper class for the old PM namespace. Can be used to load VSWAP files
//
class VSwapContainer
{
   int m_numChunks;
   int m_spriteStart;
   int m_soundStart;
   bool m_soundInfoPagePadded;
   uint8_t **m_pages;
   
   uint32_t *m_pageData;
   size_t m_pageDataSize;
   

public:
   //
   // Constructor
   //
   VSwapContainer() : m_numChunks(0), m_spriteStart(0), m_soundStart(0),
   m_soundInfoPagePadded(false), m_pages(NULL), m_pageData(NULL),
   m_pageDataSize(0)
   {
   }
   VSwapContainer(const VSwapContainer &other);
   //
   // Move constructor
   //
   VSwapContainer(VSwapContainer &&other) :
   m_numChunks(other.m_numChunks),
   m_spriteStart(other.m_spriteStart),
   m_soundStart(other.m_soundStart),
   m_soundInfoPagePadded(other.m_soundInfoPagePadded),
   m_pages(other.m_pages),
   m_pageData(other.m_pageData),
   m_pageDataSize(other.m_pageDataSize)
   {
      other.m_pages = NULL;
      other.m_pageData = NULL;
   }
   void clear();
   //
   // Destructor
   //
   ~VSwapContainer()
   {
      clear();
   }

   bool loadFile(const char *filename
//                 , PString *errmsg = NULL
                 );

   //
   // Accessors
   //
   int numChunks() const {return m_numChunks;}
   int spriteStart() const {return m_spriteStart;}
   int soundStart() const {return m_soundStart;}
   bool soundInfoPagePadded() const {return m_soundInfoPagePadded;}
   const uint8_t *const *pages() const {return m_pages;}
   
   //
   // getPageSize
   //
   // Gets size of a given entry
   //
   uint32_t getPageSize(int page) const
   {
      // Just use Quit for now,
      if(page < 0 || page >= m_numChunks)
         Quit((PString("PM_GetPageSize: Tried to access illegal page: ") << page)());
      return (uint32_t) (m_pages[page + 1] - m_pages[page]);
   }
   
   //
   // getPage
   //
   // Returns the pointer to the given buffer info
   //
   const uint8_t *getPage(int page) const
   {
      if(page < 0 || page >= m_numChunks)
         Quit(PString("PM_GetPage: Tried to access illegal page: ").concat(page)());
      return m_pages[page];
   }
   
   //
   // operator[]
   //
   // short for getPage
   //
   const uint8_t *operator[](int page) const
   {
      return getPage(page);
   }
   
   //
   // getPagePString
   //
   // Gets the page as a pstring
   //
   PString getPagePString(int page) const
   {
      return PString((const char *)getPage(page), getPageSize(page));
   }
   
   //
   // getEnd
   //
   // Returns pointer to end of buffer
   //
   const uint8_t *getEnd() const
   {
      return m_pages[m_numChunks];
   }
   
   //
   // getTexture
   //
   // Returns the texture at index
   //
   const byte *getTexture(int wallpic) const
   {
      return m_pages[wallpic];
   }
   
   //
   // getSprite
   //
   // Returns the sprite at offset index
   //
   const t_compshape *getSprite(int shapenum, bool remap = true) const
   {
      if(remap)
         shapenum = SPEAR.sp(shapenum);
      return (const t_compshape *) (const void *)
      getPage(m_spriteStart + shapenum);
   }
   
   //
   // getSound
   //
   // Returns the sound at offset index
   //
   const byte *getSound(int soundpagenum) const
   {
      return getPage(m_soundStart + soundpagenum);
   }
};
extern VSwapContainer vSwapData;


#endif
