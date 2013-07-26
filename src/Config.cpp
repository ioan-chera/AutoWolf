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

#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#include <ShlObj.h>
#else
#include <unistd.h>
#endif

#include <sys/stat.h>
#include "wl_def.h"
#include "i_system.h"
#ifdef __APPLE__
#include "macosx/CocoaFun.h"
#endif
#include "Config.h"
#include "PString.h"
#include "Exception.h"
#include "MasterDirectoryFile.h"
#include "ioan_bas.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_text.h"


Boolean cfg_nonazis;
Boolean cfg_secretstep3;
Boolean cfg_botActive;

PString cfg_dir;
int     cfg_extravbls = 0;

Boolean cfg_forcegrabmouse;
Boolean cfg_fullscreen = false;

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

Boolean cfg_debugmode = false;
int     cfg_difficulty = 1;           // default is "normal"
Boolean cfg_ignorenumchunks = false;
int     cfg_joystickindex = 0;

int     cfg_mission = 0;
Boolean cfg_nowait = false;

int     cfg_tedlevel = -1;            // default is not to start a level


#if defined(_arch_dreamcast)
Boolean  cfg_usedoublebuffering = false;
unsigned cfg_screenWidth = 320;
unsigned cfg_screenHeight = 200;
unsigned cfg_screenBits = 8;
#elif defined(GP2X)
Boolean  cfg_usedoublebuffering = true;
unsigned cfg_screenWidth = 320;
unsigned cfg_screenHeight = 240;
#if defined(GP2X_940)
unsigned cfg_screenBits = 8;
#else
unsigned cfg_screenBits = 16;
#endif
#else
Boolean  cfg_usedoublebuffering = true;
unsigned cfg_screenWidth = 640;
unsigned cfg_screenHeight = 400;
unsigned cfg_screenBits = -1;      // use "best" color depth according to libSDL
#endif


// IOANCH 20130509: arguments are now case insensitive
#define IFARG(str) if(!strcasecmp(arg, (str)))

inline static int changeDirectory(const char *locdir)
{
#if defined(_WIN32)
    return !::SetCurrentDirectory(locdir);
#else
    return chdir(locdir);
    // FIXME: don't just assume UNIX/Linux/Apple
#endif
}

//
// CFG_checkEnvVars
//
// Called from CheckParameters
//
static void CFG_checkEnvVars()
{
    const char *wolfdir = getenv("AUTOWOLFDIR");
    if(wolfdir)
    {
        if(!I_ChangeDir(wolfdir))
            throw Exception(PString("Cannot change directory to ").
                            concat(wolfdir).concat("\n"));
    }
}

//
// CheckParameters
//
// Do it early.
//
void CFG_CheckParameters(int argc, char *argv[])
{
    bool sampleRateGiven = false, audioBufferGiven = false;
    int defaultSampleRate = cfg_samplerate;
    
    cfg_botActive = true;	// IOANCH 26.05.2012: initialize with true, not false
    try
    {
        CFG_checkEnvVars();
        for(int i = 1; i < argc; i++)
        {
            char *arg = argv[i];
            // IOANCH 20130303: unification
            if(!strcasecmp(arg, "--debugmode") || !strcasecmp(arg, "--goobers"))
                cfg_debugmode = true;
            else IFARG("--baby")
                cfg_difficulty = 0;
            else IFARG("--easy")
                cfg_difficulty = 1;
            else IFARG("--normal")
                cfg_difficulty = 2;
            else IFARG("--hard")
                cfg_difficulty = 3;
            else IFARG("--nowait")
                cfg_nowait = true;
            else IFARG("--tedlevel")
            {
                if(++i >= argc)
                    throw Exception("The tedlevel option is missing the level argument!\n");
                else 
                    cfg_tedlevel = atoi(argv[i]);
            }
            else IFARG("--windowed")
                cfg_fullscreen = false;
            else IFARG("--fullscreen")
                cfg_fullscreen = true;
                // IOANCH 20121611: added --fullscreen option
            else IFARG("--windowed-mouse")
            {
                cfg_fullscreen = false;
                cfg_forcegrabmouse = true;
            }
            else IFARG("--res")
            {
                if(i + 2 >= argc)
                    throw Exception("The res option needs the width and/or the height "
                                    "argument!\n");
                else
                {
                    cfg_screenWidth = atoi(argv[++i]);
                    cfg_screenHeight = atoi(argv[++i]);
                    unsigned factor = cfg_screenWidth / 320;
                    if(cfg_screenWidth % 320 ||
                       (cfg_screenHeight != 200 * factor &&
                        cfg_screenHeight != 240 * factor))
                    {
                        throw Exception("Screen size must be a multiple of 320x200 or "
                                        "320x240!\n"); 
                    }
                }
            }
            else IFARG("--resf")
            {
                if(i + 2 >= argc)
                    throw Exception("The resf option needs the width and/or the height "
                                    "argument!\n");
                else
                {
                    cfg_screenWidth = atoi(argv[++i]);
                    cfg_screenHeight = atoi(argv[++i]);
                    if(cfg_screenWidth < 320)
                        throw Exception("Screen width must be at least 320!\n");
                    if(cfg_screenHeight < 200)
                        throw Exception("Screen height must be at least 200!\n"); 
                }
            }
            else IFARG("--bits")
            {
                if(++i >= argc)
                    throw Exception("The bits option is missing the color depth "
                                    "argument!\n");
                else
                {
                    cfg_screenBits = atoi(argv[i]);
                    switch(cfg_screenBits)
                    {
                        case 8:
                        case 16:
                        case 24:
                        case 32:
                            break;
                            
                        default:
                            throw Exception("Screen color depth must be 8, 16, "
                                            "24, or 32!\n");
                            break;
                    }
                }
            }
            else IFARG("--nodblbuf")
                cfg_usedoublebuffering = false;
            else IFARG("--extravbls")
            {
                if(++i >= argc)
                    throw Exception("The extravbls option is missing the vbls argument!\n");
                else
                {
                    cfg_extravbls = atoi(argv[i]);
                    if(cfg_extravbls < 0)
                        throw Exception("Extravbls must be positive!\n");
                }
            }
            else IFARG("--joystick")
            {
                if(++i >= argc)
                    throw Exception("The joystick option is missing the index argument!\n");
                else
                    cfg_joystickindex = atoi(argv[i]);
                // index is checked in InitGame
            }
            else IFARG("--joystickhat")
            {
                if(++i >= argc)
                    throw Exception("The joystickhat option is missing the "
                                    "index argument!\n");
                else 
                    cfg_joystickhat = atoi(argv[i]);
            }
            else IFARG("--samplerate")
            {
                if(++i >= argc)
                    throw Exception("The samplerate option is missing the "
                                    "rate argument!\n");
                else 
                    cfg_samplerate = atoi(argv[i]);
                sampleRateGiven = true;
            }
            else IFARG("--audiobuffer")
            {
                if(++i >= argc)
                    throw Exception("The audiobuffer option is missing the "
                                    "size argument!\n");
                else 
                    cfg_audiobuffer = atoi(argv[i]);
                audioBufferGiven = true;
            }
            else IFARG("--mission")
            {
                if(++i >= argc)
                    throw Exception("The mission option is missing the "
                                    "mission argument!\n");
                else
                {
                    cfg_mission = atoi(argv[i]);
                    if(cfg_mission < 0 || cfg_mission > 3)
                        throw Exception("The mission option must be between 0 "
                                        "and 3!\n");
                }
            }
            else IFARG("--configdir")
            {
                if(++i >= argc)
                    throw Exception("The configdir option is missing the dir "
                                    "argument!\n");
                else
                {
                    // IOANCH 20130307: expand tilde
                    cfg_dir.copy(argv[i]);
                }
            }
            else IFARG("--ignorenumchunks")
                cfg_ignorenumchunks = true;
            else IFARG("--help")
                throw Exception();
            // IOANCH 17.05.2012: added --nobot parameter
            else IFARG("--nobot")
                cfg_botActive = false;
            // IOANCH 17.05.2012: added --nonazis
            else IFARG("--nonazis")
                cfg_nonazis = true;
            // IOANCH 29.09.2012: added --secretstep3
            else IFARG("--secretstep3")
                cfg_secretstep3 = true;
            else IFARG("--wolfdir")
            {
                // IOANCH 20130304: added --wolfdir
                if(++i >= argc)
                    throw Exception("The wolfdir option is missing the dir argument!\n");
                else
                {
                    if(!I_ChangeDir(argv[i]))
                        throw Exception(PString("Cannot change directory to ").concat(argv[i]).concat("\n"));
                }
            }
            else
                throw Exception(PString("Unknown argument ").concat(arg).concat("\n"));
        }
    }
    catch(const Exception &e)
    {
        puts(e.info().buffer());
        printf(
               "AutoWolf v0.3\n"
               "By Ioan Chera on Wolf4SDL codebase\n"
               "Wolf4SDL: Ported by Chaos-Software (http://www.chaos-software.de.vu)\n"
               "Original Wolfenstein 3D by id Software\n\n"
#ifdef __APPLE__
               "Usage: open -a AutoWolf --args [options]\n"
#else
               "Usage: autowolf [options]\n"
#endif
               "Options:\n"
               " --nobot                Do not use bot\n"
               " --nonazis              Maps without Nazis spawned\n"
               " --secretstep3          Emulate 3-step secret wall bug\n"
               " --help                 This help page\n"
               " --tedlevel <level>     Starts the game in the given level\n"
               " --baby                 Sets the difficulty to baby for tedlevel\n"
               " --easy                 Sets the difficulty to easy for tedlevel\n"
               " --normal               Sets the difficulty to normal for tedlevel\n"
               " --hard                 Sets the difficulty to hard for tedlevel\n"
               " --nowait               Skips intro screens\n"
               " --windowed[-mouse]     Starts the game in a window [and grabs mouse]\n"
               " --res <width> <height> Sets the screen resolution\n"
               "                        (must be multiple of 320x200 or 320x240)\n"
               " --resf <w> <h>         Sets any screen resolution >= 320x200\n"
               "                        (which may result in graphic errors)\n"
               " --bits <b>             Sets the screen color depth\n"
               "                        (use this when you have palette/fading problems\n"
               "                        allowed: 8, 16, 24, 32, default: \"best\" depth)\n"
               " --nodblbuf             Don't use SDL's double buffering\n"
               " --extravbls <vbls>     Sets a delay after each frame, which may help to\n"
               "                        reduce flickering (unit is currently 8 ms, default: 0)\n"
               " --joystick <index>     Use the index-th joystick if available\n"
               "                        (-1 to disable joystick, default: 0)\n"
               " --joystickhat <index>  Enables movement with the given coolie hat\n"
               " --samplerate <rate>    Sets the sound sample rate (given in Hz, default: %i)\n"
               " --audiobuffer <size>   Sets the size of the audio buffer (-> sound latency)\n"
               "                        (given in bytes, default: 2048 / (44100 / samplerate))\n"
               " --ignorenumchunks      Ignores the number of chunks in VGAHEAD.*\n"
               "                        (may be useful for some broken mods)\n"
               " --configdir <dir>      Directory where config file and save games are stored\n"
#if defined(_arch_dreamcast) || defined(_WIN32)
               "                        (default: current directory)\n"
#elif !defined(__APPLE__)
               "                        (default: $HOME/.autowolf)\n"	// IOANCH 20130116: changed name
#else
               "                        (default: ~/Library/Application Support/com.ichera.autowolf)\n"
#endif
               // IOANCH 20130301: unification culling
               , defaultSampleRate);
        printf(
               " --mission <mission>    Mission number to play (0-3)\n"
               "                        (default: 0 -> .SOD, 1-3 -> .SD*)\n"
               );
        exit(1);
    }
    
    if(sampleRateGiven && !audioBufferGiven)
        cfg_audiobuffer = 2048 / (44100 / cfg_samplerate);
}

//
// SetupConfigLocation
//
void CFG_SetupConfigLocation()
{
    struct stat statbuf;
    
    // On Linux like systems, the configdir defaults to $HOME/.wolf4sdl
#if !defined(_arch_dreamcast)
    if(cfg_dir.length() == 0)
    {
        // Set config location to home directory for multi-user support
        // IOANCH 20130303: do it correctly
        // IOANCH 20130509: use PString for paths
       // IOANCH 20130725: abstracted this away
       cfg_dir = I_GetSettingsDir();
    }
#endif
    if(cfg_dir.length() > 0)
    {
        // Ensure config directory exists and create if necessary
        if(stat(cfg_dir(), &statbuf) != 0)
        {
            if(!I_MakeDir(cfg_dir()))
            {
                Quit("The configuration directory \"%s\" could not be created.",
                     cfg_dir());
            }
        }
    }
    
    // IOANCH 20130304: initialize bot master directory file location
    masterDir.initializeConfigLocation();
}

///////////////////////////////////////////////////////////////////////////
//
// CHECK FOR EPISODES
//
///////////////////////////////////////////////////////////////////////////
void CFG_CheckForEpisodes ()
{
   struct stat statbuf;
   
   // IOANCH 20130301: unification culling
   if(!SPEAR())
   {
      if(!stat("VSWAP.WL6", &statbuf))
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
      else if(!stat("VSWAP.WL3", &statbuf))
      {
         cfg_extension = "WL3";
         menu_missingep = 3;
         menu_newep[2].active =
         menu_newep[4].active =
         menu_epselect[1] =
         menu_epselect[2] = 1;
      }
      else if(!stat("VSWAP.WL1", &statbuf))
      {
         cfg_extension = "WL1";
         menu_missingep = 5;
      }
      else
         Quit ("NO WOLFENSTEIN 3-D DATA FILES to be found!");
      
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
            if(!stat("VSWAP.SOD", &statbuf))
               cfg_extension = "SOD";
            else
               Quit ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         case 1:
            if(!stat("VSWAP.SD1", &statbuf))
               cfg_extension = "SD1";
            else
               Quit ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         case 2:
            if(!stat("VSWAP.SD2", &statbuf))
               cfg_extension = "SD2";
            else
               Quit ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         case 3:
            if(!stat("VSWAP.SD3", &statbuf))
               cfg_extension = "SD3";
            else
               Quit ("NO SPEAR OF DESTINY DATA FILES TO BE FOUND!");
            break;
         default:
            Quit ("UNSUPPORTED MISSION!");
            break;
      }
      
      cfg_graphext = "SOD";
      cfg_audioext = "SOD";
   }
   
   cfg_configname += cfg_extension;
   cfg_savename += cfg_extension;
   cfg_demoname += cfg_extension;
}
