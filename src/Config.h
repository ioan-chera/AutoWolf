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



class PString;

//
// Config
//
// Namespace for configuration variables
//
extern Boolean  cfg_nonazis;	// no enemies spawned
extern Boolean  cfg_secretstep3;
extern Boolean  cfg_botActive;
extern PString  cfg_dir;            // IOANCH 20130509: formerly configdir
extern int      cfg_extravbls;
extern Boolean  cfg_forcegrabmouse;
extern Boolean  cfg_fullscreen;
extern int      cfg_audiobuffer;
extern int      cfg_samplerate;
extern Boolean  cfg_debugmode;
extern int      cfg_difficulty;
extern Boolean  cfg_ignorenumchunks;
extern int      cfg_joystickhat;
extern int      cfg_joystickindex;
extern int      cfg_mission;
extern Boolean  cfg_nowait;
extern int      cfg_tedlevel;
extern unsigned cfg_screenBits;
extern unsigned cfg_screenHeight;
extern unsigned cfg_screenWidth;
extern Boolean  cfg_usedoublebuffering;
    

void     CFG_CheckParameters(int argc, char *argv[]);
void     CFG_SetupConfigLocation();

#endif
