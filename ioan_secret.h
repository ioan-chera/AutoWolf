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

#include "wl_def.h"
#include "Exception.h"

//
// Secret
//
// Module for solving pushwall mazes
//
namespace Secret
{
    // CalcScore recursive component
    int RecursiveCalcScore(int tx, int ty, bool start = false);
    extern byte visited[MAPSIZE][MAPSIZE];
    // calculate the available score from this current position
    int CalcScore(int tx, int ty);
};

//
// Pushwall
//
class Pushwall
{
protected:
    // Can still be pushed?
    boolean active;
    // Current position
    word tilex, tiley;
    
public:
    Pushwall() : active(true), tilex(0), tiley(0)
    {
    }
    Pushwall(word tilex, word tiley)
    {
        if(tilex >= MAPSIZE || tiley >= MAPSIZE)
            throw Exception("pushwall tilex/tiley out of range");
        this->tilex = tilex;
        this->tiley = tiley;
    }
};

//
// ScoreMap
//
class ScoreMap
{
    
};

#endif
