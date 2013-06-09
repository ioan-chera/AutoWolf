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


#ifndef IOAN_SECRET_H_
#define IOAN_SECRET_H_

#include "List.h"
#include "m_dllist.h"

//
// ScoreMap
//
// Class for the game strategy map
// Only set on map initialization
//
class ScoreMap
{
    //
    // Solidity
    //
    // Solid block, or door (various kinds)
    //
    enum Solidity
    {
        Free,
        Solid,
        UnlockedDoor,
        LockedDoor1,
        LockedDoor2,
        LockedDoor3,
        LockedDoor4
    };
    
    //
    // PushBlock
    //
    // Secret wall/block
    //
    struct PushBlock
    {
        friend class ScoreMap;
        
        // list link
        DLListItem<PushBlock> link;
        // Block coordinates
        int tilex, tiley;
        // Usable (can be pushed)
        bool usable;
        // Visited by various graph search schemes
        bool visited;
    public:
        //
        // Constructor
        //
        PushBlock(int tx, int ty, bool us) : tilex(tx), tiley(ty), usable(us),
        visited(false)
        {
        }
    };
    
    //
    // ScoreBlock
    //
    // Class for various calculation
    //
    class ScoreBlock
    {
        friend class ScoreMap;

        // Score given by this point
        unsigned points;
        // Region colour
        int region;
        // Possible pushwall here
        PushBlock *secret;
        // Solidity kind
        Solidity solidity;
    public:
        //
        // Constructor
        //
        ScoreBlock() : points(0), region(0), secret(NULL), solidity(Free)
        {
        }
        
        // Reset the block
        void Reset()
        {
            points = 0;
            region = 0;
            secret = NULL;
            solidity = Free;
        }
    };
    
    //
    // RegionConnection
    //
    // Simple structure that consists of a region index and a list of pushwalls
    //
    struct RegionConnection
    {
        int region;
        std::unordered_set<PushBlock *> pushBlocks;
        DLListItem<RegionConnection> link;
    };
    
    // map of the blocks
    ScoreBlock map[MAPSIZE][MAPSIZE];
    // list of pushwalls
    DLList<PushBlock, &PushBlock::link> pushBlocks;
    // Number of regions
    int regionCount;
    // Region graph. Connects each region with the others, with the number of
    // pushblocks as parameter
    DLList<RegionConnection, &RegionConnection::link> *regionNeighList;

    void DeleteRegionNeighList();
    void EmptyMap();
    void EmptyPushBlockList();
    void OutputRegionGraphTGF(FILE *f = stdout) const;
    void RecursiveConnectRegion(int tx, int ty,
                                std::unordered_set<int> &regionSet,
                                std::unordered_set<PushBlock *> &secretSet);
    void RecursiveLabelRegions(int tx, int ty, int colour);
public:
    void ConnectRegions();
    void InitFromMapsegs();
    void LabelRegions();
    void TestPushBlocks();
    
    //
    // Constructor
    //
    ScoreMap() : regionCount(0), regionNeighList(NULL)
    {
        pushBlocks.head = NULL;
    }
    
    //
    // Destructor
    //
    ~ScoreMap()
    {
        DeleteRegionNeighList();
        EmptyPushBlockList();
    }
};
extern ScoreMap scoreMap;

//
// Secret
//
// Module for solving pushwall mazes
//
namespace Secret
{
    // calculate the available score from this current position
    int CalcScore(int tx, int ty);
};

#endif
