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
 
 WL_ACT2 DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_ACT2_H_
#define WL_ACT2_H_

#define s_nakedbody s_static10

// IOANCH 29.06.2012: deleted global ghosts moved to Basic

void SpawnDeadGuard (int tilex, int tiley);

void A_DeathScream (objtype *ob);
void SpawnBJVictory ();

void    T_Projectile (objtype *ob);
void A_Smoke (objtype *ob);
void    T_Stand (objtype *ob);
void    T_Path (objtype *ob);
void    A_Shoot (objtype *ob);
void    T_Chase (objtype *ob);
void    T_Ghosts (objtype *ob);
void    A_Bite (objtype *ob);
void    T_DogChase (objtype *ob);
void A_UShoot (objtype *ob);
void    T_ProjectileBossChase (objtype *ob);
void A_Launch (objtype *ob);
void A_Slurpie (objtype *ob);
void A_Victory (objtype *ob);
void A_Relaunch (objtype *ob);

void A_StartAttack (objtype *ob);
void A_Breathing (objtype *ob);
void A_Dormant (objtype *ob);
void    A_StartDeathCam (objtype *ob);
void A_DeathScream (objtype *ob);
void    A_SchabbThrow (objtype *ob);
void    T_Fake (objtype *ob);
void    A_FakeFire (objtype *ob);


void A_HitlerMorph (objtype *ob);
void A_MechaSound (objtype *ob);
void    A_GiftThrow (objtype *ob);

void T_BJRun (objtype *ob);
void T_BJJump (objtype *ob);
void A_BJDone (objtype *ob);
void A_BJYell (objtype *ob);


#endif

