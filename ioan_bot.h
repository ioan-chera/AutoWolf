//
// IOAN_BOT.H
//
// File by Ioan Chera, started 17.05.2012
//
// Basic bot setup. I'm working with classes here!
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
	static boolean panic;	// gun logic switch (use gatling gun to kill quickly)
	static byte pressuse;	// periodic switch for triggering button-down-only commands
	static short retreatwaitdelay, retreatwaitcount, retreat;	// retreat (cover) controllers
	static SearchStage searchstage;
	static int exitx, exity;
	static objtype *threater;
	static boolean explored[MAPSIZE][MAPSIZE];	// map of explored areas
	static List <objtype *> enemyrecord[MAPSIZE][MAPSIZE];	// map of known enemy locations

	// protected functions
	// Recursively explores from the given origin
	static void ExploreFill(int tx, int ty, int ox, int oy, boolean firstcall = false);
	// Finds the closest object of interest (object, hidden door, exit)
	static boolean FindShortestPath(boolean ignoreproj = false, boolean mindnazis = false, byte retreating = 0, boolean knifeinsight = false);
	// move by strafing
	static void MoveStrafe(short tangle, short dangle, boolean tryuse, byte pressuse, int nx, int ny);
	// Object of interest
	static boolean ObjectOfInterest(int dx, int dy, boolean knifeinsight = false);
	// Enemy on spot
	static objtype *EnemyFound(int dx, int dy);
	// Enemy targetted
	static objtype *EnemyOnTarget();
	// Update the enemy's known position record
	static void RecordEnemyPosition(objtype *enemy);
	// Enemy visible
	static objtype *EnemyVisible(short *angle, int *distance);
	// Enemy eager to follow
	static objtype *EnemyEager();
	// Automated and armed
	static objtype *DamageThreat(objtype *targ);
	// do retreat
	static void DoRetreat(boolean forth = false, objtype *cause = NULL);
	// like EnemiesArmed, but not restricted to the player
	static objtype *Crossfire(int x, int y, objtype *objignore = NULL, boolean justexists = false);
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
	static boolean DoMeleeAI();
	// See if there's an active behind the corner that may be knifed
	static objtype *DogGnawing(int *eangle = NULL);
	// Turn the player
	static void TurnToAngle(int dangle);
public:
	static boolean active;	// true if bots are activated
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