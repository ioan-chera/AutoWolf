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
////////////////////////////////////////////////////////////////////////////////
//
// Dealing with secret wall puzzles in Wolfenstein
//
////////////////////////////////////////////////////////////////////////////////

#include "wl_def.h"

#include "ioan_secret.h"
#include "ioan_bas.h"

#include "wl_act1.h"
#include "wl_game.h"
#include "wl_play.h"
#include "obattrib.h"

// The score map singleton
ScoreMap scoreMap;

// static block used by score calculators
static byte visited[MAPSIZE][MAPSIZE];

//
// GetScore
//
// Gets score of specified stat obj
//
inline static int GetScore(byte objid)
{
    switch(objid)
    {
        case bo_cross:
        case bo_chalice:
        case bo_bible:
        case bo_crown:
            return atr::treasures[objid - bo_cross].points;
        case bo_fullheal:
            return I_CROWNSCORE << 1;
        default:
            return 0;
    }
}

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
        score += GetScore(objid);
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

////////////////////////////////////////////////////////////////////////////////
//
// ScoreMap private functions
//
////////////////////////////////////////////////////////////////////////////////

//
// ScoreMap::ClearData
//
// Clears the entire data, either for destruction or rewriting upon level start
//
void ScoreMap::ClearData()
{
    for(unsigned i = 0; i < MAPSIZE; ++i)
        for(unsigned j = 0; j < MAPSIZE; ++j)
            map[i][j].Reset();
    pushBlocks.killAll();
    regions.killAll();
    regionCount = 0;
}

//
// ScoreMap::InitFromLevelMap
//
// Initializes the map from the mapsegs
// Startup only for now
//
void ScoreMap::InitFromLevelMap()
{
    // Reset from previous setup
    ClearData();
    
    // Scan for actorat or tilemap
    for(unsigned i = 0; i < MAPSIZE; ++i)
    {
        for(unsigned j = 0; j < MAPSIZE; ++j)
        {
            // Scan for either solid blocks (for solidity) or enemies (for score)
            objtype *check = actorat[i][j];
            bool solidblock = false;
            if (check)
            {
                if(!ISPOINTER(check))   // is a wall or static blocking object
                {
                    // Scan for doors and set solidity
                    byte door = tilemap[i][j];
                    if (door & 0x80)
                    {
                        map[i][j].solidity = UnlockedDoor;
                        byte doorlock = doorobjlist[door & ~0x80].lock;
                        if (doorlock >= dr_lock1 && doorlock <= dr_lock4)
                        {
                            map[i][j].solidity = (Solidity)(doorlock
                                                    + (byte)UnlockedDoor);
                        }
                    }
                    else
                    {
                        // solid block
                        map[i][j].solidity = Solid;
                    }
                    solidblock = true;
                }
                else
                {
                    // is an actor
                    map[i][j].points = (unsigned)atr::points[check->obclass];
                }
            }
            
            // Scan for secrets
            if (MAPSPOT(i, j, 1) == PUSHABLETILE)
            {
                PushBlock *pushBlock = new PushBlock(i, j, solidblock);
                map[i][j].secret = pushBlock;
                pushBlocks.add(pushBlock);
            }
        }
    }
    
    // Scan for items
    for (statobj_t *statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
    {
        if(!(statptr->flags & FL_BONUS))
            continue;
        map[statptr->tilex][statptr->tiley].points
        = GetScore(statptr->itemnumber);
    }
    
    // How to map each region: have an array of map indices
}

//
// ScoreMap::RecursiveLabelRegions
//
// Recursively floodfills the tiles to do region colouring
//
void ScoreMap::RecursiveLabelRegions(int tx, int ty, Region *region)
{
    if(tx < 0 || ty < 0 || tx >= MAPSIZE || ty >= MAPSIZE)
        return;
    if(map[tx][ty].solidity == Solid)
        return;
    if(map[tx][ty].region)
        return;
    map[tx][ty].region = region;
    RecursiveLabelRegions(tx - 1, ty, region);
    RecursiveLabelRegions(tx + 1, ty, region);
    RecursiveLabelRegions(tx, ty - 1, region);
    RecursiveLabelRegions(tx, ty + 1, region);
}

//
// ScoreMap::LabelRegions()
//
// Colours the regions as delimited by solid walls (but not doors)
// Needed for secret solving
//
void ScoreMap::LabelRegions()
{
    for(unsigned i = 0; i < MAPSIZE; ++i)
        for(unsigned j = 0; j < MAPSIZE; ++j)
            if(map[i][j].solidity != Solid && !map[i][j].region)
            {
                ++regionCount;
                Region *region = new Region;
                RecursiveLabelRegions(i, j, region);
                regions.add(region);
            }
}

//
// ScoreMap::RecursiveConnectRegion
//
// Scans all adjacent tiles, going through pushwalls, until encountering a
// region.
// Then it adds that region to a set, which will connect all regions with edges
//
void ScoreMap::RecursiveConnectRegion(int tx, int ty,
                                      std::unordered_set<Region *> &regionSet,
                                      std::unordered_set<PushBlock *> &secretSet)
{
    if(tx < 0 || ty < 0 || tx >= MAPSIZE || ty >= MAPSIZE)
        return;
    
    PushBlock *sec = map[tx][ty].secret;
    if(sec && !sec->visited)
    {    // Is a secret. What do? Move around
        sec->visited = true;
        secretSet.insert(sec);
        
        RecursiveConnectRegion(tx - 1, ty, regionSet, secretSet);
        RecursiveConnectRegion(tx + 1, ty, regionSet, secretSet);
        RecursiveConnectRegion(tx, ty - 1, regionSet, secretSet);
        RecursiveConnectRegion(tx, ty + 1, regionSet, secretSet);
    }
    if (map[tx][ty].region)
    {
        // Add this region to the set of connected
        
        regionSet.insert(map[tx][ty].region);
        
        return;
    }
}

//
// ScoreMap::ConnectRegions
//
// Connect the regions based on secret blocks
//
void ScoreMap::ConnectRegions()
{
    for(PushBlock *entry = pushBlocks.firstObject(); entry; 
        entry = pushBlocks.nextObject())
    {        
        // Initialize the data
        std::unordered_set<Region *> regionSet;
        std::unordered_set<PushBlock *> secretSet;
        
        // Do the recursive work
        RecursiveConnectRegion(entry->tilex, entry->tiley, regionSet, secretSet);
        
        // What should the final result be:
        // An array of regions. Each region will be associated with some neigh-
        // bour regions. Each such neighbour will be associated with a set of
        // pushwalls. It will also be either considerable or trivial
        // It's trivial if it only has one pushwall, and it can only be pushed
        // from one direction.
        
        // What I get from the beginning: a set of blocks and a set of regions,
        // from the static space. I have to take every combination of such re-
        // gions and edit the graph to have them neighbours with the other,
        // adding those pushwalls there
        
        // How to add them: I already have regionNeighList created from the la-
        // belling stage. For each region entry, I
        // scan the List for the second combination, and if it's not there, I
        // add it with the obtained secret set. If it's already there, I just
        // merge the secretSet there.
        
        // no sets obtained of either type, move on
        if(secretSet.size() < 1 || regionSet.size() < 1)
            continue;
        
        // Auxiliary function to do the connection
        // FIXME: make it a member function
        auto doConnection = [&](Region *reg1, Region *reg2)
        {
            bool found = false;
            for (RegionConnection *entry = reg1->neighList.firstObject();
                 entry;
                 entry = reg1->neighList.nextObject())
            {
                if (entry->region == reg2)
                {
                    // Neighbour region already set, just add the blocks
                    entry->pushBlocks.insert(secretSet.begin(), 
                                             secretSet.end());
                    found = true;
                    break;  // done
                }
            }
            if(!found)
            {
                RegionConnection *connection = new RegionConnection;
                connection->region = reg2;
                connection->pushBlocks = secretSet;
                reg1->neighList.add(connection);
            }
        };
        
        if (regionSet.size() == 1)
            doConnection(*regionSet.begin(), *regionSet.begin());
        else
        {
            for (auto it1 = regionSet.begin(); it1 != regionSet.end(); ++it1)
            {
                for (auto it2 = it1; it2 != regionSet.end(); ++it2)
                {
                    if(it2 == it1)
                        continue;
                    doConnection(*it1, *it2);
                    doConnection(*it2, *it1);
                }
            }
        }
    }
}

//
// ScoreMap::OutputRegionGraphTGF
//
// Writes the contents of the regionNeighList to a Trivial Graph Format text
// file, which can be read by utilities such as yWorks yEd (free of charge)
//
// Format is very simple:
// - list of node indices and their names, one for each line
// - a line with a hash character ('#') as separator
// - list of edges, represented by pairs of node indices, optionally with label
//
void ScoreMap::OutputRegionGraphTGF(FILE *f) 
{
    // Write regions
    for(Region *region = regions.firstObject();
        region;
        region = regions.nextObject())
    {
        // print each node and its label (same label as the index) per line
        fprintf(f, "%llu %llu\n", (unsigned long long)region,
                (unsigned long long)region);
    }
    fprintf(f, "#\n");
    for(Region *reg1 = regions.firstObject();
        reg1;
        reg1 = regions.nextObject())
    {
        for (RegionConnection *obj = reg1->neighList.firstObject();
             obj;
             obj = reg1->neighList.nextObject())
        {
            Region *reg2 = obj->region;
            // First, print the pair
            if((unsigned long long)reg2 >= (unsigned long long)reg1)
            {
                fprintf(f, "%llu %llu", (unsigned long long)reg1,
                        (unsigned long long)reg2);
                unsigned blockCount = 0;
                for (auto it = obj->pushBlocks.begin();
                     it != obj->pushBlocks.end();
                     ++it)
                {
                    blockCount++;
                }
                // Print the number of walls, if larger than 0
                if (blockCount > 0)
                    fprintf(f, " %u", blockCount);
                fprintf(f, "\n");
            }
        }
    }
}

//
// ScoreMap::TestPushBlocks
//
// Function used for testing blocks
//
void ScoreMap::TestPushBlocks()
{
    puts("Testing push blocks...");

    for (PushBlock *entry = pushBlocks.firstObject();
         entry;
         entry = pushBlocks.nextObject())
    {
        printf("tx=%d ty=%d us=%d\n", entry->tilex,
               entry->tiley,
               entry->usable);
    }
    OutputRegionGraphTGF(stdout);
    printf("THEN\n");
    for(Region *region = regions.firstObject();
        region;
        region = regions.nextObject())
    {
        printf("%llu has: ", (unsigned long long)region);
        for(RegionConnection *connection = region->neighList.firstObject();
            connection; connection = region->neighList.nextObject())
        {
            for(RegionConnection::PushPosition *a =
                connection->pushPositions.firstObject(); 
                a; a = connection->pushPositions.nextObject())
            {
                printf("(%d %d)-(%d %d) ", a->tx, a->ty, a->block->tilex, 
                       a->block->tiley);
            }
        }
        printf("\n");
    }
}

//
// ScoreMap::GetPushPositions
//
// For each region and its neighbours, it looks for actually pushable blocks
//
static struct {char tx, ty;} rel[] = 
{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
void ScoreMap::GetPushPositions()
{
    for(Region *region = regions.firstObject(); region; 
        region = regions.nextObject())
    {
        for(RegionConnection *neigh = region->neighList.firstObject();
            neigh;
            neigh = region->neighList.nextObject())
        {
            for(auto it = neigh->pushBlocks.begin(); 
                it != neigh->pushBlocks.end(); ++it)
            {
                PushBlock *block = *it;
                if(!block->usable)
                    continue;
                for(char j = 0; j < 4; ++j)
                {
                    int reltx = block->tilex + rel[j].tx, 
                        relty = block->tiley + rel[j].ty;
                    if(map[reltx][relty].region == region)
                    {
                        // Got one of mine. See the other side now.
                        if (!actorat[block->tilex - rel[j].tx]
                                    [block->tiley - rel[j].ty]) 
                        {
                            // Free spot. So it's pushable
                            RegionConnection::PushPosition *pp 
                            = new RegionConnection::PushPosition;
                            pp->tx = reltx;
                            pp->ty = relty;
                            pp->block = block;
                            neigh->pushPositions.add(pp);
                        }
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// ScoreMap public functions
//
////////////////////////////////////////////////////////////////////////////////

//
// ScoreMap::Build
//
// Does the entire score-map reset and build process, called from SetupGameLevel
//
void ScoreMap::Build()
{
    InitFromLevelMap();
    LabelRegions();
    ConnectRegions();
    GetPushPositions();
    TestPushBlocks();
}

////////////////////////////////////////////////////////////////////////////////
//
// Secret functions
//
////////////////////////////////////////////////////////////////////////////////

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
