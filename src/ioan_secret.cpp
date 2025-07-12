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
#include <chrono>
#include <set>
#include <unordered_set>
#include <functional>
#include <array>
#include <bitset>
#include "Config.h"
#include "id_ca.h"
#include "ioan_secret.h"
#include "obattrib.h"
#include "wl_act1.h"
#include "wl_game.h"
#include "wl_inter.h"
#include "wl_play.h"
#include "Logger.h"

namespace Secret {

static const double progressTotal = 256;

static const unsigned timelimit = 10000;

static const int DIRS[] = {-1, 1, -MAPSIZE, MAPSIZE};

// Key inventory stuff
enum KeyFlags
{
	KEY_1 = 1,
	KEY_2 = 2,
	KEY_3 = 4,
	KEY_4 = 8,
};

enum
{
	ST_WALL = 1,
	ST_EXIT = 2,
	ST_OBSTACLE = 4,
	ST_CORPSE = 8,
	ST_DOOR = 16,
	ST_PUSHABLE = 32,
	ST_TREASURE = 64,
	ST_KEY = 128,
	ST_ENEMY = 256,
};



struct PushableWall
{
	int position;
	std::vector<int> validDirections;
};

struct Tally
{
	int enemies;
	int treasure;
	int secrets;
};

// Type alias for visited tiles bitset
using VisitedMap = std::bitset<maparea>;

struct Inventory
{
	explicit Inventory(const Tally &tally) : total(tally)
	{
	}

	bool operator>(const Inventory &other) const noexcept
	{
		// Compare based on points collected, then treasures, then enemies killed, then pushes
		int myScore = totalScore();
		int otherScore = other.totalScore();
		if (myScore != otherScore)
			return myScore > otherScore;
		if (treasureCollected != other.treasureCollected)
			return treasureCollected > other.treasureCollected;
		if (enemiesKilled != other.enemiesKilled)
			return enemiesKilled > other.enemiesKilled;
		if (pushes.size() != other.pushes.size())
			return pushes.size() > other.pushes.size();
		return exitReachable && !other.exitReachable;  // Prefer reachable exits
	}

	bool operator<(const Inventory &other) const noexcept
	{
		return other > *this;
	}

	bool operator==(const Inventory &other) const noexcept
	{
		return !(*this > other) && !(other > *this);
	}

	int totalScore() const
	{
		int result = pointsCollected;
		if(treasureCollected >= total.treasure)
			result += 10000;
		if(enemiesKilled >= total.enemies)
			result += 10000;
		if((int)pushes.size() >= total.secrets)
			result += 10000;
		return result;
	}

	int treasureCollected = 0;
	int enemiesKilled = 0;
	int pointsCollected = 0;
	int keys = 0;
	std::vector<Push> pushes;
	bool exitReachable = false;

	const Tally &total;
};

struct SimTile
{
	bool blocksPush() const noexcept
	{
		return flags & (ST_WALL | ST_OBSTACLE | ST_CORPSE | ST_DOOR);
		// NOTE: enemies may also block, but only if deaf, immobile or bad floor code
		// Currently not handled
	}

	bool blocksAccess(int keys) const noexcept
	{
		if(flags & (ST_WALL | ST_OBSTACLE))
			return true;
		// Enemies can be confronted, so they won't block access
		if(flags & ST_DOOR)
			return lock && (lock & keys) == 0;
		return false;
	}

	bool operator < (const SimTile &other) const noexcept
	{
		// Needed by set
		if (flags != other.flags)
			return flags < other.flags;
		if (points != other.points)
			return points < other.points;
		return lock < other.lock;
	}

	bool operator == (const SimTile &other) const noexcept
	{
		// Needed by unordered_set
		return flags == other.flags && points == other.points && lock == other.lock;
	}

	int flags;
	int points;
	int lock;
};

// Type alias for the game map
using SimMap = std::array<SimTile, maparea>;


struct GameState
{
	explicit GameState(Tally &tally)
		: inventory(tally)
	{
		BuildSimMap(tally);
		playerPos = player->tilex + player->tiley * MAPSIZE;
	}

	SimMap simTiles = {};
	int playerPos = 0;
	Inventory inventory;

private:
	void BuildSimMap(Tally &total);
};

struct CollectionResult
{
	std::vector<PushableWall> pushableWalls;
	VisitedMap visitedTiles;
};

static bool IsValidMove(int toPos, int dir)
{
	// Check basic bounds
	if (toPos < 0 || toPos >= maparea)
		return false;

	// Check map boundaries
	int toX = toPos % MAPSIZE;
	int toY = toPos / MAPSIZE;
	if (toX < 0 || toX >= MAPSIZE || toY < 0 || toY >= MAPSIZE)
		return false;

	// Check row boundaries for horizontal moves (prevent wrap-around)
	if (dir == -1 && toX == MAPSIZE - 1) return false;  // Left move wrapping to right edge
	if (dir == 1 && toX == 0) return false;  // Right move wrapping to left edge

	return true;
}

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

inline static bool IsSolidWall(int areatile)
{
	return areatile > 0 && areatile < DOOR_VERTICAL_1;
}

} // namespace Secret

// Hash function specializations need to be in std namespace
namespace std {
	template<>
	struct hash<Secret::SimTile> {
		size_t operator()(const Secret::SimTile& tile) const {
			size_t h1 = std::hash<int>{}(tile.flags);
			size_t h2 = std::hash<int>{}(tile.points);
			size_t h3 = std::hash<int>{}(tile.lock);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	// Hash function for SimMap (std::array<SimTile, maparea>)
	template<>
	struct hash<Secret::SimMap> {
		size_t operator()(const Secret::SimMap& arr) const {
			size_t result = 0;
			hash<Secret::SimTile> hasher;
			for (size_t i = 0; i < arr.size(); ++i) {
				result ^= hasher(arr[i]) + 0x9e3779b9 + (result << 6) + (result >> 2);
			}
			return result;
		}
	};

	// Hash function for Push
	template<>
	struct hash<Secret::Push> {
		size_t operator()(const Secret::Push& push) const {
			size_t h1 = std::hash<int>{}(push.from);
			size_t h2 = std::hash<int>{}(push.wallpos);
			size_t h3 = std::hash<int>{}(push.to);
			size_t h4 = std::hash<bool>{}(push.trivial);

			// Hash the blockedTiles vector
			size_t h5 = 0;
			for (int tile : push.blockedTiles) {
				h5 ^= std::hash<int>{}(tile) + 0x9e3779b9 + (h5 << 6) + (h5 >> 2);
			}

			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
		}
	};
}

namespace Secret {

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

void GameState::BuildSimMap(Tally &total)
{
	for(int y = 0; y < MAPSIZE; ++y)
	{
		for(int x = 0; x < MAPSIZE; ++x)
		{
			SimTile &tile = simTiles[y * MAPSIZE + x];
			tile.flags = 0;
			tile.points = 0;
			tile.lock = 0;

			// Check wall data from mapSegs plane 0
			int wallValue = mapSegs(0, x, y);
			if(wallValue > 0 && wallValue < DOOR_VERTICAL_1)
			{
				tile.flags |= ST_WALL;
				if(wallValue == ELEVATORTILE)
					tile.flags |= ST_EXIT;
			}

			// Check actor data from actorat
			const objtype *actor = actorat[x][y];
			if(ISPOINTER(actor))
			{
				switch(actor->obclass)
				{
					case inertobj:
						tile.flags |= ST_CORPSE;
						break;
					case guardobj:
						tile.flags |= ST_ENEMY;
						tile.points = 100;
						total.enemies++;
						break;
					case officerobj:
						tile.flags |= ST_ENEMY;
						tile.points = 400;
						total.enemies++;
						break;
					case ssobj:
						tile.flags |= ST_ENEMY;
						tile.points = 500;
						total.enemies++;
						break;
					case dogobj:
					case spectreobj:
						tile.flags |= ST_ENEMY;
						tile.points = 200;
						total.enemies++;
						break;
					case bossobj:
					case gretelobj:
					case transobj:
					case uberobj:
					case willobj:
					case deathobj:
						tile.flags |= ST_ENEMY | ST_KEY;
						tile.points = 5000;
						tile.lock |= KEY_1;
						total.enemies++;
						break;
					case schabbobj:
					case giftobj:
					case fatobj:
					case angelobj:
						tile.flags |= ST_ENEMY;
						tile.points = 5000;
						tile.flags |= ST_EXIT;
						total.enemies++;
						break;
					case fakeobj:
						tile.flags |= ST_ENEMY;
						tile.points = 2000;
						total.enemies++;
						break;
					case mechahitlerobj:
						tile.flags |= ST_ENEMY;
						tile.points = 10000;
						tile.flags |= ST_EXIT;
						total.enemies++;
						break;
					case mutantobj:
						tile.flags |= ST_ENEMY;
						tile.points = 700;
						total.enemies++;
						break;
					default:
						break;
				}
			}
			else if(!ISPOINTER(actor))
			{
				// Non-pointer actorat values indicate obstacles
				if((uintptr_t)actor > 0 && Act1::GetStaticType(mapSegs(1, x, y)) == block)
					tile.flags |= ST_OBSTACLE;
			}

			// Check static objects from mapSegs plane 1
			int actorValue = mapSegs(1, x, y);
			if(actorValue == PUSHABLETILE)
			{
				tile.flags |= ST_PUSHABLE;
				total.secrets++;
			}
			else if(actorValue == EXITTILE)
			{
				tile.flags |= ST_EXIT;
			}
			else if(actorValue)
			{
				wl_stat_t type = Act1::GetStaticType(actorValue);
				switch(type)
				{
					case bo_cross:
						tile.flags |= ST_TREASURE;
						tile.points = 100;
						total.treasure++;
						break;
					case bo_chalice:
						tile.flags |= ST_TREASURE;
						tile.points = 500;
						total.treasure++;
						break;
					case bo_bible:
						tile.flags |= ST_TREASURE;
						tile.points = 1000;
						total.treasure++;
						break;
					case bo_crown:
						tile.flags |= ST_TREASURE;
						tile.points = 5000;
						total.treasure++;
						break;
					case bo_fullheal:
						tile.flags |= ST_TREASURE;
						tile.points = 0;
						total.treasure++;
						break;
					case bo_key1:
						tile.flags |= ST_KEY;
						tile.lock = KEY_1;
						break;
					case bo_key2:
						tile.flags |= ST_KEY;
						tile.lock = KEY_2;
						break;
					case bo_key3:
						tile.flags |= ST_KEY;
						tile.lock = KEY_3;
						break;
					case bo_key4:
						tile.flags |= ST_KEY;
						tile.lock = KEY_4;
						break;
					case bo_spear:
						tile.flags |= ST_EXIT;
						break;
					default:
						break;
				}
			}
		}
	}

	// Process doors from doorobjlist
	for(doorobj_t *door = &doorobjlist[0]; door != lastdoorobj; door++)
	{
		int x = door->tilex;
		int y = door->tiley;
		if(x >= 0 && x < MAPSIZE && y >= 0 && y < MAPSIZE)
		{
			SimTile &tile = simTiles[y * MAPSIZE + x];
			tile.flags |= ST_DOOR;

			// Set lock based on door type
			switch(door->lock)
			{
				case 1:
					tile.lock = KEY_1;
					break;
				case 2:
					tile.lock = KEY_2;
					break;
				case 3:
					tile.lock = KEY_3;
					break;
				case 4:
					tile.lock = KEY_4;
					break;
				default:
					tile.lock = 0; // No key required
					break;
			}
		}

	}
}


static void HandlePushableWall(SimMap &simTiles, int wallPos, int dir, std::vector<PushableWall> &pushableWallsFound)
{
	// Check if we can actually push this wall in this direction
	int behindPos = wallPos + dir;
	bool canPush = false;

	if (IsValidMove(behindPos, dir))
	{
		SimTile &behindTile = simTiles[behindPos];

		// Can push if the tile behind is not blocked by doors, obstacles, corpses, or walls
		if (!behindTile.blocksPush())
			canPush = true;
	}

	if (canPush)
	{
		// Check if we already found this pushable wall
		auto it = std::find_if(pushableWallsFound.begin(), pushableWallsFound.end(),
			[wallPos](const PushableWall& pw) { return pw.position == wallPos; });

		if (it != pushableWallsFound.end())
		{
			// Add this direction if not already present
			if (std::find(it->validDirections.begin(), it->validDirections.end(), dir) == it->validDirections.end())
			{
				it->validDirections.push_back(dir);
			}
		}
		else
		{
			// New pushable wall found
			PushableWall pw;
			pw.position = wallPos;
			pw.validDirections.push_back(dir);
			pushableWallsFound.push_back(pw);
		}
	}
}

static void FloodFillExplore(GameState &state, VisitedMap &visited,
							 std::vector<int> *obstaclesFound,
							 std::vector<PushableWall> *pushableWallsFound, bool visitOnly)
{
	std::queue<int> queue;
	queue.push(state.playerPos);
	visited[state.playerPos] = true;

	std::vector<std::pair<int, int>> lockedDoors;

	while (!queue.empty())
	{
		int pos = queue.front();
		queue.pop();

		SimTile &tile = state.simTiles[pos];

		// Collect treasure
		if (!visitOnly && tile.flags & ST_TREASURE)
		{
			state.inventory.treasureCollected++;
			state.inventory.pointsCollected += tile.points;
			tile.flags &= ~ST_TREASURE;
			tile.points = 0;
		}

		// Collect keys
		if (!visitOnly && tile.flags & ST_KEY)
		{
			state.inventory.keys |= tile.lock;
			tile.flags &= ~ST_KEY;
			tile.lock = 0;
			for(auto pair : lockedDoors)
			{
				if (pair.second & state.inventory.keys)
				{
					queue.push(pair.first);
					pair.second &= ~state.inventory.keys;
				}
			}
		}

		// Kill enemies
		if (!visitOnly && tile.flags & ST_ENEMY)
		{
			state.inventory.enemiesKilled++;
			state.inventory.pointsCollected += tile.points;
			tile.flags &= ~ST_ENEMY;
			tile.points = 0;
		}

		// Check if exit is reachable
		if (!visitOnly && tile.flags & ST_EXIT)
		{
			state.inventory.exitReachable = true;
		}

		// Explore adjacent tiles
		for (int dir : DIRS)
		{
			int newPos = pos + dir;
			if (visited[newPos] || !IsValidMove(newPos, dir))
				continue;

			SimTile &nextTile = state.simTiles[newPos];

			// Can't pass through walls or obstacles (unless it's a door we can open)
			if (nextTile.flags & ST_WALL)
			{
				if(!visitOnly && nextTile.flags & ST_PUSHABLE && pushableWallsFound)
				{
					HandlePushableWall(state.simTiles, newPos, dir, *pushableWallsFound);
					// Do not visit it, as we need to check it from multiple places
				}

				// Exit switches can only be operated east-west
				if(nextTile.flags & ST_EXIT && !(nextTile.flags & ST_PUSHABLE) && abs(dir) == 1)
				{
					if(!visitOnly)
						state.inventory.exitReachable = true;
					visited[newPos] = true;
				}
				continue;
			}
			if (nextTile.flags & ST_OBSTACLE)
			{
				if(!visited[newPos])
				{
					if(!visitOnly && obstaclesFound)
						obstaclesFound->push_back(newPos);
					visited[newPos] = true;
				}
				continue;
			}
			if ((nextTile.flags & ST_DOOR) && nextTile.lock != 0 && (nextTile.lock & state.inventory.keys) == 0)
			{
				if(!visitOnly)
					lockedDoors.push_back({newPos, nextTile.lock});
				visited[newPos] = true;
				continue;
			}

			visited[newPos] = true;
			queue.push(newPos);
		}
	}
}

static void FloodFillEnemiesBeyondObstacles(SimMap &simTiles, int startPos, Inventory &inventory, VisitedMap &visited)
{
	std::queue<int> queue;
	queue.push(startPos);
	visited[startPos] = true;

	while (!queue.empty())
	{
		int pos = queue.front();
		queue.pop();

		SimTile &tile = simTiles[pos];

		// Kill enemies (this is the only thing we do in obstacle flood fill)
		if (tile.flags & ST_ENEMY)
		{
			inventory.enemiesKilled++;
			inventory.pointsCollected += tile.points;
			tile.flags &= ~ST_ENEMY;
			tile.points = 0;

			// Also mark exit reachable if enemy triggers it on death
			if(tile.flags & ST_EXIT)
			{
				inventory.exitReachable = true;
			}
		}

		// Explore through obstacles (but not walls or doors)
		for (int dir : DIRS)
		{
			int newPos = pos + dir;
			if (visited[newPos] || !IsValidMove(newPos, dir))
				continue;

			SimTile &nextTile = simTiles[newPos];

			// Can't pass through walls or doors in obstacle flood fill for enemy sighting
			if (nextTile.flags & (ST_WALL | ST_DOOR))
				continue;

			visited[newPos] = true;
			queue.push(newPos);
		}
	}
}

static bool IsTrivialPush(SimMap &simTiles, const PushableWall &pushableWall)
{
	// First check: if more than one valid direction, it's not trivial
	if (pushableWall.validDirections.size() != 1)
	{
		return false;
	}

	int wallPos = pushableWall.position;
	int dir = pushableWall.validDirections[0];
	int fromPos = wallPos - dir;
	int behindPos = wallPos + dir;

	// These checks are already done in HandlePushableWall when adding valid directions
	assert(behindPos >= 0 && behindPos < maparea);
	int wallX = wallPos % MAPSIZE;
	int behindX = behindPos % MAPSIZE;
	assert(!((dir == -1 && (wallX == 0 || behindX == MAPSIZE - 1)) ||
			 (dir == 1 && (wallX == MAPSIZE - 1 || behindX == 0))));

	// Second check: do reachability test
	// From the tile in front of the pushwall, try to reach the tile behind it
	// while treating this specific pushwall as solid but other pushwalls as passable

	VisitedMap visited;
	std::queue<int> queue;
	queue.push(fromPos);
	visited[fromPos] = true;

	while (!queue.empty())
	{
		int pos = queue.front();
		queue.pop();

		// If we reached the tile behind the wall, it's not trivial
		if (pos == behindPos)
		{
			return false;
		}

		// Explore adjacent tiles
		for (int exploreDir : DIRS)
		{
			int newPos = pos + exploreDir;
			if (visited[newPos] || !IsValidMove(newPos, exploreDir))
				continue;

			SimTile &nextTile = simTiles[newPos];

			// Can't pass through obstacles or the specific pushwall we're testing
			if (nextTile.flags & ST_OBSTACLE)
				continue;
			if (newPos == wallPos)  // The specific pushwall we're testing
				continue;
			if ((nextTile.flags & ST_WALL) && !(nextTile.flags & ST_PUSHABLE))  // Regular walls
				continue;

			// CAN pass through other pushable walls or locked doors
			visited[newPos] = true;
			queue.push(newPos);
		}
	}

	// If we couldn't reach the tile behind the wall, it's a trivial push
	return true;
}

static int PushWall(SimMap &simTiles, int wallPos, int direction, Push &pushRecord)
{
	// returns the player position after the push
	int fromPos = wallPos - direction;
	int toPos = wallPos + direction;
	int toPos2 = wallPos + direction * 2;
	int toPos3 = wallPos + direction * 3;

	pushRecord.from = fromPos;
	pushRecord.wallpos = wallPos;

	SimTile &wallTile = simTiles[wallPos];

	if(IsValidMove(toPos, direction) && !simTiles[toPos].blocksPush())
	{
		wallTile.flags &= ~(ST_WALL | ST_PUSHABLE);
		int destination;
		if(IsValidMove(toPos2, direction) && !simTiles[toPos2].blocksPush())
		{
			if(cfg_secretstep3 && IsValidMove(toPos3, direction) && !simTiles[toPos3].blocksPush())
				destination = toPos3;
			else
				destination = toPos2;
		}
		else
			destination = toPos;
		simTiles[destination].flags |= ST_WALL;
		pushRecord.to = destination;
		return fromPos;
	}
	assert(false && "Invalid push operation: cannot push wall in this direction");
	return 0;  // Should never reach here, but return 0 to avoid compiler warnings
}

static CollectionResult ExploreAndCollect(GameState &state)
{
	VisitedMap visited = {};

	// Main flood fill for exploration and collection
	std::vector<int> obstaclesFound;
	std::vector<PushableWall> pushableWallsFound;
	FloodFillExplore(state, visited, &obstaclesFound, &pushableWallsFound, false);

	// Only populate with the walkable visited options
	CollectionResult result = {};
	result.visitedTiles = visited;

	// Secondary flood fill through obstacles to find enemies
	for(int obstaclePos : obstaclesFound)
	{
		FloodFillEnemiesBeyondObstacles(state.simTiles, obstaclePos, state.inventory, visited);
	}

	result.pushableWalls = std::move(pushableWallsFound);

	return result;
}

static void UpdateExitReachableResults(const GameState &state, std::vector<Inventory> &exitReachableResults)
{
	bool isBetter = true;
	for (const Inventory& existing : exitReachableResults)
	{
		if (!(state.inventory > existing))
		{
			isBetter = false;
			break;
		}
	}

	if (isBetter && !exitReachableResults.empty())
	{
		Logger::Write("New best solution found: Points=%d, Treasures=%d, Enemies=%d, Pushes=%d, Keys=%d\n",
					state.inventory.pointsCollected, state.inventory.treasureCollected,
					state.inventory.enemiesKilled, (int)state.inventory.pushes.size(),
					state.inventory.keys);
		exitReachableResults.clear();  // Clear previous results
	}

	exitReachableResults.push_back(state.inventory);
}

static Inventory EstimateMaxInventory(const GameState &state)
{
	// Create a copy to avoid modifying the original
	SimMap tempTiles = state.simTiles;
	Inventory maxInventory = state.inventory;
	VisitedMap visited;

	std::queue<int> queue;
	queue.push(state.playerPos);
	visited[state.playerPos] = true;

	std::vector<std::pair<int, int>> lockedDoors;
	std::vector<int> obstaclesFound;

	while (!queue.empty())
	{
		int pos = queue.front();
		queue.pop();

		SimTile &tile = tempTiles[pos];

		// Collect treasure
		if (tile.flags & ST_TREASURE)
		{
			maxInventory.treasureCollected++;
			maxInventory.pointsCollected += tile.points;
			tile.flags &= ~ST_TREASURE;
			tile.points = 0;
		}

		// Collect keys
		if (tile.flags & ST_KEY)
		{
			maxInventory.keys |= tile.lock;
			tile.flags &= ~ST_KEY;
			tile.lock = 0;

			// Check if we can now open any previously locked doors
			for(auto& pair : lockedDoors)
			{
				if (pair.second & maxInventory.keys)
				{
					queue.push(pair.first);
					pair.second &= ~maxInventory.keys;
				}
			}
		}

		// Kill enemies
		if (tile.flags & ST_ENEMY)
		{
			maxInventory.enemiesKilled++;
			maxInventory.pointsCollected += tile.points;
			tile.flags &= ~ST_ENEMY;
			tile.points = 0;
		}

		// Check if exit is reachable
		if (tile.flags & ST_EXIT)
		{
			maxInventory.exitReachable = true;
		}

		// Explore adjacent tiles
		for (int dir : DIRS)
		{
			int newPos = pos + dir;
			if (visited[newPos] || !IsValidMove(newPos, dir))
				continue;

			SimTile &nextTile = tempTiles[newPos];

			// Can't pass through regular walls
			if ((nextTile.flags & ST_WALL) && !(nextTile.flags & ST_PUSHABLE))
				continue;

			// Handle obstacles - track them for enemy collection
			if (nextTile.flags & ST_OBSTACLE)
			{
				if(!visited[newPos])
				{
					obstaclesFound.push_back(newPos);
					visited[newPos] = true;
				}
				continue;
			}

			if (nextTile.flags & ST_CORPSE)
				continue;

			// For doors, check if we can open them or track them for later
			if (nextTile.flags & ST_DOOR)
			{
				if (nextTile.lock != 0 && (nextTile.lock & maxInventory.keys) == 0)
				{
					// Can't open this door yet, but track it for when we get the key
					lockedDoors.push_back({newPos, nextTile.lock});
					visited[newPos] = true;
					continue;
				}
			}

			// TREAT PUSHABLE WALLS AS PASSABLE for heuristic estimation
			// This gives us the theoretical maximum if all walls could be pushed optimally

			visited[newPos] = true;
			queue.push(newPos);
		}
	}

	// Secondary flood fill through obstacles to find enemies (like FloodFillEnemies)
	for(int obstaclePos : obstaclesFound)
	{
		std::queue<int> obstacleQueue;
		obstacleQueue.push(obstaclePos);

		while (!obstacleQueue.empty())
		{
			int pos = obstacleQueue.front();
			obstacleQueue.pop();

			SimTile &tile = tempTiles[pos];

			// Kill enemies (this is the only thing we do in obstacle flood fill)
			if (tile.flags & ST_ENEMY)
			{
				maxInventory.enemiesKilled++;
				maxInventory.pointsCollected += tile.points;
				tile.flags &= ~ST_ENEMY;
				tile.points = 0;

				// Also mark exit reachable if enemy triggers it on death
				if(tile.flags & ST_EXIT)
				{
					maxInventory.exitReachable = true;
				}
			}

			// Explore through obstacles (but not walls or doors)
			for (int dir : DIRS)
			{
				int newPos = pos + dir;
				if (visited[newPos] || !IsValidMove(newPos, dir))
					continue;

				SimTile &nextTile = tempTiles[newPos];

				// Can't pass through walls or doors in obstacle flood fill for enemy sighting
				if(!(nextTile.flags & ST_PUSHABLE) && nextTile.flags & (ST_WALL | ST_DOOR))
					continue;

				visited[newPos] = true;
				obstacleQueue.push(newPos);
			}
		}
	}

	return maxInventory;
}


class BacktrackingExplorer
{
private:
	std::vector<Inventory>& exitReachableResults;
	std::chrono::steady_clock::time_point startTime;
	std::chrono::seconds timeout;
	std::unordered_set<SimMap>& visitedStates;

public:
	BacktrackingExplorer(std::vector<Inventory>& results,
						 std::chrono::steady_clock::time_point start,
						 std::chrono::seconds timeLimit,
						 std::unordered_set<SimMap>& visited)
		: exitReachableResults(results)
		, startTime(start)
		, timeout(timeLimit)
		, visitedStates(visited)
	{
	}

	void explore(GameState &state, LoadingScreen &loading, double progressMin, double progressMax);
};

void BacktrackingExplorer::explore(GameState &state, LoadingScreen &loading, double progressMin,
	double progressMax)
{
	// Check if we've exceeded the timeout
	if (std::chrono::steady_clock::now() - startTime > timeout)
		return;

	// Check if this state has been visited before
	if (visitedStates.find(state.simTiles) != visitedStates.end())
	{
		loading.Update(progressMax, progressTotal);
		return; // Already visited this state
	}
	visitedStates.insert(state.simTiles);

	bool foundTrivialPush = true;
	std::vector<PushableWall> nontrivialWalls;

	VisitedMap visitedBeforeNontrivials = {};

	// Keep exploring and pushing trivial walls until no more are found
	while (foundTrivialPush)
	{
		foundTrivialPush = false;

		// Explore and collect everything reachable, getting pushable walls encountered
		CollectionResult collectionResult = ExploreAndCollect(state);
		nontrivialWalls.clear();	// Clear because we'll encounter same non-trivial walls each time

		visitedBeforeNontrivials |= collectionResult.visitedTiles;

		// Check each pushable wall for triviality
		for (const PushableWall &pushableWall : collectionResult.pushableWalls)
		{
			if (IsTrivialPush(state.simTiles, pushableWall))
			{
				int direction = pushableWall.validDirections[0];  // We know it has exactly one
				// Push the trivial wall
				Push pushRecord;
				state.playerPos = PushWall(state.simTiles, pushableWall.position, direction, pushRecord);
				pushRecord.trivial = true;  // Mark as trivial push
				state.inventory.pushes.push_back(pushRecord);
				foundTrivialPush = true;
			}
			else
			{
				// If not trivial, add to non-trivial list
				nontrivialWalls.push_back(pushableWall);
			}
		}
	}

	Inventory maxPossibleInventory = EstimateMaxInventory(state);

	// If exit is reachable, store this inventory
	if (state.inventory.exitReachable)
	{
		UpdateExitReachableResults(state, exitReachableResults);
	}

	struct PostponedAttempt
	{
		GameState state;
		Inventory inventory;
		double progressAmount;
	};

	std::vector<PostponedAttempt> postponedAttempts;

	double loadingCurrent = progressMin;
	double loadingFactor = (progressMax - progressMin) / (double)nontrivialWalls.size();

	// Backtracking: try each non-trivial pushable wall
	for (const PushableWall &pushableWall : nontrivialWalls)
	{
		// Check timeout before each major operation
		if (std::chrono::steady_clock::now() - startTime > timeout)
			return;

		// Use all valid directions for non-trivial pushes
		double progressAmount = loadingFactor / (double)pushableWall.validDirections.size();
		for (int direction : pushableWall.validDirections)
		{
			// Create new state (copy current state)
			GameState newState = state;
			newState.inventory.exitReachable = false;  // Reset exit reachability for new state

			// Push the wall in the new state
			Push pushRecord;
			newState.playerPos = PushWall(newState.simTiles, pushableWall.position, direction, pushRecord);
			pushRecord.trivial = false;  // Mark as non-trivial push
			newState.inventory.pushes.push_back(pushRecord);
			Push &pushRef = newState.inventory.pushes.back();

			VisitedMap visitedAfterPush = {};
			FloodFillExplore(newState, visitedAfterPush, nullptr, nullptr, true);

			// Check if this push blocks access to previously reachable tiles
			VisitedMap blockedTiles = visitedBeforeNontrivials & ~visitedAfterPush;
			if (blockedTiles.any())
				for (int i = 0; i < maparea; ++i)
					if (blockedTiles[i])
						pushRef.blockedTiles.push_back(i);

			Inventory maxInventoryAfterPush = EstimateMaxInventory(newState);
			if(maxInventoryAfterPush < maxPossibleInventory)
			{
				// This risks being a bad move, so postpone it as we try better options first
				// But only if it's not definitely worse than what we just found
				if(!exitReachableResults.empty() && maxInventoryAfterPush < exitReachableResults[0])
				{
					// Logger::Write("Cancelling attempt with worse inventory: Points=%d, Treasures=%d, Enemies=%d, Pushes=%d, Keys=%d\n",
					// 				maxInventoryAfterPush.pointsCollected, maxInventoryAfterPush.treasureCollected,
					// 				maxInventoryAfterPush.enemiesKilled, (int)maxInventoryAfterPush.pushes.size(),
					// 				maxInventoryAfterPush.keys);
					loading.Update(loadingCurrent, progressTotal);
					loadingCurrent += progressAmount;
				}
				else
				{
					postponedAttempts.push_back({newState, maxInventoryAfterPush, progressAmount});
				}
				continue;
			}

			// Recursively explore this new state
			// loading.Update(loadingCurrent, progressTotal);

			explore(newState, loading, loadingCurrent, loadingCurrent + progressAmount);
			loadingCurrent += progressAmount;
		}
	}

	// Postponed attempts
	// TODO: prune them if they are worse than the current best
	for (PostponedAttempt &attempt : postponedAttempts)
	{
		if(!exitReachableResults.empty() && attempt.inventory < exitReachableResults[0])
		{
			// If this attempt is worse than the best found, skip it
			// Logger::Write("Skipping postponed attempt with worse inventory: Points=%d, Treasures=%d, Enemies=%d, Pushes=%d, Keys=%d\n",
			// 			  attempt.second.pointsCollected, attempt.second.treasureCollected,
			// 			  attempt.second.enemiesKilled, (int)attempt.second.pushes.size(),
			// 			  attempt.second.keys);
			loading.Update(loadingCurrent, progressTotal);
			loadingCurrent += attempt.progressAmount;
			continue;
		}
		// Check timeout before each major operation
		if (std::chrono::steady_clock::now() - startTime > timeout)
			return;

		// loading.Update(loadingCurrent, progressTotal);
		
		explore(attempt.state, loading, loadingCurrent, loadingCurrent + attempt.progressAmount);
		loadingCurrent += attempt.progressAmount;
	}

	loading.Update(progressMax, progressTotal);
}

static PushTree PushTreeFromReachableResults(const std::vector<Inventory> &pushResults,
	size_t startIndex)
{
	if(pushResults.empty())
		return {};

	PushTree tree;

	size_t i;
	for(i = startIndex; i < pushResults[0].pushes.size(); ++i)
	{
		const Push &push = pushResults[0].pushes[i];
		if(!push.trivial)
			break;
		tree.trivial.push_back(push);
	}
	// Now we got into a nontrivial push
	size_t nontrivialIndex = i;
	std::unordered_map<Push, std::vector<Inventory>> nontrivialPushes;
	for(const Inventory &result : pushResults)
	{
		if(result.pushes.size() <= nontrivialIndex)
			continue; // No nontrivial push in this result
		nontrivialPushes[result.pushes[nontrivialIndex]].push_back(result);
	}
	for(const auto &pair : nontrivialPushes)
	{
		PushTree childTree = PushTreeFromReachableResults(pair.second, nontrivialIndex + 1);
		tree.nontrivial.push_back({pair.first, std::move(childTree)});
	}

	return tree;
}

void PushTree::clear(const Push &push)
{
	for(size_t i = 0; i < trivial.size(); ++i)
	{
		const Push &item = trivial[i];
		if(item.from == push.from && item.wallpos == push.wallpos)
		{
			trivial[i] = trivial.back();
			trivial.pop_back();
			return;
		}
	}
	for(size_t i = 0; i < nontrivial.size(); ++i)
	{
		auto &pair = nontrivial[i];
		if(pair.first.from == push.from && pair.first.wallpos == push.wallpos)
		{
			// Extract data before modifying the vector
			std::vector<Push> childTrivial = std::move(pair.second.trivial);
			std::vector<std::pair<Push, PushTree>> childNontrivial = std::move(pair.second.nontrivial);

			// Now safely modify this tree
			trivial.insert(trivial.end(), childTrivial.begin(), childTrivial.end());
			nontrivial = std::move(childNontrivial);
			return;
		}
	}
}

// Called from SetupGameLevel, which calls ScanInfoPlane
PushTree AnalyzeSecrets()
{
	Tally total = {};
	GameState initialState(total);

	// Container to collect all inventories where exit is reachable
	std::vector<Inventory> exitReachableResults;

	// Set to track visited states to avoid redundant exploration
	std::unordered_set<SimMap> visitedStates;

	// Set up 5-second timeout for backtracking
	auto startTime = std::chrono::steady_clock::now();
	auto timeout = std::chrono::seconds(5);

	// Start the recursive exploration and backtracking
	BacktrackingExplorer explorer(exitReachableResults, startTime, timeout, visitedStates);
	{
		LoadingScreen loading;
		explorer.explore(initialState, loading, 1, progressTotal);
	}

	// Check if we timed out
	auto elapsed = std::chrono::steady_clock::now() - startTime;
	if (elapsed >= timeout)
	{
		Logger::Write("Backtracking timed out after %d seconds", (int)timeout.count());
	}

	if (exitReachableResults.empty())
	{
		Logger::Write("No solutions found with reachable exit");
		return {};
	}

	Logger::Write("Found %u solutions with reachable exit", (unsigned)exitReachableResults.size());

	// Find the maximum inventory (best solution)
	Inventory maxInventory = *std::max_element(exitReachableResults.begin(), exitReachableResults.end());

	// Keep only the inventories that are equal to the maximum (all optimal solutions)
	std::vector<Inventory> optimalSolutions;
	for (const Inventory& inv : exitReachableResults)
	{
		if (inv == maxInventory)
		{
			optimalSolutions.push_back(inv);
		}
	}

	Logger::Write("Found %u optimal solutions\n", (unsigned)optimalSolutions.size());
	if(maxInventory.enemiesKilled >= total.enemies)
		Logger::Write("All enemies killed in optimal solutions");
	if(maxInventory.treasureCollected >= total.treasure)
		Logger::Write("All treasure collected in optimal solutions");
	if((int)maxInventory.pushes.size() >= total.secrets)
		Logger::Write("All pushable walls pushed in optimal solutions");

	for(const Inventory &solution : optimalSolutions)
	{
		Logger::Write("Points: %d, Treasures: %d, Enemies: %d, Pushes: %d, Keys: %d",
					  solution.pointsCollected, solution.treasureCollected,
					  solution.enemiesKilled, (int)solution.pushes.size(),
					  solution.keys);
		for(const Push &push : solution.pushes)
		{
			Logger::Write("  Push from %d,%d to %d,%d at wall %d,%d (trivial: %s)",
						  (push.from%MAPSIZE)+1, (push.from/MAPSIZE)+1,
						  (push.to%MAPSIZE)+1, (push.to/MAPSIZE)+1,
						  (push.wallpos%MAPSIZE)+1, (push.wallpos/MAPSIZE)+1,
						  push.trivial ? "yes" : "no");
		}
	}

	return PushTreeFromReachableResults(optimalSolutions, 0);
}

// Returns a subtree if safe to push a nontrivial wall
bool PushTree::SafeToPush(int tx, int ty, int txofs, int tyofs, const Push *&nontrivialPush) const
{
	if(tx < 0 || tx >= MAPSIZE || ty < 0 || ty >= MAPSIZE)
		return false; // Out of bounds
	int wx = tx + txofs;
	int wy = ty + tyofs;
	if(wx < 0 || wx >= MAPSIZE || wy < 0 || wy >= MAPSIZE)
		return false; // Out of bounds after offset
	if(trivial.empty() && nontrivial.empty())
		return true; // No tree, nothing to check

	// Check if trivial
	for(const Push &push : trivial)
	{
		if(push.from == ty * MAPSIZE + tx && push.wallpos == wy * MAPSIZE + wx)
			return true;
	}
	for(const auto &pair : nontrivial)
	{
		if(pair.first.from == ty * MAPSIZE + tx && pair.first.wallpos == wy * MAPSIZE + wx)
		{
			nontrivialPush = &pair.first; // Found a non-trivial push that matches
			return true; // Found a non-trivial push that matches
		}
	}
	return false;
}

void PushTree::logStructure(int depth) const
{
	std::string indent(depth * 2, ' ');
	Logger::Write("%sPushTree: %u trivial, %u non-trivial",
				  indent.c_str(), (unsigned)trivial.size(), (unsigned)nontrivial.size());

	for (const Push& push : trivial)
	{
		Logger::Write("%s  Trivial push from %d,%d to %d,%d at wall %d,%d",
					  indent.c_str(),
					  (push.from%MAPSIZE)+1, (push.from/MAPSIZE)+1,
					  (push.to%MAPSIZE)+1, (push.to/MAPSIZE)+1,
					  (push.wallpos%MAPSIZE)+1, (push.wallpos/MAPSIZE)+1);
	}

	for (const auto& branch : nontrivial)
	{
		Logger::Write("%s  Non-trivial push from %d,%d to %d,%d at wall %d,%d",
					  indent.c_str(),
					  (branch.first.from%MAPSIZE)+1, (branch.first.from/MAPSIZE)+1,
					  (branch.first.to%MAPSIZE)+1, (branch.first.to/MAPSIZE)+1,
					  (branch.first.wallpos%MAPSIZE)+1, (branch.first.wallpos/MAPSIZE)+1);
		branch.second.logStructure(depth + 1);
	}
}

} // namespace Secret

