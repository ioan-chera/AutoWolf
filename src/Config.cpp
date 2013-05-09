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
