//
// IOAN_BOT.H
//
// File by Ioan Chera, started 17.05.2012
//
// Basic bot setup. I'm working with classes here!
//

#ifndef __IOAN_BOT_H__
#define __IOAN_BOT_H__

#include "wl_def.h"

//
// BotMan
//
// Bot manager class. Contains basic settings.
//
class BotMan
{
protected:
	// search data structure
	struct SData
	{
		int tilex, tiley;
		int f_score, g_score, h_score;
		int prev, next;
		boolean open;
	};

	static SData *searchset;
	static int searchsize, searchlen;

	// protected variables
	static boolean pathexists, exitfound;
	static byte nothingleft, wakeupfire;
	static int exitx, exity, exfrontx;

	// protected functions
	// Finds the path to an exit (A*)
	static boolean FindPathToExit();
	// Finds the closest object of interest (object, hidden door, exit)
	static boolean FindRandomPath();
	// Finds the exit on the map
	static boolean FindExit();
	// Empty set
	inline static void EmptySet()
	{
		searchsize = searchlen = 0;
		if(searchset)
		{
			free(searchset);
			searchset = NULL;
		}
	}
	// Add set
	static void AddToSet(const SData &data);
	// ApproximateDistance
	static int ApproxDist(int dx, int dy);
	// Object of interest
	static boolean ObjectOfInterest(int dx, int dy);
	// Enemy on spot
	static objtype *EnemyFound(int dx, int dy);
	// Enemy targetted
	static objtype *EnemyOnTarget();
	// Enemy visible
	static objtype *EnemyVisible(short *angle, int *distance);
	// Enemy eager to follow
	static objtype *EnemyEager();
	// Automated and armed
	static objtype *EnemiesArmed();
	// do retreat
	static void DoRetreat();

public:
	static boolean active;	// true if bots are activated

	// Finds the path to walk through
	static void DoCommand();

	// Unfinds the exit (on load and setup)
	inline static void MapInit()
	{
		exitfound = pathexists = false;
		nothingleft = wakeupfire = 0;
		EmptySet();
	}
};

#endif