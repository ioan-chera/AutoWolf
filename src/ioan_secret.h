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


#ifndef IOAN_SECRET_H_
#define IOAN_SECRET_H_

#include <array>
#include <stack>
#include <vector>
#include "SODFlag.h"
#include "wl_def.h"
#include "id_us.h"

// Secret push action: contains coordinates of wall and player that pushes it
class SecretPush
{
public:
	unsigned targetpos, sourcepos, prerequisite;
	SecretPush() : targetpos(0), sourcepos(0)
	{
	}
	SecretPush(unsigned a, unsigned b, unsigned c) : targetpos(a), sourcepos(b), prerequisite(c)
	{
	}
};

// Big class that solves secret puzzles. Needs to be class because it's run from threads and we don't want race conditions on shared resources
class SecretSolver
{
	// Total score one can achieve in the level
	unsigned totalscore;

	// Number of such objects
	unsigned totalsecrets;
	unsigned totalenemies;
	unsigned totaltreasure;

	// Current map
	unsigned mapnum;
	
	// Current player position
	unsigned playerpos;

	// Current operational items
	std::array<uint16_t, maparea> *wallbuf;
	std::array<uint16_t, maparea> *actorbuf;

	// Map state stacks
	// Optim note: the underlying structure should be vector
	std::stack<std::array<uint16_t, maparea>> pushstates;
	std::stack<std::array<uint16_t, maparea>> actorstates;

	// Player position states
	std::stack<unsigned> posstates;

	// Helper stuff
	struct Inventory
	{
		unsigned score;							// score
		unsigned keys;							// key inventory (uses flags in impl)
		unsigned treasurecount, enemycount;		// amount of gathered items or killed nazis
		uint8_t maxsecrets, maxenemies, maxtreasures;	// THESE ARE FLAGS (0/1). They mean whether maximum was reached

		const SecretSolver* o;					// reference to owning object

		Inventory(const SecretSolver* ss) :
		score(0),
		keys(0),
		treasurecount(0), enemycount(0),
		maxsecrets(0), maxenemies(0), maxtreasures(0),
		o(ss)
		{
		}

		unsigned totalscore() const
		{
			return score + (o->MapHasTally() ? 10000 * (maxsecrets + maxenemies + maxtreasures) : o->MapHasAutobonus() ? 15000 : 0);
		}
	};

	// Item pickup states
	std::stack<Inventory> scorestates;

	// Secret push index states
	std::stack<unsigned> choicestates;

	// Secret push list states
	std::stack<std::vector<SecretPush>> secretliststates;

    unsigned maxscore;  // maximum score obtained so far
    std::stack<unsigned> maxchoicestates;
    std::stack<std::vector<SecretPush>> maxsecretliststates;

	RandomGenerator rnd;

	// 30000 score max.
	bool MapHasTally() const
	{
		unsigned n = mapnum;
		if (!SPEAR::flag)
			return n <= 7;
		else
			return n <= 3 || (n >= 5 && n <= 8) || (n >= 10 && n <= 14) || n == 16;
	}

	// 15000 score
	bool MapHasAutobonus() const
	{
		return !SPEAR::flag ? mapnum == 9 : mapnum == 4 || mapnum == 9 || mapnum == 15 || mapnum == 18 || mapnum == 19;
	}

	// 0 score
	bool MapHasCutscene() const
	{
		return !SPEAR::flag ? mapnum == 8 : mapnum == 17 || mapnum == 20;
	}

	bool IsWall(unsigned kind) const;
	bool IsSoftWall(unsigned kind) const;
	bool IsSecretFree(unsigned pos) const;
	bool IsSecret(unsigned targetpos, unsigned sourcepos) const;
	bool IsExit(unsigned targetpos) const
	{
		return (*wallbuf)[targetpos] == 21;
	}
	bool OperateWall(unsigned targetpos, unsigned sourcepos, unsigned index);
	bool OperateWall(SecretPush pair, unsigned index);
	unsigned UndoSecret();

public:
	
	SecretSolver() :
	totalscore(0),
	totalsecrets(0),
	totalenemies(0),
	totaltreasure(0),
	mapnum(0),
	playerpos(0),
    maxscore(0),
	wallbuf(nullptr),
	actorbuf(nullptr)
	{
		rnd.initialize(static_cast<int>(time(nullptr)));
	}

	void GetLevelData();
	std::vector<SecretPush> Solve(unsigned sessionNo);
	
	static void SetSpearModuleValues();
};

#endif
