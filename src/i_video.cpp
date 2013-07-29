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
////////////////////////////////////////////////////////////////////////////////
//
// Video operations. Library-specific ops isolated here
//
////////////////////////////////////////////////////////////////////////////////


#include "wl_def.h"
#include "Config.h"
#include "i_video.h"
#include "wl_draw.h"

SDL_Surface *vid_screen = NULL;
unsigned vid_screenPitch;

SDL_Surface *vid_screenBuffer = NULL;
unsigned vid_bufferPitch;

//
// I_CreateSurface
//
// Creates a RGB surface and gives it the game palette
//
SDL_Surface *I_CreateSurface(Uint32 flags, int width, int height)
{
   SDL_Surface *ret;
   ret = SDL_CreateRGBSurface(flags, width, height, 8, 0, 0, 0, 0);
   if(!ret)
   {
      Quit("Unable to create %dx%d buffer surface: %s\n", width, height, SDL_GetError());
   }
   SDL_SetColors(ret, IMPALE(vid_palette), 0, 256);
   return ret;
}

//
// VL_SetVGAPlaneMode
//
// moved from wl_main or related
// Sets up the graphics surfaces and palettes
//
static void I_setVGAPlaneMode ()
{
	// IOANCH 12.06.2012: bumped "Automatic" on the title bars
   // IOANCH 20130202: unification process
   SDL_WM_SetCaption(SPEAR.FullTitle(), NULL);
   //    if( SPEAR())
   //        SDL_WM_SetCaption("Automatic Spear of Destiny", NULL);
   //    else
   //        SDL_WM_SetCaption("Automatic Wolfenstein 3D", NULL);
   
   if(cfg_screenBits == -1)
   {
      const SDL_VideoInfo *vidInfo = SDL_GetVideoInfo();
      cfg_screenBits = vidInfo->vfmt->BitsPerPixel;
   }
   
   vid_screen = SDL_SetVideoMode(cfg_screenWidth, cfg_screenHeight,
                                 cfg_screenBits,
                                 (cfg_usedoublebuffering ? SDL_HWSURFACE | SDL_DOUBLEBUF : 0)
                                 | (cfg_screenBits == 8 ? SDL_HWPALETTE : 0)
                                 | (cfg_fullscreen ? SDL_FULLSCREEN : 0));
   if(!vid_screen)
   {
      printf("Unable to set %ix%ix%i video mode: %s\n", cfg_screenWidth,
             cfg_screenHeight, cfg_screenBits, SDL_GetError());
      exit(1);
   }
   if((vid_screen->flags & SDL_DOUBLEBUF) != SDL_DOUBLEBUF)
      cfg_usedoublebuffering = false;
   SDL_ShowCursor(SDL_DISABLE);
   
   // IOANCH 20130202: unification process
   SDL_SetColors(vid_screen, IMPALE(vid_palette), 0, 256);
   memcpy(vid_curpal, IMPALE(vid_palette), sizeof(SDL_Color) * 256);
   
   vid_screenBuffer = I_CreateSurface(SDL_SWSURFACE, cfg_screenWidth,
                                      cfg_screenHeight);
   
   vid_screenPitch = vid_screen->pitch;
   vid_bufferPitch = vid_screenBuffer->pitch;

   // IOANCH: removed vid_curSurface, was redundant
   
   vid_scaleFactor = cfg_screenWidth/320;
   if(cfg_screenHeight/200 < vid_scaleFactor)
      vid_scaleFactor = cfg_screenHeight/200;
   
   vid_pixelangle = (short *) malloc(cfg_screenWidth * sizeof(short));
   CHECKMALLOCRESULT(vid_pixelangle);
   vid_wallheight = (int *) malloc(cfg_screenWidth * sizeof(int));
   CHECKMALLOCRESULT(vid_wallheight);
}

//
// I_InitEngine
//
// Initializes the game engine backend (such as SDL)
// from original wolf4sdl code, moved here
//
void I_InitEngine()
{
   // initialize SDL
#if defined _WIN32
   putenv("SDL_VIDEODRIVER=windib");
#endif
   if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
   {
      printf("Unable to init SDL: %s\n", SDL_GetError());
      exit(1);
   }
   
   atexit(SDL_Quit);
   
   int numJoysticks = SDL_NumJoysticks();
   if(cfg_joystickindex && (cfg_joystickindex < -1 ||
                            cfg_joystickindex >= numJoysticks))
   {
      if(!numJoysticks)
         printf("No joysticks are available to SDL!\n");
      else
         printf("The joystick index must be between -1 and %i!\n",
                numJoysticks - 1);
      exit(1);
   }
   
   // Moved from InitGame
#if defined(GP2X_940)
   GP2X_MemoryInit();
#endif
   
   
   // Init the graphics engine
   I_setVGAPlaneMode();
}

//
// I_InitAfterSignon
//
// Sets extra initialization, needed after signon
//
void I_InitAfterSignon()
{
#if defined _WIN32
   if(!cfg_fullscreen)
   {
      struct SDL_SysWMinfo wmInfo;
      SDL_VERSION(&wmInfo.version);
      
      if(SDL_GetWMInfo(&wmInfo) != -1)
      {
         HWND hwndSDL = wmInfo.window;
         DWORD style = GetWindowLong(hwndSDL, GWL_STYLE) & ~WS_SYSMENU;
         SetWindowLong(hwndSDL, GWL_STYLE, style);
         SetWindowPos(hwndSDL, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE |
                      SWP_NOZORDER | SWP_FRAMECHANGED);
      }
   }
#endif
}


////////////////////////////////////////////////////////////////////////////////
//
//                            PIXEL OPS
//
////////////////////////////////////////////////////////////////////////////////

//
// I_lockSurface
//
// Prepares a surface for editing. Returns the pixel byte array
//
inline static byte *I_lockSurface(SDL_Surface *surface)
{
   if(SDL_MUSTLOCK(surface))
   {
      if(SDL_LockSurface(surface) < 0)
         return NULL;
   }
   return (byte *) surface->pixels;
}

//
// I_LockBuffer
//
// Prepares a surface for editing. Returns the pixel byte array
//
byte *I_LockBuffer()
{
   return I_lockSurface(vid_screenBuffer);
}

//
// I_LockDirect
//
// Prepares a surface for editing. Returns the pixel byte array
//
byte *I_LockDirect()
{
   return I_lockSurface(vid_screen);
}

//
// I_UnlockSurface
//
// Releases the surface for blitting
//
inline static void I_unlockSurface(SDL_Surface *surface)
{
   if(SDL_MUSTLOCK(surface))
   {
      SDL_UnlockSurface(surface);
   }
}
//
// I_UnlockBuffer
//
// Releases the surface for blitting
//
void I_UnlockBuffer()
{
   I_unlockSurface(vid_screenBuffer);
}
void I_UnlockDirect()
{
   I_unlockSurface(vid_screen);
}

//
// I_UpdateScreen
//
// moved inline

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = I_SetColor
// =
//
////////////////////////////////////////////////////////////////////////////////


void I_SetColor	(int color, int red, int green, int blue)
{
   SDL_Color col = { static_cast<Uint8>(red), static_cast<Uint8>(green), static_cast<Uint8>(blue) };
   vid_curpal[color] = col;
   
   if(cfg_screenBits == 8)
      SDL_SetPalette(vid_screen, SDL_PHYSPAL, &col, color, 1);
   else
   {
      SDL_SetPalette(vid_screenBuffer, SDL_LOGPAL, &col, color, 1);
      SDL_BlitSurface(vid_screenBuffer, NULL, vid_screen, NULL);
      SDL_Flip(vid_screen);
   }
}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = I_GetColor
// =
//
////////////////////////////////////////////////////////////////////////////////


void I_GetColor	(int color, int *red, int *green, int *blue)
{
   SDL_Color *col = &vid_curpal[color];
   *red = col->r;
   *green = col->g;
   *blue = col->b;
}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = I_SetPalette
// =
//
////////////////////////////////////////////////////////////////////////////////


void I_SetPalette (SDL_Color *palette, bool forceupdate)
{
   memcpy(vid_curpal, palette, sizeof(SDL_Color) * 256);
   
   if(cfg_screenBits == 8)
      SDL_SetPalette(vid_screen, SDL_PHYSPAL, palette, 0, 256);
   else
   {
      SDL_SetPalette(vid_screenBuffer, SDL_LOGPAL, palette, 0, 256);
      if(forceupdate)
         I_UpdateScreen();
   }
}


//==========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = I_LatchToScreen
// =
//
////////////////////////////////////////////////////////////////////////////////


void I_LatchToScreenScaledCoord(int surf_index, int xsrc, int ysrc,
                                int width, int height, int scxdest, int scydest)
{
	assert(scxdest >= 0 && scxdest + width * vid_scaleFactor <= cfg_screenWidth
          && scydest >= 0 && scydest + height * vid_scaleFactor <= cfg_screenHeight
          && "I_LatchToScreenScaledCoord: Destination rectangle out of bounds!");
   
   SDL_Surface *source = vid_latchpics[surf_index];
	if(vid_scaleFactor == 1)
   {
      // HACK: If screenBits is not 8 and the screen is faded out, the
      //       result will be black when using SDL_BlitSurface. The reason
      //       is that the logical palette needed for the transformation
      //       to the screen color depth is not equal to the logical
      //       palette of the latch (the latch is not faded). Therefore,
      //       SDL tries to map the colors...
      //       The result: All colors are mapped to black.
      //       So, we do the blit on our own...
      if(cfg_screenBits != 8)
      {
         byte *src, *dest;
         unsigned srcPitch;
         int i, j;
         
         src = I_lockSurface(source);
         if(src == NULL) return;
         
         srcPitch = source->pitch;
         
         dest = I_LockBuffer();
         if(dest == NULL) return;
         
         for(j = 0; j < height; j++)
         {
            for(i = 0; i < width; i++)
            {
               byte col = src[(ysrc + j)*srcPitch + xsrc + i];
               dest[(scydest + j) * vid_bufferPitch + scxdest + i] = col;
            }
         }
         I_UnlockBuffer();
         I_unlockSurface(source);
      }
      else
      {
         SDL_Rect srcrect = { static_cast<Sint16>(xsrc), static_cast<Sint16>(ysrc), static_cast<Uint16>(width), static_cast<Uint16>(height) };
         SDL_Rect destrect = { static_cast<Sint16>(scxdest), static_cast<Sint16>(scydest), 0, 0 }; // width and height are ignored
         SDL_BlitSurface(source, &srcrect, vid_screenBuffer, &destrect);
      }
   }
   else
   {
      byte *src, *dest;
      unsigned srcPitch;
      int i, j, sci, scj;
      unsigned m, n;
      
      src = I_lockSurface(source);
      if(src == NULL) return;
      
      srcPitch = source->pitch;
      
      dest = I_LockBuffer();
      if(dest == NULL) return;
      
      for(j = 0, scj = 0; j < height; j++, scj += vid_scaleFactor)
      {
         for(i = 0, sci = 0; i < width; i++, sci += vid_scaleFactor)
         {
            byte col = src[(ysrc + j)*srcPitch + xsrc + i];
            for(m = 0; m < vid_scaleFactor; m++)
            {
               for(n = 0; n < vid_scaleFactor; n++)
               {
                  dest[(scydest + scj + m) * vid_bufferPitch + scxdest + sci + n] = col;
               }
            }
         }
      }
      I_UnlockBuffer();
      I_unlockSurface(source);
   }
}

//==========================================================================

//
// I_FreeLatchMem
//
void I_FreeLatchMem()
{
   int i;
   for(i = 0; i < (2 + (signed int)SPEAR.g(LATCHPICS_LUMP_END) - (signed int)SPEAR.g(LATCHPICS_LUMP_START)); i++)
   {
      SDL_FreeSurface(vid_latchpics[i]);
      vid_latchpics[i] = NULL;
   }
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = I_LoadLatchMem
// =
//
////////////////////////////////////////////////////////////////////////////////


void I_LoadLatchMem ()
{
	int	i,width,height,start,end;
	byte *src;
	SDL_Surface *surf;
   
   //
   // tile 8s
   //
   // IOANCH: use I_ call
   surf = I_CreateSurface(SDL_HWSURFACE, 8 * 8, (SPEAR.g(NUMTILE8) + 7) / 8 * 8);
   
	vid_latchpics[0] = surf;
	CA_CacheGrChunk (SPEAR.g(STARTTILE8));
	src = ca_grsegs[SPEAR.g(STARTTILE8)];
   
	for (i=0;i<(signed int)SPEAR.g(NUMTILE8);i++)
	{
		I_MemToLatch (src, 8, 8, surf, (i & 7) * 8, (i >> 3) * 8);
		src += 64;
	}
	UNCACHEGRCHUNK (SPEAR.g(STARTTILE8));
   
	vid_latchpics[1] = NULL;  // not used
   
   //
   // pics
   //
	start = SPEAR.g(LATCHPICS_LUMP_START);
	end = SPEAR.g(LATCHPICS_LUMP_END);
   
	for (i=start;i<=end;i++)
	{
		width = pictable[i-SPEAR.g(STARTPICS)].width;
		height = pictable[i-SPEAR.g(STARTPICS)].height;
      surf = I_CreateSurface(SDL_HWSURFACE, width, height);
      
		vid_latchpics[2+i-start] = surf;
		CA_CacheGrChunk (i);
		I_MemToLatch (ca_grsegs[i], width, height, surf, 0, 0);
		UNCACHEGRCHUNK(i);
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = I_MemToLatch
// =
//
////////////////////////////////////////////////////////////////////////////////


void I_MemToLatch(byte *source, int width, int height, SDL_Surface *destSurface,
                  int x, int y)
{
   byte *ptr;
   int xsrc, ysrc, pitch;
   
   assert(x >= 0 && (unsigned) x + width <= cfg_screenWidth
          && y >= 0 && (unsigned) y + height <= cfg_screenHeight
          && "I_MemToLatch: Destination rectangle out of bounds!");
   
   ptr = I_lockSurface(destSurface);
   if(ptr == NULL) return;
   
   pitch = destSurface->pitch;
   ptr += y * pitch + x;
   for(ysrc = 0; ysrc < height; ysrc++)
   {
      for(xsrc = 0; xsrc < width; xsrc++)
      {
         ptr[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2))
                                           + (xsrc & 3) * (width >> 2) * height];
      }
   }
   I_unlockSurface(destSurface);
}