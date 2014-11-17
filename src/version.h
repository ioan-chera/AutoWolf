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

#ifndef _VERSION_H_
#define _VERSION_H_

// Platform stuff
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define IOS
#else
#define OSX
#endif
#endif

#ifndef VERSIONALREADYCHOSEN              // used for batch compiling

/* Defines used for different versions */

//#define SPEAR::flag
//#define SPEARDEMO
//#define UPLOAD


//#define APOGEE_1_0
//#define APOGEE_1_1
//#define APOGEE_1_2
//#define JAPAN

/*
    Wolf3d Full v1.1 Apogee (with ReadThis)   - define CARMACIZED and APOGEE_1_1
    Wolf3d Full v1.4 Apogee (with ReadThis)   - define CARMACIZED
    Wolf3d Full v1.4 GT/ID/Activision         - define CARMACIZED and GOODTIMES
    Wolf3d Full v1.4 Imagineer (Japanese)     - define CARMACIZED and JAPAN
    Wolf3d Shareware v1.0                     - define UPLOAD and APOGEE_1_0
    Wolf3d Shareware v1.1                     - define CARMACIZED and UPLOAD and APOGEE_1_1
    Wolf3d Shareware v1.2                     - define CARMACIZED and UPLOAD and APOGEE_1_2
    Wolf3d Shareware v1.4                     - define CARMACIZED and UPLOAD
    Spear of Destiny Full and Mission Disks   - define CARMACIZED and SPEAR::flag
                                                (and GOODTIMES for no FormGen quiz)
    Spear of Destiny Demo                     - define CARMACIZED and SPEAR::flag and SPEARDEMO
*/

#endif

//#define USE_FEATUREFLAGS    // Enables the level feature flags (see bottom of wl_def.h)
//#define USE_SHADING         // Enables shading support (see wl_shade.cpp)
//#define USE_DIR3DSPR        // Enables directional 3d sprites (see wl_dir3dspr.cpp)
//#define USE_FLOORCEILINGTEX // Enables floor and ceiling textures stored in the third mapplane (see wl_floorceiling.cpp)
//#define USE_HIRES           // Enables high resolution textures/sprites (128x128)
//#define USE_PARALLAX 16     // Enables parallax sky with 16 textures per sky (see wl_parallax.cpp)
//#define USE_CLOUDSKY        // Enables cloud sky support (see wl_cloudsky.cpp)
//#define USE_STARSKY         // Enables star sky support (see wl_atmos.cpp)
//#define USE_RAIN            // Enables rain support (see wl_atmos.cpp)
//#define USE_SNOW            // Enables snow support (see wl_atmos.cpp)
//#define FIXRAINSNOWLEAKS    // Enables leaking ceilings fix (by Adam Biser, only needed if maps with rain/snow and ceilings exist)

#define DEBUGKEYS             // Comment this out to compile without the Tab debug keys
#define ARTSEXTERN
#define DEMOSEXTERN
#define PLAYDEMOLIKEORIGINAL  // When playing or recording demos, several bug fixes do not take
                              // effect to let the original demos work as in the original Wolf3D v1.4
                              // (actually better, as the second demo rarely worked)

#ifdef PLAYDEMOLIKEORIGINAL
    #define DEMOCHOOSE_ORIG_SDL(orig, sdl) ((demorecord || demoplayback) ? (orig) : (sdl))
    #define DEMOCOND_ORIG                  (demorecord || demoplayback)
    #define DEMOIF_SDL                     if(DEMOCOND_SDL)
#else
    #define DEMOCHOOSE_ORIG_SDL(orig, sdl) (sdl)
    #define DEMOCOND_ORIG                  false
    #define DEMOIF_SDL
#endif
#define DEMOCOND_SDL                   (!DEMOCOND_ORIG)

// IOANCH 11.06.2012: don't define that now if Max OSX mode
//#ifndef __AUTOWOLF_DEBUG_MAX_OS_X__
#define USE_GPL             // Replaces the MAME OPL emulator by the DosBox one, which is under a GPL license
//#endif

#endif
