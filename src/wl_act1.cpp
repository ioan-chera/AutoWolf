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
// Routines dealing mostly with map features
//
////////////////////////////////////////////////////////////////////////////////

// WL_ACT1.C

#include "wl_def.h"

#include "Logger.h"
#include "wl_act1.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_play.h"
#include "id_ca.h"
#include "ioan_bas.h"	// IOANCH 29.09.2012
#include "Config.h"
#include "SODFlag.h"
#include "ioan_bot.h"
#include "Exception.h"

////////////////////////////////////////////////////////////////////////////////
//
//                                                        STATICS
//
////////////////////////////////////////////////////////////////////////////////


statobj_t       statobjlist[MAXSTATS];
statobj_t       *laststatobj;

// IOANCH 11.06.2012: added name to made compiler stop complaining
struct _statinfo_t
{
    short      picnum;
    wl_stat_t  type;
    uint32_t   specialFlags;    // they are ORed to the statobj_t flags
};

// IOANCH 20130202: unification process
static const _statinfo_t statinfo_wl6[] =
{
    {SPR_STAT_0},                           // puddle          spr1v
    {SPR_STAT_1,block},                     // Green Barrel    "
    {SPR_STAT_2,block},                     // Table/chairs    "
    {SPR_STAT_3,block,FL_FULLBRIGHT},       // Floor lamp      "
    {SPR_STAT_4,none,FL_FULLBRIGHT},        // Chandelier      "
    {SPR_STAT_5,block},                     // Hanged man      "
    {SPR_STAT_6,bo_alpo},                   // Bad food        "
    {SPR_STAT_7,block},                     // Red pillar      "
    //
    // NEW PAGE
    //
    {SPR_STAT_8,block},                     // Tree            spr2v
    {SPR_STAT_9},                           // Skeleton flat   "
    {SPR_STAT_10,block},                    // Sink            " (SOD:gibs)
    {SPR_STAT_11,block},                    // Potted plant    "
    {SPR_STAT_12,block},                    // Urn             "
    {SPR_STAT_13,block},                    // Bare table      "
    {SPR_STAT_14,none,FL_FULLBRIGHT},       // Ceiling light   "
    {SPR_STAT_15},                          // Kitchen stuff   "
    //
    // NEW PAGE
    //
    {SPR_STAT_16,block},                    // suit of armor   spr3v
    {SPR_STAT_17,block},                    // Hanging cage    "
    {SPR_STAT_18,block},                    // SkeletoninCage  "
    {SPR_STAT_19},                          // Skeleton relax  "
    {SPR_STAT_20,bo_key1},                  // Key 1           "
    {SPR_STAT_21,bo_key2},                  // Key 2           "
    {SPR_STAT_22,block},                    // stuff             (SOD:gibs)
    {SPR_STAT_23},                          // stuff
    //
    // NEW PAGE
    //
    {SPR_STAT_24,bo_food},                  // Good food       spr4v
    {SPR_STAT_25,bo_firstaid},              // First aid       "
    {SPR_STAT_26,bo_clip},                  // Clip            "
    {SPR_STAT_27,bo_machinegun},            // Machine gun     "
    {SPR_STAT_28,bo_chaingun},              // Gatling gun     "
    {SPR_STAT_29,bo_cross},                 // Cross           "
    {SPR_STAT_30,bo_chalice},               // Chalice         "
    {SPR_STAT_31,bo_bible},                 // Bible           "
    //
    // NEW PAGE
    //
    {SPR_STAT_32,bo_crown},                 // crown           spr5v
    {SPR_STAT_33,bo_fullheal,FL_FULLBRIGHT},// one up          "
    {SPR_STAT_34,bo_gibs},                  // gibs            "
    {SPR_STAT_35,block},                    // barrel          "
    {SPR_STAT_36,block},                    // well            "
    {SPR_STAT_37,block},                    // Empty well      "
    {SPR_STAT_38,bo_gibs},                  // Gibs 2          "
    {SPR_STAT_39,block},                    // flag            "
    //
    // NEW PAGE
    //
    {SPR_STAT_40,block},                    // Call Apogee          spr7v
    //
    // NEW PAGE
    //
    {SPR_STAT_41},                          // junk            "
    {SPR_STAT_42},                          // junk            "
    {SPR_STAT_43},                          // junk            "
    {SPR_STAT_44},                          // pots            "
    {SPR_STAT_45,block},                    // stove           " (SOD:gibs)
    {SPR_STAT_46,block},                    // spears          " (SOD:gibs)
    {SPR_STAT_47},                          // vines           "

    {SPR_STAT_26,bo_clip2},                 // Clip            "
#ifdef USE_DIR3DSPR
    // These are just two examples showing the new way of using dir 3d sprites.
    // You can find the allowed values in the objflag_t enum in wl_def.h.
    {SPR_STAT_47,none,FL_DIR_VERT_MID},
    {SPR_STAT_47,block,FL_DIR_HORIZ_MID},
#endif
    {-1}                                    // terminator
};

static const _statinfo_t *statinfo;
static int statinfo_length = lengthof(statinfo_wl6);

void Act1::SetSpearModuleValues()
{
    if(!SPEAR::flag)
    {
        statinfo = statinfo_wl6; // use the original values
		return;
    }

    static _statinfo_t *statinfo_sod;
    if(statinfo && statinfo == statinfo_sod)
		return; // already initialized

    std::vector<_statinfo_t> newStatInfo;
    newStatInfo.resize(lengthof(statinfo_wl6));
    memcpy(newStatInfo.data(), statinfo_wl6, sizeof(statinfo_wl6));

    // Apply differences now
    // 1. SPR_STAT_15 changes from kitchen stuff to gibs
    newStatInfo[15].type = block;

    // 2. SPR_STAT_40 changes from call apogee to red light
    newStatInfo[40].type = none;

    // 3. SPR_STAT_44 changes from pots to gibs
    newStatInfo[44].type = block;

    newStatInfo.insert(newStatInfo.begin() + 48, 
    {
        {SPR_STAT_48, block},                    // marble pillar
        {SPR_STAT_49, bo_25clip},               // bonus 25 clip
        {SPR_STAT_50, block},                    // truck
        {SPR_STAT_51, bo_spear},                // SPEAR OF DESTINY!
    });

    // Store the result
    statinfo_sod = new _statinfo_t[newStatInfo.size()];
    memcpy(statinfo_sod, newStatInfo.data(), newStatInfo.size() * sizeof(_statinfo_t));

	statinfo = statinfo_sod; // use the modified values
	statinfo_length = (int)newStatInfo.size();
}


wl_stat_t Act1::GetStaticType(int actorid)
{
	actorid -= STATIC_ID_OFFSET;
	if(actorid < 0 || actorid >= statinfo_length)
		return none; // out of range
	return statinfo[actorid].type;
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = InitStaticList
// =
//
////////////////////////////////////////////////////////////////////////////////


void InitStaticList ()
{
    laststatobj = &statobjlist[0];
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = SpawnStatic
// =
//
////////////////////////////////////////////////////////////////////////////////


void SpawnStatic (int tilex, int tiley, int type)
{
    // IOANCH 20130202: unification process
    laststatobj->shapenum = statinfo[type].picnum;
    laststatobj->tilex = tilex;
    laststatobj->tiley = tiley;
    laststatobj->visspot = &spotvis[tilex][tiley];

    switch (statinfo[type].type)
    {
        case block:
            actorat[tilex][tiley] = (objtype *) 64;          // consider it a blocking tile
        case none:
            laststatobj->flags = 0;
            break;

        case    bo_cross:
        case    bo_chalice:
        case    bo_bible:
        case    bo_crown:
        case    bo_fullheal:
            if (!loadedgame)
                gamestate.treasuretotal++;

        case    bo_firstaid:
        case    bo_key1:
        case    bo_key2:
        case    bo_key3:
        case    bo_key4:
        case    bo_clip:
        case    bo_25clip:
        case    bo_machinegun:
        case    bo_chaingun:
        case    bo_food:
        case    bo_alpo:
        case    bo_gibs:
        case    bo_spear:
            laststatobj->flags = FL_BONUS;
            laststatobj->itemnumber = statinfo[type].type;
			// IOANCH 26.10.2012: add to list
            // ONLY if on startup, otherwise let LoadTheGame add it to list
            if(!loadedgame)
                Basic::AddItemToList(tilex, tiley, laststatobj->itemnumber);
            break;
		default:
			;
    }

    laststatobj->flags |= statinfo[type].specialFlags;

    laststatobj++;

    if (laststatobj == &statobjlist[MAXSTATS])
        throw Exception ("Too many static objects!\n");
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = PlaceItemType
// =
// = Called during game play to drop actors' items.  It finds the proper
// = item number based on the item type (bo_???).  If there are no free item
// = spots, nothing is done.
// =
//
////////////////////////////////////////////////////////////////////////////////


void PlaceItemType (int itemtype, int tilex, int tiley)
{
    int type;
    statobj_t *spot;
    
    // IOANCH 20130202: unification process
    //
    // find the item number
    //
    for (type=0; ; type++)
    {
        if (statinfo[type].picnum == -1)                    // end of list
            throw Exception ("PlaceItemType: couldn't find type!");
        if (statinfo[type].type == itemtype)
            break;
    }

    //
    // find a spot in statobjlist to put it in
    //
    for (spot=&statobjlist[0]; ; spot++)
    {
        if (spot==laststatobj)
        {
            if (spot == &statobjlist[MAXSTATS])
                return;                                     // no free spots
            laststatobj++;                                  // space at end
            break;
        }

        if (spot->shapenum == -1)                           // -1 is a free spot
            break;
    }
    //
    // place it
    //
    spot->shapenum = statinfo[type].picnum;
    spot->tilex = tilex;
    spot->tiley = tiley;
    spot->visspot = &spotvis[tilex][tiley];
    spot->flags = FL_BONUS | statinfo[type].specialFlags;
    spot->itemnumber = statinfo[type].type;
	// IOANCH: add item to list
	Basic::AddItemToList(spot->tilex, spot->tiley, spot->itemnumber);
}

////////////////////////////////////////////////////////////////////////////////
//
//                                  DOORS
//
// doorobjlist[] holds most of the information for the doors
//
// doorposition[] holds the amount the door is open, ranging from 0 to 0xffff
//        this is directly accessed by AsmRefresh during rendering
//
// The number of doors is limited to 64 because a spot in tilemap holds the
//        door number in the low 6 bits, with the high bit meaning a door 
// center
//        and bit 6 meaning a door side tile
//
// Open doors conect two areas, so sounds will travel between them and sight
//        will be checked when the player is in a connected area.
//
// Areaconnect is incremented/decremented by each door. If >0 they connect
//
// Every time a door opens or closes the areabyplayer matrix gets recalculated.
//        An area is true if it connects with the player's current spor.
//
////////////////////////////////////////////////////////////////////////////////


#define DOORWIDTH       0x7800
#define OPENTICS        300

doorobj_t       doorobjlist[MAXDOORS],*lastdoorobj;
short           doornum;

word            doorposition[MAXDOORS];             // leading edge of door 0=closed
                                                    // 0xffff = fully open

byte            areaconnect[NUMAREAS][NUMAREAS];

Boolean8         areabyplayer[NUMAREAS];


////////////////////////////////////////////////////////////////////////////////
//
// =
// = _ConnectAreas
// =
// = Scans outward from playerarea, marking all connected areas
// =
//
////////////////////////////////////////////////////////////////////////////////


static void _RecursiveConnect (int areanumber)
{
    int i;

    for (i=0;i<NUMAREAS;i++)
    {
        if (areaconnect[areanumber][i] && !areabyplayer[i])
        {
            areabyplayer[i] = true;
            _RecursiveConnect (i);
        }
    }
}


static void _ConnectAreas ()
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    areabyplayer[player->areanumber] = true;
    _RecursiveConnect (player->areanumber);
}


void InitAreas ()
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    if (player->areanumber < NUMAREAS)
        areabyplayer[player->areanumber] = true;
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = InitDoorList
// =
//
////////////////////////////////////////////////////////////////////////////////


void InitDoorList ()
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    memset (areaconnect,0,sizeof(areaconnect));

    lastdoorobj = &doorobjlist[0];
    doornum = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = SpawnDoor
// =
//
////////////////////////////////////////////////////////////////////////////////


void Act1::SpawnDoor (int tilex, int tiley, Boolean8 vertical, int lock)
{
    word *map;

    if (doornum==MAXDOORS)
        throw Exception ("64+ doors on level!");

    doorposition[doornum] = 0;              // doors start out fully closed
    lastdoorobj->tilex = tilex;
    lastdoorobj->tiley = tiley;
    lastdoorobj->vertical = vertical;
    lastdoorobj->lock = lock;
    lastdoorobj->action = dr_closed;

    actorat[tilex][tiley] = (objtype *)(uintptr_t)(doornum | 0x80);   // consider it a solid wall

    //
    // make the door tile a special tile, and mark the adjacent tiles
    // for door sides
    //
    tilemap[tilex][tiley] = doornum | 0x80;
   map = mapSegs.ptr(0, tilex, tiley);
    if (vertical)
    {
        *map = *(map-1);                        // set area number
        tilemap[tilex][tiley-1] |= 0x40;
        tilemap[tilex][tiley+1] |= 0x40;
    }
    else
    {
        *map = *(map-mapwidth);                                 // set area number
        tilemap[tilex-1][tiley] |= 0x40;
        tilemap[tilex+1][tiley] |= 0x40;
    }

    doornum++;
    lastdoorobj++;
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = OpenDoor
// =
//
////////////////////////////////////////////////////////////////////////////////


void Act1::OpenDoor (int door)
{
    if (doorobjlist[door].action == dr_open)
        doorobjlist[door].ticcount = 0;         // reset open time
    else
        doorobjlist[door].action = dr_opening;  // start it opening
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = _CloseDoor
// =
//
////////////////////////////////////////////////////////////////////////////////


static void _CloseDoor (int door)
{
    int     tilex,tiley,area;
    objtype *check;

    //
    // don't close on anything solid
    //
    tilex = doorobjlist[door].tilex;
    tiley = doorobjlist[door].tiley;

    if (actorat[tilex][tiley])
        return;

    if (player->tilex == tilex && player->tiley == tiley)
        return;

    if (doorobjlist[door].vertical)
    {
        if ( player->tiley == tiley )
        {
            if ( ((player->x+MINDIST) >>TILESHIFT) == tilex )
                return;
            if ( ((player->x-MINDIST) >>TILESHIFT) == tilex )
                return;
        }
        check = actorat[tilex-1][tiley];
        if (ISPOINTER(check) && ((check->x+MINDIST) >> TILESHIFT) == tilex )
            return;
        check = actorat[tilex+1][tiley];
        if (ISPOINTER(check) && ((check->x-MINDIST) >> TILESHIFT) == tilex )
            return;
    }
    else if (!doorobjlist[door].vertical)
    {
        if (player->tilex == tilex)
        {
            if ( ((player->y+MINDIST) >>TILESHIFT) == tiley )
                return;
            if ( ((player->y-MINDIST) >>TILESHIFT) == tiley )
                return;
        }
        check = actorat[tilex][tiley-1];
        if (ISPOINTER(check) && ((check->y+MINDIST) >> TILESHIFT) == tiley )
            return;
        check = actorat[tilex][tiley+1];
        if (ISPOINTER(check) && ((check->y-MINDIST) >> TILESHIFT) == tiley )
            return;
    }


    //
    // play door sound if in a connected area
    //
    area = mapSegs(0, doorobjlist[door].tilex, doorobjlist[door].tiley) - AREATILE;
    if (areabyplayer[area])
    {
        PlaySoundLocTile(CLOSEDOORSND,doorobjlist[door].tilex,doorobjlist[door].tiley); // JAB
    }

    doorobjlist[door].action = dr_closing;
    //
    // make the door space solid
    //
    actorat[tilex][tiley] = (objtype *)(uintptr_t)(door | 0x80);
}



////////////////////////////////////////////////////////////////////////////////
//
// =
// = OperateDoor
// =
// = The player wants to change the door's direction
// =
//
////////////////////////////////////////////////////////////////////////////////


void OperateDoor (int door)
{
    int lock;

    lock = doorobjlist[door].lock;
    if (lock >= dr_lock1 && lock <= dr_lock4)
    {
        if ( ! (gamestate.keys & (1 << (lock-dr_lock1) ) ) )
        {
            Sound::Play (NOWAYSND);                // locked
            return;
        }
    }

    switch (doorobjlist[door].action)
    {
        case dr_closed:
        case dr_closing:
            Act1::OpenDoor (door);
            break;
        case dr_open:
        case dr_opening:
            _CloseDoor (door);
            break;
    }
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = _DoorOpen
// =
// = Close the door after three seconds
// =
//
////////////////////////////////////////////////////////////////////////////////


static void _DoorOpen (int door)
{
    if ( (doorobjlist[door].ticcount += (short) tics) >= OPENTICS)
        _CloseDoor (door);
}



////////////////////////////////////////////////////////////////////////////////
//
// =
// = _DoorOpening
// =
//
////////////////////////////////////////////////////////////////////////////////


static void _DoorOpening (int door)
{
    unsigned area1,area2;
    const word *map;
    int32_t position;

    position = doorposition[door];
    if (!position)
    {
        //
        // door is just starting to open, so connect the areas
        //
        map = mapSegs.cptr(0, doorobjlist[door].tilex, doorobjlist[door].tiley);

        if (doorobjlist[door].vertical)
        {
            area1 = *(map+1);
            area2 = *(map-1);
        }
        else
        {
            area1 = *(map-mapwidth);
            area2 = *(map+mapwidth);
        }
        area1 -= AREATILE;
        area2 -= AREATILE;

        if (area1 < NUMAREAS && area2 < NUMAREAS)
        {
            areaconnect[area1][area2]++;
            areaconnect[area2][area1]++;

            if (player->areanumber < NUMAREAS)
                _ConnectAreas ();

            if (areabyplayer[area1])
                PlaySoundLocTile(OPENDOORSND,doorobjlist[door].tilex,doorobjlist[door].tiley);  // JAB
        }
    }

    //
    // slide the door by an adaptive amount
    //
    position += tics<<10;
    if (position >= 0xffff)
    {
        //
        // door is all the way open
        //
        position = 0xffff;
        doorobjlist[door].ticcount = 0;
        doorobjlist[door].action = dr_open;
        actorat[doorobjlist[door].tilex][doorobjlist[door].tiley] = 0;
    }

    doorposition[door] = (word) position;
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = _DoorClosing
// =
//
////////////////////////////////////////////////////////////////////////////////


static void _DoorClosing (int door)
{
    unsigned area1,area2;
    const word *map;
    int32_t position;
    int tilex,tiley;

    tilex = doorobjlist[door].tilex;
    tiley = doorobjlist[door].tiley;

    if ( ((int)(uintptr_t)actorat[tilex][tiley] != (door | 0x80))
        || (player->tilex == tilex && player->tiley == tiley) )
    {                       // something got inside the door
        Act1::OpenDoor (door);
        return;
    };

    position = doorposition[door];

    //
    // slide the door by an adaptive amount
    //
    position -= tics<<10;
    if (position <= 0)
    {
        //
        // door is closed all the way, so disconnect the areas
        //
        position = 0;

        doorobjlist[door].action = dr_closed;

       map = mapSegs.cptr(0, doorobjlist[door].tilex, doorobjlist[door].tiley);

        if (doorobjlist[door].vertical)
        {
            area1 = *(map+1);
            area2 = *(map-1);
        }
        else
        {
            area1 = *(map-mapwidth);
            area2 = *(map+mapwidth);
        }
        area1 -= AREATILE;
        area2 -= AREATILE;

        if (area1 < NUMAREAS && area2 < NUMAREAS)
        {
            areaconnect[area1][area2]--;
            areaconnect[area2][area1]--;

            if (player->areanumber < NUMAREAS)
                _ConnectAreas ();
        }
    }

    doorposition[door] = (word) position;
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = MoveDoors
// =
// = Called from PlayLoop
// =
//
////////////////////////////////////////////////////////////////////////////////


void MoveDoors ()
{
    int door;

    if (gamestate.victoryflag)              // don't move door during victory sequence
        return;

    for (door = 0; door < doornum; door++)
    {
        switch (doorobjlist[door].action)
        {
            case dr_open:
                _DoorOpen (door);
                break;

            case dr_opening:
                _DoorOpening(door);
                break;

            case dr_closing:
                _DoorClosing(door);
                break;
			default:
				;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                                PUSHABLE WALLS
//
////////////////////////////////////////////////////////////////////////////////


word pwallstate;
word pwallpos;                  // amount a pushable wall has been moved (0-63)
word pwallx,pwally;
byte pwalldir,pwalltile;
static int _dirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}};

////////////////////////////////////////////////////////////////////////////////
//
// =
// = PushWall
// =
//
////////////////////////////////////////////////////////////////////////////////


void PushWall (int checkx, int checky, int dir)
{
    int oldtile, dx, dy;

    if (pwallstate)
        return;

    oldtile = tilemap[checkx][checky];
    if (!oldtile)
        return;

    dx = _dirs[dir][0];
    dy = _dirs[dir][1];

    if (actorat[checkx+dx][checky+dy])
    {
        Sound::Play (NOWAYSND);
        return;
    }
    actorat[checkx+dx][checky+dy] = (objtype *)(uintptr_t) (tilemap[checkx+dx][checky+dy] = oldtile);

	if (bot.haspushes && bot.pushes.size())
		bot.pushes.pop_back();
    gamestate.secretcount++;
    pwallx = checkx;
    pwally = checky;
    pwalldir = dir;
    pwallstate = 1;
    pwallpos = 0;
    pwalltile = tilemap[pwallx][pwally];
    tilemap[pwallx][pwally] = 64;
    tilemap[pwallx+dx][pwally+dy] = 64;
   mapSegs.setAt(1, pwallx, pwally, 0);
   mapSegs.setAt(0, pwallx, pwally, mapSegs(0, player->tilex, player->tiley));
   // set correct floorcode (BrotherTank's fix)

    // IOANCH 20130601: play at location
    PlaySoundLocTile(PUSHWALLSND, pwallx, pwally);
//    Sound::Play (PUSHWALLSND);
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = MovePWalls
// =
//
////////////////////////////////////////////////////////////////////////////////


void MovePWalls ()
{
    int oldblock,oldtile;

    if (!pwallstate)
        return;

    oldblock = pwallstate/128;

    pwallstate += (word)tics;

    if (pwallstate/128 != oldblock)
    {
        // block crossed into a new block
        oldtile = pwalltile;

        //
        // the tile can now be walked into
        //
        tilemap[pwallx][pwally] = 0;
        actorat[pwallx][pwally] = 0;
       mapSegs.setAt(0, pwallx, pwally, player->areanumber + AREATILE);

        int dx=_dirs[pwalldir][0], dy=_dirs[pwalldir][1];
        //
        // see if it should be pushed farther
        //
		// IOANCH 29.09.2012: secretstep3 parameter in action
       if (pwallstate >= (cfg_secretstep3 ? 384 : 256))
          // only move two tiles fix
        {
            //
            // the block has been pushed two tiles
            //
            pwallstate = 0;
            tilemap[pwallx+dx][pwally+dy] = oldtile;
            return;
        }
        else
        {
            int xl,yl,xh,yh;
            xl = (player->x-PLAYERSIZE) >> TILESHIFT;
            yl = (player->y-PLAYERSIZE) >> TILESHIFT;
            xh = (player->x+PLAYERSIZE) >> TILESHIFT;
            yh = (player->y+PLAYERSIZE) >> TILESHIFT;

            pwallx += dx;
            pwally += dy;

            if (actorat[pwallx+dx][pwally+dy]
                || (xl<=pwallx+dx && pwallx+dx<=xh && yl<=pwally+dy && pwally+dy<=yh))
            {
                pwallstate = 0;
                tilemap[pwallx][pwally] = oldtile;
                return;
            }
            actorat[pwallx+dx][pwally+dy] = (objtype *)(uintptr_t) (tilemap[pwallx+dx][pwally+dy] = oldtile);
            tilemap[pwallx+dx][pwally+dy] = 64;
        }
    }

    pwallpos = (pwallstate/2)&63;
}
