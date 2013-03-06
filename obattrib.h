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

#ifndef Wolf4SDL_obattrib_h
#define Wolf4SDL_obattrib_h

#include "wl_def.h"

#define ATR_NEARBY_THREAT 1

//
// atr
//
// Actor info attributes
//
namespace atr
{
    //
    // IOANCH 20130305: class attributes
    //
    // Hit points
    extern const int hitpoints[][4];
    // Moving speeds
    struct speed
    {
        int patrol;
        int chase;
    };
    extern const speed speeds[];
    // Sounds
    struct sound
    {
        int sight;
    };
    extern const sound sounds[];
    // States
    struct state
    {
        statetype *stand;
        statetype *patrol;
        statetype *chase;
        statetype *pain;
        statetype *altpain;
    };
    extern const state states[];
    // Spawn actions
    struct action
    {
        void (* spawn)();
        void (* die)();
    };
    extern const action actions[];
    // threat range
    extern const int threatrange[];
    // other flags
    extern const uint64_t flags[];
};


#endif
