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
////////////////////////////////////////////////////////////////////////////////
//
// Configuration controller. 
//
////////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include "wl_def.h"
#include "FileSystem.h"
#include "i_system.h"
#ifdef __APPLE__
#include "macosx/CocoaFun.h"
#endif
#include "Config.h"
#include "PString.h"
#include "Exception.h"
#include "id_ca.h"
#include "ioan_bas.h"
#include "SODFlag.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_text.h"


bool cfg_nonazis;
bool cfg_secretstep3;
bool cfg_botActive;

std::string cfg_dir;
std::string cfg_wolfdir;

int     cfg_extravbls;

bool cfg_forcegrabmouse;
bool cfg_fullscreen;

#if defined(_arch_dreamcast)
int     cfg_joystickhat = 0;
int     cfg_samplerate = 11025;  // higher samplerates result in "out of memory"
int     cfg_audiobuffer = 4096 / (44100 / cfg_samplerate);
#elif defined(GP2X_940)
int     cfg_joystickhat = -1;
int     cfg_samplerate = 11025;  // higher samplerates result in "out of memory"
int     cfg_audiobuffer = 128;
#else
int     cfg_joystickhat = -1;
int     cfg_samplerate = 44100;
int     cfg_audiobuffer = 2048 / (44100 / cfg_samplerate);
#endif

bool cfg_debugmode = false;
int     cfg_difficulty = 1;           // default is "normal"
bool cfg_ignorenumchunks = false;
int     cfg_joystickindex = 0;

int     cfg_mission = 0;
bool cfg_nowait = false;

int     cfg_tedlevel = -1;            // default is not to start a level


#if defined(_arch_dreamcast)
bool  cfg_usedoublebuffering = false;
unsigned cfg_logicalWidth = LOGIC_WIDTH;
unsigned cfg_logicalHeight = LOGIC_HEIGHT;
unsigned cfg_screenBits = 8;
#elif defined(GP2X)
bool  cfg_usedoublebuffering = true;
unsigned cfg_logicalWidth = LOGIC_WIDTH;
unsigned cfg_logicalHeight = 240;
#if defined(GP2X_940)
unsigned cfg_screenBits = 8;
#else
unsigned cfg_screenBits = 16;
#endif
#else
bool  cfg_usedoublebuffering = true;
unsigned cfg_logicalWidth = 640;
unsigned cfg_logicalHeight = 400;
// CURRENTLY TOUCHSCREEN ONLY
unsigned vid_correctedWidth = 640;
unsigned vid_correctedHeight = 480;
float vid_correctedRatio = (float)vid_correctedWidth / vid_correctedHeight;
unsigned cfg_screenBits = -1;      // use "best" color depth according to libSDL
#endif
float cfg_logicalRatio = (float)cfg_logicalWidth / cfg_logicalHeight;

bool cfg_norestore;


// IOANCH 20130509: arguments are now case insensitive
#define IFARG(str) if(!strcasecmp(arg, (str)))

//inline static int changeDirectory(const char *locdir)
//{
//#if defined(_WIN32)
//    return !::SetCurrentDirectory(locdir);
//#else
//    return chdir(locdir);
//    // FIXME: don't just assume UNIX/Linux/Apple
//#endif
//}

//
// SetupConfigLocation
//
void CFG_SetupConfigLocation()
{
    // On Linux like systems, the configdir defaults to $HOME/.wolf4sdl
#if !defined(_arch_dreamcast)
    if(cfg_dir.empty())
    {
        // Set config location to home directory for multi-user support
        // IOANCH 20130303: do it correctly
        // IOANCH 20130509: use PString for paths
       // IOANCH 20130725: abstracted this away
       cfg_dir = I_GetSettingsDir();
    }
#endif
    if(!cfg_dir.empty())
    {
        // Ensure config directory exists and create if necessary
        if(FileSystem::FileExists(cfg_dir.c_str()) == false)
        {
			I_MakeDir(cfg_dir);
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//
// CHECK FOR EPISODES
//
///////////////////////////////////////////////////////////////////////////
void CFG_CheckForEpisodes ()
{
   // IOANCH 20130301: unification culling
   if(!SPEAR::flag)
   {
      if(FileSystem::FileExists(FileSystem::FindCaseInsensitive(cfg_wolfdir, "VSWAP.WL6").c_str()))
      {
         cfg_extension = "WL6";
         menu_newep[2].active =
         menu_newep[4].active =
         menu_newep[6].active =
         menu_newep[8].active =
         menu_newep[10].active =
         menu_epselect[1] =
         menu_epselect[2] =
         menu_epselect[3] =
         menu_epselect[4] =
         menu_epselect[5] = 1;
      }
	  else if (FileSystem::FileExists(FileSystem::FindCaseInsensitive(cfg_wolfdir, "VSWAP.WL3").c_str()))
      {
         cfg_extension = "WL3";
         menu_missingep = 3;
         menu_newep[2].active =
         menu_newep[4].active =
         menu_epselect[1] =
         menu_epselect[2] = 1;
      }
	  else if (FileSystem::FileExists(FileSystem::FindCaseInsensitive(cfg_wolfdir, "VSWAP.WL1").c_str()))
      {
         cfg_extension = "WL1";
         menu_missingep = 5;
      }
      else
         throw Exception ("NO WOLFENSTEIN 3-D DATA FILES to be found!");
      
      cfg_graphext = cfg_extension;
      cfg_audioext = cfg_extension;
      cfg_endfilename += cfg_extension;
   }
   else
   {
      // IOANCH 20130301: unification culling
      switch (cfg_mission)
      {
         case 0:
			 if (FileSystem::FileExists(FileSystem::FindCaseInsensitive(cfg_wolfdir, "VSWAP.SOD").c_str()))
               cfg_extension = "SOD";
            else
               throw Exception ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         case 1:
			 if (FileSystem::FileExists(FileSystem::FindCaseInsensitive(cfg_wolfdir, "VSWAP.SD1").c_str()))
               cfg_extension = "SD1";
            else
               throw Exception ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         case 2:
			 if (FileSystem::FileExists(FileSystem::FindCaseInsensitive(cfg_wolfdir, "VSWAP.SD2").c_str()))
               cfg_extension = "SD2";
            else
               throw Exception ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         case 3:
			 if (FileSystem::FileExists(FileSystem::FindCaseInsensitive(cfg_wolfdir, "VSWAP.SD3").c_str()))
               cfg_extension = "SD3";
            else
               throw Exception ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         default:
            throw Exception ("UNSUPPORTED MISSION!");
            break;
      }
      
      cfg_graphext = "SOD";
      cfg_audioext = "SOD";
   }
   
   cfg_configname += cfg_extension;
   cfg_savename += cfg_extension;
   cfg_demoname += cfg_extension;
}
