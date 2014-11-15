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

#include <string>

class PString;

//
// Config
//
// Namespace for configuration variables
//
extern bool  cfg_nonazis;	// no enemies spawned
extern bool  cfg_secretstep3;
extern bool  cfg_botActive;
extern std::string  cfg_dir;            // IOANCH 20130509: formerly configdir
extern std::string  cfg_wolfdir;
extern int      cfg_extravbls;
extern bool  cfg_forcegrabmouse;
extern bool  cfg_fullscreen;
extern int      cfg_audiobuffer;
extern int      cfg_samplerate;
extern bool  cfg_debugmode;
extern int      cfg_difficulty;
extern bool  cfg_ignorenumchunks;
extern int      cfg_joystickhat;
extern int      cfg_joystickindex;
extern int      cfg_mission;
extern bool  cfg_nowait;
extern int      cfg_tedlevel;
extern unsigned cfg_screenBits;
extern unsigned cfg_logicalHeight;
extern unsigned cfg_logicalWidth;
extern unsigned cfg_displayWidth, cfg_displayHeight;
extern float    cfg_displayRatio;
extern float	cfg_logicalRatio;
extern bool  cfg_usedoublebuffering;
extern bool cfg_norestore;
    
void     CFG_SetupConfigLocation();

#endif
