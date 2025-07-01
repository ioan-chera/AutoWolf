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
// Game maintenance
//
////////////////////////////////////////////////////////////////////////////////

// WL_GAME.C

#include <memory>
#include <mutex>
#include <thread>
#include "wl_def.h"
#include "Logger.h"
#include "foreign.h"
#include "version.h"
#include "wl_act1.h"
#include "wl_act2.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_inter.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_play.h"
// IOANCH 17.05.2012
#include "ioan_bas.h"
#include "ioan_bot.h"
#include "i_video.h"
#include "i_system.h"
#include "id_ca.h"
#include "id_sd.h"
#include "id_vh.h"
#include "ioan_secret.h"
#include "List.h"
#include "Config.h"

#ifdef MYPROFILE
#include <TIME.H>
#endif

/*
=============================================================================

                             GLOBAL VARIABLES

=============================================================================
*/

Boolean8         ingame,fizzlein;
bool			g_inGameLoop;
gametype        gamestate;
byte            bordercol=VIEWCOLOR;        // color of the Change View/Ingame border

// IOANCH 20130302: unification
int32_t         spearx,speary;
unsigned        spearangle;
Boolean8         spearflag;

#ifdef USE_FEATUREFLAGS
int ffDataTopLeft, ffDataTopRight, ffDataBottomLeft, ffDataBottomRight;
#endif

//
// ELEVATOR BACK MAPS - REMEMBER (-1)!!
//
int ElevatorBackTo[]={1,1,7,3,5,3};

void SetupGameLevel ();
void DrawPlayScreen ();
void GameLoop ();

/*
=============================================================================

                             LOCAL VARIABLES

=============================================================================
*/



//===========================================================================
//===========================================================================


/*
==========================
=
= SetSoundLoc - Given the location of an object (in terms of global
=       coordinates, held in globalsoundx and globalsoundy), munges the values
=       for an approximate distance from the left and right ear, and puts
=       those values into leftchannel and rightchannel.
=
= JAB
=
==========================
*/

int leftchannel, rightchannel;
#define ATABLEMAX 15
byte righttable[ATABLEMAX][ATABLEMAX * 2] = {
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 0, 0, 0, 0, 0, 1, 3, 5, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 6, 4, 0, 0, 0, 0, 0, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 4, 1, 0, 0, 0, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 5, 4, 2, 1, 0, 1, 2, 3, 5, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 5, 4, 3, 2, 2, 3, 3, 5, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 4, 4, 4, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 6, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};
byte lefttable[ATABLEMAX][ATABLEMAX * 2] = {
{ 8, 8, 8, 8, 8, 8, 8, 8, 5, 3, 1, 0, 0, 0, 0, 0, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 0, 0, 0, 0, 0, 4, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 1, 0, 0, 0, 1, 4, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 5, 3, 2, 1, 0, 1, 2, 4, 5, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 5, 3, 3, 2, 2, 3, 4, 5, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 4, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 6, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};

void
SetSoundLoc(fixed gx,fixed gy)
{
    fixed   xt,yt;
    int     x,y;

//
// translate point to view centered coordinates
//
    gx -= viewx;
    gy -= viewy;

//
// calculate newx
//
    xt = FixedMul(gx,viewcos);
    yt = FixedMul(gy,viewsin);
    x = (xt - yt) >> TILESHIFT;

//
// calculate newy
//
    xt = FixedMul(gx,viewsin);
    yt = FixedMul(gy,viewcos);
    y = (yt + xt) >> TILESHIFT;

    if (y >= ATABLEMAX)
        y = ATABLEMAX - 1;
    else if (y <= -ATABLEMAX)
        y = -ATABLEMAX;
    if (x < 0)
        x = -x;
    if (x >= ATABLEMAX)
        x = ATABLEMAX - 1;
    leftchannel  =  lefttable[x][y + ATABLEMAX];
    rightchannel = righttable[x][y + ATABLEMAX];

#if 0
    CenterWindow(8,1);
    US_PrintSigned(leftchannel);
    US_Print(",");
    US_PrintSigned(rightchannel);
    I_UpdateScreen();
#endif
}

/*
==========================
=
= SetSoundLocGlobal - Sets up globalsoundx & globalsoundy and then calls
=       UpdateSoundLoc() to transform that into relative channel volumes. Those
=       values are then passed to the Sound Manager so that they'll be used for
=       the next sound played (if possible).
=
= JAB
=
==========================
*/
void PlaySoundLocGlobal(word s,fixed gx,fixed gy)
{
    SetSoundLoc(gx, gy);
    SD_PositionSound(leftchannel, rightchannel);

    int channel = Sound::Play((soundnames) s);
    if(channel)
    {
        channelSoundPos[channel - 1].globalsoundx = gx;
        channelSoundPos[channel - 1].globalsoundy = gy;
        channelSoundPos[channel - 1].valid = 1;
    }
}

//
// PlaySoundLocTile
//
// IOANCH: added PlaySoundLocTile as a function, for added flexibility
// objtype source is optional and means the actor (usually a Nazi) who was
// be responsible for that sound
//
void    PlaySoundLocActor(word s, const objtype *ob,  objtype *source)
{
   // First play the sound
   PlaySoundLocGlobal(s, ob->x, ob->y);
   
   // Then put it in bot's memory
   if(cfg_botActive && source)
   {
      HeardEvent *hevent = new HeardEvent;
	   hevent->set(ob->x, ob->y, SPEAR::sd(s), I_GetTicks(), source, 0);
      bot.heardEvents.add(hevent);
   }
}

void UpdateSoundLoc()
{
/*    if (sd_soundPositioned)
    {
        SetSoundLoc(globalsoundx,globalsoundy);
        SD_SetPosition(leftchannel,rightchannel);
    }*/

    for(int i = 0; i < MIX_CHANNELS; i++)
    {
        if(channelSoundPos[i].valid)
        {
            SetSoundLoc(channelSoundPos[i].globalsoundx,
                channelSoundPos[i].globalsoundy);
            SD_SetPosition(i, leftchannel, rightchannel);
        }
    }
}

/*
**      JAB End
*/

/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

static void ScanInfoPlane()
{
    unsigned x,y;
    int      tile;
    const word     *start;

    start = mapSegs[1];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *start++;
            if (!tile)
                continue;

            switch (tile)
            {
                case PLAYER_START_NORTH:
                case PLAYER_START_EAST:
                case PLAYER_START_SOUTH:
                case PLAYER_START_WEST:
                    SpawnPlayer(x, y, NORTH + tile - PLAYER_START_NORTH);
                    break;

                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:

                case 31:
                case 32:
                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:

                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:

                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:

                case 55:
                case 56:
                case 57:
                case 58:
                case 59:
                case 60:
                case 61:
                case 62:

                case 63:
                case 64:
                case 65:
                case 66:
                case 67:
                case 68:
                case 69:
                case 70:
                case 71:
                case 72:
                    // IOANCH 20130302: unification
                    SpawnStatic(x,y,tile-Act1::STATIC_ID_OFFSET);
                    break;
                case 73:
                case 74:
                    if(SPEAR::flag)
                        SpawnStatic(x,y,tile-Act1::STATIC_ID_OFFSET);
                    break;

//
// P wall
//
                case PUSHABLETILE:
                    if (!loadedgame)
                        gamestate.secrettotal++;
                    break;

//
// guard
//
                case Game::GuardStandHard1:
                case Game::GuardStandHard2:
                case Game::GuardStandHard3:
                case Game::GuardStandHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::GuardStandMedium1:
                case Game::GuardStandMedium2:
                case Game::GuardStandMedium3:
                case Game::GuardStandMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::GuardStandEasy1:
                case Game::GuardStandEasy2:
                case Game::GuardStandEasy3:
                case Game::GuardStandEasy4:	// IOANCH 17.05.2012: use centralized function
						 Basic::SpawnEnemy(guardobj, x, y, tile - Game::GuardStandEasy1);
                    break;


                case Game::GuardPatrolHard1:
                case Game::GuardPatrolHard2:
                case Game::GuardPatrolHard3:
                case Game::GuardPatrolHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::GuardPatrolMedium1:
                case Game::GuardPatrolMedium2:
                case Game::GuardPatrolMedium3:
                case Game::GuardPatrolMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::GuardPatrolEasy1:
                case Game::GuardPatrolEasy2:
                case Game::GuardPatrolEasy3:
                case Game::GuardPatrolEasy4:	// IOANCH 17.05.2012: use centralized function
						 Basic::SpawnEnemy(guardobj, x, y, tile - Game::GuardPatrolEasy1, true);
                    break;

                case 124:
                    SpawnDeadGuard (x,y);
                    break;
//
// officer
//
                case Game::OfficerStandHard1:
                case Game::OfficerStandHard2:
                case Game::OfficerStandHard3:
                case Game::OfficerStandHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::OfficerStandMedium1:
                case Game::OfficerStandMedium2:
                case Game::OfficerStandMedium3:
                case Game::OfficerStandMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::OfficerStandEasy1:
                case Game::OfficerStandEasy2:
                case Game::OfficerStandEasy3:
                case Game::OfficerStandEasy4:	// IOANCH 17.05.2012: use centralized function
						 Basic::SpawnEnemy(officerobj, x, y, tile - Game::OfficerStandEasy1);
                    break;


                case Game::OfficerPatrolHard1:
                case Game::OfficerPatrolHard2:
                case Game::OfficerPatrolHard3:
                case Game::OfficerPatrolHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::OfficerPatrolMedium1:
                case Game::OfficerPatrolMedium2:
                case Game::OfficerPatrolMedium3:
                case Game::OfficerPatrolMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::OfficerPatrolEasy1:
                case Game::OfficerPatrolEasy2:
                case Game::OfficerPatrolEasy3:
                case Game::OfficerPatrolEasy4:	// IOANCH 17.05.2012: use centralized function
						 Basic::SpawnEnemy(officerobj, x, y, tile - Game::OfficerPatrolEasy1, true);
                    break;


//
// ss
//
                case Game::SSStandHard1:
                case Game::SSStandHard2:
                case Game::SSStandHard3:
                case Game::SSStandHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::SSStandMedium1:
                case Game::SSStandMedium2:
                case Game::SSStandMedium3:
                case Game::SSStandMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::SSStandEasy1:
                case Game::SSStandEasy2:
                case Game::SSStandEasy3:
                case Game::SSStandEasy4:	// IOANCH 17.05.2012: use centralized function
						 Basic::SpawnEnemy(ssobj, x, y, tile - Game::SSStandEasy1);
                    break;


                case Game::SSPatrolHard1:
                case Game::SSPatrolHard2:
                case Game::SSPatrolHard3:
                case Game::SSPatrolHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::SSPatrolMedium1:
                case Game::SSPatrolMedium2:
                case Game::SSPatrolMedium3:
                case Game::SSPatrolMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::SSPatrolEasy1:
                case Game::SSPatrolEasy2:
                case Game::SSPatrolEasy3:
                case Game::SSPatrolEasy4:	// IOANCH 17.05.2012: use centralized function
						 Basic::SpawnEnemy(ssobj, x, y, tile - Game::SSPatrolEasy1, true);
                    break;

//
// dogs
//
                case Game::DogStandHard1:
                case Game::DogStandHard2:
                case Game::DogStandHard3:
                case Game::DogStandHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::DogStandMedium1:
                case Game::DogStandMedium2:
                case Game::DogStandMedium3:
                case Game::DogStandMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::DogStandEasy1:
                case Game::DogStandEasy2:
                case Game::DogStandEasy3:
                case Game::DogStandEasy4:	// IOANCH 17.05.2012: use centralized function
						 Basic::SpawnEnemy(dogobj, x, y, tile - Game::DogStandEasy1);
                    break;


                case Game::DogPatrolHard1:
                case Game::DogPatrolHard2:
                case Game::DogPatrolHard3:
                case Game::DogPatrolHard4:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::DogPatrolMedium1:
                case Game::DogPatrolMedium2:
                case Game::DogPatrolMedium3:
                case Game::DogPatrolMedium4:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= Game::NormalSkillOffset;
                case Game::DogPatrolEasy1:
                case Game::DogPatrolEasy2:
                case Game::DogPatrolEasy3:
                case Game::DogPatrolEasy4:	// IOANCH 17.05.2012: use centralized function
					Basic::SpawnEnemy(dogobj, x, y, tile - Game::DogPatrolEasy1, true);
                    break;

//
// boss
//
                    // IOANCH 20130302: unification

                case Game::BossHans:	// IOANCH 17.05.2012: use centralized function
				    if(!SPEAR::flag) Basic::SpawnEnemy(bossobj, x, y);
                    break;
                case Game::BossGretel:
                    if(!SPEAR::flag) Basic::SpawnEnemy(gretelobj, x, y);
                    break;
                case Game::BossGift:
                    if(!SPEAR::flag) Basic::SpawnEnemy(giftobj, x, y);
                    break;
                case Game::BossFat:
                    if(!SPEAR::flag) Basic::SpawnEnemy(fatobj, x, y);
                    break;
                case Game::BossSchabbs:
                    if(!SPEAR::flag) Basic::SpawnEnemy(schabbobj, x, y);
                    break;
                case Game::FakeHitler:
                    if(!SPEAR::flag) Basic::SpawnEnemy(fakeobj, x, y);
                    break;
                case Game::BossHitler:
                    if(!SPEAR::flag) Basic::SpawnEnemy(mechahitlerobj, x, y);
                    break;

                case Game::Spectre:
                    if(SPEAR::flag) Basic::SpawnEnemy(spectreobj, x, y);
                    break;
                case Game::BossAngel:
                    if(SPEAR::flag) Basic::SpawnEnemy(angelobj, x, y);
                    break;
                case Game::BossTrans:
                    if(SPEAR::flag) Basic::SpawnEnemy(transobj, x, y);
                    break;
                case Game::BossUber:
                    if(SPEAR::flag) Basic::SpawnEnemy(uberobj, x, y);
                    break;
                case Game::BossWill:
                    if(SPEAR::flag) Basic::SpawnEnemy(willobj, x, y);
                    break;
                case Game::BossDeath:
                    if(SPEAR::flag) Basic::SpawnEnemy(deathobj, x, y);
                    break;

                    // mutants
                case Game::MutantStandHard1:
                case Game::MutantStandHard2:
                case Game::MutantStandHard3:
                case Game::MutantStandHard4:
                    if(gamestate.difficulty < gd_hard)
                        break;
                    tile -= Game::MutantSkillOffset;
                case Game::MutantStandMedium1:
                case Game::MutantStandMedium2:
                case Game::MutantStandMedium3:
                case Game::MutantStandMedium4:
                    if(gamestate.difficulty < gd_medium)
                        break;
                    tile -= Game::MutantSkillOffset;
                case Game::MutantStandEasy1:
                case Game::MutantStandEasy2:
                case Game::MutantStandEasy3:
                case Game::MutantStandEasy4:
                    Basic::SpawnEnemy(mutantobj, x, y, tile - Game::MutantStandEasy1);
                    break;

                case Game::MutantPatrolHard1:
                case Game::MutantPatrolHard2:
                case Game::MutantPatrolHard3:
                case Game::MutantPatrolHard4:
                    if(gamestate.difficulty < gd_hard)
                        break;
                    tile -= Game::MutantSkillOffset;
                case Game::MutantPatrolMedium1:
                case Game::MutantPatrolMedium2:
                case Game::MutantPatrolMedium3:
                case Game::MutantPatrolMedium4:
                    if(gamestate.difficulty < gd_medium)
                        break;
                    tile -= Game::MutantSkillOffset;
                case Game::MutantPatrolEasy1:
                case Game::MutantPatrolEasy2:
                case Game::MutantPatrolEasy3:
                case Game::MutantPatrolEasy4:
                    Basic::SpawnEnemy(mutantobj, x, y, tile - Game::MutantPatrolEasy1, true);
                    break;

//
// ghosts
//
                    // IOANCH 20130302: unification

                case 224:	// IOANCH 17.05.2012: use centralized function
					if(!SPEAR::flag) Basic::SpawnEnemy(ghostobj, x, y, 0, false, 
                                                 en_blinky);
                    break;
                case 225:	// IOANCH 17.05.2012: use centralized function
					if(!SPEAR::flag) Basic::SpawnEnemy(ghostobj, x, y, 0, false, 
                                                 en_clyde);
                    break;
                case 226:	// IOANCH 17.05.2012: use centralized function
					if(!SPEAR::flag) Basic::SpawnEnemy(ghostobj, x, y, 0, false,
                                                 en_pinky);
                    break;
                case 227:	// IOANCH 17.05.2012: use centralized function
					if(!SPEAR::flag) Basic::SpawnEnemy(ghostobj, x, y, 0, false,
                                                 en_inky);
                    break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = SetupGameLevel
// =
//
////////////////////////////////////////////////////////////////////////////////

void SetupGameLevel ()
{
    int  x,y;
    word *map;
    word tile;

	// Start a new session
	StartNewSession();

    if (!loadedgame)
    {
        gamestate.TimeCount
            = gamestate.secrettotal
            = gamestate.killtotal
            = gamestate.treasuretotal
            = gamestate.secretcount
            = gamestate.killcount
            = gamestate.treasurecount
            = pwallstate = pwallpos = facetimes = 0;
        LastAttacker = NULL;
        killerobj = NULL;
    }

    if (demoplayback || demorecord)
    {
       wolfRnd.initialize(false);
    }
    else
    {
       wolfRnd.initialize(true);
    }

//
// load the level
//
   // IOANCH: use C++ construct
   mapSegs.cacheMap(gamestate.mapon + 10 * gamestate.episode, gamestate.episode);

#ifdef USE_FEATUREFLAGS
    // Temporary definition to make things clearer
    #define MXX MAPSIZE - 1

    // Read feature flags data from map corners and overwrite corners with adjacent tiles
    ffDataTopLeft     = MAPSPOT(0,   0,   0); MAPSPOT(0,   0,   0) = MAPSPOT(1,       0,       0);
    ffDataTopRight    = MAPSPOT(MXX, 0,   0); MAPSPOT(MXX, 0,   0) = MAPSPOT(MXX,     1,       0);
    ffDataBottomRight = MAPSPOT(MXX, MXX, 0); MAPSPOT(MXX, MXX, 0) = MAPSPOT(MXX - 1, MXX,     0);
    ffDataBottomLeft  = MAPSPOT(0,   MXX, 0); MAPSPOT(0,   MXX, 0) = MAPSPOT(0,       MXX - 1, 0);

    #undef MXX
#endif
	
	// IOANCH 17.05.2012: initialize bot
    if(cfg_botActive)
        bot.MapInit();

//
// copy the wall data to a data segment array
//
    memset (tilemap,0,sizeof(tilemap));
    memset (actorat,0,sizeof(actorat));
    map = mapSegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile<AREATILE)
            {
                // solid wall
                tilemap[x][y] = (byte) tile;
                actorat[x][y] = (objtype *)(uintptr_t) tile;
            }
            else
            {
                // area floor
                tilemap[x][y] = 0;
                actorat[x][y] = 0;
            }
        }
    }

//
// spawn doors
//

	// IOANCH: reset lists
	Basic::livingNazis.removeAll();
	Basic::thrownProjectiles.killAll();
	Basic::EmptyItemList();
	
    InitActorList ();                       // start spawning things with a clean slate
    InitDoorList ();
    InitStaticList ();

    map = mapSegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile >= DOOR_VERTICAL_1 && tile <= DOOR_HORIZONTAL_6)
            {
                // door
                switch (tile)
                {
                    case DOOR_VERTICAL_1:
                    case DOOR_VERTICAL_2:
                    case DOOR_VERTICAL_3:
                    case DOOR_VERTICAL_4:
                    case DOOR_VERTICAL_5:
                    case DOOR_VERTICAL_6:
						Act1::SpawnDoor (x,y,1,(tile-DOOR_VERTICAL_1)/2);
                        break;
                    case DOOR_HORIZONTAL_1:
                    case DOOR_HORIZONTAL_2:
                    case DOOR_HORIZONTAL_3:
                    case DOOR_HORIZONTAL_4:
                    case DOOR_HORIZONTAL_5:
                    case DOOR_HORIZONTAL_6:
                        Act1::SpawnDoor (x,y,0,(tile-DOOR_HORIZONTAL_1)/2);
                        break;
                }
            }
        }
    }

//
// spawn actors
//
    ScanInfoPlane ();


//
// take out the ambush markers
//
    map = mapSegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile == AMBUSHTILE)
            {
                tilemap[x][y] = 0;
                if ( (unsigned)(uintptr_t)actorat[x][y] == AMBUSHTILE)
                    actorat[x][y] = NULL;

                if (*map >= AREATILE)
                    tile = *map;
                if (*(map-1-mapwidth) >= AREATILE)
                    tile = *(map-1-mapwidth);
                if (*(map-1+mapwidth) >= AREATILE)
                    tile = *(map-1+mapwidth);
                if ( *(map-2) >= AREATILE)
                    tile = *(map-2);

                *(map-1) = tile;
            }
        }
    }


//
// have the caching manager load and purge stuff to make sure all marks
// are in memory
//
   audioSegs.loadAllSounds(sd_soundMode);

   Secret::AnalyzeSecrets();

    // IOANCH: try to solve secret puzzle here
//   std::shared_ptr<SecretSolver> ssolver(new SecretSolver);
//   ssolver->GetLevelData();
//   unsigned sessionNo = g_sessionNo;
//	std::thread thread([ssolver, sessionNo]{
//		
//		std::vector<SecretPush> pushes = ssolver->Solve(sessionNo);
//		
//		if (sessionNo != g_sessionNo)	// cancel if session changed
//			return;
//		
//		if (pushes.size() == 0)
//			Logger::Write("Timeout finding a sequence, knocking myself out.");
//		
//		AddPostCommand([pushes]
//        {
//			// Mark as "haspushes" anyway. The bot will know what to do about it.
//            bot.SetPushList(pushes);
//		}, sessionNo);
//	});
//	thread.detach();

//    scoreMap.TestPushBlocks();
}


//==========================================================================

// 
// DrawPlayBorderSides
// 
// To fix window overwrites
//
void DrawPlayBorderSides()
{
    if(viewsize == 21) return;

	const int sw = cfg_logicalWidth;
	const int sh = cfg_logicalHeight;
	const int vw = viewwidth;
	const int vh = viewheight;
	const int px = vid_scaleFactor; // size of one "pixel"

	const int h  = sh - px * STATUSLINES;
	const int xl = sw / 2 - vw / 2;
	const int yl = (h - vh) / 2;

    if(xl != 0)
    {
	    VL_BarScaledCoord(0,            0, xl - px,     h, bordercol);                 // left side
	    VL_BarScaledCoord(xl + vw + px, 0, xl - px * 2, h, bordercol);                 // right side
    }

    if(yl != 0)
    {
	    VL_BarScaledCoord(0, 0,            sw, yl - px, bordercol);                    // upper side
	    VL_BarScaledCoord(0, yl + vh + px, sw, yl - px, bordercol);                    // lower side
    }

    if(xl != 0)
    {
        // Paint game view border lines
	    VL_BarScaledCoord(xl - px, yl - px, vw + px, px,          0);                      // upper border
	    VL_BarScaledCoord(xl,      yl + vh, vw + px, px,          bordercol - 2);          // lower border
	    VL_BarScaledCoord(xl - px, yl - px, px,      vh + px,     0);                      // left border
	    VL_BarScaledCoord(xl + vw, yl - px, px,      vh + px * 2, bordercol - 2);          // right border
	    VL_BarScaledCoord(xl - px, yl + vh, px,      px,          bordercol - 3);          // lower left highlight
    }
    else
    {
        // Just paint a lower border line
        VL_BarScaledCoord(0, yl+vh, vw, px, bordercol-2);       // lower border
    }
}


/*
===================
=
= DrawStatusBorder
=
===================
*/

void DrawStatusBorder (byte color)
{
    int statusborderw = (cfg_logicalWidth-vid_scaleFactor*LOGIC_WIDTH)/2;

    VL_BarScaledCoord (0,0,cfg_logicalWidth,cfg_logicalHeight-vid_scaleFactor*(STATUSLINES-3),color);
    VL_BarScaledCoord (0,cfg_logicalHeight-vid_scaleFactor*(STATUSLINES-3),
        statusborderw+vid_scaleFactor*8,vid_scaleFactor*(STATUSLINES-4),color);
    VL_BarScaledCoord (0,cfg_logicalHeight-vid_scaleFactor*2,cfg_logicalWidth,vid_scaleFactor*2,color);
    VL_BarScaledCoord (cfg_logicalWidth-statusborderw-vid_scaleFactor*8, cfg_logicalHeight-vid_scaleFactor*(STATUSLINES-3),
        statusborderw+vid_scaleFactor*8,vid_scaleFactor*(STATUSLINES-4),color);

    VL_BarScaledCoord (statusborderw+vid_scaleFactor*9, cfg_logicalHeight-vid_scaleFactor*3,
        vid_scaleFactor*97, vid_scaleFactor*1, color-1);
    VL_BarScaledCoord (statusborderw+vid_scaleFactor*106, cfg_logicalHeight-vid_scaleFactor*3,
        vid_scaleFactor*161, vid_scaleFactor*1, color-2);
    VL_BarScaledCoord (statusborderw+vid_scaleFactor*267, cfg_logicalHeight-vid_scaleFactor*3,
        vid_scaleFactor*44, vid_scaleFactor*1, color-3);
    VL_BarScaledCoord (cfg_logicalWidth-statusborderw-vid_scaleFactor*9, cfg_logicalHeight-vid_scaleFactor*(STATUSLINES-4),
        vid_scaleFactor*1, vid_scaleFactor*20, color-2);
    VL_BarScaledCoord (cfg_logicalWidth-statusborderw-vid_scaleFactor*9, cfg_logicalHeight-vid_scaleFactor*(STATUSLINES/2-4),
        vid_scaleFactor*1, vid_scaleFactor*14, color-3);
}


/*
===================
=
= DrawPlayBorder
=
===================
*/

void DrawPlayBorder ()
{
	const int px = vid_scaleFactor; // size of one "pixel"

    if (bordercol != VIEWCOLOR)
        DrawStatusBorder(bordercol);
    else
    {
        const int statusborderw = (cfg_logicalWidth-px*LOGIC_WIDTH)/2;
        VL_BarScaledCoord (0, cfg_logicalHeight-px*STATUSLINES,
            statusborderw+px*8, px*STATUSLINES, bordercol);
        VL_BarScaledCoord (cfg_logicalWidth-statusborderw-px*8, cfg_logicalHeight-px*STATUSLINES,
            statusborderw+px*8, px*STATUSLINES, bordercol);
    }

    if((unsigned) viewheight == cfg_logicalHeight) return;

    VL_BarScaledCoord (0,0,cfg_logicalWidth,cfg_logicalHeight-px*STATUSLINES,bordercol);

    const int xl = cfg_logicalWidth/2-viewwidth/2;
    const int yl = (cfg_logicalHeight-px*STATUSLINES-viewheight)/2;
    VL_BarScaledCoord (xl,yl,viewwidth,viewheight,0);

    if(xl != 0)
    {
        // Paint game view border lines
        VL_BarScaledCoord(xl-px, yl-px, viewwidth+px, px, 0);                      // upper border
        VL_BarScaledCoord(xl, yl+viewheight, viewwidth+px, px, bordercol-2);       // lower border
        VL_BarScaledCoord(xl-px, yl-px, px, viewheight+px, 0);                     // left border
        VL_BarScaledCoord(xl+viewwidth, yl-px, px, viewheight+2*px, bordercol-2);  // right border
        VL_BarScaledCoord(xl-px, yl+viewheight, px, px, bordercol-3);              // lower left highlight
    }
    else
    {
        // Just paint a lower border line
        VL_BarScaledCoord(0, yl+viewheight, viewwidth, px, bordercol-2);       // lower border
    }
}


/*
===================
=
= DrawPlayScreen
=
===================
*/

void DrawPlayScreen ()
{
    VWB_DrawPicScaledCoord ((cfg_logicalWidth-vid_scaleFactor*LOGIC_WIDTH)/2,cfg_logicalHeight-vid_scaleFactor*STATUSLINES,SPEAR::g(STATUSBARPIC));
    DrawPlayBorder ();

    DrawFace ();
    DrawHealth ();
    DrawLives ();
    DrawLevel ();
    DrawAmmo ();
    DrawKeys ();
    DrawWeapon ();
    DrawScore ();
}

void ShowActStatus()
{
    // Draw status bar without borders
    const byte *source = graphSegs[SPEAR::g(STATUSBARPIC)];
    int	picnum = SPEAR::g(STATUSBARPIC) - SPEAR::g(STARTPICS);
   GraphicLoader::pictabletype psize = graphSegs.sizeAt(picnum);
    int destx = (cfg_logicalWidth-vid_scaleFactor*LOGIC_WIDTH)/2 + 9 * vid_scaleFactor;
    int desty = cfg_logicalHeight - (psize.height - 4) * vid_scaleFactor;
    VL_MemToScreenScaledCoord(source, psize.width, psize.height, 9, 4, destx,
                              desty, psize.width - 18, psize.height - 7);

    ingame = false;
    DrawFace ();
    DrawHealth ();
    DrawLives ();
    DrawLevel ();
    DrawAmmo ();
    DrawKeys ();
    DrawWeapon ();
    DrawScore ();
    ingame = true;
}


//==========================================================================

/*
==================
=
= StartDemoRecord
=
==================
*/

std::string    cfg_demoname("DEMO?.");

#ifndef REMDEBUG
#define MAXDEMOSIZE     8192

void StartDemoRecord (int levelnumber)
{
    demobuffer=I_CheckedMalloc(MAXDEMOSIZE);
    demoptr = (int8_t *) demobuffer;
    lastdemoptr = demoptr+MAXDEMOSIZE;

    *demoptr = levelnumber;
    demoptr += 4;                           // leave space for length
    demorecord = true;
}


/*
==================
=
= FinishDemoRecord
=
==================
*/

void FinishDemoRecord ()
{
    int32_t    length,level;

    demorecord = false;

    length = (int32_t) (demoptr - (int8_t *)demobuffer);

    demoptr = ((int8_t *)demobuffer)+1;
    demoptr[0] = (int8_t) length;
    demoptr[1] = (int8_t) (length >> 8);
    demoptr[2] = 0;

    VW_FadeIn();
    CenterWindow(24,3);
    PrintY+=6;
    fontnumber=0;
    SETFONTCOLOR(0,15);
    US_Print(" Demo number (0-9): ");
    I_UpdateScreen();
	
	std::string path;

    if (US_LineInput (px,py,str,NULL,true,1,0))
    {
        level = atoi (str);
        if (level>=0 && level<=9)
        {
            cfg_demoname[4] = static_cast<char>('0'+level);
			path = cfg_dir;
			ConcatSubpath(path, cfg_demoname);
            CA_WriteFile (path, demobuffer, length);
        }
    }

    free(demobuffer);
}

//==========================================================================

/*
==================
=
= RecordDemo
=
= Fades the screen out, then starts a demo.  Exits with the screen faded
=
==================
*/

void RecordDemo ()
{
    int level,esc,maps;

    CenterWindow(26,3);
    PrintY+=6;
    graphSegs.cacheChunk(SPEAR::g(STARTFONT));
    fontnumber=0;
    SETFONTCOLOR(0,15);
    if(!SPEAR::flag)
    {
    // IOANCH 20130301: unification culling
        US_Print("  Demo which level(1-60): ");
        maps = 60;
    }
    else
    {
        US_Print("  Demo which level(1-21): ");
        maps = 21;
    }

    I_UpdateScreen();
    VW_FadeIn ();
    esc = !US_LineInput (px,py,str,NULL,true,2,0);
    if (esc)
        return;

    level = atoi (str);
    level--;

    if (level >= maps || level < 0)
        return;

    VW_FadeOut ();
    // IOANCH 20130302: unification
    if(!SPEAR::flag)
    {
        NewGame (gd_hard,level/10);
        gamestate.mapon = level%10;
    }
    else
    {
        NewGame (gd_hard,0);
        gamestate.mapon = level;
    }

    StartDemoRecord (level);

    DrawPlayScreen ();
    VW_FadeIn ();

    startgame = false;
    demorecord = true;

    SetupGameLevel ();
    StartMusic ();

    if(cfg_usedoublebuffering) I_UpdateScreen();
    fizzlein = true;

    PlayLoop ();

    demoplayback = false;

    StopMusic ();
    VW_FadeOut ();
    Sound::StopDigitized ();

    FinishDemoRecord ();
}
#else
void FinishDemoRecord () {return;}
void RecordDemo () {return;}
#endif



//==========================================================================

/*
==================
=
= PlayDemo
=
= Fades the screen out, then starts a demo.  Exits with the screen unfaded
=
==================
*/

void PlayDemo (int demonumber)
{
    int length;
#ifdef DEMOSEXTERN
// debug: load chunk
    // IOANCH 20130301: unification culling
    int dems[4]={static_cast<int>(SPEAR::g(T_DEMO0)),static_cast<int>(SPEAR::g(T_DEMO1)),static_cast<int>(SPEAR::g(T_DEMO2)),static_cast<int>(SPEAR::g(T_DEMO3))};

    graphSegs.cacheChunk(dems[demonumber]);
    demoptr = (int8_t *) graphSegs[dems[demonumber]];
#else
    cfg_demoname[4] = '0'+demonumber;
    CA_LoadFile (cfg_demoname.buffer(),&demobuffer);
    demoptr = (int8_t *)demobuffer;
#endif

    NewGame (1,0);
    gamestate.mapon = *demoptr++;
    gamestate.difficulty = gd_hard;
    length = READWORD(*(uint8_t **)&demoptr);
    // TODO: Seems like the original demo format supports 16 MB demos
    //       But T_DEM00 and T_DEM01 of Wolf have a 0xd8 as third length size...
    demoptr++;
    lastdemoptr = demoptr-4+length;

    VW_FadeOut ();

    SETFONTCOLOR(0,15);
    DrawPlayScreen ();

    startgame = false;
    demoplayback = true;

    SetupGameLevel ();
    StartMusic ();

    PlayLoop ();

#ifdef DEMOSEXTERN
    graphSegs.uncacheChunk(dems[demonumber]);
#else
    MM_FreePtr (&demobuffer);
#endif

    demoplayback = false;

    StopMusic ();
    Sound::StopDigitized ();
}

//==========================================================================

/*
==================
=
= Died
=
==================
*/

#define DEATHROTATE 2

static void Died ()
{
    float   fangle;
    int32_t dx,dy;
    int     iangle,curangle,clockwise,counter,change;

    if (vid_screenfaded)
    {
        ThreeDRefresh ();
        VW_FadeIn ();
    }

    gamestate.weapon = (weapontype) -1;                     // take away weapon
    Sound::Play (PLAYERDEATHSND);

    //
    // swing around to face attacker
    //
    if(killerobj)
    {
        dx = killerobj->x - player->x;
        dy = player->y - killerobj->y;

        fangle = (float) atan2((float) dy, (float) dx);     // returns -pi to pi
        if (fangle<0)
            fangle = (float) (M_PI*2+fangle);

        iangle = (int) (fangle/(M_PI*2)*ANGLES);
    }
    else
    {
        iangle = player->angle + ANGLES / 2;
        if(iangle >= ANGLES) iangle -= ANGLES;
    }

    if (player->angle > iangle)
    {
        counter = player->angle - iangle;
        clockwise = ANGLES-player->angle + iangle;
    }
    else
    {
        clockwise = iangle - player->angle;
        counter = player->angle + ANGLES-iangle;
    }

    curangle = player->angle;

    if (clockwise<counter)
    {
        //
        // rotate clockwise
        //
        if (curangle>iangle)
            curangle -= ANGLES;
        do
        {
            change = tics*DEATHROTATE;
            if (curangle + change > iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle >= ANGLES)
                player->angle -= ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }
    else
    {
        //
        // rotate counterclockwise
        //
        if (curangle<iangle)
            curangle += ANGLES;
        do
        {
            change = -(int)tics*DEATHROTATE;
            if (curangle + change < iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle < 0)
                player->angle += ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }

    //
    // fade to red
    //
    FinishPaletteShifts ();

    if(cfg_usedoublebuffering) I_UpdateScreen();

    VL_BarScaledCoord (viewscreenx,viewscreeny,viewwidth,viewheight,4);

    myInput.clearKeysDown ();

    FizzleFade(viewscreenx,viewscreeny,viewwidth,viewheight,70,false);

    myInput.userInput(100);
    SD_WaitSoundDone ();
    Sound::StopDigitized();

    gamestate.lives--;

    if (gamestate.lives > -1)
    {
        gamestate.health = I_PLAYERHEALTH;	// IOANCH
        gamestate.weapon = gamestate.bestweapon
            = gamestate.chosenweapon = wp_pistol;
        gamestate.ammo = STARTAMMO;
        gamestate.keys = 0;
        pwallstate = pwallpos = 0;
        gamestate.attackframe = gamestate.attackcount =
            gamestate.weaponframe = 0;

        if(viewsize != 21)
        {
            DrawKeys ();
            DrawWeapon ();
            DrawAmmo ();
            DrawHealth ();
            DrawFace ();
            DrawLives ();
        }
    }
}

//==========================================================================

/*
===================
=
= GameLoop
=
===================
*/

void GameLoop ()
{
    Boolean8 died;
#ifdef MYPROFILE
    clock_t start,end;
#endif

restartgame:
    Sound::StopDigitized ();
    SETFONTCOLOR(0,15);
    VW_FadeOut();
    DrawPlayScreen ();
    died = false;
    do
    {
		g_inGameLoop = true;

        if (!loadedgame)
            gamestate.score = gamestate.oldscore;
        if(!died || viewsize != 21) DrawScore();

        startgame = false;
        if (!loadedgame)
            SetupGameLevel ();
        // IOANCH 20130302: unification
        if (SPEAR::flag && gamestate.mapon == 20)      // give them the key allways
        {
            gamestate.keys |= 1;
            DrawKeys ();
        }

        ingame = true;
        if(loadedgame)
        {
            ContinueMusic(lastgamemusicoffset);
            loadedgame = false;
        }
        else StartMusic ();

        if (!died)
            PreloadGraphics ();             // TODO: Let this do something useful!
        else
        {
            died = false;
            fizzlein = true;
        }

        DrawLevel ();
        // IOANCH 20130302: unification
startplayloop:
        PlayLoop ();

        if (SPEAR::flag && spearflag)
        {
           // IOANCH 20130725: added spear notification
			std::lock_guard<std::mutex> lock(g_playloopMutex);
            I_Notify("Got the Spear!");
			Sound::Stop();
            Sound::Play(GETSPEARSND);
            if (sd_digiMode != sds_Off)
            {
                Delay(150);
            }
            else
                SD_WaitSoundDone();

            Sound::StopDigitized ();
            gamestate.oldscore = gamestate.score;
            gamestate.mapon = 20;
            SetupGameLevel ();
            StartMusic ();
            player->x = spearx;
            player->y = speary;
            player->angle = (short)spearangle;
            spearflag = false;
            Thrust (0,0);
            goto startplayloop;
        }

        StopMusic ();
        ingame = false;

        if (demorecord && playstate != ex_warped)
            FinishDemoRecord ();

        if (startgame || loadedgame)
            goto restartgame;

       // IOANCH 20130725: send notifications on level exit
        switch (playstate)
        {
            case ex_completed:
            case ex_secretlevel:
              
                I_Notify(PString("Finished E").concat(gamestate.episode + 1).
                       Putc('M').concat(gamestate.mapon + 1)());
              
                if(viewsize == 21) DrawPlayScreen();
                gamestate.keys = 0;
                DrawKeys ();
                VW_FadeOut ();

                Sound::StopDigitized ();

                LevelCompleted ();              // do the intermission
                if(viewsize == 21) DrawPlayScreen();

// IOANCH 20130301: unification culling


                gamestate.oldscore = gamestate.score;
                // IOANCH 20130302: unification
                if(!SPEAR::flag)
                {
                    if (gamestate.mapon == 9)
                        gamestate.mapon = ElevatorBackTo[gamestate.episode];    // back from secret
                    else
                    {
                        //
                        // GOING TO SECRET LEVEL
                        //
                        if (playstate == ex_secretlevel)
                            gamestate.mapon = 9;
                        else
                        {

                            //
                            // GOING TO NEXT LEVEL
                            //
                            gamestate.mapon++; 
                        }
                    }
                }
                else
                {
                    
#define FROMSECRET1             3
#define FROMSECRET2             11

                    //
                    // GOING TO SECRET LEVEL
                    //
                    if (playstate == ex_secretlevel)
                        switch(gamestate.mapon)
                    {
                        case FROMSECRET1: gamestate.mapon = 18; break;
                        case FROMSECRET2: gamestate.mapon = 19; break;
                    }
                    else
                    {
                        //
                        // COMING BACK FROM SECRET LEVEL
                        //
                        if (gamestate.mapon == 18 || gamestate.mapon == 19)
                            switch(gamestate.mapon)
                        {
                            case 18: gamestate.mapon = FROMSECRET1+1; break;
                            case 19: gamestate.mapon = FROMSECRET2+1; break;
                        }
                        else
                        {
                            //
                            // GOING TO NEXT LEVEL
                            //
                            gamestate.mapon++;
                        }
                    }
                }
                break;

            case ex_died:
                I_Notify(PString("Died in E").concat(gamestate.episode + 1).
                       Putc('M').concat(gamestate.mapon + 1)());

                Died ();
                died = true;                    // don't "get psyched!"

				if (gamestate.lives > -1)
				{
					break;                          // more lives left
				}

				DestroySavedInstance();
				g_inGameLoop = false;

                VW_FadeOut ();
                if(cfg_logicalHeight % LOGIC_HEIGHT != 0)
                    I_ClearScreen(0);

#ifdef _arch_dreamcast
                DC_StatusClearLCD();
#endif

                Sound::StopDigitized ();

                CheckHighScore (gamestate.score,gamestate.mapon+1);
                // IOANCH 20130301: unification culling

                strcpy(MainMenu[mi_viewscores].string,STR_VS);

                MainMenu[mi_viewscores].routine = CP_ViewScores;

                return;

            case ex_victorious:
				DestroySavedInstance();
				g_inGameLoop = false;

                I_Notify("Victory!");

                if(viewsize == 21) DrawPlayScreen();
                // IOANCH 20130302: unification
                if(!SPEAR::flag)
                    VW_FadeOut ();
                else
                    VL_FadeOut (0,255,0,17,17,300);
                Sound::StopDigitized ();

                Victory ();

                Sound::StopDigitized ();

                CheckHighScore (gamestate.score,gamestate.mapon+1);
                // IOANCH 20130301: unification culling

                strcpy(MainMenu[mi_viewscores].string,STR_VS);

                MainMenu[mi_viewscores].routine = CP_ViewScores;
                return;

            default:
                if(viewsize == 21) DrawPlayScreen();
                Sound::StopDigitized ();
                break;
        }
    } while (1);

//	DestroySavedInstance();
//	g_inGameLoop = false;
}
