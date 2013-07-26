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
// Miscellaneous system operations. Inspired from Doom/Eternity's
//
////////////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "wl_def.h"
#include "macosx/CocoaFun.h"
#include "i_system.h"

//
// I_Delay
//
// Delays the system for the given milliseconds
//
void I_Delay(unsigned ms)
{
   SDL_Delay((Uint32)ms);
}

//
// I_GetTicks
//
// Gets the number of ticks since start
//
uint32_t I_GetTicks(void)
{
   return (uint32_t)SDL_GetTicks();
}

//
// I_Notify
//
// Posts a notification, depending on platform
//
void I_Notify(const char *msg)
{
#ifdef __APPLE__
   Cocoa_Notify(SPEAR.FullTitle(), msg);
#endif
}

//
// I_MakeDir
//
// Makes a directory
//
Boolean I_MakeDir(const char *dirname)
{
#ifdef _WIN32
   return CreateDirectory(dirname, NULL);
#else
   return !mkdir(dirname, 0x755);
#endif
}

//
// I_ChangeDir
//
// Sets current dir
//
Boolean I_ChangeDir(const char *dirname)
{
#ifdef _WIN32
   return SetCurrentDirectory(dirname);
#else
   return !chdir(dirname);
#endif
}

//
// I_GetSettingsDir
//
// Gets the settings directory
//
PString I_GetSettingsDir()
{
#ifdef __APPLE__
   const char *appsupdir = Cocoa_ApplicationSupportDirectory();
   if(appsupdir == NULL)
      Quit("Your Application Support directory is not defined. You must "
           "set this before playing.");
   return appsupdir;
#elif defined(_WIN32)
   TCHAR appdatdir[MAX_PATH];
   if(!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appdatdir)))
      Quit("Your Application Data directory is not defined. You must "
           "set this before playing.");
   return PString(appdatdir).concatSubpath("AutoWolf");
#else
   const char *homeenv = getenv("HOME");
   if(homeenv == NULL)
      Quit("Your $HOME directory is not defined. You must set this before "
           "playing.");
   return PString(homeenv).concatSubpath(".autowolf");
#endif

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
   
   vid_screenBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, cfg_screenWidth,
                                           cfg_screenHeight, 8, 0, 0, 0, 0);
   if(!vid_screenBuffer)
   {
      printf("Unable to create screen buffer surface: %s\n", SDL_GetError());
      exit(1);
   }
   
   // IOANCH 20130202: unification process
   SDL_SetColors(vid_screenBuffer, IMPALE(vid_palette), 0, 256);
   
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
void I_UpdateScreen()
{
	SDL_BlitSurface(vid_screenBuffer, NULL, vid_screen, NULL);
	SDL_Flip(vid_screen);
}

