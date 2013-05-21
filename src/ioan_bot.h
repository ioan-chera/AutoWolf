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
    //
    // Mood
    //    
    // Daily mood for the bot. Can combine these values.
    //
    enum Mood
    {
        MOOD_TAKEFIRSTEXIT = 0x1,       // also part of MOOD_TAKEFIRSTEXIT
        MOOD_DONTBACKFORSECRETS = 2,
        MOOD_DONTHUNTTREASURE = 4,
        MOOD_DONTHUNTNAZIS = 8,
        MOOD_DONTHUNTSECRETS = 0x10,
        MOOD_JUSTGOTOEXIT = 0x20,
    };
    unsigned mood;
    
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
    } searchstage;
	PathArray path;     // search data structure
	Boolean panic;	// gun logic switch (use gatling gun to kill quickly)
	byte pressuse;	// periodic switch for triggering button-down-only commands
	short retreatwaitdelay, retreatwaitcount, retreat;	// retreat controllers
	int exitx, exity;
	objtype *threater;
	List <objtype *> enemyrecord[MAPSIZE][MAPSIZE];	// map of known enemies
    Boolean explored[MAPSIZE][MAPSIZE];	// map of explored areas
    int knownExitX, knownExitY;
    

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
	objtype *EnemyOnTarget(Boolean solidActors = false) const;
	objtype *EnemyVisible(short *angle, int *distance,
                                 Boolean solidActors = false);
	objtype *EnemyEager() const;

	objtype *DamageThreat(const objtype *targ) const;
	void DoRetreat(Boolean forth = false, objtype *cause = NULL) const;
	objtype *Crossfire(int x, int y, const objtype *objignore = NULL,
                              Boolean justexists = false) const;
	objtype *IsProjectile(int tx, int ty, int dist = 1,
                          short *angle = NULL, int *distance = NULL) const;
	objtype *IsEnemyBlocking(int tx, int ty) const;
	objtype *IsEnemyNearby(int tx, int ty) const;
    
    void ExecuteStrafe(int mx, int my, int nx, int ny, Boolean tryuse) const;
	void MoveByStrafe();
	void ChooseWeapon() const;
	void DoCombatAI(int eangle, int edist);
	void DoNonCombatAI();
	void DoMeleeAI(short eangle, int edist);
	void TurnToAngle(int dangle) const;
    
    void StoreAcquiredData(const uint8_t *digeststring) const;
    void GetExploredData(const uint8_t *digeststring);
    
    void aggregateMoods();
public:
    
    //
    // BotMan
    //
    // Constructor
    //
    BotMan() :
    searchstage(SSGeneral),
    panic(false),
    pressuse(0),
    retreatwaitdelay(0),
    retreatwaitcount(0),
    retreat(0),
    exitx(0),
    exity(0),
    knownExitX(0),
    knownExitY(0),
    threater(NULL),
    mood(0)
    {
        memset(explored, 0, sizeof(explored));
    }
    
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