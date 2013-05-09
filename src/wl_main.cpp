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

// WL_MAIN.C

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "foreign.h"
#include "version.h"
#include "wl_def.h"
#include "wl_act1.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_inter.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_play.h"
#pragma hdrstop
#include "wl_atmos.h"
#include <SDL_syswm.h>
// IOANCH 17.05.2012
#include "ioan_bot.h"
#include "ioan_bas.h"
#include "List.h"
#include "Exception.h"

/*
=============================================================================

                             WOLFENSTEIN 3-D

                        An Id Software production

                             by John Carmack

=============================================================================
*/

// IOANCH 20130303: unification
extern byte signon_wl6[];
extern byte signon_sod[];

/*
=============================================================================

                             LOCAL CONSTANTS

=============================================================================
*/


#define FOCALLENGTH     (0x5700l)               // in global coordinates
#define VIEWGLOBAL      0x10000                 // globals visable flush to wall

#define VIEWWIDTH       256                     // size of view window
#define VIEWHEIGHT      144

/*
=============================================================================

                            GLOBAL VARIABLES

=============================================================================
*/

char    str[80];
int     dirangle[9] = {0,ANGLES/8,2*ANGLES/8,3*ANGLES/8,4*ANGLES/8,
                       5*ANGLES/8,6*ANGLES/8,7*ANGLES/8,ANGLES};

//
// proejection variables
//
fixed    focallength;
unsigned screenofs;
int      viewscreenx, viewscreeny;
int      viewwidth;
int      viewheight;
short    centerx;
int      shootdelta;           // pixels away from centerx a target can be
fixed    scale;
int32_t  heightnumerator;


void    Quit (const char *error,...);

boolean startgame;
boolean loadedgame;
int     mouseadjustment;

PString configdir;
PString configname("CONFIG.");

//
// Command line parameter variables
//
boolean param_debugmode = false;
boolean param_nowait = false;
int     param_difficulty = 1;           // default is "normal"
int     param_tedlevel = -1;            // default is not to start a level
int     param_joystickindex = 0;

#if defined(_arch_dreamcast)
int     param_joystickhat = 0;
int     param_samplerate = 11025;       // higher samplerates result in "out of memory"
int     param_audiobuffer = 4096 / (44100 / param_samplerate);
#elif defined(GP2X_940)
int     param_joystickhat = -1;
int     param_samplerate = 11025;       // higher samplerates result in "out of memory"
int     param_audiobuffer = 128;
#else
int     param_joystickhat = -1;
int     param_samplerate = 44100;
int     param_audiobuffer = 2048 / (44100 / param_samplerate);
#endif

int     param_mission = 0;
boolean param_goodtimes = false;
boolean param_ignorenumchunks = false;

/*
=============================================================================

                            LOCAL VARIABLES

=============================================================================
*/


/*
====================
=
= ReadConfig
=
====================
*/

void ReadConfig(void)
{
    SDMode  sd;
    SMMode  sm;
    SDSMode sds;

    PString configpath;

#ifdef _arch_dreamcast
    DC_LoadFromVMU(configname.buffer());
#endif

    configpath = configdir.withSubpath(configname);

    const int file = open(configpath.buffer(), O_RDONLY | O_BINARY);
    if (file != -1)
    {
        //
        // valid config file
        //
        word tmp;
        read(file,&tmp,sizeof(tmp));
        if(tmp!=0xfefa)
        {
            close(file);
            goto noconfig;
        }
        read(file,Scores,sizeof(HighScore) * MaxScores);

        read(file,&sd,sizeof(sd));
        read(file,&sm,sizeof(sm));
        read(file,&sds,sizeof(sds));

        read(file,&mouseenabled,sizeof(mouseenabled));
        read(file,&joystickenabled,sizeof(joystickenabled));
        boolean dummyJoypadEnabled;
        read(file,&dummyJoypadEnabled,sizeof(dummyJoypadEnabled));
        boolean dummyJoystickProgressive;
        read(file,&dummyJoystickProgressive,sizeof(dummyJoystickProgressive));
        int dummyJoystickPort = 0;
        read(file,&dummyJoystickPort,sizeof(dummyJoystickPort));

        read(file,dirscan,sizeof(dirscan));
        read(file,buttonscan,sizeof(buttonscan));
        read(file,buttonmouse,sizeof(buttonmouse));
        read(file,buttonjoy,sizeof(buttonjoy));

        read(file,&viewsize,sizeof(viewsize));
        read(file,&mouseadjustment,sizeof(mouseadjustment));

        close(file);

        if ((sd == sdm_AdLib || sm == smm_AdLib) && !AdLibPresent
                && !SoundBlasterPresent)
        {
            sd = sdm_PC;
            sm = smm_Off;
        }

        if ((sds == sds_SoundBlaster && !SoundBlasterPresent))
            sds = sds_Off;

        // make sure values are correct

        if(mouseenabled) mouseenabled=true;
        if(joystickenabled) joystickenabled=true;

        if (!MousePresent)
            mouseenabled = false;
        if (!IN_JoyPresent())
            joystickenabled = false;

        if(mouseadjustment<0) mouseadjustment=0;
        else if(mouseadjustment>9) mouseadjustment=9;

        if(viewsize<4) viewsize=4;
        else if(viewsize>21) viewsize=21;

        MainMenu[6].active=1;
        MainItems.curpos=0;
    }
    else
    {
        //
        // no config file, so select by hardware
        //
noconfig:
        if (SoundBlasterPresent || AdLibPresent)
        {
            sd = sdm_AdLib;
            sm = smm_AdLib;
        }
        else
        {
            sd = sdm_PC;
            sm = smm_Off;
        }

        if (SoundBlasterPresent)
            sds = sds_SoundBlaster;
        else
            sds = sds_Off;

        if (MousePresent)
            mouseenabled = true;

        if (IN_JoyPresent())
            joystickenabled = true;

        viewsize = 19;                          // start with a good size
        mouseadjustment=5;
    }

    SD_SetMusicMode (sm);
    SD_SetSoundMode (sd);
    SD_SetDigiDevice (sds);
}

/*
====================
=
= WriteConfig
=
====================
*/

void WriteConfig(void)
{
    PString configpath;

#ifdef _arch_dreamcast
    fs_unlink(configname.buffer());
#endif

    configpath = configdir.withSubpath(configname);

    const int file = open(configpath.buffer(), O_CREAT | O_WRONLY | O_BINARY, 0644);
    if (file != -1)
    {
        word tmp=0xfefa;
        write(file,&tmp,sizeof(tmp));
        write(file,Scores,sizeof(HighScore) * MaxScores);

        write(file,&SoundMode,sizeof(SoundMode));
        write(file,&MusicMode,sizeof(MusicMode));
        write(file,&DigiMode,sizeof(DigiMode));

        write(file,&mouseenabled,sizeof(mouseenabled));
        write(file,&joystickenabled,sizeof(joystickenabled));
        boolean dummyJoypadEnabled = false;
        write(file,&dummyJoypadEnabled,sizeof(dummyJoypadEnabled));
        boolean dummyJoystickProgressive = false;
        write(file,&dummyJoystickProgressive,sizeof(dummyJoystickProgressive));
        int dummyJoystickPort = 0;
        write(file,&dummyJoystickPort,sizeof(dummyJoystickPort));

        write(file,dirscan,sizeof(dirscan));
        write(file,buttonscan,sizeof(buttonscan));
        write(file,buttonmouse,sizeof(buttonmouse));
        write(file,buttonjoy,sizeof(buttonjoy));

        write(file,&viewsize,sizeof(viewsize));
        write(file,&mouseadjustment,sizeof(mouseadjustment));

        close(file);
    }
#ifdef _arch_dreamcast
    DC_SaveToVMU(configname.buffer(), NULL);
#endif
}


//===========================================================================

/*
=====================
=
= NewGame
=
= Set up new game to start from the beginning
=
=====================
*/

void NewGame (int difficulty,int episode)
{
    memset (&gamestate,0,sizeof(gamestate));
    gamestate.difficulty = difficulty;
    gamestate.weapon = gamestate.bestweapon
            = gamestate.chosenweapon = wp_pistol;
    gamestate.health = I_PLAYERHEALTH;	// IOANCH 25.10.2012: named constants
    gamestate.ammo = STARTAMMO;
    gamestate.lives = 3;
    gamestate.nextextra = EXTRAPOINTS;
    gamestate.episode=episode;

    startgame = true;
}

//===========================================================================

void DiskFlopAnim(int x,int y)
{
    static int8_t which=0;
    if (!x && !y)
        return;
    // IOANCH 20130302: unification
    VWB_DrawPic(x,y,gfxvmap[C_DISKLOADING1PIC][SPEAR()]+which);
    VW_UpdateScreen();
    which^=1;
}


int32_t DoChecksum(byte *source,unsigned size,int32_t checksum)
{
    unsigned i;

    for (i=0;i<size-1;i++)
    checksum += source[i]^source[i+1];

    return checksum;
}


/*
==================
=
= SaveTheGame
=
==================
*/

extern statetype s_grdstand;
extern statetype s_player;

boolean SaveTheGame(FILE *file,int x,int y)
{
//    struct diskfree_t dfree;
//    int32_t avail,size,checksum;
    int checksum;
    objtype *ob;
    objtype nullobj;
    statobj_t nullstat;

/*    if (_dos_getdiskfree(0,&dfree))
        Quit("Error in _dos_getdiskfree call");

    avail = (int32_t)dfree.avail_clusters *
                  dfree.bytes_per_sector *
                  dfree.sectors_per_cluster;

    size = 0;
    for (ob = player; ob ; ob=ob->next)
        size += sizeof(*ob);
    size += sizeof(nullobj);

    size += sizeof(gamestate) +
            sizeof(LRstruct)*LRpack +
            sizeof(tilemap) +
            sizeof(actorat) +
            sizeof(laststatobj) +
            sizeof(statobjlist) +
            sizeof(doorposition) +
            sizeof(pwallstate) +
            sizeof(pwalltile) +
            sizeof(pwallx) +
            sizeof(pwally) +
            sizeof(pwalldir) +
            sizeof(pwallpos);

    if (avail < size)
    {
        Message(STR_NOSPACE1"\n"STR_NOSPACE2);
        return false;
    }*/

    checksum = 0;

    DiskFlopAnim(x,y);
    fwrite(&gamestate,sizeof(gamestate),1,file);
    checksum = DoChecksum((byte *)&gamestate,sizeof(gamestate),checksum);

    DiskFlopAnim(x,y);
    fwrite(&LevelRatios[0],sizeof(LRstruct)*LRpack,1,file);
    checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*LRpack,checksum);

    DiskFlopAnim(x,y);
    fwrite(tilemap,sizeof(tilemap),1,file);
    checksum = DoChecksum((byte *)tilemap,sizeof(tilemap),checksum);
    DiskFlopAnim(x,y);

    int i;
    for(i=0;i<MAPSIZE;i++)
    {
        for(int j=0;j<MAPSIZE;j++)
        {
            word actnum;
            objtype *objptr=actorat[i][j];
            if(ISPOINTER(objptr))
                actnum=0x8000 | (word)(objptr-objlist);
            else
                actnum=(word)(uintptr_t)objptr;
            fwrite(&actnum,sizeof(actnum),1,file);
            checksum = DoChecksum((byte *)&actnum,sizeof(actnum),checksum);
        }
    }

    fwrite (areaconnect,sizeof(areaconnect),1,file);
    fwrite (areabyplayer,sizeof(areabyplayer),1,file);

    // player object needs special treatment as it's in WL_AGENT.CPP and not in
    // WL_ACT2.CPP which could cause problems for the relative addressing

    ob = player;
    DiskFlopAnim(x,y);
    memcpy(&nullobj,ob,sizeof(nullobj));
    nullobj.state=(statetype *) ((uintptr_t)nullobj.state-(uintptr_t)&s_player);
    fwrite(&nullobj,sizeof(nullobj),1,file);
    ob = ob->next;

    DiskFlopAnim(x,y);
    for (; ob ; ob=ob->next)
    {
        memcpy(&nullobj,ob,sizeof(nullobj));
        nullobj.state=(statetype *) ((uintptr_t)nullobj.state-(uintptr_t)&s_grdstand);
        fwrite(&nullobj,sizeof(nullobj),1,file);
    }
    nullobj.active = ac_badobject;          // end of file marker
    DiskFlopAnim(x,y);
    fwrite(&nullobj,sizeof(nullobj),1,file);

    DiskFlopAnim(x,y);
    word laststatobjnum=(word) (laststatobj-statobjlist);
    fwrite(&laststatobjnum,sizeof(laststatobjnum),1,file);
    checksum = DoChecksum((byte *)&laststatobjnum,sizeof(laststatobjnum),checksum);

    DiskFlopAnim(x,y);
    for(i=0;i<MAXSTATS;i++)
    {
        memcpy(&nullstat,statobjlist+i,sizeof(nullstat));
        nullstat.visspot=(byte *) ((uintptr_t) nullstat.visspot-(uintptr_t)spotvis);
        fwrite(&nullstat,sizeof(nullstat),1,file);
        checksum = DoChecksum((byte *)&nullstat,sizeof(nullstat),checksum);
    }

    DiskFlopAnim(x,y);
    fwrite (doorposition,sizeof(doorposition),1,file);
    checksum = DoChecksum((byte *)doorposition,sizeof(doorposition),checksum);
    DiskFlopAnim(x,y);
    fwrite (doorobjlist,sizeof(doorobjlist),1,file);
    checksum = DoChecksum((byte *)doorobjlist,sizeof(doorobjlist),checksum);

    DiskFlopAnim(x,y);
    fwrite (&pwallstate,sizeof(pwallstate),1,file);
    checksum = DoChecksum((byte *)&pwallstate,sizeof(pwallstate),checksum);
    fwrite (&pwalltile,sizeof(pwalltile),1,file);
    checksum = DoChecksum((byte *)&pwalltile,sizeof(pwalltile),checksum);
    fwrite (&pwallx,sizeof(pwallx),1,file);
    checksum = DoChecksum((byte *)&pwallx,sizeof(pwallx),checksum);
    fwrite (&pwally,sizeof(pwally),1,file);
    checksum = DoChecksum((byte *)&pwally,sizeof(pwally),checksum);
    fwrite (&pwalldir,sizeof(pwalldir),1,file);
    checksum = DoChecksum((byte *)&pwalldir,sizeof(pwalldir),checksum);
    fwrite (&pwallpos,sizeof(pwallpos),1,file);
    checksum = DoChecksum((byte *)&pwallpos,sizeof(pwallpos),checksum);

    //
    // WRITE OUT CHECKSUM
    //
    fwrite (&checksum,sizeof(checksum),1,file);

    fwrite (&lastgamemusicoffset,sizeof(lastgamemusicoffset),1,file);

    return(true);
}

//===========================================================================

/*
==================
=
= LoadTheGame
=
==================
*/

boolean LoadTheGame(FILE *file,int x,int y)
{
    int32_t checksum,oldchecksum;
    objtype nullobj;
    statobj_t nullstat;

    checksum = 0;

    DiskFlopAnim(x,y);
    fread (&gamestate,sizeof(gamestate),1,file);
    checksum = DoChecksum((byte *)&gamestate,sizeof(gamestate),checksum);

    DiskFlopAnim(x,y);
    fread (&LevelRatios[0],sizeof(LRstruct)*LRpack,1,file);
    checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*LRpack,checksum);

    DiskFlopAnim(x,y);
    SetupGameLevel ();

    DiskFlopAnim(x,y);
    fread (tilemap,sizeof(tilemap),1,file);
    checksum = DoChecksum((byte *)tilemap,sizeof(tilemap),checksum);

    DiskFlopAnim(x,y);

    int actnum=0, i;
    for(i=0;i<MAPSIZE;i++)
    {
        for(int j=0;j<MAPSIZE;j++)
        {
            fread (&actnum,sizeof(word),1,file);
            checksum = DoChecksum((byte *) &actnum,sizeof(word),checksum);
            if(actnum&0x8000)
                actorat[i][j]=objlist+(actnum&0x7fff);
            else
                actorat[i][j]=(objtype *)(uintptr_t) actnum;
        }
    }

    fread (areaconnect,sizeof(areaconnect),1,file);
    fread (areabyplayer,sizeof(areabyplayer),1,file);

    InitActorList ();
    DiskFlopAnim(x,y);
    fread (player,sizeof(*player),1,file);
    player->state=(statetype *) ((uintptr_t)player->state+(uintptr_t)&s_player);

    while (1)
    {
        DiskFlopAnim(x,y);
        fread (&nullobj,sizeof(nullobj),1,file);
        if (nullobj.active == ac_badobject)
            break;
        GetNewActor ();
        nullobj.state=(statetype *) ((uintptr_t)nullobj.state+(uintptr_t)&s_grdstand);
        // don't copy over the links
        memcpy (newobj,&nullobj,sizeof(nullobj)-8);
    }

    DiskFlopAnim(x,y);
    word laststatobjnum;
    fread (&laststatobjnum,sizeof(laststatobjnum),1,file);
    laststatobj=statobjlist+laststatobjnum;
    checksum = DoChecksum((byte *)&laststatobjnum,sizeof(laststatobjnum),checksum);

    DiskFlopAnim(x,y);
    for(i=0;i<MAXSTATS;i++)
    {
        fread(&nullstat,sizeof(nullstat),1,file);
        checksum = DoChecksum((byte *)&nullstat,sizeof(nullstat),checksum);
        nullstat.visspot=(byte *) ((uintptr_t)nullstat.visspot+(uintptr_t)spotvis);
        memcpy(statobjlist+i,&nullstat,sizeof(nullstat));
        // IOANCH 20130304: Add newly dropped items to list
        if(nullstat.flags & FL_BONUS && nullstat.shapenum != -1)
        {
            Basic::AddItemToList(nullstat.tilex, nullstat.tiley,
                                 nullstat.itemnumber);
        }
    }

    DiskFlopAnim(x,y);
    fread (doorposition,sizeof(doorposition),1,file);
    checksum = DoChecksum((byte *)doorposition,sizeof(doorposition),checksum);
    DiskFlopAnim(x,y);
    fread (doorobjlist,sizeof(doorobjlist),1,file);
    checksum = DoChecksum((byte *)doorobjlist,sizeof(doorobjlist),checksum);

    DiskFlopAnim(x,y);
    fread (&pwallstate,sizeof(pwallstate),1,file);
    checksum = DoChecksum((byte *)&pwallstate,sizeof(pwallstate),checksum);
    fread (&pwalltile,sizeof(pwalltile),1,file);
    checksum = DoChecksum((byte *)&pwalltile,sizeof(pwalltile),checksum);
    fread (&pwallx,sizeof(pwallx),1,file);
    checksum = DoChecksum((byte *)&pwallx,sizeof(pwallx),checksum);
    fread (&pwally,sizeof(pwally),1,file);
    checksum = DoChecksum((byte *)&pwally,sizeof(pwally),checksum);
    fread (&pwalldir,sizeof(pwalldir),1,file);
    checksum = DoChecksum((byte *)&pwalldir,sizeof(pwalldir),checksum);
    fread (&pwallpos,sizeof(pwallpos),1,file);
    checksum = DoChecksum((byte *)&pwallpos,sizeof(pwallpos),checksum);

    if (gamestate.secretcount)      // assign valid floorcodes under moved pushwalls
    {
        word *map, *obj; word tile, sprite;
        map = mapsegs[0]; obj = mapsegs[1];
        for (y=0;y<mapheight;y++)
            for (x=0;x<mapwidth;x++)
            {
                tile = *map++; sprite = *obj++;
                if (sprite == PUSHABLETILE && !tilemap[x][y]
                    && (tile < AREATILE || tile >= (AREATILE+NUMMAPS)))
                {
                    if (*map >= AREATILE)
                        tile = *map;
                    if (*(map-1-mapwidth) >= AREATILE)
                        tile = *(map-1-mapwidth);
                    if (*(map-1+mapwidth) >= AREATILE)
                        tile = *(map-1+mapwidth);
                    if ( *(map-2) >= AREATILE)
                        tile = *(map-2);

                    *(map-1) = tile; *(obj-1) = 0;
                }
            }
    }

    Thrust(0,0);    // set player->areanumber to the floortile you're standing on

    fread (&oldchecksum,sizeof(oldchecksum),1,file);

    fread (&lastgamemusicoffset,sizeof(lastgamemusicoffset),1,file);
    if(lastgamemusicoffset<0) lastgamemusicoffset=0;


    if (oldchecksum != checksum)
    {
        Message(STR_SAVECHT1"\n"
                STR_SAVECHT2"\n"
                STR_SAVECHT3"\n"
                STR_SAVECHT4);

        IN_ClearKeysDown();
        IN_Ack();

        gamestate.oldscore = gamestate.score = 0;
        gamestate.lives = 1;
        gamestate.weapon =
            gamestate.chosenweapon =
            gamestate.bestweapon = wp_pistol;
        gamestate.ammo = 8;
    }
	
	// IOANCH 30.06.2012: unlink all dead Nazis from the list
	for(objtype *obj = lastobj; obj; obj = obj->prev)
	{
		if(!(obj->flags & FL_SHOOTABLE))
			Basic::livingNazis.remove(obj);
		switch(obj->obclass)
		{
			case needleobj:
			case fireobj:
			case rocketobj:
			case hrocketobj:
			case sparkobj:
                // IOANCH 20130303: unification
				if(obj->state != &s_boom1 && obj->state != &s_boom2 && obj->state != &s_boom3
				   && obj->state != &s_hboom1 && obj->state != &s_hboom2 && obj->state != &s_hboom3
				   )
				{
					Basic::thrownProjectiles.add(obj);
				}
				
			default:
				;
		}
	}

    return true;
}

//===========================================================================

/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
    US_Shutdown ();         // This line is completely useless...
    SD_Shutdown ();
    PM_Shutdown ();
    IN_Shutdown ();
    VW_Shutdown ();
    CA_Shutdown ();
#if defined(GP2X_940)
    GP2X_Shutdown();
#endif
}


//===========================================================================

/*
==================
=
= BuildTables
=
= Calculates:
=
= scale                 projection constant
= sintable/costable     overlapping fractional tables
=
==================
*/

const float radtoint = (float)(FINEANGLES/2/PI);

void BuildTables (void)
{
    //
    // calculate fine tangents
    //

    int i;
    for(i=0;i<FINEANGLES/8;i++)
    {
        double tang=tan((i+0.5)/radtoint);
        finetangent[i]=(int32_t)(tang*GLOBAL1);
        finetangent[FINEANGLES/4-1-i]=(int32_t)((1/tang)*GLOBAL1);
    }

    //
    // costable overlays sintable with a quarter phase shift
    // ANGLES is assumed to be divisable by four
    //

    float angle=0;
    float anglestep=(float)(PI/2/ANGLEQUAD);
    for(i=0; i<ANGLEQUAD; i++)
    {
        fixed value=(int32_t)(GLOBAL1*sin(angle));
        sintable[i]=sintable[i+ANGLES]=sintable[ANGLES/2-i]=value;
        sintable[ANGLES-i]=sintable[ANGLES/2+i]=-value;
        angle+=anglestep;
    }
    sintable[ANGLEQUAD] = 65536;
    sintable[3*ANGLEQUAD] = -65536;

#if defined(USE_STARSKY) || defined(USE_RAIN) || defined(USE_SNOW)
    Init3DPoints();
#endif
}

//===========================================================================


/*
====================
=
= CalcProjection
=
= Uses focallength
=
====================
*/

void CalcProjection (int32_t focal)
{
    int     i;
    int    intang;
    float   angle;
    double  tang;
    int     halfview;
    double  facedist;

    focallength = focal;
    facedist = focal+MINDIST;
    halfview = viewwidth/2;                                 // half view in pixels

    //
    // calculate scale value for vertical height calculations
    // and sprite x calculations
    //
    scale = (fixed) (halfview*facedist/(VIEWGLOBAL/2));

    //
    // divide heightnumerator by a posts distance to get the posts height for
    // the heightbuffer.  The pixel height is height>>2
    //
    heightnumerator = (TILEGLOBAL*scale)>>6;

    //
    // calculate the angle offset from view angle of each pixel's ray
    //

    for (i=0;i<halfview;i++)
    {
        // start 1/2 pixel over, so viewangle bisects two middle pixels
        tang = (int32_t)i*VIEWGLOBAL/viewwidth/facedist;
        angle = (float) atan(tang);
        intang = (int) (angle*radtoint);
        pixelangle[halfview-1-i] = intang;
        pixelangle[halfview+i] = -intang;
    }
}



//===========================================================================

/*
===================
=
= SetupWalls
=
= Map tile values to scaled pics
=
===================
*/

void SetupWalls (void)
{
    int     i;

    horizwall[0]=0;
    vertwall[0]=0;

    for (i=1;i<MAXWALLTILES;i++)
    {
        horizwall[i]=(i-1)*2;
        vertwall[i]=(i-1)*2+1;
    }
}

//===========================================================================

/*
==========================
=
= SignonScreen
=
==========================
*/

void SignonScreen (void)                        // VGA version
{
    VL_SetVGAPlaneMode ();

    VL_MungePic (IMPALE(signon),320,200);
    VL_MemToScreen (IMPALE(signon),320,200,0,0);
}


/*
==========================
=
= FinishSignon
=
==========================
*/

void FinishSignon (void)
{
    // IOANCH 20130303: unification
    if(!SPEAR())
    {
        VW_Bar (0,189,300,11,VL_GetPixel(0,0));
        WindowX = 0;
        WindowW = 320;
        PrintY = 190;

        // IOANCH 20130301: unification culling

        SETFONTCOLOR(14,4);

        #ifdef SPANISH
        US_CPrint ("Oprima una tecla");
        #else
        US_CPrint ("Press a key");
        #endif



        VH_UpdateScreen();

        if (!param_nowait)
            IN_Ack ();

        // IOANCH 20130301: unification culling

        VW_Bar (0,189,300,11,VL_GetPixel(0,0));

        PrintY = 190;
        SETFONTCOLOR(10,4);

        #ifdef SPANISH
        US_CPrint ("pensando...");
        #else
        US_CPrint ("Working...");
        #endif

        VH_UpdateScreen();


        SETFONTCOLOR(0,15);
    }
    else
    {
        VH_UpdateScreen();

        if (!param_nowait)
            VW_WaitVBL(3*70);
    }

}

//===========================================================================

/*
=====================
=
= InitDigiMap
=
=====================
*/

// channel mapping:
//  -1: any non reserved channel
//   0: player weapons
//   1: boss weapons

// IOANCH 20130301: SPEAR() unification
static int wolfdigimap_wl6[] =
{
	// These first sounds are in the upload version

	HALTSND_wl6,                0,  -1,
	DOGBARKSND_wl6,             1,  -1,
	CLOSEDOORSND_wl6,           2,  -1,
	OPENDOORSND_wl6,            3,  -1,
	ATKMACHINEGUNSND_wl6,       4,   0,
	ATKPISTOLSND_wl6,           5,   0,
	ATKGATLINGSND_wl6,          6,   0,
	SCHUTZADSND_wl6,            7,  -1,
	GUTENTAGSND_wl6,            8,  -1,
	MUTTISND_wl6,               9,  -1,
	BOSSFIRESND_wl6,            10,  1,
	SSFIRESND_wl6,              11, -1,
	DEATHSCREAM1SND_wl6,        12, -1,
	DEATHSCREAM2SND_wl6,        13, -1,
	DEATHSCREAM3SND_wl6,        13, -1,
	TAKEDAMAGESND_wl6,          14, -1,
	PUSHWALLSND_wl6,            15, -1,
	
	LEBENSND_wl6,               20, -1,
	NAZIFIRESND_wl6,            21, -1,
	SLURPIESND_wl6,             22, -1,
	
	YEAHSND_wl6,                32, -1,
	// IOANCH 20130301: unification culling

	// These are in all other episodes
	DOGDEATHSND_wl6,            16, -1,
	AHHHGSND_wl6,               17, -1,
	DIESND_wl6,                 18, -1,
	EVASND_wl6,                 19, -1,
	
	TOT_HUNDSND_wl6,            23, -1,
	MEINGOTTSND_wl6,            24, -1,
	SCHABBSHASND_wl6,           25, -1,
	HITLERHASND_wl6,            26, -1,
	SPIONSND_wl6,               27, -1,
	NEINSOVASSND_wl6,           28, -1,
	DOGATTACKSND_wl6,           29, -1,
	LEVELDONESND_wl6,           30, -1,
	MECHSTEPSND_wl6,            31, -1,
	
	SCHEISTSND_wl6,             33, -1,
	DEATHSCREAM4SND_wl6,        34, -1,         // AIIEEE
	DEATHSCREAM5SND_wl6,        35, -1,         // DEE-DEE
	DONNERSND_wl6,              36, -1,         // EPISODE 4 BOSS DIE
	EINESND_wl6,                37, -1,         // EPISODE 4 BOSS SIGHTING
	ERLAUBENSND_wl6,            38, -1,         // EPISODE 6 BOSS SIGHTING
	DEATHSCREAM6SND_wl6,        39, -1,         // FART
	DEATHSCREAM7SND_wl6,        40, -1,         // GASP
	DEATHSCREAM8SND_wl6,        41, -1,         // GUH-BOY!
	DEATHSCREAM9SND_wl6,        42, -1,         // AH GEEZ!
	KEINSND_wl6,                43, -1,         // EPISODE 5 BOSS SIGHTING
	MEINSND_wl6,                44, -1,         // EPISODE 6 BOSS DIE
	ROSESND_wl6,                45, -1,         // EPISODE 5 BOSS DIE
	


	LASTSOUND_wl6
};

static int wolfdigimap_sod[] =
{
	// These first sounds are in the upload version

	//
	// SPEAR() OF DESTINY DIGISOUNDS
	//
	HALTSND_sod,                0,  -1,
	CLOSEDOORSND_sod,           2,  -1,
	OPENDOORSND_sod,            3,  -1,
	ATKMACHINEGUNSND_sod,       4,   0,
	ATKPISTOLSND_sod,           5,   0,
	ATKGATLINGSND_sod,          6,   0,
	SCHUTZADSND_sod,            7,  -1,
	BOSSFIRESND_sod,            8,   1,
	SSFIRESND_sod,              9,  -1,
	DEATHSCREAM1SND_sod,        10, -1,
	DEATHSCREAM2SND_sod,        11, -1,
	TAKEDAMAGESND_sod,          12, -1,
	PUSHWALLSND_sod,            13, -1,
	AHHHGSND_sod,               15, -1,
	LEBENSND_sod,               16, -1,
	NAZIFIRESND_sod,            17, -1,
	SLURPIESND_sod,             18, -1,
	LEVELDONESND_sod,           22, -1,
	DEATHSCREAM4SND_sod,        23, -1,         // AIIEEE
	DEATHSCREAM3SND_sod,        23, -1,         // DOUBLY-MAPPED!!!
	DEATHSCREAM5SND_sod,        24, -1,         // DEE-DEE
	DEATHSCREAM6SND_sod,        25, -1,         // FART
	DEATHSCREAM7SND_sod,        26, -1,         // GASP
	DEATHSCREAM8SND_sod,        27, -1,         // GUH-BOY!
	DEATHSCREAM9SND_sod,        28, -1,         // AH GEEZ!
	GETGATLINGSND_sod,          38, -1,         // Got Gat replacement
	
// IOANCH 20130301: unification culling
	DOGBARKSND_sod,             1,  -1,
	DOGDEATHSND_sod,            14, -1,
	SPIONSND_sod,               19, -1,
	NEINSOVASSND_sod,           20, -1,
	DOGATTACKSND_sod,           21, -1,
	TRANSSIGHTSND_sod,          29, -1,         // Trans Sight
	TRANSDEATHSND_sod,          30, -1,         // Trans Death
	WILHELMSIGHTSND_sod,        31, -1,         // Wilhelm Sight
	WILHELMDEATHSND_sod,        32, -1,         // Wilhelm Death
	UBERDEATHSND_sod,           33, -1,         // Uber Death
	KNIGHTSIGHTSND_sod,         34, -1,         // Death Knight Sight
	KNIGHTDEATHSND_sod,         35, -1,         // Death Knight Death
	ANGELSIGHTSND_sod,          36, -1,         // Angel Sight
	ANGELDEATHSND_sod,          37, -1,         // Angel Death
	GETSPEARSND_sod,            39, -1,         // Got Spear replacement

	LASTSOUND_sod
};

void InitDigiMap (void)
{
    int *map;
	
	// IOANCH 20130301: unification
	map = IMPALE(wolfdigimap);
	unsigned int lastvalue = IMPALE((unsigned int)LASTSOUND);

    for (; *map != lastvalue; map += 3)
    {
        DigiMap[map[0]] = map[1];
        DigiChannel[map[1]] = map[2];
        SD_PrepareSound(map[1]);
    }
}

// IOANCH 20130303: unification
CP_iteminfo MusicItems_wl6={CTL_X,CTL_Y,6,0,32};
CP_itemtype MusicMenu_wl6[]=
    {
        {1,"Get Them!",0},
        {1,"Searching",0},
        {1,"P.O.W.",0},
        {1,"Suspense",0},
        {1,"War March",0},
        {1,"Around The Corner!",0},

        {1,"Nazi Anthem",0},
        {1,"Lurking...",0},
        {1,"Going After Hitler",0},
        {1,"Pounding Headache",0},
        {1,"Into the Dungeons",0},
        {1,"Ultimate Conquest",0},

        {1,"Kill the S.O.B.",0},
        {1,"The Nazi Rap",0},
        {1,"Twelfth Hour",0},
        {1,"Zero Hour",0},
        {1,"Ultimate Conquest",0},
        {1,"Wolfpack",0}
    };
CP_iteminfo MusicItems_sod={CTL_X,CTL_Y-20,9,0,32};
CP_itemtype MusicMenu_sod[]=
    {
        {1,"Funky Colonel Bill",0},
        {1,"Death To The Nazis",0},
        {1,"Tiptoeing Around",0},
        {1,"Is This THE END?",0},
        {1,"Evil Incarnate",0},
        {1,"Jazzin' Them Nazis",0},
        {1,"Puttin' It To The Enemy",0},
        {1,"The SS Gonna Get You",0},
        {1,"Towering Above",0}
    };


// IOANCH 20130301: unification culling
void DoJukebox(void)
{
    int which,lastsong=-1;
    unsigned start;
    // IOANCH 20130303: unification
    unsigned songs_wl6[]=
    {
        GETTHEM_MUS_wl6,
        SEARCHN_MUS_wl6,
        POW_MUS_wl6,
        SUSPENSE_MUS_wl6,
        WARMARCH_MUS_wl6,
        CORNER_MUS_wl6,

        NAZI_OMI_MUS_wl6,
        PREGNANT_MUS_wl6,
        GOINGAFT_MUS_wl6,
        HEADACHE_MUS_wl6,
        DUNGEON_MUS_wl6,
        ULTIMATE_MUS_wl6,

        INTROCW3_MUS_wl6,
        NAZI_RAP_MUS_wl6,
        TWELFTH_MUS_wl6,
        ZEROHOUR_MUS_wl6,
        ULTIMATE_MUS_wl6,
        PACMAN_MUS_wl6
    };
    unsigned songs_sod[]=
    {
        XFUNKIE_MUS_sod,             // 0
        XDEATH_MUS_sod,              // 2
        XTIPTOE_MUS_sod,             // 4
        XTHEEND_MUS_sod,             // 7
        XEVIL_MUS_sod,               // 17
        XJAZNAZI_MUS_sod,            // 18
        XPUTIT_MUS_sod,              // 21
        XGETYOU_MUS_sod,             // 22
        XTOWER2_MUS_sod              // 23
    };
    unsigned *songs = IMPALE(songs);
    // IOANCH 20130303: unification
    CP_iteminfo &MusicItems = IMPALE(MusicItems);
    CP_itemtype *MusicMenu = IMPALE(MusicMenu);

    IN_ClearKeysDown();
    if (!AdLibPresent && !SoundBlasterPresent)
        return;

    MenuFadeOut();

    // IOANCH 20130301: unification culling
    start = SPEAR() ? 0 : ((SDL_GetTicks()/10)%3)*6;

    CA_CacheGrChunk (gfxvmap[STARTFONT][SPEAR()]+1);
    
    if(SPEAR())
        CacheLump (gfxvmap[BACKDROP_LUMP_START][SPEAR()],gfxvmap[BACKDROP_LUMP_END][SPEAR()]);
    else
        CacheLump (gfxvmap[CONTROLS_LUMP_START][SPEAR()],gfxvmap[CONTROLS_LUMP_END][SPEAR()]);
    CA_LoadAllSounds ();
    // IOANCH 20130302: unification
    fontnumber=1;
    ClearMScreen ();
    VWB_DrawPic(112,184,gfxvmap[C_MOUSELBACKPIC][SPEAR()]);
    DrawStripes (10);
    SETFONTCOLOR (TEXTCOLOR,BKGDCOLOR);

    if(!SPEAR())
        DrawWindow (CTL_X-2,CTL_Y-6,280,13*7,BKGDCOLOR);
    else
        DrawWindow (CTL_X-2,CTL_Y-26,280,13*10,BKGDCOLOR);

    DrawMenu (&MusicItems,&MusicMenu[start]);

    SETFONTCOLOR (READHCOLOR,BKGDCOLOR);
    PrintY=15;
    WindowX = 0;
    WindowY = 320;
    US_CPrint ("Robert's Jukebox");

    SETFONTCOLOR (TEXTCOLOR,BKGDCOLOR);
    VW_UpdateScreen();
    MenuFadeIn();

    do
    {
        which = HandleMenu(&MusicItems,&MusicMenu[start],NULL);
        if (which>=0)
        {
            if (lastsong >= 0)
                MusicMenu[start+lastsong].active = 1;

            StartCPMusic(songs[start + which]);
            MusicMenu[start+which].active = 2;
            DrawMenu (&MusicItems,&MusicMenu[start]);
            VW_UpdateScreen();
            lastsong = which;
        }
    } while(which>=0);

    MenuFadeOut();
    IN_ClearKeysDown();
    // IOANCH 20130303: unification

    if(SPEAR())
        UnCacheLump (gfxvmap[BACKDROP_LUMP_START][SPEAR()],gfxvmap[BACKDROP_LUMP_END][SPEAR()]);
    else
        UnCacheLump (gfxvmap[CONTROLS_LUMP_START][SPEAR()],gfxvmap[CONTROLS_LUMP_END][SPEAR()]);

}


/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

static void InitGame()
{
// IOANCH 20130301: unification culling
    boolean didjukebox=false;

    // initialize SDL
#if defined _WIN32
    putenv("SDL_VIDEODRIVER=directx");
#endif
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    int numJoysticks = SDL_NumJoysticks();
    if(param_joystickindex && (param_joystickindex < -1 || 
                               param_joystickindex >= numJoysticks))
    {
        if(!numJoysticks)
            printf("No joysticks are available to SDL!\n");
        else
            printf("The joystick index must be between -1 and %i!\n", 
                   numJoysticks - 1);
        exit(1);
    }

#if defined(GP2X_940)
    GP2X_MemoryInit();
#endif

    SignonScreen ();

#if defined _WIN32
    if(!fullscreen)
    {
        struct SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);

        if(SDL_GetWMInfo(&wmInfo) != -1)
        {
            HWND hwndSDL = wmInfo.window;
            DWORD style = GetWindowLong(hwndSDL, GWL_STYLE) & ~WS_SYSMENU;
            SetWindowLong(hwndSDL, GWL_STYLE, style);
            SetWindowPos(hwndSDL, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | 
                         SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }
#endif
	VW_UpdateScreen();
	
	// IOANCH 20121218: Load bot data
	BotMan::LoadData();

    VH_Startup ();
    IN_Startup ();
    PM_Startup ();
    SD_Startup ();
    CA_Startup ();
    US_Startup ();

    // TODO: Will any memory checking be needed someday??
#ifdef NOTYET
    // IOANCH 20130303: unification
    if (!SPEAR() && mminfo.mainmem < 235000L || SPEAR() && mminfo.mainmem < 257000L && !MS_CheckParm("debugmode"))
    {
        byte *screen;

        CA_CacheGrChunk (gfxvmap[ERRORSCREEN][SPEAR()]);
        screen = grsegs[gfxvmap[ERRORSCREEN][SPEAR()]];
        ShutdownId();
/*        memcpy((byte *)0xb8000,screen+7+7*160,17*160);
        gotoxy (1,23);*/
        exit(1);
    }
#endif


//
// build some tables
//
    InitDigiMap ();

    ReadConfig ();

    SetupSaveGames();

//
// HOLDING DOWN 'M' KEY?
//
	IN_ProcessEvents();

// IOANCH 20130301: unification culling
    if (Keyboard[sc_M])
    {
        DoJukebox();
        didjukebox=true;
    }
    else


//
// draw intro screen stuff
//
    IntroScreen ();

#ifdef _arch_dreamcast
    //TODO: VMU Selection Screen
#endif

//
// load in and lock down some basic chunks
//

    CA_CacheGrChunk(gfxvmap[STARTFONT][SPEAR()]);
    CA_CacheGrChunk(gfxvmap[STATUSBARPIC][SPEAR()]);

    LoadLatchMem ();
    BuildTables ();          // trig tables
    SetupWalls ();

    NewViewSize (viewsize);

//
// initialize variables
//
    InitRedShifts ();
// IOANCH 20130301: unification culling
    if(!didjukebox)
        FinishSignon();

#ifdef NOTYET
    vdisp = (byte *) (0xa0000+PAGE1START);
    vbuf = (byte *) (0xa0000+PAGE2START);
#endif
}

//===========================================================================

/*
==========================
=
= SetViewSize
=
==========================
*/

boolean SetViewSize (unsigned width, unsigned height)
{
    viewwidth = width&~15;                  // must be divisable by 16
    viewheight = height&~1;                 // must be even
    centerx = viewwidth/2-1;
    shootdelta = viewwidth/10;
    if((unsigned) viewheight == screenHeight)
        viewscreenx = viewscreeny = screenofs = 0;
    else
    {
        viewscreenx = (screenWidth-viewwidth) / 2;
        viewscreeny = (screenHeight-scaleFactor*STATUSLINES-viewheight)/2;
        screenofs = viewscreeny*screenWidth+viewscreenx;
    }

//
// calculate trace angles and projection constants
//
    CalcProjection (FOCALLENGTH);

    return true;
}


void ShowViewSize (int width)
{
    int oldwidth,oldheight;

    oldwidth = viewwidth;
    oldheight = viewheight;

    if(width == 21)
    {
        viewwidth = screenWidth;
        viewheight = screenHeight;
        VWB_BarScaledCoord (0, 0, screenWidth, screenHeight, 0);
    }
    else if(width == 20)
    {
        viewwidth = screenWidth;
        viewheight = screenHeight - scaleFactor*STATUSLINES;
        DrawPlayBorder ();
    }
    else
    {
        viewwidth = width*16*screenWidth/320;
        viewheight = (int) (width*16*HEIGHTRATIO*screenHeight/200);
        DrawPlayBorder ();
    }

    viewwidth = oldwidth;
    viewheight = oldheight;
}


void NewViewSize (int width)
{
    viewsize = width;
    if(viewsize == 21)
        SetViewSize(screenWidth, screenHeight);
    else if(viewsize == 20)
        SetViewSize(screenWidth, screenHeight - scaleFactor * STATUSLINES);
    else
        SetViewSize(width*16*screenWidth/320, (unsigned) (width*16*HEIGHTRATIO*screenHeight/200));
}



//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (const char *errorStr, ...)
{
#ifdef NOTYET
    byte *screen;
#endif
    char error[256];
    if(errorStr != NULL)
    {
        va_list vlist;
        va_start(vlist, errorStr);
        vsprintf(error, errorStr, vlist);
        va_end(vlist);
    }
    else error[0] = 0;

    if (!pictable)  // don't try to display the red box before it's loaded
    {
        ShutdownId();
        if (error && *error)
        {
#ifdef NOTYET
            SetTextCursor(0,0);
#endif
            puts(error);
#ifdef NOTYET
            SetTextCursor(0,2);
#endif
            VW_WaitVBL(100);
        }
        exit(1);
    }

    if (!error || !*error)
    {
#ifdef NOTYET
        // IOANCH 20130301: unification culling

        CA_CacheGrChunk (gfxvmap[ORDERSCREEN][SPEAR()]);
        screen = grsegs[gfxvmap[ORDERSCREEN][SPEAR()]];

#endif
        WriteConfig ();
    }
#ifdef NOTYET
    else
    {
        CA_CacheGrChunk (gfxvmap[ERRORSCREEN][SPEAR()]);
        screen = grsegs[gfxvmap[ERRORSCREEN][SPEAR()]];
    }
#endif

    ShutdownId ();

    if (error && *error)
    {
#ifdef NOTYET
        memcpy((byte *)0xb8000,screen+7,7*160);
        SetTextCursor(9,3);
#endif
        puts(error);
#ifdef NOTYET
        SetTextCursor(0,7);
#endif
        VW_WaitVBL(200);
        exit(1);
    }
    else
    if (!error || !(*error))
    {
#ifdef NOTYET
        // IOANCH 20130301: unification culling

        memcpy((byte *)0xb8000,screen+7,24*160); // 24 for SPEAR()/UPLOAD compatibility

        SetTextCursor(0,23);
#endif
    }

    exit(0);
}

//===========================================================================



/*
=====================
=
= DemoLoop
=
=====================
*/


static void DemoLoop()
{
    int LastDemo = 0;

//
// check for launch from ted
//
    if (param_tedlevel != -1)
    {
        param_nowait = true;
        EnableEndGameMenuItem();
        NewGame(param_difficulty,0);

        // IOANCH 20130303: unification
        if(!SPEAR())
        {
            gamestate.episode = param_tedlevel/10;
            gamestate.mapon = param_tedlevel%10;
        }
        else
        {
            gamestate.episode = 0;
            gamestate.mapon = param_tedlevel;
        }
        GameLoop();
        Quit (NULL);
    }


//
// main game cycle
//

#ifndef DEMOTEST
// IOANCH 20130301: unification culling


    StartCPMusic(INTROSONG);
// IOANCH 20130301: unification culling

    if (!param_nowait)
        PG13 ();


#endif

    while (1)
    {
        while (!param_nowait)
        {
//
// title page
//
#ifndef DEMOTEST

            // IOANCH 20130303: unification
            if(SPEAR())
            {
                SDL_Color pal[256];
                CA_CacheGrChunk (gfxvmap[TITLEPALETTE][SPEAR()]);
                VL_ConvertPalette(grsegs[gfxvmap[TITLEPALETTE][SPEAR()]], pal, 256);

                CA_CacheGrChunk (gfxvmap[TITLE1PIC][SPEAR()]);
                VWB_DrawPic (0,0,gfxvmap[TITLE1PIC][SPEAR()]);
                UNCACHEGRCHUNK (gfxvmap[TITLE1PIC][SPEAR()]);

                CA_CacheGrChunk (gfxvmap[TITLE2PIC][SPEAR()]);
                VWB_DrawPic (0,80,gfxvmap[TITLE2PIC][SPEAR()]);
                UNCACHEGRCHUNK (gfxvmap[TITLE2PIC][SPEAR()]);
                VW_UpdateScreen ();
                VL_FadeIn(0,255,pal,30);

                UNCACHEGRCHUNK (gfxvmap[TITLEPALETTE][SPEAR()]);
            }
            else
            {
                CA_CacheScreen (gfxvmap[TITLEPIC][SPEAR()]);
                VW_UpdateScreen ();
                VW_FadeIn();
            }
            if (IN_UserInput(TickBase*15))
                break;
            VW_FadeOut();
//
// credits page
//
            CA_CacheScreen (gfxvmap[CREDITSPIC][SPEAR()]);
            VW_UpdateScreen();
            VW_FadeIn ();
            if (IN_UserInput(TickBase*10))
                break;
            VW_FadeOut ();
//
// high scores
//
            DrawHighScores ();
            VW_UpdateScreen ();
            VW_FadeIn ();

            if (IN_UserInput(TickBase*10))
                break;
#endif
//
// demo
//

// IOANCH 20130301: unification culling
            PlayDemo (LastDemo++%4);




            if (playstate == ex_abort)
                break;
            VW_FadeOut();
            if(screenHeight % 200 != 0)
                VL_ClearScreen(0);
            StartCPMusic(INTROSONG);
        }

        VW_FadeOut ();

#ifdef DEBUGKEYS
        if (Keyboard[sc_Tab] && param_debugmode)
            RecordDemo ();
        else
            US_ControlPanel (0);
#else
        US_ControlPanel (0);
#endif

        if (startgame || loadedgame)
        {
            GameLoop ();
            if(!param_nowait)
            {
                VW_FadeOut();
                StartCPMusic(INTROSONG);
            }
        }
    }
}


//===========================================================================

// IOANCH 20130509: arguments are now case insensitive
#define IFARG(str) if(!strcasecmp(arg, (str)))

//
// CheckParameters
//
// Do it early.
//
void CheckParameters(int argc, char *argv[])
{
    bool sampleRateGiven = false, audioBufferGiven = false;
    int defaultSampleRate = param_samplerate;

    BotMan::active = true;	// IOANCH 26.05.2012: initialize with true, not false
    try
    {
        for(int i = 1; i < argc; i++)
        {
            char *arg = argv[i];
            // IOANCH 20130303: unification
            if(!strcasecmp(arg, "--debugmode") || !strcasecmp(arg, "--goobers"))
                param_debugmode = true;
            else IFARG("--baby")
                param_difficulty = 0;
            else IFARG("--easy")
                param_difficulty = 1;
            else IFARG("--normal")
                param_difficulty = 2;
            else IFARG("--hard")
                param_difficulty = 3;
            else IFARG("--nowait")
                param_nowait = true;
            else IFARG("--tedlevel")
            {
                if(++i >= argc)
                    throw Exception("The tedlevel option is missing the level argument!\n");
                else 
                    param_tedlevel = atoi(argv[i]);
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
                    param_joystickindex = atoi(argv[i]);   
                // index is checked in InitGame
            }
            else IFARG("--joystickhat")
            {
                if(++i >= argc)
                    throw Exception("The joystickhat option is missing the index argument!\n");
                else 
                    param_joystickhat = atoi(argv[i]);
            }
            else IFARG("--samplerate")
            {
                if(++i >= argc)
                    throw Exception("The samplerate option is missing the rate argument!\n");
                else 
                    param_samplerate = atoi(argv[i]);
                sampleRateGiven = true;
            }
            else IFARG("--audiobuffer")
            {
                if(++i >= argc)
                    throw Exception("The audiobuffer option is missing the size argument!\n");
                else 
                    param_audiobuffer = atoi(argv[i]);
                audioBufferGiven = true;
            }
            else IFARG("--mission")
            {
                if(++i >= argc)
                    throw Exception("The mission option is missing the mission argument!\n");
                else
                {
                    param_mission = atoi(argv[i]);
                    if(param_mission < 0 || param_mission > 3)
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
                        configdir = trans;
                        if(trans[len] != '/' && trans[len] != '\\')
                            configdir += '/';
                        free(trans);
                    }
                    else
                        throw Exception("The config directory couldn't be set!\n");
                }
            }
            else IFARG("--goodtimes")
                param_goodtimes = true;
            else IFARG("--ignorenumchunks")
                param_ignorenumchunks = true;
            else IFARG("--help")
                throw Exception();
                // IOANCH 17.05.2012: added --nobot parameter
            else IFARG("--nobot")
                BotMan::active = false;
                // IOANCH 17.05.2012: added --nonazis
            else IFARG("--nonazis")
                Basic::nonazis = true;
                // IOANCH 29.09.2012: added --secretstep3
            else IFARG("--secretstep3")
                Basic::secretstep3 = true;
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
#else
               "                        (default: $HOME/.autowolf)\n"	// IOANCH 20130116: changed name
#endif
               // IOANCH 20130301: unification culling
               , defaultSampleRate);
        printf(
               " --mission <mission>    Mission number to play (0-3)\n"
               "                        (default: 0 -> .SOD, 1-3 -> .SD*)\n"
               " --goodtimes            Disable copy protection quiz\n"
               );
        exit(1);
    }
    
    if(sampleRateGiven && !audioBufferGiven)
        param_audiobuffer = 2048 / (44100 / param_samplerate);
}

/*
==========================
=
= main
=
==========================
*/

//
// main
//
// Main program start
//
int main (int argc, char *argv[])
{
#if defined(_arch_dreamcast)
    DC_Init();
#else
    CheckParameters(argc, argv);
#endif
    // IOANCH: unification: set the SPEAR() global var
    SPEAR.Initialize("");
    
    CheckForEpisodes();

    InitGame();

    DemoLoop();

    Quit("Demo loop exited???");
    return 1;
}
