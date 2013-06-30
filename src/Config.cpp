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


#include <sys/stat.h>
#ifdef __APPLE__
#include "macosx/CocoaFun.h"
#endif
#include "Config.h"
#include "PString.h"
#include "Exception.h"
#include "MasterDirectoryFile.h"
#include "ioan_bas.h"

#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#include <ShlObj.h>
#else
#include <unistd.h>
#endif

Boolean Config::nonazis;
Boolean Config::secretstep3;
Boolean Config::botActive;

PString Config::dir;
int     Config::extravbls = 0;

Boolean Config::forcegrabmouse;
Boolean Config::fullscreen = false;

#if defined(_arch_dreamcast)
int     Config::joystickhat = 0;
int     Config::samplerate = 11025;       // higher samplerates result in "out of memory"
int     Config::audiobuffer = 4096 / (44100 / Config::samplerate);
#elif defined(GP2X_940)
int     Config::joystickhat = -1;
int     Config::samplerate = 11025;       // higher samplerates result in "out of memory"
int     Config::audiobuffer = 128;
#else
int     Config::joystickhat = -1;
int     Config::samplerate = 44100;
int     Config::audiobuffer = 2048 / (44100 / Config::samplerate);
#endif

Boolean Config::debugmode = false;
int     Config::difficulty = 1;           // default is "normal"
Boolean Config::ignorenumchunks = false;
int     Config::joystickindex = 0;

int     Config::mission = 0;
Boolean Config::nowait = false;

int     Config::tedlevel = -1;            // default is not to start a level


#if defined(_arch_dreamcast)
Boolean  Config::usedoublebuffering = false;
unsigned Config::screenWidth = 320;
unsigned Config::screenHeight = 200;
unsigned Config::screenBits = 8;
#elif defined(GP2X)
Boolean  Config::usedoublebuffering = true;
unsigned Config::screenWidth = 320;
unsigned Config::screenHeight = 240;
#if defined(GP2X_940)
unsigned Config::screenBits = 8;
#else
unsigned Config::screenBits = 16;
#endif
#else
Boolean  Config::usedoublebuffering = true;
unsigned Config::screenWidth = 640;
unsigned Config::screenHeight = 400;
unsigned Config::screenBits = -1;      // use "best" color depth according to libSDL
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
// Config::CheckEnvVars
//
// Called from CheckParameters
//
void Config::CheckEnvVars()
{
    const char *wolfdir = getenv("AUTOWOLFDIR");
    if(wolfdir)
    {
        if(changeDirectory(wolfdir))
            throw Exception(PString("Cannot change directory to ").concat(wolfdir).concat("\n"));
    }
}

//
// CheckParameters
//
// Do it early.
//
void Config::CheckParameters(int argc, char *argv[])
{
    bool sampleRateGiven = false, audioBufferGiven = false;
    int defaultSampleRate = samplerate;
    
    botActive = true;	// IOANCH 26.05.2012: initialize with true, not false
    try
    {
        CheckEnvVars();
        for(int i = 1; i < argc; i++)
        {
            char *arg = argv[i];
            // IOANCH 20130303: unification
            if(!strcasecmp(arg, "--debugmode") || !strcasecmp(arg, "--goobers"))
                debugmode = true;
            else IFARG("--baby")
                difficulty = 0;
            else IFARG("--easy")
                difficulty = 1;
            else IFARG("--normal")
                difficulty = 2;
            else IFARG("--hard")
                difficulty = 3;
            else IFARG("--nowait")
                nowait = true;
            else IFARG("--tedlevel")
            {
                if(++i >= argc)
                    throw Exception("The tedlevel option is missing the level argument!\n");
                else 
                    tedlevel = atoi(argv[i]);
            }
            else IFARG("--windowed")
                fullscreen = false;
            else IFARG("--fullscreen")
                fullscreen = true;  // IOANCH 20121611: added --fullscreen option
            else IFARG("--windowed-mouse")
            {
                fullscreen = false;
                forcegrabmouse = true;
            }
            else IFARG("--res")
            {
                if(i + 2 >= argc)
                    throw Exception("The res option needs the width and/or the height "
                                    "argument!\n");
                else
                {
                    screenWidth = atoi(argv[++i]);
                    screenHeight = atoi(argv[++i]);
                    unsigned factor = screenWidth / 320;
                    if(screenWidth % 320 || (screenHeight != 200 * factor &&
                                                     screenHeight != 240 * factor))
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
                    screenWidth = atoi(argv[++i]);
                    screenHeight = atoi(argv[++i]);
                    if(screenWidth < 320)
                        throw Exception("Screen width must be at least 320!\n");
                    if(screenHeight < 200)
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
                    screenBits = atoi(argv[i]);
                    switch(screenBits)
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
                usedoublebuffering = false;
            else IFARG("--extravbls")
            {
                if(++i >= argc)
                    throw Exception("The extravbls option is missing the vbls argument!\n");
                else
                {
                    extravbls = atoi(argv[i]);
                    if(extravbls < 0)
                        throw Exception("Extravbls must be positive!\n");
                }
            }
            else IFARG("--joystick")
            {
                if(++i >= argc)
                    throw Exception("The joystick option is missing the index argument!\n");
                else
                    joystickindex = atoi(argv[i]);   
                // index is checked in InitGame
            }
            else IFARG("--joystickhat")
            {
                if(++i >= argc)
                    throw Exception("The joystickhat option is missing the "
                                    "index argument!\n");
                else 
                    joystickhat = atoi(argv[i]);
            }
            else IFARG("--samplerate")
            {
                if(++i >= argc)
                    throw Exception("The samplerate option is missing the "
                                    "rate argument!\n");
                else 
                    samplerate = atoi(argv[i]);
                sampleRateGiven = true;
            }
            else IFARG("--audiobuffer")
            {
                if(++i >= argc)
                    throw Exception("The audiobuffer option is missing the "
                                    "size argument!\n");
                else 
                    audiobuffer = atoi(argv[i]);
                audioBufferGiven = true;
            }
            else IFARG("--mission")
            {
                if(++i >= argc)
                    throw Exception("The mission option is missing the "
                                    "mission argument!\n");
                else
                {
                    mission = atoi(argv[i]);
                    if(mission < 0 || mission > 3)
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
                    dir.copy(argv[i]);
                }
            }
            else IFARG("--ignorenumchunks")
                ignorenumchunks = true;
            else IFARG("--help")
                throw Exception();
            // IOANCH 17.05.2012: added --nobot parameter
            else IFARG("--nobot")
                botActive = false;
            // IOANCH 17.05.2012: added --nonazis
            else IFARG("--nonazis")
                nonazis = true;
            // IOANCH 29.09.2012: added --secretstep3
            else IFARG("--secretstep3")
                secretstep3 = true;
            else IFARG("--wolfdir")
            {
                // IOANCH 20130304: added --wolfdir
                if(++i >= argc)
                    throw Exception("The wolfdir option is missing the dir argument!\n");
                else
                {
                    if(changeDirectory(argv[i]))
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
        audiobuffer = 2048 / (44100 / samplerate);
}

//
// SetupConfigLocation
//
void Config::SetupConfigLocation()
{
    struct stat statbuf;
    
    // On Linux like systems, the configdir defaults to $HOME/.wolf4sdl
#if !defined(_arch_dreamcast)
    if(dir.length() == 0)
    {
        // Set config location to home directory for multi-user support
        // IOANCH 20130303: do it correctly
        // IOANCH 20130509: use PString for paths
#ifdef __APPLE__
        const char *appsupdir = Cocoa_ApplicationSupportDirectory();
        if(appsupdir == NULL)
            Quit("Your Application Support directory is not defined. You must "
                 "set this before playing.");
        dir = appsupdir;
#elif defined(_WIN32)
        TCHAR appdatdir[MAX_PATH];
        if(!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appdatdir)))
            Quit("Your Application Data directory is not defined. You must "
                 "set this before playing.");
        dir.copy(appdatdir).concatSubpath("AutoWolf");
#else
        const char *homeenv = getenv("HOME");
        if(homeenv == NULL)
            Quit("Your $HOME directory is not defined. You must set this before "
                 "playing.");
        dir.copy(homeenv).concatSubpath(".autowolf");
#endif
    }
#endif
    if(dir.length() > 0)
    {
        // Ensure config directory exists and create if necessary
        if(stat(dir.buffer(), &statbuf) != 0)
        {
#ifdef _WIN32
            if(_mkdir(dir.buffer()) != 0)
#else
            if(mkdir(dir.buffer(), 0755) != 0)
#endif
            {
                Quit("The configuration directory \"%s\" could not be created.",
                     dir.buffer());
            }
        }
    }
    
    // IOANCH 20130304: initialize bot master directory file location
    MasterDirectoryFile::MainDir().initializeConfigLocation();
}