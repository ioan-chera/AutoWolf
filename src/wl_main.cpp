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
////////////////////////////////////////////////////////////////////////////////
//
// Game startup and initialization
//
////////////////////////////////////////////////////////////////////////////////

// WL_MAIN.C

#include <mutex>
#include <sstream>
#include <system_error>
#include "wl_def.h"
#include "FileSystem.h"
#include "Logger.h"
#include "CommandLine.h"
#include "foreign.h"
#include "StdStringExtensions.h"
#include "version.h"
#include "wl_act1.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_inter.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_play.h"
#include "Exception.h"
#include "ShellUnicode.h"

// IOANCH 17.05.2012
#include "Config.h"
#include "i_system.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_pm.h"
#include "id_sd.h"
#include "id_vh.h"
#include "ioan_bas.h"
#include "ioan_bot.h"
#include "List.h"
#include "MasterDirectoryFile.h"
#ifdef __APPLE__
#include "macosx/CocoaFun.h"
#endif
#include "wl_atmos.h"

////////////////////////////////////////////////////////////////////////////////
//
// WOLFENSTEIN 3-D
// 
// An Id Software production
// 
// by John Carmack
//
////////////////////////////////////////////////////////////////////////////////

// IOANCH 20130303: unification
extern byte signon_wl6[];
extern byte signon_sod[];

////////////////////////////////////////////////////////////////////////////////
//
// LOCAL CONSTANTS
// 
////////////////////////////////////////////////////////////////////////////////

#define FOCALLENGTH     (0x5700l)               // in global coordinates
#define VIEWGLOBAL      0x10000                 // globals visable flush to wall

#define VIEWWIDTH       256                     // size of view window
#define VIEWHEIGHT      144

////////////////////////////////////////////////////////////////////////////////
//
// GLOBAL VARIABLES
// 
////////////////////////////////////////////////////////////////////////////////

char    str[80];
int     dirangle[9] = {0,ANGLES/8,2*ANGLES/8,3*ANGLES/8,4*ANGLES/8,
                       5*ANGLES/8,6*ANGLES/8,7*ANGLES/8,ANGLES};

static const char g_instanceSaveName[] = "instance.awo";
static const char g_fullInstanceSaveTag[] = "full";
static const char g_partialInstanceSaveTag[] = "part";

bool g_appForcedToQuit;

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


Boolean8 startgame;
Boolean8 loadedgame;
int32_t     mouseadjustment;  // IOANCH: int32_t for safely saving

std::string cfg_configname("CONFIG.");

////////////////////////////////////////////////////////////////////////////////
//
// LOCAL VARIABLES
//

//
// main_configHardwareValues
//
// Sets the default values
//
static void main_configHardwareValues(SDMode &sd, SMMode  &sm, SDSMode &sds)
{
   if (sd_soundBlasterPresent || sd_adLibPresent)
   {
      sd = sdm_AdLib;
      sm = smm_AdLib;
   }
   else
   {
      sd = sdm_PC;
      sm = smm_Off;
   }
   
   if (sd_soundBlasterPresent)
      sds = sds_SoundBlaster;
   else
      sds = sds_Off;
   
   if (myInput.mousePresent())
      mouseenabled = true;
   
   if (myInput.joyPresent())
      joystickenabled = true;
   
   viewsize = 19;                          // start with a good size
   mouseadjustment=5;

}

//
// main_continueReadOldConfig
//
// IOANCH: Reads the old config, after the 0xfefa signature. Fixed it so it's
// surely type size correct
//
static void main_continueReadOldConfig(FILE *file, SDMode &o_sd, SMMode  &o_sm,
                                       SDSMode &o_sds)
{
//   SDMode  sd;
//   SMMode  sm;
//   SDSMode sds;
   int32_t  sd;
   int32_t  sm;
   int32_t sds;

   for (int i = 0; i < MaxScores; ++i)
   {
      fread(Scores[i].name, sizeof(Scores[i].name), 1, file);
      fread(&Scores[i].score, sizeof(Scores[i].score), 1, file);
      fread(&Scores[i].completed, sizeof(Scores[i].completed), 1, file);
      fread(&Scores[i].episode, sizeof(Scores[i].episode), 1, file);
   }
   
   fread(&sd,sizeof(sd), 1, file);
   fread(&sm,sizeof(sm), 1, file);
   fread(&sds,sizeof(sds), 1, file);
   
   fread(&mouseenabled,sizeof(mouseenabled), 1, file);
   fread(&joystickenabled,sizeof(joystickenabled), 1, file);
   Boolean8 dummyJoypadEnabled;
   fread(&dummyJoypadEnabled,sizeof(dummyJoypadEnabled), 1, file);
   Boolean8 dummyJoystickProgressive;
   fread(&dummyJoystickProgressive,sizeof(dummyJoystickProgressive), 1, file);
   int32_t dummyJoystickPort = 0;
   fread(&dummyJoystickPort,sizeof(dummyJoystickPort), 1, file);
   
   fread(dirscan,sizeof(dirscan), 1, file);
   fread(buttonscan,sizeof(buttonscan), 1, file);
   fread(buttonmouse,sizeof(buttonmouse), 1, file);
   fread(buttonjoy,sizeof(buttonjoy), 1, file);
   
   fread(&viewsize,sizeof(viewsize), 1, file);
   fread(&mouseadjustment,sizeof(mouseadjustment), 1, file);
   
   fclose(file);
   
   
   o_sm = (SMMode)sm;
   o_sd = (SDMode)sd;
   o_sds = (SDSMode)sds;
}

//
// main_ReadConfig
//
// Reads the settings file
// IOANCH 20130731: modified to use fopen/fread and MasterDirectoryFile
//
void main_ReadConfig()
{
   SDMode  sd;
   SMMode  sm;
   SDSMode sds;

   std::string configpath;

#ifdef _arch_dreamcast
   DC_LoadFromVMU(cfg_configname.buffer());
#endif

   configpath = cfg_dir;
   ConcatSubpath(configpath, cfg_configname);

   main_configHardwareValues(sd, sm, sds);
	FILE* file = ShellUnicode::fopen(configpath.c_str(), "rb");
	
   if (file)
   {
      //
      // valid config file
      //
      // IOANCH: look first for old cfg signature
      //
      word tmp;
      fread(&tmp, sizeof(tmp), 1, file);
      if(tmp == 0xfefa)
      {
         // File is old config
         main_continueReadOldConfig(file, sd, sm, sds);
      }
      else
      {
         fclose(file);
         // File is probably a master directory
         MasterDirectoryFile cfgFile;
         cfgFile.initialize(cfg_configname);
         if(cfgFile.loadFromFile(configpath))
         {
            auto pfile = dynamic_cast<PropertyFile *>(cfgFile.getFileWithName("Config", PROPERTY_FILE_HEADER));
            if(pfile)
            {
             
               // Now read it. Do it like Wolf4SDL
               PString buf;
#define GET_STR_PROPERTY(name, sz) \
if(pfile->hasProperty(#name, Property::PStringVal)) \
{ \
   buf = pfile->getStringValue(#name); \
   if(buf.length() == (sz)) \
      memcpy(name, buf(), (sz)); \
}

#define GET_INT_PROPERTY(name, spectype) \
if(pfile->hasProperty(#name, Property::Int32)) \
   (name) = static_cast<spectype>(pfile->getIntValue(#name));

               GET_STR_PROPERTY(Scores, sizeof(HighScore) * MaxScores)
               
               GET_INT_PROPERTY(sd, SDMode)
               GET_INT_PROPERTY(sm, SMMode)
               GET_INT_PROPERTY(sds, SDSMode)
               
               GET_INT_PROPERTY(mouseenabled, Boolean8)
               GET_INT_PROPERTY(joystickenabled, Boolean8)
               
               GET_STR_PROPERTY(dirscan, sizeof(dirscan))
               GET_STR_PROPERTY(buttonscan, sizeof(buttonscan))
               GET_STR_PROPERTY(buttonmouse, sizeof(buttonmouse))
               GET_STR_PROPERTY(buttonjoy, sizeof(buttonjoy))
               
               GET_INT_PROPERTY(viewsize, int)
               GET_INT_PROPERTY(mouseadjustment, int32_t)
            
            }
#undef GET_STR_PROPERTY
#undef GET_INT_PROPERTY
         }
      }
   }
   
   if ((sd == sdm_AdLib || sm == smm_AdLib) && !sd_adLibPresent
       && !sd_soundBlasterPresent)
   {
      sd = sdm_PC;
      sm = smm_Off;
   }
   
   if ((sds == sds_SoundBlaster && !sd_soundBlasterPresent))
      sds = sds_Off;
   
   // make sure values are correct
   
   if(mouseenabled) mouseenabled=true;
   if(joystickenabled) joystickenabled=true;
   
   if (!myInput.mousePresent())
      mouseenabled = false;
   if (!myInput.joyPresent())
      joystickenabled = false;
   
   if(mouseadjustment<0) mouseadjustment=0;
   else if(mouseadjustment>9) mouseadjustment=9;
   
   if(viewsize<4) viewsize=4;
   else if(viewsize>21) viewsize=21;
   
   MainMenu[6].active=1;
   MainItems.curpos=0;

   SD_SetMusicMode (sm);
   SD_SetSoundMode (sd);
   SD_SetDigiDevice (sds);
}

/*
====================
=
= main_WriteConfig
=
====================
*/
// IOANCH: same changes as with write
void main_WriteConfig()
{
    std::string configpath;
   SDMode sd = sd_soundMode;
   SMMode sm = sd_musicMode;
   SDSMode sds = sd_digiMode;

#ifdef _arch_dreamcast
    fs_unlink(cfg_configname.buffer());
#endif

	configpath = cfg_dir;
	ConcatSubpath(configpath, cfg_configname);
   
   MasterDirectoryFile cfgFile;
   cfgFile.initialize(cfg_configname);
	
   PropertyFile *pFile = new PropertyFile;
   pFile->initialize("Config");
	
#define PUT_STR_PROPERTY(name, sz) \
pFile->setStringValue(#name, PString(reinterpret_cast<const char *>(name), (sz)));
#define PUT_INT_PROPERTY(name) \
pFile->setIntValue(#name, static_cast<int>(name));
   PUT_STR_PROPERTY(Scores, sizeof(HighScore) * MaxScores)
   
   PUT_INT_PROPERTY(sd)
   PUT_INT_PROPERTY(sm)
   PUT_INT_PROPERTY(sds)
   
   PUT_INT_PROPERTY(mouseenabled)
   PUT_INT_PROPERTY(joystickenabled)
   PUT_STR_PROPERTY(dirscan, sizeof(dirscan))
   PUT_STR_PROPERTY(buttonscan, sizeof(buttonscan))
   PUT_STR_PROPERTY(buttonmouse, sizeof(buttonmouse))
   PUT_STR_PROPERTY(buttonjoy, sizeof(buttonjoy))
   
   PUT_INT_PROPERTY(viewsize)
   PUT_INT_PROPERTY(mouseadjustment)
   
   cfgFile.addFile(pFile);
   cfgFile.saveToFile(configpath);
   
#undef PUT_INT_PROPERTY
#undef PUT_STR_PROPERTY
   
#ifdef _arch_dreamcast
    DC_SaveToVMU(cfg_configname.buffer(), NULL);
#endif
}


//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// NewGame
//
// Set up new game to start from the beginning
//
////////////////////////////////////////////////////////////////////////////////

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
    VWB_DrawPic(x,y,SPEAR::g(C_DISKLOADING1PIC)+which);
    I_UpdateScreen();
    which^=1;
}


int32_t DoChecksum(byte *source,unsigned size,int32_t checksum)
{
    unsigned i;

    for (i=0;i<size-1;i++)
    checksum += source[i]^source[i+1];

    return checksum;
}


////////////////////////////////////////////////////////////////////////////////
//
//  SaveTheGame
//
////////////////////////////////////////////////////////////////////////////////

extern statetype s_grdstand;
extern statetype s_player;

Boolean8 SaveTheGame(FILE *file,int x,int y)
{
//    struct diskfree_t dfree;
//    int32_t avail,size,checksum;
    int checksum;
    objtype *ob;
    objtype nullobj;
    statobj_t nullstat;

/*    if (_dos_getdiskfree(0,&dfree))
        throw Exception("Error in _dos_getdiskfree call");

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

////////////////////////////////////////////////////////////////////////////////
//
//  LoadTheGame
//
////////////////////////////////////////////////////////////////////////////////

Boolean8 LoadTheGame(FILE *file,int x,int y)
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
        map = mapSegs[0]; obj = mapSegs[1];
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

        myInput.clearKeysDown();
        myInput.ack();

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
				//	Basic::thrownProjectiles.add(obj);
				}
				
			default:
				;
		}
	}

    return true;
}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
//  ShutdownId
//
//  Shuts down all ID_?? managers
//
////////////////////////////////////////////////////////////////////////////////

static void ShutdownId ()
{
    US_Shutdown ();         // This line is completely useless...
    SD_Shutdown ();
    vSwapData.clear();
    myInput.close();
    VL_Shutdown ();
    CA_Shutdown ();
#if defined(GP2X_940)
    GP2X_Shutdown();
#endif
}


//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
//  main_BuildTables
//
//  Calculates:
//
//  scale                 projection constant
//  sintable/costable     overlapping fractional tables
//
////////////////////////////////////////////////////////////////////////////////

static const float radtoint = (float)(FINEANGLES/2/PI);

static void main_BuildTables ()
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

////////////////////////////////////////////////////////////////////////////////
//
// CalcProjection
//
// Uses focallength
//
////////////////////////////////////////////////////////////////////////////////

static void CalcProjection (int32_t focal)
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
        vid_pixelangle[halfview-1-i] = intang;
        vid_pixelangle[halfview+i] = -intang;
    }
}



//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// main_SetupWalls
//
// Map tile values to scaled pics
//
////////////////////////////////////////////////////////////////////////////////

static void main_SetupWalls ()
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

////////////////////////////////////////////////////////////////////////////////
//
// SignonScreen
//
////////////////////////////////////////////////////////////////////////////////

// IOANCH 20130726: gave prefix
static void VL_SignonScreen ()                        // VGA version
{
    // IOANCH 20130510: moved the VGA function upstream right above this one

    VL_MungePic (IMPALE(signon),320,200);
    VL_MemToScreen (IMPALE(signon),320,200,0,0);
}


////////////////////////////////////////////////////////////////////////////////
//
// FinishSignon
//
////////////////////////////////////////////////////////////////////////////////

static void FinishSignon ()
{
    // IOANCH 20130303: unification
    if(!SPEAR::flag)
    {
        VL_Bar (0,189,300,11,VL_GetPixel(0,0));
        WindowX = 0;
        WindowW = LOGIC_WIDTH;
        PrintY = 190;

        // IOANCH 20130301: unification culling

        SETFONTCOLOR(14,4);

        #ifdef SPANISH
        US_CPrint ("Oprima una tecla");
        #else
        US_CPrint ("Press a key");
        #endif



        I_UpdateScreen();

#ifndef TOUCHSCREEN
        if (!cfg_nowait)
            myInput.ack ();
#endif

        // IOANCH 20130301: unification culling

        VL_Bar (0,189,300,11,VL_GetPixel(0,0));

        PrintY = 190;
        SETFONTCOLOR(10,4);

        #ifdef SPANISH
        US_CPrint ("pensando...");
        #else
        US_CPrint ("Working...");
        #endif

        I_UpdateScreen();


        SETFONTCOLOR(0,15);
    }
    else
    {
        I_UpdateScreen();
#ifndef TOUCHSCREEN
        if (!cfg_nowait)
            VL_WaitVBL(3*70);
#endif
    }

}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// main_InitDigiMap
//
////////////////////////////////////////////////////////////////////////////////

// channel mapping:
//  -1: any non reserved channel
//   0: player weapons
//   1: boss weapons

// IOANCH 20130301: SPEAR::flag unification
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
	// SPEAR::flag OF DESTINY DIGISOUNDS
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

void main_InitDigiMap ()
{
    int *map;
	
	// IOANCH 20130301: unification
	map = IMPALE(wolfdigimap);
	unsigned int lastvalue = IMPALE((unsigned int)LASTSOUND);

    for (; *map != (int)lastvalue; map += 3)
    {
        sd_digiMap[map[0]] = map[1];
        sd_digiChannel[map[1]] = map[2];
        SD_PrepareSound(map[1]);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// InitGame
//
// Load a few things right away
//
////////////////////////////////////////////////////////////////////////////////

static void IntroScreen ();

static void InitGame()
{
// IOANCH 20130301: unification culling
    bool didjukebox=false;
   
   // IOANCH 20130726: moved SDL init away
   I_InitEngine();
   
   // IOANCH: moved joystick check here, out of I_InitEngine
   IN_InitVerifyJoysticks();
   
   // IOANCH: set scale factor and pixelangle/wallheights values/arrays
   VL_SetViewportValues();
   
    // IOANCH 20130510: moved the function here
    VL_SignonScreen ();
   
   I_InitAfterSignon();

	I_UpdateScreen();
	
	// IOANCH 20121218: Load bot data
    masterDir.loadFromFile(masterDirectoryFilePath);
    bot.SetMood();

    VH_Startup ();  // sets some pseudorandom numbers
    myInput.initialize();  // sets up the input devices

	
	vSwapData.loadFile(FileSystem::FindCaseInsensitive(cfg_wolfdir, std::string("VSWAP.") + cfg_extension).c_str());

    SD_Startup ();  // Sound engine initialization (e.g. SDL_mixer)
    CA_Startup ();  // The rest of the data.
    US_Startup ();  // Miscellaneous (like random numbers)


//
// build some tables
//
    main_InitDigiMap ();

    main_ReadConfig ();

    menu_SetupSaveGames();

//
// HOLDING DOWN 'M' KEY?
//
	myInput.processEvents();

// IOANCH 20130301: unification culling
    if (myInput.keyboard(sc_M))
    {
        Menu::DoJukebox();
        didjukebox=true;
    }
    else
	{

//
// draw intro screen stuff
//
		IntroScreen ();
	}

#ifdef _arch_dreamcast
    //TODO: VMU Selection Screen
#endif

//
// load in and lock down some basic chunks
//

    graphSegs.cacheChunk(SPEAR::g(STARTFONT));
    graphSegs.cacheChunk(SPEAR::g(STATUSBARPIC));

    I_LoadLatchMem ();
    main_BuildTables ();          // trig tables
    main_SetupWalls ();

    main_NewViewSize (viewsize);

//
// initialize variables
//
    InitRedShifts ();
// IOANCH 20130301: unification culling
    if(!didjukebox)
        FinishSignon();

}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// SetViewSize
// Makes some Pizza
//
////////////////////////////////////////////////////////////////////////////////
Boolean8 SetViewSize (unsigned width, unsigned height)
{
    viewwidth = width&~15;                  // must be divisable by 16
    viewheight = height&~1;                 // must be even
    centerx = viewwidth/2-1;
    shootdelta = viewwidth/10;
    if((unsigned) viewheight == cfg_screenHeight)
        viewscreenx = viewscreeny = screenofs = 0;
    else
    {
        viewscreenx = (cfg_screenWidth-viewwidth) / 2;
        viewscreeny = (cfg_screenHeight-vid_scaleFactor*STATUSLINES-viewheight)/2;
        screenofs = viewscreeny*cfg_screenWidth+viewscreenx;
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
        viewwidth = cfg_screenWidth;
        viewheight = cfg_screenHeight;
        VL_BarScaledCoord (0, 0, cfg_screenWidth, cfg_screenHeight, 0);
    }
    else if(width == 20)
    {
        viewwidth = cfg_screenWidth;
        viewheight = cfg_screenHeight - vid_scaleFactor*STATUSLINES;
        DrawPlayBorder ();
    }
    else
    {
        viewwidth = width*16*cfg_screenWidth/LOGIC_WIDTH;
        viewheight = (int) (width*16*HEIGHTRATIO*cfg_screenHeight/LOGIC_HEIGHT);
        DrawPlayBorder ();
    }

    viewwidth = oldwidth;
    viewheight = oldheight;
}


void main_NewViewSize (int width)
{
    viewsize = width;
    if(viewsize == 21)
        SetViewSize(cfg_screenWidth, cfg_screenHeight);
    else if(viewsize == 20)
        SetViewSize(cfg_screenWidth, cfg_screenHeight - vid_scaleFactor * STATUSLINES);
    else
        SetViewSize(width*16*cfg_screenWidth/LOGIC_WIDTH, (unsigned) (width*16*HEIGHTRATIO*cfg_screenHeight/LOGIC_HEIGHT));
}



//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// Quit
//
////////////////////////////////////////////////////////////////////////////////
// IOANCH 20130509: now the error is stored globally, so it can be gathered
#ifndef __ANDROID__
#define EXIT(a) throw ExitThrowable(a)
#else
#define EXIT(a) exit(a)
#endif
void Quit ()
{
    if (!graphSegs.hasPictable())  // don't try to display the red box before it's loaded
    {
        ShutdownId();
		EXIT(1);
    }

    main_WriteConfig ();

    ShutdownId ();

	EXIT(0);
}

////////////////////////////////////////////////////////////////////
//
// HANDLE INTRO SCREEN (SYSTEM CONFIG)
//
////////////////////////////////////////////////////////////////////
static void IntroScreen ()
{
    // IOANCH 20130303: unification
#define MAINCOLOR (SPEAR::flag ? 0x4f : 0x6c)
#define EMSCOLOR (SPEAR::flag ? 0x4f : 0x6c)
#define XMSCOLOR (SPEAR::flag ? 0x4f : 0x6c)
#define FILLCOLOR       14
	
    int i;
	
    //
    // DRAW MAIN MEMORY
    //
	
    for (i = 0; i < 10; i++)
        VL_Bar (49, 163 - 8 * i, 6, 5, MAINCOLOR - i);
    for (i = 0; i < 10; i++)
        VL_Bar (89, 163 - 8 * i, 6, 5, EMSCOLOR - i);
    for (i = 0; i < 10; i++)
        VL_Bar (129, 163 - 8 * i, 6, 5, XMSCOLOR - i);
	
    //
    // FILL BOXES
    //
    if (myInput.mousePresent())
        VL_Bar (164, 82, 12, 2, FILLCOLOR);
	
    if (myInput.joyPresent())
        VL_Bar (164, 105, 12, 2, FILLCOLOR);
	
    if (sd_adLibPresent && !sd_soundBlasterPresent)
        VL_Bar (164, 128, 12, 2, FILLCOLOR);
	
    if (sd_soundBlasterPresent)
        VL_Bar (164, 151, 12, 2, FILLCOLOR);
	
}

/*
 ==================
 =
 = PG13
 =
 ==================
 */

static void PG13 ()
{
    VW_FadeOut ();
    VL_Bar (0, 0, LOGIC_WIDTH, LOGIC_HEIGHT, 0x82);     // background
	
    graphSegs.cacheChunk (SPEAR::g(PG13PIC));
    VWB_DrawPic (216, 110, SPEAR::g(PG13PIC));
    I_UpdateScreen ();
	
    graphSegs.uncacheChunk (SPEAR::g(PG13PIC));
	
    VW_FadeIn ();
    myInput.userInput (TickBase * 7);
	
    VW_FadeOut ();
}

////////////////////////////////////////////////////////////////////////////////
//
// DemoLoop
//
////////////////////////////////////////////////////////////////////////////////

static bool RestoreInstanceState(const char* path);
static void DemoLoop()
{
    int LastDemo = 0;
	bool restoreInstance = false;
	std::string savepath = cfg_dir;
	ConcatSubpath(savepath, g_instanceSaveName);
	
	if(cfg_norestore)
		DestroySavedInstance();
	else
	{
		if (FileSystem::FileExists(savepath.c_str()))
		{
			restoreInstance = true;
		}
	}

//
// check for launch from ted
//
    if (!restoreInstance && cfg_tedlevel != -1)
    {
       cfg_nowait = true;
        EnableEndGameMenuItem();
        NewGame(cfg_difficulty,0);

        // IOANCH 20130303: unification
        if(!SPEAR::flag)
        {
            gamestate.episode = cfg_tedlevel/10;
            gamestate.mapon = cfg_tedlevel%10;
        }
        else
        {
            gamestate.episode = 0;
            gamestate.mapon = cfg_tedlevel;
        }
        GameLoop();
        Quit ();
    }

//
// main game cycle
//

#ifndef DEMOTEST
// IOANCH 20130301: unification culling

	if (!restoreInstance)
		StartCPMusic(INTROSONG);
// IOANCH 20130301: unification culling

    if (!cfg_nowait && !restoreInstance)
        PG13 ();


#endif

    while (1)
    {
        while (!cfg_nowait && !restoreInstance)
        {
//
// title page
//
#ifndef DEMOTEST

            // IOANCH 20130303: unification
            if(SPEAR::flag)
            {
                SDL_Color pal[256];
                graphSegs.cacheChunk (SPEAR::g(TITLEPALETTE));
                VL_ConvertPalette(graphSegs[SPEAR::g(TITLEPALETTE)], pal, 256);

                graphSegs.cacheChunk (SPEAR::g(TITLE1PIC));
                VWB_DrawPic (0,0,SPEAR::g(TITLE1PIC));
                graphSegs.uncacheChunk (SPEAR::g(TITLE1PIC));

                graphSegs.cacheChunk (SPEAR::g(TITLE2PIC));
                VWB_DrawPic (0,80,SPEAR::g(TITLE2PIC));
                graphSegs.uncacheChunk (SPEAR::g(TITLE2PIC));
                I_UpdateScreen ();
                VL_FadeIn(0,255,pal,30);

                graphSegs.uncacheChunk (SPEAR::g(TITLEPALETTE));
            }
            else
            {
                graphSegs.cacheScreen (SPEAR::g(TITLEPIC));
                I_UpdateScreen ();
                VW_FadeIn();
            }
            if (myInput.userInput(TickBase*15))
                break;
            VW_FadeOut();
//
// credits page
//
            graphSegs.cacheScreen (SPEAR::g(CREDITSPIC));
            I_UpdateScreen();
            VW_FadeIn ();
            if (myInput.userInput(TickBase*10))
                break;
            VW_FadeOut ();
//
// high scores
//
            DrawHighScores ();
            I_UpdateScreen ();
            VW_FadeIn ();

            if (myInput.userInput(TickBase*10))
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
            if(cfg_screenHeight % LOGIC_HEIGHT != 0)
                I_ClearScreen(0);
            StartCPMusic(INTROSONG);
        }

        VW_FadeOut ();

		if (!restoreInstance || !RestoreInstanceState(savepath.c_str()))
		{
#ifdef DEBUGKEYS
			if (myInput.keyboard(sc_Tab) && cfg_debugmode)
				RecordDemo ();
			else
				Menu::ControlPanel(sc_None);
#else
			Menu::ControlPanel(0);
#endif
		}
		restoreInstance = false;
        if (startgame || loadedgame)
        {
            GameLoop ();
            if(!cfg_nowait)
            {
                VW_FadeOut();
                StartCPMusic(INTROSONG);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// main
//
////////////////////////////////////////////////////////////////////////////////

std::string stringByReplacing(const char* text, const char* what, const char* towhat)
{
	std::string str(text);
	
	size_t whatlen = strlen(what);
	size_t towhatlen = strlen(towhat);
	
	size_t pos = (size_t)-towhatlen;
	while((pos = str.find(what, pos + towhatlen)) != std::string::npos)
	{
		str.replace(pos, whatlen, towhat);
	}
	
	return str;
}

static void showErrorAlert(const char* message, const char* title)
{
	Logger::Write("%s: %s", title, message);
#ifdef _WIN32
	MessageBoxW(nullptr, UTF8ToWideChar(message).c_str(), UTF8ToWideChar(title).c_str(), MB_OK | MB_ICONERROR);
#elif defined(__APPLE__)
	Cocoa_DisplayErrorAlert(message, title);
#elif defined(__ANDROID__)
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s: %s", title, message);
#else
	std::stringstream ss;
	ss << "zenity --error --title='" << stringByReplacing(title, "'", "\\'") << "' --text='" << 
			stringByReplacing(message, "'", "\\'") << "'";
		
	system(ss.str().c_str());
#endif
}

static bool RestoreInstanceState(const char* path)
{
	Logger::Write("Restoring instance state...");
	FILE * file = ShellUnicode::fopen(path, "rb");
	if (!file)
	{
		Logger::Write("...file not found");
		return false;
	}

	char vam[5];
	vam[4] = 0;
	fread(vam, 1, 4, file);
	bool isPartial = false;
	if (!strcmp(vam, g_fullInstanceSaveTag))
		;
	else if (!strcmp(vam, g_partialInstanceSaveTag))
		isPartial = true;
	else
	{
		Logger::Write("...bad header");
		fclose(file);
		return false;
	}

	loadedgame = true;
	if (!isPartial)
	{
		LoadTheGame(file, 0, 0);
	}
	else
	{
		fread(&gamestate, sizeof(gamestate), 1, file);
		fread(&LevelRatios[0], sizeof(LRstruct)*LRpack, 1, file);
		SetupGameLevel();
	}

	Logger::Write("...success");
	fclose(file);
	return true;
}

void SaveFullInstanceState()
{
	Logger::Write("Saving instance state");
	std::string savepath = cfg_dir;
	ConcatSubpath(savepath, g_instanceSaveName);

	ShellUnicode::unlink(savepath.c_str());
	FILE * file = ShellUnicode::fopen(savepath.c_str(), "wb");
	if (!file)
		return;

	fwrite(g_fullInstanceSaveTag, 1, 4, file);
	SaveTheGame(file, 0, 0);
	fclose(file);
}

// Called on death
void SavePartialInstanceState()
{
	Logger::Write("Saving death state");
	std::string savepath = cfg_dir;
	ConcatSubpath(savepath, g_instanceSaveName);

	ShellUnicode::unlink(savepath.c_str());
	FILE * file = ShellUnicode::fopen(savepath.c_str(), "wb");
	if (!file)
		return;

	fwrite(g_partialInstanceSaveTag, 1, 4, file);
	
	gametype stockgame = gamestate;
	stockgame.score = stockgame.oldscore;
	stockgame.lives = gamestate.lives - 1;
	stockgame.health = I_PLAYERHEALTH;
	stockgame.weapon = stockgame.bestweapon
		= stockgame.chosenweapon = wp_pistol;
	stockgame.ammo = STARTAMMO;
	stockgame.keys = 0;
	stockgame.attackframe = stockgame.attackcount =
		stockgame.weaponframe = 0;

	stockgame.TimeCount
		= stockgame.secrettotal
		= stockgame.killtotal
		= stockgame.treasuretotal = 0;

	fwrite(&stockgame, sizeof(stockgame), 1, file);
	fwrite(&LevelRatios[0], sizeof(LRstruct)*LRpack, 1, file);

	fclose(file);
}

void DestroySavedInstance()
{
	Logger::Write("Destroying saved instance");
	std::string savepath = cfg_dir;
	ConcatSubpath(savepath, g_instanceSaveName);
	ShellUnicode::remove(savepath.c_str());
}

//
// SDL HANDLE MOBILE APP EVENTS QUICKLY
//
#ifndef USE_SDL1_2
static int handleMobileAppEvent(void* userdata, SDL_Event* event)
{
	if (event->type == SDL_APP_WILLENTERBACKGROUND)
	{
		Sound::Stop();
		Sound::MusicOff();
		if (g_inGameLoop)
		{
			if (ingame)
			{
				
				std::lock_guard<std::mutex> lock(g_playloopMutex);
				bot.SaveData();
				SaveFullInstanceState();
			}
			// else: either dead or in elevator at this point. The instance save is handled elsewhere then
		}
		return 0;
	}
	else if (event->type == SDL_APP_TERMINATING)
	{
		Logger::Write("SDL_APP_TERMINATING");
		g_appForcedToQuit = true;
		exit(0);
	}
	else if (event->type == SDL_APP_DIDENTERFOREGROUND)
	{
		Sound::MusicOn();
		return 0;
	}
	return 1;
}
#endif

//
// main
//
// Main program start
//
#if defined(__APPLE__) || defined(__ANDROID__)
#define main SDL_main
#endif
int SDL_main(int argc, TChar *argv[])
{
	//TestListPerf();
	try
	{
#if defined(_arch_dreamcast)
		DC_Init();
#else
		// argc and argv are null in windows, which uses WinMain from sdl_winmain.cpp
		CommandLine::Feed(argc, argv);
		CommandLine::Parse();
#endif
		// IOANCH: unification: set the SPEAR::flag global var
		SPEAR::Initialize(cfg_wolfdir);
		SPEAR::SetGlobalValues();

		// IOANCH 20130509: this changes config values. Call moved here from CFE.
		CFG_SetupConfigLocation();
		CFG_CheckForEpisodes();

		InitGame();
#ifndef USE_SDL1_2
		SDL_SetEventFilter(handleMobileAppEvent, nullptr);
#endif

		DemoLoop();

		throw Exception("Demo loop exited???");
	}
	catch (const ExitThrowable& e)
	{
		return e.code;
	}
	catch (const std::system_error& e)
	{
		ShutdownId();
		
		std::string message(e.what());
		message += "(";
		message += e.code().value();
		message += ": ";
		message += e.code().message();
		message += ")";
		showErrorAlert(message.c_str(), "AutoWolf System Error");
		return 3;
	}
	catch (const std::exception& e)
	{
		ShutdownId();
		
		showErrorAlert(e.what(), "AutoWolf Error");
		return 1;
	}
	catch (...)
	{
		ShutdownId();
		
		showErrorAlert("An unknown error occurred! Please contact the author about this!", "AutoWolf Error");
		return 2;
	}

	return 0;
}
