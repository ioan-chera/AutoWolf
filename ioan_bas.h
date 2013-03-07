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

#include "wl_def.h"
#include "List.h"

//class LinkList;

//
// Basic
//
// General functions class, to manipulate global functions
//
class Basic
{
protected:
	// Moved some global functions here
	static objtype *SpawnStand (classtype which, int tilex, int tiley, int dir);
	static objtype *SpawnPatrol (classtype which, int tilex, int tiley, int dir);
	static objtype *SpawnBoss (classtype which, int tilex, int tiley);
	
    // IOANCH 20130202: unification process
	static void SpawnGhosts (int which, int tilex, int tiley);
    
	static int markov[27][27], marktot[27];
	
	static List<byte> itemList[MAPSIZE][MAPSIZE];	// list of items per map tile

public:
	static boolean nonazis;	// no enemies spawned
	static boolean secretstep3;	// make secret walls go three steps (for troublesome maps)
	static List<void *> livingNazis, thrownProjectiles;
	
	static void EmptyItemList();	// empty itemList
	static void AddItemToList(int tx, int ty, byte itemtype)
	{
		itemList[tx][ty].add(itemtype);
	}
	static void RemoveItemFromList(int tx, int ty, byte itemtype)
	{
		itemList[tx][ty].remove(itemtype);
	}
	static byte FirstObjectAt(int tx, int ty)
	{
		return itemList[tx][ty].firstObject();
	}
	static byte NextObjectAt(int tx, int ty)
	{
		return itemList[tx][ty].nextObject();
	}


	// Spawns a Nazi (originally they were separate functions; no more)
	static void SpawnEnemy(classtype cl, int tilex, int tiley, int dir = 0, 
                           boolean patrol = false, enemy_t ghost = en_blinky);

	// Checks if it's an enemy
	static boolean IsEnemy(classtype cls)
	{
		switch(cls)
		{
		case guardobj:
		case officerobj:
		case ssobj:
		case dogobj:
		case bossobj:
		case schabbobj:
		case fakeobj:
		case mechahitlerobj:
		case mutantobj:
		case ghostobj:
		case realhitlerobj:
		case gretelobj:
		case giftobj:
		case fatobj:
		case spectreobj:
		case angelobj:
		case transobj:
		case uberobj:
		case willobj:
		case deathobj:
			return true;
		default:	// silence compiler
			;
		}
		return false;
	}
	// if it's dangerous
	static boolean IsAutomatic(classtype cls)
	{
		switch(cls)
		{
		case ssobj:
		case bossobj:
		case fakeobj:
		case mechahitlerobj:
		case mutantobj:
		case realhitlerobj:
		case gretelobj:
		case fatobj:
		case transobj:
		case uberobj:
		case willobj:
		case deathobj:
			return true;
		default:
			;
		}
		return false;
	}
	// if it's a boss (no pain chance)
	static boolean IsBoss(classtype cls)
	{
		switch(cls)
		{
		case bossobj:
		case schabbobj:
		case fakeobj:
		case mechahitlerobj:
		case realhitlerobj:
		case giftobj:
		case gretelobj:
		case fatobj:
		case transobj:
		case uberobj:
		case willobj:
		case deathobj:
		case angelobj:
			return true;
			default:
				;
		}
		return false;
	}

	// Check if it's armed
	static boolean IsDamaging(objtype *ret, int dist);

	// Generic check line
	static boolean GenericCheckLine (int x1, int y1, int x2, int y2);

	// Get coordinates
	inline static int Major(int t)
	{
		return (t << TILESHIFT) + (1 << (TILESHIFT - 1));
	}

	// Write a random name
	static void MarkovWrite(char *c, int nmax);
	
	// Centre the angle
	static int CentreAngle(int whatangle, int centangle)
	{
		int dangle = whatangle - centangle;
		if(dangle > 180)
			dangle -= 360;
		else if(dangle <= -180)
			dangle += 360;
		return dangle;
	}
	
	// get angle from dir
	static int DirAngle(int mx, int my, int nx, int ny)
	{
		nx = Basic::Major(nx);
		ny = Basic::Major(ny);
		
		double dx = (double)(nx - mx);
		double dy = -(double)(ny - my);
		double ang = atan2(dy, dx);
		int rang = (int)(180.0/PI*ang);
		while(rang < 0)
			rang += 360;
		while(rang >= 360)
			rang -= 360;
		return rang/5*5;
		
		/*
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
		return 0;*/
	}
	
	// ApproximateDistance
	static int ApproxDist(int dx, int dy);
    
    static char *NewStringTildeExpand(const char *basedir);

};

#endif