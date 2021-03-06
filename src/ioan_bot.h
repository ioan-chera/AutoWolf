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
#include "id_us.h"
#include "List.h"
#include "PathArray.h"
#include "StdStringExtensions.h"
#include "MapExploration.h"
#include "ioan_secret.h"

// Changing static to dynamic:
// static keyword disappears WHERE IT APPLIES TO EACH INDIVIDUAL
// put const on functions that don't change
// put a constructor that zeroes the fields (and removes them from implementation
// For now, put a global singleton here and in the implementation

const char masterDirectoryFileName[] = "AutoWolf.data";
#define MASTERDIR_MAPSDIRECTORY "Maps"

extern std::string masterDirectoryFilePath;
extern std::string cfg_dir;
inline static void bot_initializeConfigLocation()
{
	masterDirectoryFilePath = cfg_dir;
	ConcatSubpath(masterDirectoryFilePath, masterDirectoryFileName);
}

//
// HeardEvent
//
// Something generated by sound that the bot may react to
//
struct HeardEvent
{
   fixed x;                // sound coordinates
   fixed y;
   word sound;             // the sound played
   longword time;          // when it was heard. Old sound sources get removed
   objtype *cause;   // the actor who may have produced the sound
   
   longword passed;
	
	void set(fixed ix, fixed iy, word isound, longword itime, objtype* icause, longword ipassed)
	{
		x = ix;
		y = iy;
		sound = isound;
		time = itime;
		cause = icause;
		passed = ipassed;
	}
};

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
    class MoodBox
    {
    public:
        enum Mood
        {
            MOOD_TAKEFIRSTEXIT = 0x1,       // also part of MOOD_TAKEFIRSTEXIT
            MOOD_DONTBACKFORSECRETS = 2,
            MOOD_DONTHUNTTREASURE = 4,
            MOOD_DONTHUNTNAZIS = 8,
            MOOD_DONTHUNTSECRETS = 0x10,
            MOOD_JUSTGOTOEXIT = 0x20,
        };
    private:
        unsigned mood;
        void aggregate()
        {
            if (mood & MOOD_JUSTGOTOEXIT) {
                mood |= MOOD_TAKEFIRSTEXIT;
            }
        }
    public:
        MoodBox() : mood(0)
        {
        }
        void SetMood(unsigned inMood);
        unsigned operator()()
        {
            return mood;
        }
    } moodBox;
    
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
	Boolean8 panic;	// gun logic switch (use gatling gun to kill quickly)
	byte pressuse;	// periodic switch for triggering button-down-only commands
	short retreatwaitdelay, retreatwaitcount, retreat;	// retreat controllers
	int exitx, exity;
	
	List <objtype *> enemyrecord[MAPSIZE][MAPSIZE];	// map of known enemies
	
	bool m_chokemap[MAPSIZE][MAPSIZE];	// map of choke points

   MapExploration mapExploration;	// map of explored areas
    int knownExitX, knownExitY;
    
    objtype *threater;
    
   RandomGenerator botRnd;
    

	void ExploreFill(int tx, int ty, fixed ox, fixed oy,
                            Boolean8 firstcall = false);

	Boolean8 FindShortestPath(Boolean8 ignoreproj = false,
                                    Boolean8 mindnazis = false,
                                    byte retreating = 0,
                                    Boolean8 knifeinsight = false);

	void MoveStrafe(short tangle, short dangle, Boolean8 tryuse,
                           byte pressuse, int nx, int ny);

   bool secretVerify(int tx, int ty, int txofs, int tyofs);
	Boolean8 ObjectOfInterest(int dx, int dy,
                                    Boolean8 knifeinsight = false);
	objtype *EnemyOnTarget(Boolean8 solidActors = false) const;
	objtype *EnemyVisible(short *angle, int *distance,
                                 Boolean8 solidActors = false);
	objtype *EnemyEager() const;

	objtype *DamageThreat(const objtype *targ) ;
	void DoRetreat(Boolean8 forth = false, objtype *cause = nullptr) const;
	objtype *Crossfire(int x, int y, const objtype *objignore = nullptr,
                              Boolean8 justexists = false) ;
	objtype *IsProjectile(int tx, int ty, int dist = 1,
                          short *angle = nullptr, int *distance = nullptr) const;
	objtype *IsEnemyBlocking(int tx, int ty) const;
	objtype *IsEnemyNearby(int tx, int ty) const;
    
    void ExecuteStrafe(int mx, int my, int nx, int ny, bool tryuse) const;
	void MoveByStrafe();
	void ChooseWeapon() const;
	void DoCombatAI(int eangle, int edist);
	void DoNonCombatAI();
	void DoMeleeAI(short eangle, int edist);
	void TurnToAngle(int dangle) const;
    
    void StoreAcquiredData(const uint8_t *digeststring) const;
    void GetExploredData(const uint8_t *digeststring);
   
   HeardEvent *RefreshSound(HeardEvent *he);
	
	void getChokeData();
public:
   
	std::vector<SecretPush> pushes;
	bool haspushes;
    void SetPushList(const std::vector<SecretPush> &pushes);

   List <HeardEvent *> heardEvents;                // list of recent sounds
   
    //
    // BotMan
    //
    // Constructor
    //
    BotMan() :
    moodBox(),
    searchstage(SSGeneral),
    panic(false),
    pressuse(0),
    retreatwaitdelay(0),
    retreatwaitcount(0),
    retreat(0),
    exitx(0),
    exity(0),
    mapExploration(),
    knownExitX(0),
    knownExitY(0),
    threater(NULL),
	haspushes(false)
    {

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
