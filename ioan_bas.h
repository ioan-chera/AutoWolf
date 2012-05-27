//
// IOAN_BAS.H
//
// File by Ioan Chera, started 17.05.2012
//
// Basic general setup. New functions and such placed here, instead of randomly through the exe
//

#ifndef __IOAN_BAS_H__
#define __IOAN_BAS_H__

#include "wl_def.h"

//
// Basic
//
// General functions class, to manipulate global functions
//
class Basic
{
protected:
	// Moved some global functions here
	static void SpawnStand (enemy_t which, int tilex, int tiley, int dir);
	static void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir);

#ifndef SPEAR
	static void SpawnBoss (int tilex, int tiley);
	static void SpawnGretel (int tilex, int tiley);
	static void SpawnGhosts (int which, int tilex, int tiley);
	static void SpawnSchabbs (int tilex, int tiley);
	static void SpawnGift (int tilex, int tiley);
	static void SpawnFat (int tilex, int tiley);
	static void SpawnFakeHitler (int tilex, int tiley);
	static void SpawnHitler (int tilex, int tiley);
#else
	static void SpawnTrans (int tilex, int tiley);
	static void SpawnUber (int tilex, int tiley);
	static void SpawnWill (int tilex, int tiley);
	static void SpawnDeath (int tilex, int tiley);
	static void SpawnAngel (int tilex, int tiley);
	static void SpawnSpectre (int tilex, int tiley);
#endif
	static int markov[27][27], marktot[27];

public:
	static boolean nonazis;	// no enemies spawned

	// Spawns a Nazi (originally they were separate functions; no more)
	static void SpawnEnemy(enemy_t which, int tilex, int tiley, int dir = 0, boolean patrol = false);

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
};

#endif