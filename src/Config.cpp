/*
 *  Config.cpp
 *  Wolf4SDL
 *
 *  Created by Ioan Chera on 5/9/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "PString.h"
#include "Config.h"
#include "Exception.h"


boolean Config::nonazis;
boolean Config::secretstep3;
boolean Config::botActive;

PString Config::dir;
int     Config::extravbls = 0;

boolean Config::forcegrabmouse;
boolean Config::fullscreen = true;

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

boolean Config::debugmode = false;
int     Config::difficulty = 1;           // default is "normal"
boolean Config::ignorenumchunks = false;
int     Config::joystickindex = 0;

int     Config::mission = 0;
boolean Config::nowait = false;

int     Config::tedlevel = -1;            // default is not to start a level


#if defined(_arch_dreamcast)
boolean  Config::usedoublebuffering = false;
unsigned Config::screenWidth = 320;
unsigned Config::screenHeight = 200;
unsigned Config::screenBits = 8;
#elif defined(GP2X)
boolean  Config::usedoublebuffering = true;
unsigned Config::screenWidth = 320;
unsigned Config::screenHeight = 240;
#if defined(GP2X_940)
unsigned Config::screenBits = 8;
#else
unsigned Config::screenBits = 16;
#endif
#else
boolean  Config::usedoublebuffering = true;
unsigned Config::screenWidth = 640;
unsigned Config::screenHeight = 400;
unsigned Config::screenBits = -1;      // use "best" color depth according to libSDL
#endif


// IOANCH 20130509: arguments are now case insensitive
#define IFARG(str) if(!strcasecmp(arg, (str)))

//
// CheckParameters
//
// Do it early.
//
void Config::CheckParameters(int argc, char *argv[])
{
    bool sampleRateGiven = false, audioBufferGiven = false;
    int defaultSampleRate = Config::samplerate;
    
    Config::botActive = true;	// IOANCH 26.05.2012: initialize with true, not false
    try
    {
        for(int i = 1; i < argc; i++)
        {
            char *arg = argv[i];
            // IOANCH 20130303: unification
            if(!strcasecmp(arg, "--debugmode") || !strcasecmp(arg, "--goobers"))
                Config::debugmode = true;
            else IFARG("--baby")
                Config::difficulty = 0;
            else IFARG("--easy")
                Config::difficulty = 1;
            else IFARG("--normal")
                Config::difficulty = 2;
            else IFARG("--hard")
                Config::difficulty = 3;
            else IFARG("--nowait")
                Config::nowait = true;
            else IFARG("--tedlevel")
            {
                if(++i >= argc)
                    throw Exception("The tedlevel option is missing the level argument!\n");
                else 
                    Config::tedlevel = atoi(argv[i]);
            }
            else IFARG("--windowed")
                Config::fullscreen = false;
            else IFARG("--fullscreen")
                Config::fullscreen = true;  // IOANCH 20121611: added --fullscreen option 
            else IFARG("--windowed-mouse")
            {
                Config::fullscreen = false;
                Config::forcegrabmouse = true;
            }
            else IFARG("--res")
            {
                if(i + 2 >= argc)
                    throw Exception("The res option needs the width and/or the height "
                                    "argument!\n");
                else
                {
                    Config::screenWidth = atoi(argv[++i]);
                    Config::screenHeight = atoi(argv[++i]);
                    unsigned factor = Config::screenWidth / 320;
                    if(Config::screenWidth % 320 || (Config::screenHeight != 200 * factor && 
                                                     Config::screenHeight != 240 * factor))
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
                    Config::screenWidth = atoi(argv[++i]);
                    Config::screenHeight = atoi(argv[++i]);
                    if(Config::screenWidth < 320)
                        throw Exception("Screen width must be at least 320!\n");
                    if(Config::screenHeight < 200)
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
                    Config::screenBits = atoi(argv[i]);
                    switch(Config::screenBits)
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
                Config::usedoublebuffering = false;
            else IFARG("--extravbls")
            {
                if(++i >= argc)
                    throw Exception("The extravbls option is missing the vbls argument!\n");
                else
                {
                    Config::extravbls = atoi(argv[i]);
                    if(Config::extravbls < 0)
                        throw Exception("Extravbls must be positive!\n");
                }
            }
            else IFARG("--joystick")
            {
                if(++i >= argc)
                    throw Exception("The joystick option is missing the index argument!\n");
                else 
                    Config::joystickindex = atoi(argv[i]);   
                // index is checked in InitGame
            }
            else IFARG("--joystickhat")
            {
                if(++i >= argc)
                    throw Exception("The joystickhat option is missing the index argument!\n");
                else 
                    Config::joystickhat = atoi(argv[i]);
            }
            else IFARG("--samplerate")
            {
                if(++i >= argc)
                    throw Exception("The samplerate option is missing the rate argument!\n");
                else 
                    Config::samplerate = atoi(argv[i]);
                sampleRateGiven = true;
            }
            else IFARG("--audiobuffer")
            {
                if(++i >= argc)
                    throw Exception("The audiobuffer option is missing the size argument!\n");
                else 
                    Config::audiobuffer = atoi(argv[i]);
                audioBufferGiven = true;
            }
            else IFARG("--mission")
            {
                if(++i >= argc)
                    throw Exception("The mission option is missing the mission argument!\n");
                else
                {
                    Config::mission = atoi(argv[i]);
                    if(Config::mission < 0 || Config::mission > 3)
                        throw Exception("The mission option must be between 0 and 3!\n");
                }
            }
            else IFARG("--configdir")
            {
                if(++i >= argc)
                    throw Exception("The configdir option is missing the dir argument!\n");
                else
                {
                    // IOANCH 20130307: expand tilde
                    char *trans = Basic::NewStringTildeExpand(argv[i]);
                    if(trans)
                    {
                        size_t len = strlen(trans);
                        Config::dir = trans;
                        if(trans[len] != '/' && trans[len] != '\\')
                            Config::dir += '/';
                        free(trans);
                    }
                    else
                        throw Exception("The config directory couldn't be set!\n");
                }
            }
            else IFARG("--ignorenumchunks")
                Config::ignorenumchunks = true;
            else IFARG("--help")
                throw Exception();
            // IOANCH 17.05.2012: added --nobot parameter
            else IFARG("--nobot")
                Config::botActive = false;
            // IOANCH 17.05.2012: added --nonazis
            else IFARG("--nonazis")
                Config::nonazis = true;
            // IOANCH 29.09.2012: added --secretstep3
            else IFARG("--secretstep3")
                Config::secretstep3 = true;
            else IFARG("--wolfdir")
            {
                // IOANCH 20130304: added --wolfdir
                if(++i >= argc)
                    throw Exception("The wolfdir option is missing the dir argument!\n");
                else
                {
                    char *trans = Basic::NewStringTildeExpand(argv[i]);
#if defined(_WIN32)
                    int cdres = !::SetCurrentDirectory(trans);
#else
                    int cdres = chdir(trans);
                    // FIXME: don't just assume UNIX/Linux/Apple
#endif
                    if(cdres)
                        throw Exception(PString("Cannot change directory to ") + trans + "\n");
                    if(trans)
                        free(trans);
                }
            }
            else
                throw Exception(PString("Unknown argument ") + arg + "\n");
        }
    }
    catch(const Exception &e)
    {
        puts(e.info().buffer());
        printf(
               "AutoWolf v0.3\n"
               "By Ioan Chera on Wolf4SDL codebase"
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
        Config::audiobuffer = 2048 / (44100 / Config::samplerate);
}
