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
	static boolean  nonazis;	// no enemies spawned
    static boolean  secretstep3;
    static boolean  botActive;
    static PString  dir;            // IOANCH 20130509: formerly configdir
    static int      extravbls;
    static boolean  forcegrabmouse;
    static boolean  fullscreen;
    static int      audiobuffer;
    static int      samplerate;
    static boolean  debugmode;
    static int      difficulty;
    static boolean  ignorenumchunks;
    static int      joystickhat;
    static int      joystickindex;
    static int      mission;
    static boolean  nowait;
    static int      tedlevel;

    static unsigned screenBits;
    static unsigned screenHeight;
    static unsigned screenWidth;
    static boolean  usedoublebuffering;
    
    static void     CheckEnvVars();
public:

    static void     CheckParameters(int argc, char *argv[]);
    static void     SetupConfigLocation();
    
    static boolean  NoNazis()              {return nonazis;}
    static boolean  SecretStep3()          {return secretstep3;}
    static boolean  BotActive()            {return botActive;}
    static PString  Dir()                  {return dir;}
    static int      ExtraVBLs()            {return extravbls;}
    static boolean  ForceGrabMouse()       {return forcegrabmouse;}
    static boolean  FullScreen()           {return fullscreen;}
    static int      AudioBuffer()          {return audiobuffer;}
    static int      SampleRate()           {return samplerate;}
    static boolean  DebugMode()            {return debugmode;}
    static int      Difficulty()           {return difficulty;}
    static boolean  IgnoreNumChunks()      {return ignorenumchunks;}
    static int      JoystickHat()          {return joystickhat;}
    static int      JoystickIndex()        {return joystickindex;}
    static int      Mission()              {return mission;}
    static boolean  NoWait()               {return nowait;}
    static int      TedLevel()             {return tedlevel;}
    static unsigned ScreenBits()           {return screenBits;}
    static unsigned ScreenHeight()         {return screenHeight;}
    static unsigned ScreenWidth()          {return screenWidth;}
    static boolean  UseDoubleBuffering()   {return usedoublebuffering;}
    
    static void     SetExtraVBLs(int value){extravbls = value;}
    static void     SetNoWait(boolean value){nowait = value;}
    static void     SetScreenBits(unsigned value){screenBits = value;}
    static void     SetUseDoubleBuffering(boolean value)
    {usedoublebuffering = value;}
};

#endif
