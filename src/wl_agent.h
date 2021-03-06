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

/*
 =============================================================================
 
 WL_AGENT DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_AGENT_H_
#define WL_AGENT_H_

extern  short         anglefrac;
extern  int           facecount, facetimes;
extern  int32_t       thrustspeed;
extern  objtype      *LastAttacker;

namespace Agent
{
    void    GetBonus (statobj_t *check);
}

void    Thrust (int angle, int32_t speed);
void    SpawnPlayer (int tilex, int tiley, int dir);
void    TakeDamage (int points,objtype *attacker);
void    GivePoints (int32_t points);
void    GiveWeapon (int weapon);
void    GiveAmmo (int ammo);
void    GiveKey (int key);

//
// player state info
//

void    StatusDrawFace(unsigned picnum);
void    DrawFace ();
void    DrawHealth ();
void    HealSelf (int points);
void    DrawLevel ();
void    DrawLives ();
void    GiveExtraMan ();
void    DrawScore ();
void    DrawWeapon ();
void    DrawKeys ();
void    DrawAmmo ();

#endif