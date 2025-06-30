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
////////////////////////////////////////////////////////////////////////////////
//
// Dealing with secret wall puzzles in Wolfenstein
//
////////////////////////////////////////////////////////////////////////////////

#include <queue>
#include "id_ca.h"
#include "ioan_secret.h"
#include "obattrib.h"
#include "wl_act1.h"
#include "wl_game.h"
#include "wl_play.h"
#include "Logger.h"

static const unsigned timelimit = 10000;

// Key inventory stuff
enum KeyFlags
{
	KEY_1 = 1,
	KEY_2 = 2,
	KEY_3 = 4,
	KEY_4 = 8,
};

inline static unsigned GetX(unsigned what)
{
	return what % MAPSIZE;
}

inline static unsigned GetY(unsigned what)
{
	return what / MAPSIZE;
}

inline static bool IsPlayerStart(unsigned kind)
{
	return kind >= PLAYER_START_NORTH && kind <= PLAYER_START_WEST;
}

// Chosen to be 512 items long

static const unsigned pointsFor[] = {
//	  0    1    2    3     4     5    6    7    8    9    10    11    12    13    14   15    16    17   18   19   20   21    22    23    24    25   26   27   28    29

	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0,	//   0
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,  100,  500, 1000, 5000,   0,   0,   0,    0,	//  30
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0,	//  60
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0, 5000, 5000, 100, 100, 100, 100,  100,  100,  100,  100, 400, 400, 400,  400,	//  90
	400, 400, 400, 400,    0, 5000, 500, 500, 500, 500,  500,  500,  500,  500,  200, 200,  200,  200, 200, 200, 200, 200, 5000, 5000,  100,  100, 100, 100, 100,  100,	// 120
	100, 100, 400, 400,  400,  400, 400, 400, 400, 400, 2000, 5000,  500,  500,  500, 500,  500,  500, 500, 500, 200, 200,  200,  200,  200,  200, 200, 200,   0, 5000,	// 150
	100, 100, 100, 100,  100,  100, 100, 100, 400, 400,  400,  400,  400,  400,  400, 400, 5000, 5000, 500, 500, 500, 500,  500,  500,  500,  500, 200, 200, 200,  200,	// 180
	200, 200, 200, 200, 5000, 5000, 700, 700, 700, 700,  700,  700,  700,  700,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,  700,  700, 700, 700, 700,  700,	// 210
	700, 700,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,  700,  700,  700, 700,  700,  700, 700, 700,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0,	// 240
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 270
  	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 300
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 330
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 360
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 390
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 420
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 450
	  0,   0,   0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,    0,   0,    0,    0,   0,   0,   0,   0,    0,    0,    0,    0,   0,   0,   0,    0, // 480
	  0,   0,                                                                                                                                                           // 510

};

// Doesn't matter whether i choose macro or small/inline stuff here

bool SecretSolver::IsSoftWall(unsigned pos) const
{
	return Act1::GetStaticType((*actorbuf)[pos]) == block;
}

bool SecretSolver::IsWall(unsigned kind) const
{
	if ((*wallbuf)[kind] && (*wallbuf)[kind] < DOOR_VERTICAL_1)
		return true;
	return IsSoftWall(kind);
}

inline static unsigned PointsFor(unsigned kind)
{
	return kind < lengthof(pointsFor) ? pointsFor[kind] : 0;
}

static bool IsTreasure(unsigned kind, int &score)
{
	wl_stat_t type = Act1::GetStaticType(kind);
	switch(type)
	{
	case bo_cross:
	case bo_chalice:
	case bo_bible:
	case bo_crown:
		score = atr::treasures[type - bo_cross].points;
		return true;
	case bo_fullheal:
		score = 0;
		return true;
	default:
		return false;
	}
	return false;
}
bool SecretSolver::IsSecretFree(unsigned pos) const
{
	if (IsWall(pos))
		return false;
	if (PointsFor((*actorbuf)[pos]) && ((*actorbuf)[pos] < 52 || (*actorbuf)[pos] >= 56))
		return false;
	if ((*actorbuf)[pos] == 124)
		return false;
	return true;
}

bool SecretSolver::IsSecret(unsigned targetpos, unsigned sourcepos) const
{
	if ((*actorbuf)[targetpos] != PUSHABLETILE)	// secret spot
		return false;
	if (!(*wallbuf)[targetpos] || (*wallbuf)[targetpos] >= 106)	// not a wall
		return false;
	if (IsSecretFree(targetpos + targetpos - sourcepos))	// can push
		return true;
	return false;
}

bool SecretSolver::OperateWall(unsigned targetpos, unsigned sourcepos, unsigned index)
{
	if (!IsSecret(targetpos, sourcepos))
		return false;

	//printf("Pushing %s from %s\n", Coords(targetpos).c_str(), Coords(sourcepos).c_str());

	pushstates.push(std::array<uint16_t, maparea>());
	actorstates.push(std::array<uint16_t, maparea>());
	posstates.push(sourcepos);
	scorestates.push(scorestates.top());
	choicestates.push(index);

	memcpy(pushstates.top().data(), wallbuf->data(), pushstates.top().size() * sizeof(uint16_t));
	memcpy(actorstates.top().data(), actorbuf->data(), actorstates.top().size() * sizeof(uint16_t));

	// Push the wall and update current state
	wallbuf = &pushstates.top();
	actorbuf = &actorstates.top();
	playerpos = posstates.top();

	uint16_t walltype = (*wallbuf)[targetpos];
	// We already determined that the next spot is empty. See if the second next is.
	if (IsSecretFree(targetpos + (targetpos - sourcepos) * 2))
		(*wallbuf)[targetpos + (targetpos - sourcepos) * 2] = walltype;
	else
		(*wallbuf)[targetpos + (targetpos - sourcepos)] = walltype;
	(*wallbuf)[targetpos] = (*wallbuf)[sourcepos];
	(*actorbuf)[targetpos] = 0;	// consume secret trigger
	return true;
}

bool SecretSolver::OperateWall(SecretPush pair, unsigned index)
{
	return OperateWall(pair.targetpos, pair.sourcepos, index);
}

//
// Checks if a push is certain to be unique per block
//
bool SecretSolver::CheckSurePush(unsigned targetpos, unsigned sourcepos)
{
    // Check if the player can reach any adjacent position to targetpos, other than sourcepos
    // But first clear all non-targetpos spots
    std::queue<unsigned> tiles;
    std::vector<bool> visited;
    visited.resize(maparea);
    visited[playerpos] = true;
    tiles.push(playerpos);

    while(!tiles.empty())
    {
        unsigned pos = tiles.front();
        tiles.pop();
        if(pos % MAPSIZE < MAPSIZE - 2)
        {
            if(pos + 1 != targetpos && (!IsWall(pos + 1) || (*actorbuf)[pos + 1] == PUSHABLETILE) &&
               !visited[pos + 1])
            {
                tiles.push(pos + 1);
                visited[pos + 1] = true;
            }
            else if(pos + 1 == targetpos && pos != sourcepos)
                return false;
        }
        else if(pos % MAPSIZE > 1)
        {
            if(pos - 1 != targetpos && (!IsWall(pos - 1) || (*actorbuf)[pos - 1] == PUSHABLETILE) &&
               !visited[pos - 1])
            {
                tiles.push(pos - 1);
                visited[pos - 1] = true;
            }
            else if(pos - 1 == targetpos && pos != sourcepos)
                return false;
        }
        else if(pos / MAPSIZE < MAPSIZE - 2)
        {
            if(pos + MAPSIZE != targetpos && (!IsWall(pos + MAPSIZE) || (*actorbuf)[pos + MAPSIZE] == PUSHABLETILE) &&
               !visited[pos + MAPSIZE])
            {
                tiles.push(pos + MAPSIZE);
                visited[pos + MAPSIZE] = true;
            }
            else if(pos + MAPSIZE == targetpos && pos != sourcepos)
                return false;
        }
        else if(pos / MAPSIZE > 1)
        {
            if(pos - MAPSIZE != targetpos && (!IsWall(pos - MAPSIZE) || (*actorbuf)[pos - MAPSIZE] == PUSHABLETILE) &&
               !visited[pos - MAPSIZE])
            {
                tiles.push(pos - MAPSIZE);
                visited[pos - MAPSIZE] = true;
            }
            else if(pos - MAPSIZE == targetpos && pos != sourcepos)
                return false;
        }
    }
    // Can push it
    uint16_t walltype = (*wallbuf)[targetpos];
    // We already determined that the next spot is empty. See if the second next is.
    if (IsSecretFree(targetpos + (targetpos - sourcepos) * 2))
        (*wallbuf)[targetpos + (targetpos - sourcepos) * 2] = walltype;
    else
        (*wallbuf)[targetpos + (targetpos - sourcepos)] = walltype;
    (*wallbuf)[targetpos] = (*wallbuf)[sourcepos];
    (*actorbuf)[targetpos] = 0;    // consume secret trigger
    return true;
}

unsigned SecretSolver::UndoSecret()
{
	if (pushstates.size() <= 1)
		return (unsigned)-1;

	pushstates.pop();
	wallbuf = &pushstates.top();

	actorstates.pop();
	actorbuf = &actorstates.top();

	posstates.pop();
	playerpos = posstates.top();

	scorestates.pop();

	unsigned index = choicestates.top();
	choicestates.pop();
	return index + 1;
}

// THIS WILL BE CALLED FROM A BACKGROUND THREAD
// Optim note: use unchecked array accessors ([]) instead of at(). Speed gets doubled.
std::vector<SecretPush> SecretSolver::Solve(unsigned sessionNo)
{
	Uint32 ticks = SDL_GetTicks();

    auto finalize = [](std::stack<unsigned> &choicestates,
                       std::stack<std::vector<SecretPush>> &secretliststates,
                       std::vector<SecretPush> &finality)
    {
        while (!choicestates.empty() && !secretliststates.empty())
        {
            SecretPush push = secretliststates.top()[choicestates.top()];
            finality.push_back(push);

            choicestates.pop();
            secretliststates.pop();
        }
    };

    auto maxpushes = [finalize, this]()
    {
        if(maxscore > 0)
        {
            Logger::Write("Got maximum score %u\n", maxscore);
            std::vector<SecretPush> finality;
            finalize(maxchoicestates, maxsecretliststates, finality);
            return finality;
        }
        return std::vector<SecretPush>();
    };

	try
	{
		if (!GetX(playerpos) || !GetY(playerpos) || GetX(playerpos) >= MAPSIZE - 1 || GetY(playerpos) >= MAPSIZE - 1)
			return maxpushes();

		std::queue<unsigned> tiles;
		std::array<std::stack<unsigned>, 4> lockedtiles;

		secretliststates.push(std::vector<SecretPush>());
		std::vector<SecretPush> *secrets = &secretliststates.top();

		tiles.push(playerpos);

		std::array<uint8_t, maparea> visited;
		visited.fill(0);
		visited[playerpos] = 2;

		unsigned needed, neededindex = 0;
		unsigned secretindex = 0;
		unsigned pos;
		unsigned exitcount = 0;

		unsigned prerequisite = 0;

		bool consumeactor;
//		unsigned count = 0;
		do
		{
			if (SDL_GetTicks() - ticks >= timelimit || sessionNo != g_sessionNo)
			{
				//				printf("Count: %u\n", count);
				return maxpushes();
			}

			//			++count;

			if (secretindex < secrets->size())
			{
				SecretPush pair = (*secrets)[secretindex];

				//puts("push");
				OperateWall(pair, secretindex);

				if (totalsecrets && MapHasTally() && choicestates.size() == totalsecrets)
					scorestates.top().maxsecrets = 1;

				secretindex = 0;
				secretliststates.push(std::vector<SecretPush>());
				secrets = &secretliststates.top();

				// Empty door lists
				for (auto stck : lockedtiles)
				while (!stck.empty())
					stck.pop();
				exitcount = 0;
				while (!tiles.empty())
					tiles.pop();
				tiles.push(playerpos);
				visited.fill(0);
				visited[playerpos] = 2;
			}

			prerequisite = 0;
			while (!tiles.empty())
			{

				pos = tiles.front();
				tiles.pop();

				needed = 0;
				if (visited[pos] == 2)
					switch ((*wallbuf)[pos])
				{
					case DOOR_VERTICAL_2:
					case DOOR_HORIZONTAL_2:
						//printf("Found door %u\n", KEY_1);
						needed = KEY_1;
						neededindex = 0;
						break;
					case DOOR_VERTICAL_3:
					case DOOR_HORIZONTAL_3:
						//printf("Found door %u\n", KEY_2);
						needed = KEY_2;
						neededindex = 1;
						break;
					case DOOR_VERTICAL_4:
					case DOOR_HORIZONTAL_4:
						needed = KEY_3;
						neededindex = 2;
						break;
					case DOOR_VERTICAL_5:
					case DOOR_HORIZONTAL_5:
						needed = KEY_4;
						neededindex = 3;
						break;
				}

				if (needed && !(needed & scorestates.top().keys))
				{
					//printf("No key available\n");
					// no key available here
					lockedtiles[neededindex].push(pos);
					continue;
				}

				consumeactor = false;
				if (PointsFor((*actorbuf)[pos]))
				{
					if ((*actorbuf)[pos] >= 52 && (*actorbuf)[pos] < 56)
					{
						if (visited[pos] == 2)
						{
							scorestates.top().treasurecount++;
							scorestates.top().score += PointsFor((*actorbuf)[pos]);
							consumeactor = true;
							prerequisite = pos;
						}
					}
					else
					{
						scorestates.top().enemycount++;
						scorestates.top().score += PointsFor((*actorbuf)[pos]);
						consumeactor = true;
					}
				}

				if (visited[pos] == 2 && (*actorbuf)[pos] == 56 && MapHasTally())
				{
					scorestates.top().treasurecount++;
					(*actorbuf)[pos] = 0;
					prerequisite = pos;
				}

				neededindex = (unsigned)-1;
				if (visited[pos] == 2)
					switch ((*actorbuf)[pos])
				{
					case 43:
					case 214:
					case 197:
					case 125:
					case 142:
					case 143:
					case 161:
						//printf("Found key %u\n", KEY_1);
						scorestates.top().keys |= KEY_1;
						neededindex = 0;
						break;
					case 44:
						//printf("Found key %u\n", KEY_2);
						scorestates.top().keys |= KEY_2;
						neededindex = 1;
						break;
					case 45:
						scorestates.top().keys |= KEY_3;
						neededindex = 2;
						break;
					case 46:
						scorestates.top().keys |= KEY_4;
						neededindex = 3;
						break;
				}

				if (neededindex < 4 || consumeactor)
				{
					(*actorbuf)[pos] = 0;	// consume it.
					prerequisite = pos;
				}

				if (neededindex < 4)
				{
					//printf("Emptying stack %u\n", neededindex);
					while (!lockedtiles[neededindex].empty())
					{
						//printf("Popping pos (%u %u)\n", lockedtiles.at(neededindex).top() % WOLF3D_MAPSIZE, lockedtiles.at(neededindex).top() / WOLF3D_MAPSIZE);
						tiles.push(lockedtiles[neededindex].top());
						lockedtiles[neededindex].pop();
					}
				}

				if (pos % MAPSIZE > 1)
				{
					if (!IsWall(pos - 1) && visited[pos - 1] < visited[pos])
					{
						tiles.push(pos - 1);
						visited[pos - 1] = visited[pos];
					}
					else if (IsSoftWall(pos - 1) && visited[pos - 1] < 1)
					{
						tiles.push(pos - 1);
						visited[pos - 1] = 1;
					}
					else if (visited[pos] == 2 && IsSecret(pos - 1, pos))
					{
						//printf("Found secret from %u %u to %u %u\n", pos % WOLF3D_MAPSIZE, pos / WOLF3D_MAPSIZE, (pos - 1) % WOLF3D_MAPSIZE, (pos - 1) / WOLF3D_MAPSIZE);
                        if(!CheckSurePush(pos - 1, pos))
                            secrets->push_back(SecretPush(pos - 1, pos, 0));
                        else
                        {
                            tiles.push(pos - 1);
                            visited[pos - 1] = 2;
                        }
					}
					else if (visited[pos] == 2 && IsExit(pos - 1))
						exitcount++;
				}

				if (pos % MAPSIZE < MAPSIZE - 2)
				{
					if (!IsWall(pos + 1) && visited[pos + 1] < visited[pos])	// maximum legal is 62 = mapsize - 2
					{
						tiles.push(pos + 1);
						visited[pos + 1] = visited[pos];
					}
					else if (IsSoftWall(pos + 1) && visited[pos + 1] < 1)
					{
						tiles.push(pos + 1);
						visited[pos + 1] = 1;
					}
					else if (visited[pos] == 2 && IsSecret(pos + 1, pos))
					{
						//printf("Found secret from %u %u to %u %u\n", pos % WOLF3D_MAPSIZE, pos / WOLF3D_MAPSIZE, (pos + 1) % WOLF3D_MAPSIZE, (pos + 1) / WOLF3D_MAPSIZE);
                        if(!CheckSurePush(pos + 1, pos))
                            secrets->push_back(SecretPush(pos + 1, pos, 0));
                        else
                        {
                            tiles.push(pos + 1);
                            visited[pos + 1] = 2;
                        }
					}
					else if (visited[pos] == 2 && IsExit(pos + 1))
						exitcount++;
				}

				if (pos / MAPSIZE > 1)
				{
					if (!IsWall(pos - MAPSIZE) && visited[pos - MAPSIZE] < visited[pos])
					{
						tiles.push(pos - MAPSIZE);
						visited[pos - MAPSIZE] = visited[pos];
					}
					else if (IsSoftWall(pos - MAPSIZE) && visited[pos - MAPSIZE] < 1)
					{
						tiles.push(pos - MAPSIZE);
						visited[pos - MAPSIZE] = 1;
					}
					else if (visited[pos] == 2 && IsSecret(pos - MAPSIZE, pos))
					{
						//printf("Found secret from %u %u to %u %u\n", pos % WOLF3D_MAPSIZE, pos / WOLF3D_MAPSIZE, (pos - WOLF3D_MAPSIZE) % WOLF3D_MAPSIZE, (pos - WOLF3D_MAPSIZE) / WOLF3D_MAPSIZE);
                        if(!CheckSurePush(pos - MAPSIZE, pos))
                            secrets->push_back(SecretPush(pos - MAPSIZE, pos, 0));
                        else
                        {
                            tiles.push(pos - MAPSIZE);
                            visited[pos - MAPSIZE] = 2;
                        }
					}
				}

				if (pos / MAPSIZE < MAPSIZE - 2)
				{
					if (!IsWall(pos + MAPSIZE) && visited[pos + MAPSIZE] < visited[pos])
					{
						tiles.push(pos + MAPSIZE);
						visited[pos + MAPSIZE] = visited[pos];
					}
					else if (IsSoftWall(pos + MAPSIZE) && visited[pos + MAPSIZE] < 1)
					{
						tiles.push(pos + MAPSIZE);
						visited[pos + MAPSIZE] = 1;
					}
					else if (visited[pos] == 2 && IsSecret(pos + MAPSIZE, pos))
					{
						//printf("Found secret from %u %u to %u %u\n", pos % WOLF3D_MAPSIZE, pos / WOLF3D_MAPSIZE, (pos + WOLF3D_MAPSIZE) % WOLF3D_MAPSIZE, (pos + WOLF3D_MAPSIZE) / WOLF3D_MAPSIZE);
                        if(!CheckSurePush(pos + MAPSIZE, pos))
                            secrets->push_back(SecretPush(pos + MAPSIZE, pos, 0));
                        else
                        {
                            tiles.push(pos + MAPSIZE);
                            visited[pos + MAPSIZE] = 2;
                        }
					}
				}
			}

			//std::random_shuffle(secrets->begin(), secrets->end(), [this](int n){
			//	return rnd() % n;
			//});

			if (secretliststates.size() > 1 && prerequisite)
				for (SecretPush& push : *secrets)
					push.prerequisite = prerequisite;

			if (totalenemies && MapHasTally() && scorestates.top().enemycount == totalenemies)
				scorestates.top().maxenemies = 1;
			if (totaltreasure && MapHasTally() && scorestates.top().treasurecount == totaltreasure)
				scorestates.top().maxtreasures = 1;

            unsigned total = scorestates.top().totalscore();
            if(exitcount)
            {
                if (total == totalscore)
                {
                    //("Found maximum score for pushes!");
                    //WritePushes();
                    std::vector<SecretPush> finality;
                    secretliststates.pop();
                    finalize(choicestates, secretliststates, finality);
                    return finality;
                }
                if(total > maxscore)
                {
                    maxscore = total;
                    maxchoicestates = choicestates;
                    maxsecretliststates = secretliststates;
                    maxsecretliststates.pop();
                }
            }

			// We now got the secret walls
			if (secrets->empty())
			{
				do
				{
					//puts("pop");
					secretindex = UndoSecret();

					if (secretindex != (unsigned)-1)
					{
						secretliststates.pop();
						secrets = &secretliststates.top();
					}

				} while (secretindex != (unsigned)-1 && secretindex >= secrets->size());
			}
		} while (secretindex != (unsigned)-1);

        return maxpushes();
	}
	catch (const std::exception &e)
	{
		// Can't abort from here!
		Logger::Write("ERROR during %s: %s", __FUNCTION__, e.what());
		return std::vector<SecretPush>();
	}
	return std::vector<SecretPush>();
}

void SecretSolver::GetLevelData()
{
	// Init data
	// Exceptions will be caught by game's quit handler
	mapnum = gamestate.mapon;


	totalscore = totalsecrets = totalenemies = totaltreasure = 0;
	unsigned ppos = 0;
	for (int y = 0; y < MAPSIZE; ++y)
		for (int x = 0; x < MAPSIZE; ++x)
		{
			unsigned kind = mapSegs[1][y * MAPSIZE + x];
			int score = 0;
			if(IsTreasure(kind, score))
			{
				if(MapHasTally() || score)	// do not bother counting extra life if there's no maxing stake
					totaltreasure++;
				totalscore += score;
			}
			else if (PointsFor(kind))
			{
				totalenemies++;
				totalscore += PointsFor(kind);
			}
			if (kind == PUSHABLETILE)
				totalsecrets++;
			if (IsPlayerStart(kind))
			{
				ppos = y * MAPSIZE + x;
			}
		}
		
	pushstates.push(std::array<uint16_t, maparea>());
	actorstates.push(std::array<uint16_t, maparea>());
	wallbuf = &pushstates.top();
	actorbuf = &actorstates.top();
	
	memcpy(wallbuf->data(), mapSegs[0], sizeof(*wallbuf));
	memcpy(actorbuf->data(), mapSegs[1], sizeof(*actorbuf));


	if (MapHasTally())
	{
		if (totaltreasure)
			totalscore += 10000;
		if (totalenemies)
			totalscore += 10000;
		if (totalsecrets)
			totalscore += 10000;
	}
	else if (MapHasAutobonus())
		totalscore += 15000;

	posstates.push(ppos);
	playerpos = ppos;

	scorestates.push(Inventory(this));
}
