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
    class PushBlock
    {
        friend class ScoreMap;
        
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
    // RegionConnection
    //
    // Simple structure that consists of a region index and a list of pushwalls
    //
    class Region;
    struct RegionConnection
    {
        struct PushPosition 
        {
            int tx, ty;
            PushBlock *block;
            Boolean operator ==(const PushPosition &other) const {return tx == other.tx && ty == other.ty && block == other.block;}
        };
        Region *region;
        std::unordered_set<PushBlock *> pushBlocks;
        std::unordered_set<PushPosition *> pushPositions;
    };
    
    class ScoreBlock;
    typedef std::pair<int, int> IntPair;
    //
    // Region
    //
    // Map cell connected to other regions by pushwalls
    //
    class Region
    {
    public:
        // Each region will have its list of pairs of 'map' indices
        List<IntPair *> map;
        // Region graph. Connects each region with the others, with the number of
        // pushblocks as parameter
        List<RegionConnection *> neighList;
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
        Region *region;
        // Possible pushwall here
        PushBlock *secret;
        // Solidity kind
        Solidity solidity;
    public:
        //
        // Constructor
        //
        ScoreBlock() : points(0), region(NULL), secret(NULL), solidity(Free)
        {
        }
        
        // Reset the block
        void Reset()
        {
            points = 0;
            region = NULL;
            secret = NULL;
            solidity = Free;
        }
    };
    
    // map of the blocks
    ScoreBlock map[MAPSIZE][MAPSIZE];
    // list of pushwalls
    List<PushBlock *> pushBlocks;
    // Number of regions
    int regionCount;
    // Region list
    List<Region *> regions;

    void ClearData();
    void ConnectRegions();
    void DeleteRegionNeighList();
    void GetPushPositions();
    void InitFromLevelMap();
    void LabelRegions();
    void OutputRegionGraphTGF(FILE *f = stdout);
    void RecursiveConnectRegion(int tx, int ty,
                                std::unordered_set<Region *> &regionSet,
                                std::unordered_set<PushBlock *> &secretSet);
    void RecursiveLabelRegions(int tx, int ty, Region *region);
    void TestPushBlocks();
    
public:
    void Build();
    
    //
    // Constructor
    //
    ScoreMap() : regionCount(0)
    {
    }
    
    //
    // Destructor
    //
    ~ScoreMap()
    {
        ClearData();
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
