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


#ifndef WL_DEF_H
#define WL_DEF_H

// IOANCH 20130303: SPEAR() variable
#define IMPALE(a) (SPEAR() ? a##_sod : a##_wl6)
#define IMPALED(a, b) (SPEAR() ? a##_sod b : a##_wl6 b)

#include <fcntl.h>
#if defined(_arch_dreamcast)
#	include <string.h>
#	include "dc/dc_main.h"
#elif !defined(_WIN32)
#	include <stdint.h>
#	include <string.h>
#	include <stdarg.h>
#endif

#if !defined O_BINARY
#	define O_BINARY 0
#endif

#pragma pack(1)

// IOANCH 20130307: block definition
#ifdef APPLE_NON_CPP11
#define LAM ^
#define LAMDECL ^
#else
#define LAM []
#define LAMDECL *
#endif

// IOANCH 20130307: path length
#define MAX_PATH_LENGTH 2048

// IOANCH 20130301: unification
#include "audioabstract.h"
// IOANCH: added sprite header
#include "sprabstract.h"

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef int8_t boolean;
typedef void * memptr;

typedef struct
{
    int x,y;
} Point;
typedef struct
{
    Point ul,lr;
} Rect;

void Quit(const char *errorStr, ...);

#include "SODFlag.h"
#include "id_pm.h"
#include "id_sd.h"
#include "id_in.h"
#include "id_vl.h"
#include "id_vh.h"
#include "id_us.h"
#include "id_ca.h"

#define MAPSPOT(x,y,plane) (mapsegs[plane][((y)<<mapshift)+(x)])

#define SIGN(x)         ((x)>0?1:-1)
#define ABS(x)          ((int)(x)>0?(x):-(x))
#define LABS(x)         ((int32_t)(x)>0?(x):-(x))

#define abs(x) ABS(x)

/*
=============================================================================

                            GLOBAL CONSTANTS

=============================================================================
*/

#define MAXTICS 10
#define DEMOTICS        4

#define MAXACTORS       150         // max number of nazis, etc / map
#define MAXSTATS        400         // max number of lamps, bonus, etc
#define MAXDOORS        64          // max number of sliding doors
#define MAXWALLTILES    64          // max number of wall tiles

//
// tile constants
//

#define ICONARROWS      90
#define PUSHABLETILE    98
#define EXITTILE        99          // at end of castle
#define AREATILE        107         // first of NUMAREAS floor tiles
#define NUMAREAS        37
#define ELEVATORTILE    21
#define AMBUSHTILE      106
#define ALTELEVATORTILE 107

#define NUMBERCHARS     9


//----------------

#define EXTRAPOINTS     40000

#define PLAYERSPEED     3000
#define RUNSPEED        6000

#define SCREENSEG       0xa000

#define SCREENBWIDE     80

#define HEIGHTRATIO     0.50            // also defined in id_mm.c

#define BORDERCOLOR     3
#define FLASHCOLOR      5
#define FLASHTICS       4
// IOANCH 20130302: unification
#define LRpack_wl6 8
#define LRpack_sod 20
#define LRpack IMPALE(LRpack)

#define PLAYERSIZE      MINDIST         // player radius
#define MINACTORDIST    0x10000l        // minimum dist from player center
                                        // to any actor center

#define NUMLATCHPICS    100

#undef M_PI
#define PI              3.141592657
#define M_PI PI

#define GLOBAL1         (1l<<16)
#define TILEGLOBAL      GLOBAL1
#define PIXGLOBAL       (GLOBAL1/64)
#define TILESHIFT       16l
#define UNSIGNEDSHIFT   8

#define ANGLES          360             // must be divisable by 4
#define ANGLEQUAD       (ANGLES/4)
#define FINEANGLES      3600
#define ANG90           (FINEANGLES/4)
#define ANG180          (ANG90*2)
#define ANG270          (ANG90*3)
#define ANG360          (ANG90*4)
#define VANG90          (ANGLES/4)
#define VANG180         (VANG90*2)
#define VANG270         (VANG90*3)
#define VANG360         (VANG90*4)

#define MINDIST         (0x5800l)

#define mapshift        6
#define MAPSIZE         (1<<mapshift)
#define maparea         MAPSIZE*MAPSIZE

#define mapheight       MAPSIZE
#define mapwidth        MAPSIZE

#ifdef USE_HIRES

#define TEXTURESHIFT    7
#define TEXTURESIZE     (1<<TEXTURESHIFT)
#define TEXTUREFROMFIXEDSHIFT 2
#define TEXTUREMASK     (TEXTURESIZE*(TEXTURESIZE-1))

#define SPRITESCALEFACTOR 1

#else

#define TEXTURESHIFT    6
#define TEXTURESIZE     (1<<TEXTURESHIFT)
#define TEXTUREFROMFIXEDSHIFT 4
#define TEXTUREMASK     (TEXTURESIZE*(TEXTURESIZE-1))

#define SPRITESCALEFACTOR 2

#endif

#define NORTH   0
#define EAST    1
#define SOUTH   2
#define WEST    3


#define STATUSLINES     40

#define SCREENSIZE      (SCREENBWIDE*208)
#define PAGE1START      0
#define PAGE2START      (SCREENSIZE)
#define PAGE3START      (SCREENSIZE*2u)
#define FREESTART       (SCREENSIZE*3u)


#define PIXRADIUS       512

#define STARTAMMO       8


// object flag values

typedef enum
{
    FL_SHOOTABLE        = 0x00000001,
    FL_BONUS            = 0x00000002,
    FL_NEVERMARK        = 0x00000004,
    FL_VISABLE          = 0x00000008,
    FL_ATTACKMODE       = 0x00000010,
    FL_FIRSTATTACK      = 0x00000020,
    FL_AMBUSH           = 0x00000040,
    FL_NONMARK          = 0x00000080,
    FL_FULLBRIGHT       = 0x00000100,
#ifdef USE_DIR3DSPR
    // you can choose one of the following values in wl_act1.cpp
    // to make a static sprite a directional 3d sprite
    // (see example at the end of the statinfo array)
    FL_DIR_HORIZ_MID    = 0x00000200,
    FL_DIR_HORIZ_FW     = 0x00000400,
    FL_DIR_HORIZ_BW     = 0x00000600,
    FL_DIR_VERT_MID     = 0x00000a00,
    FL_DIR_VERT_FW      = 0x00000c00,
    FL_DIR_VERT_BW      = 0x00000e00,

    // these values are just used to improve readability of code
    FL_DIR_NONE         = 0x00000000,
    FL_DIR_POS_MID      = 0x00000200,
    FL_DIR_POS_FW       = 0x00000400,
    FL_DIR_POS_BW       = 0x00000600,
    FL_DIR_POS_MASK     = 0x00000600,
    FL_DIR_VERT_FLAG    = 0x00000800,
    FL_DIR_MASK         = 0x00000e00,
#endif
    // next free bit is   0x00001000
} objflag_t;

//
// IOANCH 25.10.2012
// Other constants that had to be named
//
#define I_PLAYERHEALTH		100
#define I_FIRSTAIDHEALTH	25
#define I_FOODHEALTH			10
#define I_DOGFOODHEALTH		4
#define I_BLOODHEALTHTHRESHOLD	10
#define I_CROSSSCORE			100
#define I_CHALICESCORE		500
#define I_TREASURESCORE		1000
#define I_CROWNSCORE			5000
#define I_GUARDSCORE			100
#define I_OFFICERSCORE		400
#define I_MUTANTSCORE		700
#define I_SSSCORE				500
#define I_DOGSCORE			200
#define I_BOSSSCORE			5000
#define I_FAKEHITLERSCORE	2000
#define I_SPECTRESCORE		200
#define I_MAXAMMO				99
#define I_CLIPAMMO			8
#define I_SEMICLIPAMMO		4
#define I_BOXAMMO				25
#define I_GUNAMMO				6
#define I_BONUSAMMO			25


//
// sprite constants
//

// IOANCH 20130302: now sprites are elsewhere (see inclusions) above


/*
=============================================================================

                               GLOBAL TYPES

=============================================================================
*/

typedef enum {
    di_north,
    di_east,
    di_south,
    di_west
} controldir_t;

typedef enum {
    dr_normal,
    dr_lock1,
    dr_lock2,
    dr_lock3,
    dr_lock4,
    dr_elevator
} door_t;

typedef enum {
    ac_badobject = -1,
    ac_no,
    ac_yes,
    ac_allways
} activetype;

typedef enum {
    nothing,
    playerobj,
    inertobj,
    guardobj,
    officerobj,
    ssobj,
    dogobj,
    bossobj,
    schabbobj,
    fakeobj,
    mechahitlerobj,
    mutantobj,
    needleobj,
    fireobj,
    bjobj,
    ghostobj,
    realhitlerobj,
    gretelobj,
    giftobj,
    fatobj,
    rocketobj,

    spectreobj,
    angelobj,
    transobj,
    uberobj,
    willobj,
    deathobj,
    hrocketobj,
    sparkobj
} classtype;

typedef enum {
    none,
    block,
    bo_gibs,
    bo_alpo,
    bo_firstaid,
    bo_key1,
    bo_key2,
    bo_key3,
    bo_key4,
    bo_cross,
    bo_chalice,
    bo_bible,
    bo_crown,
    bo_clip,
    bo_clip2,
    bo_machinegun,
    bo_chaingun,
    bo_food,
    bo_fullheal,
    bo_25clip,
    bo_spear
} wl_stat_t;

typedef enum {
    east,
    northeast,
    north,
    northwest,
    west,
    southwest,
    south,
    southeast,
    nodir
} dirtype;


#define NUMENEMIES  22
typedef enum {
    en_guard,
    en_officer,
    en_ss,
    en_dog,
    en_boss,
    en_schabbs,
    en_fake,
    en_hitler,
    en_mutant,
    en_blinky,
    en_clyde,
    en_pinky,
    en_inky,
    en_gretel,
    en_gift,
    en_fat,
    en_spectre,
    en_angel,
    en_trans,
    en_uber,
    en_will,
    en_death
} enemy_t;

typedef void (* statefunc) (void *);

// IOANCH 20130306: added state flags
#define STF_DAMAGING 1

typedef struct statestruct
{
    boolean rotate;
    short   shapenum;           // a shapenum of -1 means get from ob->temp1
    short   tictime;
    void    (*think) (void *),(*action) (void *);
    struct  statestruct *next;
    // IOANCH 20130306: added state flags
    uint64_t flags;
} statetype;


//---------------------
//
// trivial actor structure
//
//---------------------

typedef struct statstruct
{
    byte      tilex,tiley;
    short     shapenum;           // if shapenum == -1 the obj has been removed
    byte      *visspot;
    uint32_t  flags;
    byte      itemnumber;
} statobj_t;


//---------------------
//
// door actor structure
//
//---------------------

typedef enum
{
    dr_open,dr_closed,dr_opening,dr_closing
} doortype;

typedef struct doorstruct
{
    byte     tilex,tiley;
    boolean  vertical;
    byte     lock;
    doortype action;
    short    ticcount;
} doorobj_t;


//--------------------
//
// thinking actor structure
//
//--------------------

typedef struct objstruct
{
    activetype  active;
    short       ticcount;
    classtype   obclass;
    statetype   *state;

    uint32_t    flags;              // FL_SHOOTABLE, etc

    int32_t     distance;           // if negative, wait for that door to open
    dirtype     dir;

    fixed       x,y;
    word        tilex,tiley;
	fixed		recordx, recordy;	// IOANCH 20121211: record position for the bot
    byte        areanumber;

    short       viewx;
    word        viewheight;
    fixed       transx,transy;      // in global coord

    short       angle;
    short       hitpoints;
    int32_t     speed;

    short       temp1,temp2,hidden;
    struct objstruct *next,*prev;
} objtype;

enum
{
    bt_nobutton=-1,
    bt_attack=0,
    bt_strafe,
    bt_run,
    bt_use,
    bt_readyknife,
    bt_readypistol,
    bt_readymachinegun,
    bt_readychaingun,
    bt_nextweapon,
    bt_prevweapon,
    bt_esc,
    bt_pause,
    bt_strafeleft,
    bt_straferight,
    bt_moveforward,
    bt_movebackward,
    bt_turnleft,
    bt_turnright,
    NUMBUTTONS
};


#define NUMWEAPONS      4
typedef enum
{
    wp_knife,
    wp_pistol,
    wp_machinegun,
    wp_chaingun
} weapontype;


enum
{
    gd_baby,
    gd_easy,
    gd_medium,
    gd_hard
};

//---------------
//
// gamestate structure
//
//---------------

typedef struct
{
    short       difficulty;
    short       mapon;
    int32_t     oldscore,score,nextextra;
    short       lives;
    short       health;
    short       ammo;
    short       keys;
    weapontype  bestweapon,weapon,chosenweapon;

    short       faceframe;
    short       attackframe,attackcount,weaponframe;

    short       episode,secretcount,treasurecount,killcount,
                secrettotal,treasuretotal,killtotal;
    int32_t     TimeCount;
    int32_t     killx,killy;
    boolean     victoryflag;            // set during victory animations
} gametype;


typedef enum
{
    ex_stillplaying,
    ex_completed,
    ex_died,
    ex_warped,
    ex_resetgame,
    ex_loadedgame,
    ex_victorious,
    ex_abort,
    ex_demodone,
    ex_secretlevel
} exit_t;


extern word *mapsegs[MAPPLANES];
extern int mapon;

/*
=============================================================================

                             WL_MAIN DEFINITIONS

=============================================================================
*/




/*
=============================================================================

                         WL_GAME DEFINITIONS

=============================================================================
*/


/*
=============================================================================

                            WL_PLAY DEFINITIONS

=============================================================================
*/

/*
=============================================================================

                                WL_INTER

=============================================================================
*/


/*
=============================================================================

                                WL_DEBUG

=============================================================================
*/

int DebugKeys (void);

/*
=============================================================================

                            WL_DRAW DEFINITIONS

=============================================================================
*/

/*
=============================================================================

                             WL_STATE DEFINITIONS

=============================================================================
*/

/*
=============================================================================

                             WL_AGENT DEFINITIONS

=============================================================================
*/




/*
=============================================================================

                             WL_ACT1 DEFINITIONS

=============================================================================
*/




// IOANCH 20130310: modularization

/*
=============================================================================

                             WL_ACT2 DEFINITIONS

=============================================================================
*/



/*
=============================================================================

                             WL_TEXT DEFINITIONS

=============================================================================
*/


/*
=============================================================================

                               GP2X DEFINITIONS

=============================================================================
*/

#if defined(GP2X)

#if defined(GP2X_940)
void GP2X_MemoryInit(void);
void GP2X_Shutdown(void);
#endif
void GP2X_ButtonDown(int button);
void GP2X_ButtonUp(int button);

#endif


/*
=============================================================================

                             MISC DEFINITIONS

=============================================================================
*/

static inline fixed FixedMul(fixed a, fixed b)
{
	return (fixed)(((int64_t)a * b + 0x8000) >> 16);
}

#define GetTicks() ((SDL_GetTicks()*7)/100)

#define ISPOINTER(x) ((((uintptr_t)(x)) & ~0xffff) != 0)

#define CHECKMALLOCRESULT(x) if(!(x)) Quit("Out of memory at %s:%i", __FILE__, __LINE__)

#ifdef _WIN32
    #define strcasecmp stricmp
    #define strncasecmp strnicmp
    #define snprintf _snprintf
    #define itoanoreturn itoa
    #define ltoanoreturn ltoa
#else
    #define itoa(value, string, radix) (sprintf(string, "%d", value), string)
    #define ltoa(value, string, radix) (sprintf(string, "%ld", value), string)
    #define itoanoreturn(value, string, radix) sprintf(string, "%d", value)
    #define ltoanoreturn(value, string, radix) sprintf(string, "%ld", value)
#if 0
    static inline char* itoa(int value, char* string, int radix)
    {
        sprintf(string, "%d", value);
        return string;
    }

    static inline char* ltoa(long value, char* string, int radix)
    {
        sprintf(string, "%ld", value);
        return string;
    }
#endif
#endif

#define lengthof(x) (sizeof(x) / sizeof(*(x)))
#define endof(x)    ((x) + lengthof(x))

static inline word READWORD(byte *&ptr)
{
    word val = ptr[0] | ptr[1] << 8;
    ptr += 2;
    return val;
}

static inline longword READLONGWORD(byte *&ptr)
{
    longword val = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
    ptr += 4;
    return val;
}


/*
=============================================================================

                           FEATURE DEFINITIONS

=============================================================================
*/

#ifdef USE_FEATUREFLAGS
    // The currently available feature flags
    #define FF_STARSKY      0x0001
    #define FF_PARALLAXSKY  0x0002
    #define FF_CLOUDSKY     0x0004
    #define FF_RAIN         0x0010
    #define FF_SNOW         0x0020

    // The ffData... variables contain the 16-bit values of the according corners of the current level.
    // The corners are overwritten with adjacent tiles after initialization in SetupGameLevel
    // to avoid interpretation as e.g. doors.
    extern int ffDataTopLeft, ffDataTopRight, ffDataBottomLeft, ffDataBottomRight;

    /*************************************************************
     * Current usage of ffData... variables:
     * ffDataTopLeft:     lower 8-bit: ShadeDefID
     * ffDataTopRight:    FeatureFlags
     * ffDataBottomLeft:  CloudSkyDefID or ParallaxStartTexture
     * ffDataBottomRight: unused
     *************************************************************/

    // The feature flags are stored as a wall in the upper right corner of each level
    static inline word GetFeatureFlags()
    {
        return ffDataTopRight;
    }

#endif

#ifdef USE_FLOORCEILINGTEX
    void DrawFloorAndCeiling(byte *vbuf, unsigned vbufPitch, int min_wallheight);
#endif

#ifdef USE_PARALLAX
    void DrawParallax(byte *vbuf, unsigned vbufPitch);
#endif

#ifdef USE_DIR3DSPR
    void Scale3DShape(byte *vbuf, unsigned vbufPitch, statobj_t *ob);
#endif

//
// IOANCH 17.05.2012
//
// Other externs
//

// Spawn frames and data
extern statetype   s_attack;
extern statetype s_grdstand;
extern statetype s_ofcstand;
extern statetype s_mutstand;
extern statetype s_ssstand;

extern statetype s_grdpath1;
extern statetype s_ofcpath1;
extern statetype s_sspath1;
extern statetype s_mutpath1;
extern statetype s_dogpath1;

// IOANCH 20.05.2012
extern  statetype s_grdshoot1;
extern  statetype s_grdshoot2;
extern  statetype s_grdshoot3;
extern  statetype s_ofcshoot1;
extern  statetype s_ofcshoot2;
extern  statetype s_ofcshoot3;
extern  statetype s_ssshoot1;
extern  statetype s_ssshoot2;
extern  statetype s_ssshoot3;
extern  statetype s_ssshoot4;
extern  statetype s_ssshoot5;
extern  statetype s_ssshoot6;
extern  statetype s_ssshoot7;
extern  statetype s_ssshoot8;
extern  statetype s_ssshoot9;
extern  statetype s_mutshoot1;
extern  statetype s_mutshoot2;
extern  statetype s_mutshoot3;
extern  statetype s_mutshoot4;

extern statetype s_boom1;
extern statetype s_boom2;
extern statetype s_boom3;

// IOANCH 20130302: unification
extern statetype s_bossstand;
extern statetype s_gretelstand;
extern statetype s_schabbdie2;
extern statetype s_schabbstand;
extern statetype s_clydechase1;
extern statetype s_pinkychase1;
extern statetype s_inkychase1;
extern statetype s_giftdie2;
extern statetype s_giftstand;
extern statetype s_fatdie2;
extern statetype s_fatstand;
extern statetype s_hitlerdie2;
extern statetype s_fakestand;
extern statetype s_mechastand;

// IOANCH 20.05.2012
extern  statetype s_bossshoot1;
extern  statetype s_bossshoot2;
extern  statetype s_bossshoot3;
extern  statetype s_bossshoot4;
extern  statetype s_bossshoot5;
extern  statetype s_bossshoot6;
extern  statetype s_bossshoot7;
extern  statetype s_bossshoot8;
extern  statetype s_schabbshoot1;
extern  statetype s_schabbshoot2;
extern  statetype s_fakeshoot1;
extern  statetype s_fakeshoot2;
extern  statetype s_fakeshoot3;
extern  statetype s_fakeshoot4;
extern  statetype s_fakeshoot5;
extern  statetype s_fakeshoot6;
extern  statetype s_fakeshoot7;
extern  statetype s_fakeshoot8;
extern  statetype s_fakeshoot9;
extern  statetype s_mechashoot1;
extern  statetype s_mechashoot2;
extern  statetype s_mechashoot3;
extern  statetype s_mechashoot4;
extern  statetype s_mechashoot5;
extern  statetype s_mechashoot6;
extern  statetype s_hitlershoot1;
extern  statetype s_hitlershoot2;
extern  statetype s_hitlershoot3;
extern  statetype s_hitlershoot4;
extern  statetype s_hitlershoot5;
extern  statetype s_hitlershoot6;
extern  statetype s_gretelshoot1;
extern  statetype s_gretelshoot2;
extern  statetype s_gretelshoot3;
extern  statetype s_gretelshoot4;
extern  statetype s_gretelshoot5;
extern  statetype s_gretelshoot6;
extern  statetype s_gretelshoot7;
extern  statetype s_gretelshoot8;
extern  statetype s_giftshoot1;
extern  statetype s_giftshoot2;
extern  statetype s_fatshoot1;
extern  statetype s_fatshoot2;
extern  statetype s_fatshoot3;
extern  statetype s_fatshoot4;
extern  statetype s_fatshoot5;
extern  statetype s_fatshoot6;

extern  statetype s_angelshoot1;
extern  statetype s_angelshoot2;
extern  statetype s_angelshoot3;
extern  statetype s_transshoot1;
extern  statetype s_transshoot2;
extern  statetype s_transshoot3;
extern  statetype s_transshoot4;
extern  statetype s_transshoot5;
extern  statetype s_transshoot6;
extern  statetype s_transshoot7;
extern  statetype s_transshoot8;
extern  statetype s_ubershoot1;
extern  statetype s_ubershoot2;
extern  statetype s_ubershoot3;
extern  statetype s_ubershoot4;
extern  statetype s_ubershoot5;
extern  statetype s_ubershoot6;
extern  statetype s_ubershoot7;
extern  statetype s_willshoot1;
extern  statetype s_willshoot2;
extern  statetype s_willshoot3;
extern  statetype s_willshoot4;
extern  statetype s_willshoot5;
extern  statetype s_willshoot6;
extern  statetype s_deathshoot1;
extern  statetype s_deathshoot2;
extern  statetype s_deathshoot3;
extern  statetype s_deathshoot4;
extern  statetype s_deathshoot5;
extern  statetype s_transdie01;
extern statetype s_transstand;
extern statetype s_uberdie01;
extern statetype s_uberstand;
extern statetype s_willdie2;
extern statetype s_willstand;
extern statetype s_deathdie2;
extern statetype s_deathstand;
extern statetype s_angeldie11;
extern statetype s_angelstand;
extern statetype s_spectrewait1;
extern statetype s_hboom1;
extern statetype s_hboom2;
extern statetype s_hboom3;

// IOANCH 26.05.2012
extern objtype *lastobj;

#endif
