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

struct Vector
{
	Vector operator+(const Vector& other) const noexcept
	{
		return Vector{x + other.x, y + other.y};
	}

	Vector operator-(const Vector& other) const noexcept
	{
		return Vector{x - other.x, y - other.y};
	}

	bool inMapBounds() const noexcept
	{
		return x >= 0 && x < MAPSIZE && y >= 0 && y < MAPSIZE;
	}

	int flat() const noexcept
	{
		return y * MAPSIZE + x;
	}

	int x, y;
};

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

inline static bool IsSolidWall(int areatile)
{
	return areatile > 0 && areatile < DOOR_VERTICAL_1;
}

struct Pushable
{
	Vector location;
	int numPushPoints;
	Vector pushPoints[4];
};

enum class AreaAttrib
{
	none,
	wall,
	door,
};

enum class ActorAttrib
{
	none,
	obstacle,
	pushable,
	corpse,
	treasure,
	key,
};

struct SimTile
{
	bool blocksPush() const noexcept
	{
		return area == AreaAttrib::wall || area == AreaAttrib::door || actor == ActorAttrib::obstacle ||
				 actor == ActorAttrib::corpse || hasEnemy;
	}

	bool blocksAccess() const noexcept
	{
		return area == AreaAttrib::wall || actor == ActorAttrib::obstacle;
	}

	AreaAttrib area;
	ActorAttrib actor;
	bool hasEnemy;	// Must be separate because patrolling actors are shifted
	int lock;
	int points;
	bool hasExit;
};

static int DoorTypeToLock(int doorType)
{
	switch(doorType)
	{
		case DOOR_VERTICAL_2:
		case DOOR_HORIZONTAL_2:
			return KEY_1;
			break;
		case DOOR_VERTICAL_3:
		case DOOR_HORIZONTAL_3:
			return KEY_2;
			break;
		case DOOR_VERTICAL_4:
		case DOOR_HORIZONTAL_4:
			return KEY_3;
			break;
		case DOOR_VERTICAL_5:
		case DOOR_HORIZONTAL_5:
			return KEY_4;
			break;
		default:
			break;
	}
	return 0;
}

static std::vector<SimTile> BuildSimMap()
{
	std::vector<SimTile> simTiles;
	simTiles.resize(maparea);
	for(int y = 0; y < MAPSIZE; ++y)
	{
		for(int x = 0; x < MAPSIZE; ++x)
		{
			SimTile &tile = simTiles[y * MAPSIZE + x];
			// Detecting wall
			const objtype *actor = actorat[x][y];
			if(!ISPOINTER(actor))
			{
				int value = mapSegs(0, x, y);
				if(value > 0 && value < DOOR_VERTICAL_1)
				{
					if(value == ELEVATORTILE)
						tile.hasExit = true;
					tile.area = AreaAttrib::wall;
				}
				else if(value >= DOOR_VERTICAL_1 && value <= DOOR_HORIZONTAL_6)
				{
					tile.area = AreaAttrib::door;
					tile.lock = DoorTypeToLock(value);
				}
				else
				{
					// In other cases, if it's still not a pointer actorat, then it's a solid wall
					tile.area = AreaAttrib::none;
					tile.actor = ActorAttrib::obstacle;
				}
			}
			else
			{
				// In this case we may have an actor type
				switch(actor->obclass)
				{
					case inertobj:
						tile.actor = ActorAttrib::corpse;
						break;
					case guardobj:
						tile.hasEnemy = true;
						tile.points = 100;
						break;
					case officerobj:
						tile.hasEnemy = true;
						tile.points = 400;
						break;
					case ssobj:
						tile.hasEnemy = true;
						tile.points = 500;
						break;
					case dogobj:
					case spectreobj:	// NOTE: this one actually gives as many points
						tile.hasEnemy = true;
						tile.points = 200;
						break;
					case bossobj:
					case gretelobj:
					case transobj:
					case uberobj:
					case willobj:
					case deathobj:
						tile.hasEnemy = true;
						tile.points = 5000;
						tile.lock = KEY_1;
						break;
					case schabbobj:
					case giftobj:
					case fatobj:
					case angelobj:
						tile.hasEnemy = true;
						tile.points = 5000;
						tile.hasExit = true;
						break;
					case fakeobj:
						tile.hasEnemy = true;
						tile.points = 2000;
						break;
					case mechahitlerobj:
						tile.hasEnemy = true;
						tile.points = 10000;
						tile.hasExit = true;
						break;
					case mutantobj:
						tile.hasEnemy = true;
						tile.points = 700;
						break;
					default:
						break;
				}
			}

			int actorvalue = mapSegs(1, x, y);

			if(actorvalue == PUSHABLETILE)
				tile.actor = ActorAttrib::pushable;
			else if(actorvalue == EXITTILE)
				tile.hasExit = true;
			else if(actorvalue)
			{
				wl_stat_t type = Act1::GetStaticType(actorvalue);
				switch(type)
				{
					case bo_cross:
						tile.actor = ActorAttrib::treasure;
						tile.points = 100;
						break;
					case bo_chalice:
						tile.actor = ActorAttrib::treasure;
						tile.points = 500;
						break;
					case bo_bible:
						tile.actor = ActorAttrib::treasure;
						tile.points = 1000;
						break;
					case bo_crown:
						tile.actor = ActorAttrib::treasure;
						tile.points = 5000;
						break;
					case bo_fullheal:
						tile.actor = ActorAttrib::treasure;
						tile.points = 0;	// no points for full heal
						break;
					case bo_key1:
						tile.actor = ActorAttrib::key;
						tile.lock = KEY_1;
						break;
					case bo_key2:
						tile.actor = ActorAttrib::key;
						tile.lock = KEY_2;
						break;
					case bo_key3:
						tile.actor = ActorAttrib::key;
						tile.lock = KEY_3;
						break;
					case bo_key4:
						tile.actor = ActorAttrib::key;
						tile.lock = KEY_4;
						break;
					case bo_spear:
						tile.hasExit = true;
						break;
					default:
						break;
				}
			}
		}
	}
}

static bool Push(std::vector<SimTile> &simTiles, Vector pushPos, Vector wallPos)
{
	if(simTiles[pushPos.flat()].blocksAccess())
		return false;
	if(simTiles[wallPos.flat()].actor != ActorAttrib::pushable ||
	   simTiles[wallPos.flat()].area != AreaAttrib::wall)
	{
		return false;
	}
	Vector nextPos = wallPos + (wallPos - pushPos);
	Vector nextPos2 = nextPos + (wallPos - pushPos);
	if(nextPos2.inMapBounds() && !simTiles[nextPos2.flat()].blocksPush())
		simTiles[nextPos2.flat()].area = AreaAttrib::wall;
	else if(nextPos.inMapBounds() && !simTiles[nextPos.flat()].blocksPush())
		simTiles[nextPos.flat()].area = AreaAttrib::wall;
	else
		return false;

	simTiles[wallPos.flat()].area = AreaAttrib::none;
	simTiles[wallPos.flat()].actor = ActorAttrib::none;

	return true;
}

// Called from SetupGameLevel, which calls ScanInfoPlane
void Secret::AnalyzeSecrets()
{
	std::vector<SimTile> simTiles = BuildSimMap();

	Vector playerpos = Vector{player->tilex, player->tiley};

	// Look for trivial secrets
	std::queue<Vector> tiles;
	tiles.push(playerpos);

	std::vector<std::vector<bool>> visited;
	visited.resize(maparea);
	for(auto &v : visited)
		v.resize(maparea, false);

	static const Vector dirs[] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

	std::unordered_map<int, Pushable> pushables;

	// Look for reachable pushables
	while(!tiles.empty())
	{
		Vector pos = tiles.front();
		tiles.pop();

		visited[pos.y][pos.x] = true;

		for(Vector dir : dirs)
		{
			Vector candidate = pos + dir;
			if(!candidate.inMapBounds() || visited[candidate.y][candidate.x])
				continue;
			int candidatePos = candidate.y * MAPSIZE + candidate.x;
			if(simTiles[candidatePos].actor == ActorAttrib::obstacle)
				continue;

			if(simTiles[candidatePos].area == AreaAttrib::wall)
			{
				if(simTiles[candidatePos].actor != ActorAttrib::pushable)
					continue;
				// Pushable tile now
				Pushable &push = pushables[candidatePos];
				push.location = candidate;
				Vector nextPos = candidate + dir;
				if(nextPos.inMapBounds() && !simTiles[nextPos.flat()].blocksPush())
				{
					assert(push.numPushPoints < 4);
					push.pushPoints[push.numPushPoints++] = pos;
				}

				continue;
			}
			tiles.push(candidate);
		}
	}

	// Now it's time to build the model world

	bool found = 0;
	for(const auto &pair : pushables)
	{
		if(pair.second.numPushPoints == 1)
		{
			Vector pushPoint = pair.second.pushPoints[0];
			Vector location = pair.second.location;
			printf("Pushing trivial wall (%d %d)->(%d %d)\n", pushPoint.x, pushPoint.y,
				   location.x, location.y);
			Push(simTiles, pushPoint, location);
			// TODO
		}
	}
}
