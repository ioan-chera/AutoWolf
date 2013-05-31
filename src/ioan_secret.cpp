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

#include "wl_def.h"

#include "ioan_secret.h"
#include "ioan_bas.h"

#include "wl_act1.h"
#include "wl_game.h"
#include "wl_play.h"
#include "obattrib.h"

DLList<PushBlock, &PushBlock::link> Secret::pushBlockList = {NULL};

// static block used by score calculators
static byte visited[MAPSIZE][MAPSIZE];

//
// RecursiveCalcScore
//
// Recursive function for CalcScore
//
static int RecursiveCalcScore(int tx, int ty, Boolean start = false)
{
    static int totalscore = 0;
    
    if(tx <= 0 || ty <= 0 || tx >= MAPSIZE - 1 || ty >= MAPSIZE - 1)
        return totalscore;

    objtype *check = actorat[tx][ty];
    byte door = tilemap[tx][ty];

    if(visited[tx][ty])
        return totalscore;

    if(door & 0x80)    // on door
    {
        // abort if it's locked, closed and I don't have key for it
        door = door & ~0x80;
        byte lock = doorobjlist[door].lock;
        if (lock >= dr_lock1 && lock <= dr_lock4)
            if (doorobjlist[door].action != dr_open &&
                doorobjlist[door].action != dr_opening &&
                !(gamestate.keys & (1 << (lock-dr_lock1))))
            {
                return totalscore;
            }
    }
    else if(check && !ISPOINTER(check))
        return totalscore;    // solid, impassable

    if(start)
    {
        totalscore = 0;
        // reset visited-map
        memset(visited, 0, MAPSIZE*MAPSIZE*sizeof(byte));
    }
    int score = 0;

    visited[tx][ty] = 1;

    byte objid;
    for(objid = Basic::FirstObjectAt(tx, ty); objid;
        objid = Basic::NextObjectAt(tx, ty))
    {
        // FIXME: put these into a table
        switch(objid)
        {
        case bo_cross:
            score += I_CROSSSCORE;
            break;
        case bo_chalice:
            score += I_CHALICESCORE;
            break;
        case bo_bible:
            score += I_TREASURESCORE;
            break;
        case bo_crown:
            score += I_CROWNSCORE;
            break;
        case bo_fullheal:
            score += I_CROWNSCORE << 1;
            break;
        }
    }
    // Possible error: multiple actors sharing same spot, only one is counted
    // (maybe even a corpse?)
    if(check && ISPOINTER(check) && check->hitpoints > 0)
        score += atr::points[check->obclass];

    // now add the score to the total recursive score
    totalscore += score;

    RecursiveCalcScore(tx + 1, ty);
    RecursiveCalcScore(tx - 1, ty);
    RecursiveCalcScore(tx, ty + 1);
    RecursiveCalcScore(tx, ty - 1);

    return totalscore;
}

//
// ScoreMap::InitFromSegs
//
// Initializes the map from the mapsegs
// Startup only for now
//
void ScoreMap::InitFromSegs()
{
    for(unsigned i = 0; i < MAPSIZE; ++i)
    {
        for(unsigned j = 0; j < MAPSIZE; ++j)
        {
            objtype *check = actorat[i][j];
            byte door = tilemap[i][j];
            if (door & 0x80)
            {
                map[i][j].solidity = Solidity::UnlockedDoor;
                byte doorlock = doorobjlist[door & ~0x80].lock;
                if (doorlock >= dr_lock1 && doorlock <= dr_lock4)
                {
                    map[i][j].solidity = (Solidity)(doorlock
                                            + (byte)Solidity::UnlockedDoor);
                }
            }
            if (check && !ISPOINTER(check))
            {
                // solid block
                map[i][j].solidity = Solidity::Solid;
            }
        }
    }
}

//
// Secret::CalcScore
//
// calculate the available score from this current position
//
int Secret::CalcScore(int tx, int ty)
{
    int ret = 0;

    ret = RecursiveCalcScore(tx, ty, true);
    
    return ret;
}

//
// Secret::CountMapSecrets
//
// Gets the list of all map secrets
//
void Secret::CountMapSecrets()
{
    // use MAPSPOT(x, y, 1) to scan for secret block
    // MAPSIZE is the map side length
    PushBlock *added;
    for (unsigned i = 0; i < MAPSIZE; ++i)
    {
        for(unsigned j = 0; j < MAPSIZE; ++j)
        {
            if(MAPSPOT(i, j, 1) == PUSHABLETILE)
            {
                // Found a secret wall
                added = new PushBlock(i, j);
                pushBlockList.insert(added);
            }
        }
    }
}