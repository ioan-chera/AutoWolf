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

#include "wl_def.h"
#include "Config.h"
#include "i_system.h"
#include "PString.h"
#include "macosx/CocoaFun.h"
#include "SODFlag.h"
#include "StdStringExtensions.h"
#include "wl_draw.h"
#include "wl_main.h"

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
void I_MakeDir(const std::string& dirname)
{
#ifdef _WIN32
   BOOL result = CreateDirectoryW(UTF8ToWideChar(dirname).c_str(), nullptr);
   if(!result)
	   throw std::system_error(std::error_code(GetLastError(), std::system_category()), std::string("Cannot create directory ") + dirname);
#else
   return !mkdir(dirname, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
#endif
}

//
// I_ChangeDir
//
// Sets current dir
//
void I_ChangeDir(const std::string& dirname)
{
#ifdef _WIN32
	BOOL result = SetCurrentDirectoryW(UTF8ToWideChar(dirname).c_str());
	if(!result) 
		throw std::system_error(std::error_code(GetLastError(), std::system_category()), std::string("Cannot change directory to ") + dirname);
#else
	int result = chdir(dirname);
	if (result != 0)
		throw std::system_error(std::error_code(errno, std::system_category()), std::string("Cannot change directory to ") + dirname);
#endif
}

//
// I_GetSettingsDir
//
// Gets the settings directory
//
std::string I_GetSettingsDir()
{
#ifdef __APPLE__
   char *appsupdir = Cocoa_CreateApplicationSupportPathString();
   if(appsupdir == NULL)
      Quit("Your Application Support directory is not defined. You must "
           "set this before playing.");
	PString ret(appsupdir);
	free(appsupdir);
   return ret;
#elif defined(_WIN32)
   wchar_t appdatdir[MAX_PATH];
   HRESULT result = SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appdatdir);
   if(!SUCCEEDED(result))
	   throw std::system_error(std::error_code(result & 0xffff, std::system_category()), "Your Application Data directory is not defined. You must set this before playing.");
   return ConcatSubpath(WideCharToUTF8(appdatdir), "AutoWolf");
#else
   const char *homeenv = getenv("HOME");
   if(homeenv == NULL)
      Quit("Your $HOME directory is not defined. You must set this before "
           "playing.");
   return PString(homeenv).concatSubpath(".autowolf");
#endif

}

//
// I_CheckedMalloc
//
// Does malloc and quits if null. Replaced all CHECKMALLOCRESULT occurences with
// this. I lost the file and line information with this, though.
//
void *I_CheckedMalloc(size_t sz)
{
   void *ret = malloc(sz);
   if(!ret)
      Quit("Out of memory!");
   return ret;
}

//
// I_CheckedRealloc
//
// Same as above, but with realloc
//
void *I_CheckedRealloc(void *ptr, size_t sz)
{
   void *ret = realloc(ptr, sz);
   if(!ret)
      Quit("Out of memory!");
   return ret;
}
