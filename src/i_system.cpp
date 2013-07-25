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
