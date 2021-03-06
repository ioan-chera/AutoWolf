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


#ifndef IOAN_BAS_H_
#define IOAN_BAS_H_

#include "List.h"
#include "PString.h"

// projectile-shooter correspondence
struct ProjShooter
{
   objtype *proj, *shooter;
};


//
// Basic
//
// General functions namespace, to manipulate global functions
//
namespace Basic
{
	extern List<objtype *> livingNazis;
   extern List<ProjShooter *> thrownProjectiles;
	
	void EmptyItemList();	// empty itemList
	void AddItemToList(int tx, int ty, byte itemtype);
	void RemoveItemFromList(int tx, int ty, byte itemtype);
	byte FirstObjectAt(int tx, int ty);
	byte NextObjectAt(int tx, int ty);


	// Spawns a Nazi (originally they were separate functions; no more)
	void SpawnEnemy(classtype cl, int tilex, int tiley, int dir = 0, 
                           Boolean8 patrol = false, enemy_t ghost = en_blinky);

	// Checks if it's an enemy
	Boolean8 IsEnemy(classtype cls);
	// if it's a boss (no pain chance)
	Boolean8 IsBoss(classtype cls);

	// Check if it's armed
	Boolean8 IsDamaging(objtype *ret, int dist);

	// Generic check line
	Boolean8 GenericCheckLine (int x1, int y1, int x2, int y2, 
                              Boolean8 solidActors = false);
    
    // Check if can knife
    objtype *CheckKnifeEnemy();

	// Get coordinates
	inline fixed Major(int t)
	{
		return (t << TILESHIFT) + (1 << (TILESHIFT - 1));
	}

	// Write a random name
	void MarkovWrite(char *c, int nmax);
	
	// Centre the angle
	inline int CentreAngle(int whatangle, int centangle)
	{
		int dangle = whatangle - centangle;
		if(dangle > 180)
			dangle -= 360;
		else if(dangle <= -180)
			dangle += 360;
		return dangle;
	}
	
	// get angle from dir
	inline int DirAngle(int mx, int my, int nx, int ny)
	{
		
		double dx = (double)(nx - mx);
		double dy = -(double)(ny - my);
		double ang = atan2(dy, dx);
		int rang = (int)(180.0/PI*ang);
		while(rang < 0)
			rang += 360;
		while(rang >= 360)
			rang -= 360;
		return rang/5*5;
		
#if 0
		if(nx > mx && ny == my)
			return 0;
		if(ny > my && nx == mx)
			return 270;
		if(nx < mx && ny == my)
			return 180;
		if(ny < my && nx == mx)
			return 90;
		
		if(nx > mx && ny < my)
			return 45;
		if(nx < mx && ny < my)
			return 135;
		if(nx < mx && ny > my)
			return 225;
		if(nx > mx && ny > my)
			return 315;
		return 0;
#endif
	}
	
	// ApproximateDistance
	int ApproxDist(int dx, int dy);
    
   bool IsGunShotSound(word s);  // returns true if sound is gunshot
   bool IsInFront(short angle, fixed cx, fixed cy, fixed tx, fixed ty);

   void GetFacingAngle(int &checkx, int &checky);
};

#endif