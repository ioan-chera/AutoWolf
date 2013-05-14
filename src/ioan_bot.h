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


#ifndef IOAN_BOT_H_
#define IOAN_BOT_H_

#include "wl_def.h"
#include "List.h"
class HistoryRatio;
class PathArray;

//
// SearchStage
//
// Separate search objects into stages. Must be in order here
//
enum SearchStage
{
	SSGeneral,		// search normal things
	SSOnePushWalls,	// only now look for one-push-walls (TO BE MODIFIED)
	SSSecretLift,	// only now look for secret exit
	SSNormalLift	// only now look for normal exit
};

//
// BotMan
//
// Bot manager class. Contains basic settings.
//
class BotMan
{
protected:
	// search data structure
	static PathArray path;

	// protected variables
	static Boolean panic;	// gun logic switch (use gatling gun to kill quickly)
	static byte pressuse;	// periodic switch for triggering button-down-only commands
	static short retreatwaitdelay, retreatwaitcount, retreat;	// retreat (cover) controllers
	static SearchStage searchstage;
	static int exitx, exity;
	static objtype *threater;
	static Boolean explored[MAPSIZE][MAPSIZE];	// map of explored areas
	static List <objtype *> enemyrecord[MAPSIZE][MAPSIZE];	// map of known enemy locations

	// protected functions
	// Recursively explores from the given origin
	static void ExploreFill(int tx, int ty, int ox, int oy, Boolean firstcall = false);
	// Finds the closest object of interest (object, hidden door, exit)
	static Boolean FindShortestPath(Boolean ignoreproj = false, Boolean mindnazis = false, byte retreating = 0, Boolean knifeinsight = false);
	// move by strafing
	static void MoveStrafe(short tangle, short dangle, Boolean tryuse, byte pressuse, int nx, int ny);
	// Object of interest
	static Boolean ObjectOfInterest(int dx, int dy, Boolean knifeinsight = false);
	// Enemy on spot
	static objtype *EnemyFound(int dx, int dy);
	// Enemy targetted
	static objtype *EnemyOnTarget(Boolean solidActors = false);
	// Enemy visible
	static objtype *EnemyVisible(short *angle, int *distance, Boolean solidActors = false);
	// Enemy eager to follow
	static objtype *EnemyEager();
	// Automated and armed
	static objtype *DamageThreat(objtype *targ);
	// do retreat
	static void DoRetreat(Boolean forth = false, objtype *cause = NULL);
	// like EnemiesArmed, but not restricted to the player
	static objtype *Crossfire(int x, int y, objtype *objignore = NULL, Boolean justexists = false);
	// Like enemyvisible, but for any spot
	static objtype *GenericEnemyVisible(int tx, int ty);
	// Test if there's a projectile there
	static objtype *IsProjectile(int tx, int ty, int dist = 1, short *angle = NULL, int *distance = NULL);
	// Test if there's an enemy
	static objtype *IsEnemyBlocking(int tx, int ty);
	static objtype *IsEnemyNearby(int tx, int ty);
	// Move by path only by strafing
	static void MoveByStrafe();
	// Choose weapon
	static void ChooseWeapon();
	// Do combat AI
	static void DoCombatAI(int eangle, int edist);
	// Do noncombat AI
	static void DoNonCombatAI();
	// Do knife AI (when guns won't work)
	static void DoMeleeAI(short eangle, int edist);
	// See if there's an active behind the corner that may be knifed
	static objtype *DogGnawing(int *eangle = NULL);
	// Turn the player
	static void TurnToAngle(int dangle);
public:
	// Update the enemy's known position record
	static void RecordEnemyPosition(objtype *enemy);

    static objtype *damagetaken;
	static HistoryRatio shootRatio;

	// Finds the path to walk through
	static void DoCommand();

	// Unfinds the exit (on load and setup)
	static void MapInit();
	
	// Saves data to data file
	static void SaveData();
	
	// Loads data from file
	static void LoadData();
};

#endif