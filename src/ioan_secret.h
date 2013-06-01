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
        // Block coordinates
        int tilex, tiley;
        // Usable
        bool usable;
        // list link
        DLListItem<PushBlock> link;
    public:
        //
        // Constructor
        //
        PushBlock(int tx, int ty, bool us) : tilex(tx), tiley(ty), usable(us)
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
        
        Solidity solidity;
        // Score given by this point
        unsigned points;
        // Possible pushwall here
        PushBlock *secret;
        
    public:
        //
        // Constructor
        //
        ScoreBlock() : solidity(Free), points(0), secret(NULL)
        {
        }
        
        // Reset the block
        void Reset()
        {
            solidity = Free;
            points = 0;
            secret = NULL;
        }
    };
    
    // map of the blocks
    ScoreBlock map[MAPSIZE][MAPSIZE];
    // list of pushwalls
    DLList<PushBlock, &PushBlock::link> pushBlocks;
    
    void EmptyMap();
    void EmptyPushBlockList();
public:
    void InitFromMapsegs();
    void TestPushBlocks();
    
    //
    // Constructor
    //
    ScoreMap() : pushBlocks({NULL})
    {
    }
    
    //
    // Destructor
    //
    ~ScoreMap()
    {
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
