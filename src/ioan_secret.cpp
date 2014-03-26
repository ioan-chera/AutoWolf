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
#include "wl_game.h"
#include "wl_play.h"
#include "Logger.h"

static const unsigned SecretClass = 98;
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
	return kind >= 19 && kind <= 22;
}

bool SecretSolver::IsSoftWall(unsigned kind) const
{
	switch ((*actorbuf)[kind])
	{
	case 24:
	case 25:
	case 26:
	case 28:
	case 30:
	case 31:
	case 33:
	case 34:
	case 35:
	case 36:
		return true;
	case 38:
		if (SPEAR::flag)
			return true;
		break;
	case 39:
	case 40:
	case 41:
	case 45:
	case 58:
	case 59:
	case 60:
	case 62:
	case 63:
		if (!SPEAR::flag)
			return true;
		break;
	case 67:
		if (SPEAR::flag)
			return true;
		break;
	case 68:
	case 69:
		return true;
	case 71:
	case 73:
		if (SPEAR::flag)
			return true;
		break;
	}
	return false;
}

bool SecretSolver::IsWall(unsigned kind) const
{
	if ((*wallbuf)[kind] && (*wallbuf)[kind] < 90)
		return true;
	return IsSoftWall(kind);
}

static unsigned PointsFor(unsigned kind)
{
	switch (kind)
	{
	case 52:
		return 100;
	case 53:
		return 500;
	case 54:
		return 1000;
	case 55:
		return 5000;
		// guard
	case 180:
	case 181:
	case 182:
	case 183:
	case 144:
	case 145:
	case 146:
	case 147:
	case 108:
	case 109:
	case 110:
	case 111:
	case 184:
	case 185:
	case 186:
	case 187:
	case 148:
	case 149:
	case 150:
	case 151:
	case 112:
	case 113:
	case 114:
	case 115:
		return 100;
	case 188:
	case 189:
	case 190:
	case 191:
	case 152:
	case 153:
	case 154:
	case 155:
	case 116:
	case 117:
	case 118:
	case 119:
	case 192:
	case 193:
	case 194:
	case 195:
	case 156:
	case 157:
	case 158:
	case 159:
	case 120:
	case 121:
	case 122:
	case 123:
		return 400;
	case 198:
	case 199:
	case 200:
	case 201:
	case 162:
	case 163:
	case 164:
	case 165:
	case 126:
	case 127:
	case 128:
	case 129:
	case 202:
	case 203:
	case 204:
	case 205:
	case 166:
	case 167:
	case 168:
	case 169:
	case 130:
	case 131:
	case 132:
	case 133:
		return 500;
	case 206:
	case 207:
	case 208:
	case 209:
	case 170:
	case 171:
	case 172:
	case 173:
	case 134:
	case 135:
	case 136:
	case 137:
	case 210:
	case 211:
	case 212:
	case 213:
	case 174:
	case 175:
	case 176:
	case 177:
	case 138:
	case 139:
	case 140:
	case 141:
		return 200;
	case 214:
	case 197:
	case 215:
	case 179:
	case 196:
		return 5000;
	case 160:
		return 2000;
	case 178:
		return 10000;
	case 106:
	case 107:
	case 125:
	case 142:
	case 143:
	case 161:
		return 5000;
	case 252:
	case 253:
	case 254:
	case 255:
	case 234:
	case 235:
	case 236:
	case 237:
	case 216:
	case 217:
	case 218:
	case 219:
	case 256:
	case 257:
	case 258:
	case 259:
	case 238:
	case 239:
	case 240:
	case 241:
	case 220:
	case 221:
	case 222:
	case 223:
		return 700;
	}
	return 0;
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
	if ((*actorbuf)[targetpos] != SecretClass)	// secret spot
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

	try
	{
		if (!GetX(playerpos) || !GetY(playerpos) || GetX(playerpos) >= MAPSIZE - 1 || GetY(playerpos) >= MAPSIZE - 1)
			return std::vector<SecretPush>();

		std::queue<unsigned> tiles;
		std::array<std::stack<unsigned>, 4> lockedtiles;

		secretliststates.push(std::vector<SecretPush>());
		std::vector<SecretPush> *secrets = &secretliststates.top();

		tiles.push(playerpos);

		std::array<uint8_t, maparea> visited;
		visited.fill(0);
		visited[playerpos] = 2;

		unsigned needed, neededindex;
		unsigned secretindex = 0;
		unsigned pos;
		unsigned exitcount = 0;

		bool consumeactor;
//		unsigned count = 0;
		do
		{
			if (SDL_GetTicks() - ticks >= timelimit || sessionNo != g_sessionNo)
			{
//				printf("Count: %u\n", count);
				return std::vector<SecretPush>();
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
			while (!tiles.empty())
			{

				pos = tiles.front();
				tiles.pop();

				needed = 0;
				if (visited[pos] == 2)
					switch ((*wallbuf)[pos])
					{
					case 92:
					case 93:
						//printf("Found door %u\n", KEY_1);
						needed = KEY_1;
						neededindex = 0;
						break;
					case 94:
					case 95:
						//printf("Found door %u\n", KEY_2);
						needed = KEY_2;
						neededindex = 1;
						break;
					case 96:
					case 97:
						needed = KEY_3;
						neededindex = 2;
						break;
					case 98:
					case 99:
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
					(*actorbuf)[pos] = 0;	// consume it.

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
						secrets->push_back(SecretPush(pos - 1, pos));
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
						secrets->push_back(SecretPush(pos + 1, pos));
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
						secrets->push_back(SecretPush(pos - MAPSIZE, pos));
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
						secrets->push_back(SecretPush(pos + MAPSIZE, pos));
					}
				}
			}

			if (totalenemies && MapHasTally() && scorestates.top().enemycount == totalenemies)
				scorestates.top().maxenemies = 1;
			if (totaltreasure && MapHasTally() && scorestates.top().treasurecount == totaltreasure)
				scorestates.top().maxtreasures = 1;

			if (scorestates.top().totalscore() == totalscore && exitcount)
			{
				//("Found maximum score for pushes!");
				//WritePushes();
				std::vector<SecretPush> finality;
				secretliststates.pop();
				while (!choicestates.empty() && !secretliststates.empty())
				{
					SecretPush push = secretliststates.top()[choicestates.top()];
					finality.push_back(push);

					//Logger::Write("%s from %s\n", Coords(push.targetpos).c_str(), Coords(push.sourcepos).c_str());

					choicestates.pop();
					secretliststates.pop();
				}
				return finality;
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

	pushstates.push(std::array<uint16_t, maparea>());
	actorstates.push(std::array<uint16_t, maparea>());
	wallbuf = &pushstates.top();
	actorbuf = &actorstates.top();

	memcpy(wallbuf->data(), mapSegs[0], sizeof(*wallbuf));
	memcpy(actorbuf->data(), mapSegs[1], sizeof(*actorbuf));

	totalscore = totalsecrets = totalenemies = totaltreasure = 0;
	unsigned ppos = 0;
	for (int y = 0; y < MAPSIZE; ++y)
		for (int x = 0; x < MAPSIZE; ++x)
		{
			unsigned kind = (*actorbuf)[y * MAPSIZE + x];
			if (PointsFor(kind))
			{
				if (kind >= 52 && kind < 56)
					totaltreasure++;
				else
					totalenemies++;
				totalscore += PointsFor(kind);
			}
			else if (kind == 56 && MapHasTally())
				totaltreasure++;
			if (kind == SecretClass)
				totalsecrets++;
			if (IsPlayerStart(kind))
			{
				ppos = y * MAPSIZE + x;
			}
		}

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