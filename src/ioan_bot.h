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

#include "HistoryRatio.h"
#include "List.h"
#include "PathArray.h"

enum BotMood
{
    MOOD_TAKEFIRSTEXIT = 0x1,       // also part of MOOD_TAKEFIRSTEXIT
    MOOD_DONTBACKFORSECRETS = 2,
    MOOD_DONTHUNTTREASURE = 4,
    MOOD_DONTHUNTNAZIS = 8,
    MOOD_DONTHUNTSECRETS = 0x10,
    MOOD_JUSTGOTOEXIT = 0x20,
};

// Changing static to dynamic:
// static keyword disappears WHERE IT APPLIES TO EACH INDIVIDUAL
// put const on functions that don't change
// put a constructor that zeroes the fields (and removes them from implementation
// For now, put a global singleton here and in the implementation

//
// BotMan
//
// Bot manager class. Contains basic settings.
//
class BotMan
{
protected:
    
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
        SSNormalLift,	// only now look for normal exit
        SSMax
    };
    
	// search data structure
	PathArray path;

	// protected variables
	Boolean panic;	// gun logic switch (use gatling gun to kill quickly)
	byte pressuse;	// periodic switch for triggering button-down-only commands
	short retreatwaitdelay, retreatwaitcount, retreat;	// retreat (cover) controllers
	SearchStage searchstage;
	int exitx, exity;
	objtype *threater;
	List <objtype *> enemyrecord[MAPSIZE][MAPSIZE];	// map of known enemy locations

    unsigned mood;
    
    //
    // DATA GATHERED FROM DATABASE
    //
    Boolean explored[MAPSIZE][MAPSIZE];	// map of explored areas
    Point2D<int> knownExitPoint;
    

	void ExploreFill(int tx, int ty, int ox, int oy,
                            Boolean firstcall = false);

	Boolean FindShortestPath(Boolean ignoreproj = false,
                                    Boolean mindnazis = false,
                                    byte retreating = 0,
                                    Boolean knifeinsight = false);

	void MoveStrafe(short tangle, short dangle, Boolean tryuse,
                           byte pressuse, int nx, int ny);

	Boolean ObjectOfInterest(int dx, int dy,
                                    Boolean knifeinsight = false);
	objtype *EnemyOnTarget(Boolean solidActors = false);
	objtype *EnemyVisible(short *angle, int *distance,
                                 Boolean solidActors = false);
	objtype *EnemyEager();

	objtype *DamageThreat(objtype *targ);
	void DoRetreat(Boolean forth = false, objtype *cause = NULL);
	objtype *Crossfire(int x, int y, objtype *objignore = NULL,
                              Boolean justexists = false);
	objtype *IsProjectile(int tx, int ty, int dist = 1,
                                 short *angle = NULL, int *distance = NULL);
	objtype *IsEnemyBlocking(int tx, int ty);
	objtype *IsEnemyNearby(int tx, int ty);
    
	void MoveByStrafe();
	void ChooseWeapon();
	void DoCombatAI(int eangle, int edist);
	void DoNonCombatAI();
	void DoMeleeAI(short eangle, int edist);
	void TurnToAngle(int dangle);
    
    void StoreAcquiredData(const uint8_t *digeststring) const;
    void GetExploredData(const uint8_t *digeststring);
public:
	// Update the enemy's known position record
	void RecordEnemyPosition(objtype *enemy);

    objtype *damagetaken;
	HistoryRatio shootRatio;

	// Finds the path to walk through
	void DoCommand();

	// Unfinds the exit (on load and setup)
	void MapInit();
	
	// Saves data to data file
	void SaveData() const;
    
    // Set occasional mood
    void SetMood();
};
extern BotMan bot;

#endif