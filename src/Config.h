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
namespace Config 
{
	extern boolean nonazis;	// no enemies spawned
	extern boolean secretstep3;	// make secret walls go three steps (for troublesome maps)
    extern boolean botActive;
    extern PString dir;            // IOANCH 20130509: formerly configdir
    extern int     extravbls;
    extern boolean forcegrabmouse;
    extern boolean fullscreen;
    extern int     audiobuffer;
    extern int     samplerate;
    extern boolean debugmode;
    extern int     difficulty;
    extern boolean ignorenumchunks;
    extern int     joystickhat;
    extern int     joystickindex;
    extern int     mission;
    extern boolean nowait;
    extern int     tedlevel;
    
    extern unsigned screenBits;
    extern unsigned screenHeight;
    extern unsigned screenWidth;
    extern boolean  usedoublebuffering;
}

#endif
