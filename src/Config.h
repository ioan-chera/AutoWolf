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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "wl_def.h"

class PString;

//
// Config
//
// Namespace for configuration variables
//
class Config 
{
	static boolean nonazis;	// no enemies spawned
    static boolean secretstep3;	// make secret walls go three steps (for bugged maps)
    static boolean botActive;
    static PString dir;            // IOANCH 20130509: formerly configdir
    static int     extravbls;
    static boolean forcegrabmouse;
    static boolean fullscreen;
    static int     audiobuffer;
    static int     samplerate;
    static boolean debugmode;
    static int     difficulty;
    static boolean ignorenumchunks;
    static int     joystickhat;
    static int     joystickindex;
    static int     mission;
    static boolean nowait;
    static int     tedlevel;

    static unsigned screenBits;
    static unsigned screenHeight;
    static unsigned screenWidth;
    static boolean  usedoublebuffering;
    
public:
    static void CheckParameters(int argc, char *argv[]);
    
    static boolean NoNazis()             const {return nonazis;}	// no enemies spawned
    static boolean SecretStep3()         const {return secretstep3;}	// make secret walls go three steps (for bugged maps)
    static boolean BotActive()           const {return botActive;}
    static PString Dir()                 const {return dir;}            // IOANCH 20130509: formerly configdir
    static int     ExtraVBLs()           const {return extravbls;}
    static boolean ForceGrabMouse()      const {return forcegrabmouse;}
    static boolean Fullscreen()          const {return fullscreen;}
    static int     AudioBuffer()         const {return audiobuffer;}
    static int     SampleRate()          const {return samplerate;}
    static boolean DebugMode()           const {return debugmode;}
    static int     Difficulty()          const {return difficulty;}
    static boolean IgnoreNumChunks()     const {return ignorenumchunks;}
    static int     JoystickHat()         const {return joystickhat;}
    static int     JoystickIndex()       const {return joystickindex;}
    static int     Mission()             const {return mission;}
    static boolean NoWait()              const {return nowait;}
    static int     TedLevel()            const {return tedlevel;}
    
    static unsigned ScreenBits()         const {return screenBits;}
    static unsigned ScreenHeight()       const {return screenHeight;}
    static unsigned ScreenWidth()        const {return screenWidth;}
    static boolean  UseDoubleBuffering() const {return usedoublebuffering;}
};

#endif
