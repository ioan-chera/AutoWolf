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
   
   vid_curSurface = vid_screenBuffer;
   vid_curPitch = vid_bufferPitch;
   
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
// I_LockSurface
//
// Prepares a surface for editing. Returns the pixel byte array
//
byte *I_LockSurface(SDL_Surface *surface)
{
   if(SDL_MUSTLOCK(surface))
   {
      if(SDL_LockSurface(surface) < 0)
         return NULL;
   }
   return (byte *) surface->pixels;
}

//
// I_UnlockSurface
//
// Releases the surface for blitting
//
void I_UnlockSurface(SDL_Surface *surface)
{
   if(SDL_MUSTLOCK(surface))
   {
      SDL_UnlockSurface(surface);
   }
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
      SDL_SetPalette(vid_curSurface, SDL_LOGPAL, &col, color, 1);
      SDL_BlitSurface(vid_curSurface, NULL, vid_screen, NULL);
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