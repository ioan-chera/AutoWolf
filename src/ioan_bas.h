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

//
// Basic
//
// General functions namespace, to manipulate global functions
//
namespace Basic
{
	extern List<void *> livingNazis, thrownProjectiles;
	
	void EmptyItemList();	// empty itemList
	void AddItemToList(int tx, int ty, byte itemtype);
	void RemoveItemFromList(int tx, int ty, byte itemtype);
	byte FirstObjectAt(int tx, int ty);
	byte NextObjectAt(int tx, int ty);


	// Spawns a Nazi (originally they were separate functions; no more)
	void SpawnEnemy(classtype cl, const Point2D<int> &tilePoint, int dir = 0,
                           Boolean patrol = false, enemy_t ghost = en_blinky);

	// Checks if it's an enemy
	Boolean IsEnemy(classtype cls);
	// if it's a boss (no pain chance)
	Boolean IsBoss(classtype cls);

	// Check if it's armed
	Boolean IsDamaging(objtype *ret, int dist);

	// Generic check line
	Boolean GenericCheckLine (const Point2D<int> &point1,
                              const Point2D<int> &point2,
                              Boolean solidActors = false);
    
    // Check if can knife
    objtype *CheckKnifeEnemy();

	// Get coordinates
	inline fixed Major(int t)
	{
		return (t << TILESHIFT) + (1 << (TILESHIFT - 1));
	}
    inline Point2D<fixed> Major(const Point2D<int> &p)
    {
        Point2D<fixed> ret;
        ret.x = Major(p.x);
        ret.y = Major(p.y);
        return ret;
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
	inline int DirAngle(const Point2D<int> &mPoint,
                        Point2D<int> nPoint)
	{
        nPoint = Basic::Major(nPoint);
		
        Point2D<double> dPoint;
        
		dPoint.x = (double)(nPoint.x - mPoint.x);
		dPoint.y = -(double)(nPoint.y - mPoint.y);
		double ang = atan2(dPoint.y, dPoint.x);
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
    int ApproxDist(const Point2D<int> &dPoint);
    
    PString &SetStringTildeExpand(const char *basedir, PString &dest);

};

#endif