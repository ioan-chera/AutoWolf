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

#include "version.h"
#include "wl_def.h"
#include "Config.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_vh.h"
#include "Logger.h"
#include "PString.h"
#include "SODFlag.h"
#include "wl_main.h"
#include "wl_draw.h"
#include "Exception.h"
#include "Platform.h"

#ifdef IOS
#include "CocoaFun.h"
#endif

// used globally
unsigned vid_screenPitch;
unsigned vid_bufferPitch;

#ifdef USE_SDL1_2
static SDL_Surface* vid_screen;
#else
SDL_Window* vid_window;
SDL_Renderer* vid_renderer;
static SDL_Texture* vid_texture;
#endif

int vid_screenWidth, vid_screenHeight;

volatile bool g_forceSDLRestart;

static SDL_Surface *vid_screenBuffer;

static SDL_Surface *vid_latchpics[NUMLATCHPICS];

//
// I_createSurface
//
// Creates a RGB surface and gives it the game palette
//
static SDL_Surface *I_createSurface(Uint32 flags, int width, int height)
{
   SDL_Surface *ret;
   ret = SDL_CreateRGBSurface(flags, width, height, 8, 0, 0, 0, 0);
   if(!ret)
   {
      throw Exception((PString("Unable to create ")<<width<<"x"<<height<<" buffer surface: "<<SDL_GetError())());
   }
//   SDL_SetColors(ret, IMPALE(vid_palette), 0, 256);
   return ret;
}

#ifdef USE_SDL1_2
static void I_initSDL12()
{
    SDL_WM_SetCaption(SPEAR::FullTitle(), NULL);
    if(cfg_screenBits == (unsigned)-1)
    {
        const SDL_VideoInfo *vidInfo = SDL_GetVideoInfo();
        cfg_screenBits = vidInfo->vfmt->BitsPerPixel;
    }
    
    vid_screen = SDL_SetVideoMode(cfg_logicalWidth, cfg_logicalHeight,
                                  cfg_screenBits,
                                  (cfg_usedoublebuffering ? SDL_HWSURFACE | SDL_DOUBLEBUF : 0)
                                  | (cfg_screenBits == 8 ? SDL_HWPALETTE : 0)
                                  | (cfg_fullscreen ? SDL_FULLSCREEN : 0));
    
    if(!vid_screen)
    {
        throw Exception((std::string("Unable to set video mode: ")
                         + SDL_GetError()).c_str());
    }
    
    if((vid_screen->flags & SDL_DOUBLEBUF) != SDL_DOUBLEBUF)
        cfg_usedoublebuffering = false;
    
    SDL_SetColors(vid_screen, IMPALE(vid_palette), 0, 256);
    memcpy(vid_curpal, IMPALE(vid_palette), sizeof(SDL_Color) * 256);
}
#endif

void I_RecreateRenderer()
{
#ifndef USE_SDL1_2
    if(vid_texture)
        SDL_DestroyTexture(vid_texture);
    if(vid_renderer)
        SDL_DestroyRenderer(vid_renderer);
    if(vid_window)
        SDL_DestroyWindow(vid_window);
    
    // Reload vid_screen attributes
    if(Platform::touchscreen)
    {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        vid_screenWidth = mode.w;
        vid_screenHeight = mode.h;
    }
    
    Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI;
    if(Platform::touchscreen || cfg_fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    
    vid_window = SDL_CreateWindow(SPEAR::FullTitle(),
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  cfg_fullscreen ? 0 : cfg_logicalWidth,
                                  cfg_fullscreen ? 0 : cfg_logicalHeight,
                                  flags);
    
    if(!vid_window)
    {
        throw Exception((std::string("Unable to create SDL window: ")
                         + SDL_GetError()).c_str());
    }
    
    vid_renderer = SDL_CreateRenderer(vid_window, -1, 0);
    if(!vid_renderer)
    {
        throw Exception((std::string("Unable to create SDL renderer: ")
	                        + SDL_GetError()).c_str());
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    
    // make the scaled rendering look smoother.
    if(Platform::touchscreen)
    {
        // aspect ratio correction on iOS. For Android it's done differently
        vid_correctedWidth = cfg_logicalWidth;
        vid_correctedHeight = 3 * vid_correctedWidth / 4;
    }
    else
    {
        // Also copy the values into vid_screen attributes
        vid_screenWidth = vid_correctedWidth = cfg_logicalWidth;
        vid_screenHeight = vid_correctedHeight = cfg_logicalHeight;
    }

    vid_correctedRatio = (float)vid_correctedWidth / vid_correctedHeight;
    
    if(SDL_RenderSetLogicalSize(vid_renderer,
                                vid_correctedWidth,
                                vid_correctedHeight) < 0)
    {
        throw Exception((std::string("Unable to set SDL renderer logical"
                                     " size: ") + SDL_GetError()).c_str());
    }
    
    vid_texture = SDL_CreateTexture(vid_renderer,
                                    SDL_PIXELFORMAT_ABGR8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    cfg_logicalWidth,
                                    cfg_logicalHeight);
    if(!vid_texture)
    {
        throw Exception((std::string("Unable to create SDL texture: ")
                         + SDL_GetError()).c_str());
    }
    
#endif
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
#ifdef USE_SDL1_2
   putenv("SDL_VIDEODRIVER=windib");
#endif
#endif
   if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
   {
	   throw Exception((std::string("Unable to init SDL: ") + SDL_GetError()).c_str());
   }
   
   atexit(SDL_Quit);
   
   // Moved from InitGame
#if defined(GP2X_940)
   GP2X_MemoryInit();
#endif

#ifdef USE_SDL1_2
    I_initSDL12();
#else
    I_RecreateRenderer();
#endif
	
//	if((vid_screen->flags & SDL_DOUBLEBUF) != SDL_DOUBLEBUF)
//		cfg_usedoublebuffering = false;
	SDL_ShowCursor(SDL_DISABLE);
	
	// IOANCH 20130202: unification process
//	SDL_SetColors(vid_screen, IMPALE(vid_palette), 0, 256);
	memcpy(vid_curpal, IMPALE(vid_palette), sizeof(SDL_Color) * 256);
	
	vid_screenBuffer = I_createSurface(SDL_SWSURFACE, cfg_logicalWidth, cfg_logicalHeight);
	
	vid_bufferPitch = vid_screenBuffer->pitch;
	
	// IOANCH: removed vid_curSurface, was redundant
//#ifdef __ANDROID__
//	SDL_SetWindowSize(vid_window, cfg_logicalWidth, cfg_logicalHeight);
//#endif
    
#ifdef IOS
//    Cocoa_HideStatusBar();
    Cocoa_DisableIdleTimer();
    Cocoa_PutBackButton();
#endif
	
}

//
// I_InitAfterSignon
//
// Sets extra initialization, needed after signon
//
void I_InitAfterSignon()
{
#if defined _WIN32 && 0
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
SDL_Color *I_LockDirect()
{
#ifdef USE_SDL1_2
    return reinterpret_cast<SDL_Color*>(I_lockSurface(vid_screen));
#else
    void *vpixels;
    int pitch;
    SDL_LockTexture(vid_texture, nullptr, &vpixels, &pitch);
   return static_cast<SDL_Color *>(vpixels);
#endif
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
#ifdef USE_SDL1_2
    I_unlockSurface(vid_screen);
#else
    SDL_UnlockTexture(vid_texture);
#endif
}

//
// I_UpdateScreen
//
void I_UpdateDirect()
{
#ifdef USE_SDL1_2
    SDL_Flip(vid_screen);
#else
	SDL_RenderCopy(vid_renderer, vid_texture, nullptr, nullptr);
	SDL_RenderPresent(vid_renderer);
#endif
}

void I_UpdateScreen()
{
#ifdef USE_SDL1_2
    SDL_BlitSurface(vid_screenBuffer, NULL, vid_screen, NULL);
	SDL_Flip(vid_screen);
#else
	unsigned x, y;
    SDL_Color *vid_trueBuffer = I_LockDirect();
	for (y = 0; y < cfg_logicalHeight; ++y)
	{
		for (x = 0; x < cfg_logicalWidth; ++x)
		{
			vid_trueBuffer[y * cfg_logicalWidth + x] = vid_curpal[static_cast<uint8_t*>(vid_screenBuffer->pixels)[y * cfg_logicalWidth + x]];
		}
	}
    I_UnlockDirect();
	I_UpdateDirect();
#endif
}
void I_ClearScreen(int color)
{
   SDL_FillRect(vid_screenBuffer, NULL, color);
}
void I_SaveBufferBMP(const char *fname)
{
   SDL_SaveBMP(vid_screenBuffer, fname);
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
#ifdef USE_SDL1_2
    if(cfg_screenBits == 8)
        SDL_SetPalette(vid_screen, SDL_PHYSPAL, palette, 0, 256);
    else
    {
        SDL_SetPalette(vid_screenBuffer, SDL_LOGPAL, palette, 0, 256);
        if(forceupdate)
            I_UpdateScreen();
    }
#else
   I_UpdateScreen();
#endif
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
	assert(scxdest >= 0 && scxdest + width * vid_scaleFactor <= cfg_logicalWidth
          && scydest >= 0 && scydest + height * vid_scaleFactor <= cfg_logicalHeight
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
         if(src == nullptr)
			 return;
         
         srcPitch = source->pitch;
         
         dest = I_LockBuffer();
         if(dest == nullptr)
			 return;
         
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
      if(src == nullptr)
		  return;
      
      srcPitch = source->pitch;
      
      dest = I_LockBuffer();
      if(dest == nullptr)
		  return;
      
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
   for(i = 0; i < (2 + (signed int)SPEAR::g(LATCHPICS_LUMP_END) - (signed int)SPEAR::g(LATCHPICS_LUMP_START)); i++)
   {
      SDL_FreeSurface(vid_latchpics[i]);
      vid_latchpics[i] = nullptr;
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
   GraphicLoader::pictabletype psize;
	int	i,start,end;
	const byte *src;
	SDL_Surface *surf;
   
   //
   // tile 8s
   //
   // IOANCH: use I_ call
   surf = I_createSurface(SDL_SWSURFACE, 8 * 8, (SPEAR::g(NUMTILE8) + 7) / 8 * 8);
   
	vid_latchpics[0] = surf;
   graphSegs.cacheChunk(SPEAR::g(STARTTILE8));
	src = graphSegs[SPEAR::g(STARTTILE8)];
   
	for (i=0;i<(signed int)SPEAR::g(NUMTILE8);i++)
	{
		I_MemToLatch (src, 8, 8, surf, (i & 7) * 8, (i >> 3) * 8);
		src += 64;
	}
	graphSegs.uncacheChunk (SPEAR::g(STARTTILE8));
   
	vid_latchpics[1] = NULL;  // not used
   
   //
   // pics
   //
	start = SPEAR::g(LATCHPICS_LUMP_START);
	end = SPEAR::g(LATCHPICS_LUMP_END);
   
	for (i=start;i<=end;i++)
	{
		psize = graphSegs.sizeAt(i-SPEAR::g(STARTPICS));
      surf = I_createSurface(SDL_SWSURFACE, psize.width, psize.height);
      
		vid_latchpics[2+i-start] = surf;
		graphSegs.cacheChunk (i);
		I_MemToLatch (graphSegs[i], psize.width, psize.height, surf, 0, 0);
		graphSegs.uncacheChunk(i);
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = I_MemToLatch
// =
//
////////////////////////////////////////////////////////////////////////////////


void I_MemToLatch(const byte *source, int width, int height, SDL_Surface *destSurface,
                  int x, int y)
{
   byte *ptr;
   int xsrc, ysrc, pitch;
   
   assert(x >= 0 && (unsigned) x + width <= cfg_logicalWidth
          && y >= 0 && (unsigned) y + height <= cfg_logicalHeight
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

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_ScreenToScreen
// =
//
////////////////////////////////////////////////////////////////////////////////
void I_LatchToScreen (int surf_index, int x, int y)
{
   SDL_Surface *source = vid_latchpics[surf_index];
   I_LatchToScreenScaledCoord(surf_index,0,0,source->w,source->h,
                              vid_scaleFactor*x,vid_scaleFactor*y);
}
void I_LatchToScreenScaledCoord (int surf_index, int scx, int scy)
{
   SDL_Surface *source = vid_latchpics[surf_index];
   I_LatchToScreenScaledCoord(surf_index,0,0,source->w,source->h,scx,scy);
}

//void VL_ScreenToScreen (SDL_Surface *source, SDL_Surface *dest)
//{
//   SDL_BlitSurface(source, NULL, dest, NULL);
//}
