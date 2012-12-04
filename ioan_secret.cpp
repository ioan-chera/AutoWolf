//
// IOAN_SECRET.H
//
// File by Ioan Chera, started 25.10.2012
//
// Functions handling the secret maze solving logic
//
#include "ioan_secret.h"
#include "ioan_bas.h"

byte Secret::visited[MAPSIZE][MAPSIZE];

//
// Secret::CalcScoreAdd
//
// Recursive function for CalcScore
//
int Secret::CalcScoreAdd(int tx, int ty, bool start)
{
	static int totalscore = 0;

	objtype *check = actorat[tx][ty];
	byte door = tilemap[tx][ty];

	if(visited[tx][ty])
		return totalscore;

	if(door & 0x80)	// on door
	{
		// abort if it's locked, closed and I don't have key for it
		door = door & ~0x80;
		byte lock = doorobjlist[door].lock;
		if (lock >= dr_lock1 && lock <= dr_lock4)
			if (doorobjlist[door].action != dr_open &&
				doorobjlist[door].action != dr_opening && !(gamestate.keys & (1 << (lock-dr_lock1) ) ) )
			{
				return totalscore;
			}
	}
	else if(check && !ISPOINTER(check))
		return totalscore;	// solid, impassable

	if(start)
	{
		totalscore = 0;
		memset(visited, 0, MAPSIZE*MAPSIZE*sizeof(byte));	// reset visited-map
	}
	int score = 0;

	visited[tx][ty] = 1;

	byte objid;
	for(objid = Basic::FirstObjectAt(tx, ty); objid; objid = Basic::NextObjectAt(tx, ty))
	{
		switch(objid)
		{
		case bo_cross:
			score += I_CROSSSCORE;
			break;
		case bo_chalice:
			score += I_CHALICESCORE;
			break;
		case bo_bible:
			score += I_TREASURESCORE;
			break;
		case bo_crown:
			score += I_CROWNSCORE;
			break;
		case bo_fullheal:
			score += I_CROWNSCORE << 1;
			break;
		}
	}
	// Possible error: multiple actors sharing same spot, only one is counted (maybe even a corpse?)
	if(check && ISPOINTER(check) && check->hitpoints > 0)
	{
		switch(check->obclass)
		{
		case guardobj:
			score += I_GUARDSCORE;
			break;
		case officerobj:
			score += I_OFFICERSCORE;
			break;
		case ssobj:
			score += I_SSSCORE;
			break;
		case dogobj:
			score += I_DOGSCORE;
			break;
		case mutantobj:
			score += I_MUTANTSCORE;
			break;
		case fakeobj:
			score += I_FAKEHITLERSCORE;
			break;
		case spectreobj:
			score += I_SPECTRESCORE;
			break;
		case bossobj:
		case schabbobj:
		case mechahitlerobj:
		case realhitlerobj:
		case gretelobj:
		case giftobj:
		case fatobj:
		case angelobj:
		case transobj:
		case uberobj:
		case willobj:
		case deathobj:
				score += I_BOSSSCORE;
			default:
				;
		}
	}

	// now add the score to the total recursive score
	totalscore += score;

	CalcScoreAdd(tx + 1, ty);
	CalcScoreAdd(tx - 1, ty);
	CalcScoreAdd(tx, ty + 1);
	CalcScoreAdd(tx, ty - 1);

	return totalscore;
}

//
// Secret::CalcScore
//
// calculate the available score from this current position
//
int Secret::CalcScore(int tx, int ty)
{
	int ret = 0;

	ret = CalcScoreAdd(tx, ty, true);
	
	return ret;
}

//

