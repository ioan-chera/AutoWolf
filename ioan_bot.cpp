//
// IOAN_BOT.CPP
//
// File by Ioan Chera, started 17.05.2012
//
// Basic bot setup. I'm working with classes here!
//

#include "ioan_bot.h"
#include "ioan_bas.h"

// static class member definition
boolean BotMan::active;
// protected ones
boolean BotMan::pathexists, BotMan::exitfound;
byte BotMan::nothingleft, BotMan::wakeupfire;
int BotMan::exitx, BotMan::exity, BotMan::exfrontx;
BotMan::SData *BotMan::searchset;
int BotMan::searchsize, BotMan::searchlen;

//
// BotMan::FindExit
//
// Finds the exit on the map
//
boolean BotMan::FindExit()
{
	int i, j;
	objtype *check1, *check2;

	for(i = 0; i < MAPSIZE; ++i)
	{
		for(j = 0; j < MAPSIZE; ++j)
		{
			// Look if one of the ways is free
			check1 = j > 0 ? actorat[j - 1][i] : (objtype *)1;
			check2 = j < MAPSIZE - 1 ? actorat[j + 1][i] : (objtype *)1;
			
			if(tilemap[j][i] == ELEVATORTILE)
			{
				if(check1 && !ISPOINTER(check1) && check2 && !ISPOINTER(check2))
				{
					// blocked
					continue;
				}
				else
				{
					// found
					exitx = j;
					exity = i;
					if(!check1 || ISPOINTER(check1))
						exfrontx = j - 1;
					else if(!check2 || ISPOINTER(check2))
						exfrontx = j + 1;
					return true;
				}
			}
		}
	}
	return false;
}

//
// BotMan::ApproxDist
//
// Approx. distance (too lazy to use sqrt)
//
int BotMan::ApproxDist(int dx, int dy)
{
	dx = abs(dx);
	dy = abs(dy);
	//if(dx < dy)
	//	return dx + dy - (dx >> 1);
	//return dx + dy - (dy >> 1);
	return dx + dy;
}

//
// BotMan::AddToSet
//
// Adds copy of value to set
//
void BotMan::AddToSet(const SData &data)
{
	if(++searchlen > searchsize)
	{
		searchset = (SData*)realloc(searchset, searchsize < 32 ? (searchsize = 32) * sizeof(SData) 
			: (searchsize = 2 * searchsize) * sizeof(SData));
		CHECKMALLOCRESULT(searchset);
	}
	searchset[searchlen - 1] = data;
}

//
// BotMan::FindPathToExit
//
// Finds the path to an exit (A*)
//
boolean BotMan::FindPathToExit()
{
	int i, j;
	// Look for map exit
	if(!exitfound)
		exitfound = FindExit();
	if(!exitfound)
		return false;

	SData data;
	data.g_score = 0;
	data.f_score = data.h_score = ApproxDist(exfrontx - player->tilex, exity - player->tiley);
	data.open = true;
	data.tilex = player->tilex;
	data.tiley = player->tiley;
	data.prev = -1;

	EmptySet();
	AddToSet(data);

	boolean openfound;
	int fmin, imin, ifound;
	int cx, cy;
	objtype *check;
	boolean neighinvalid, neighfound;
	int tentative_g_score;
	while(1)
	{
		// look for open sets
		openfound = false;
		fmin = INT_MAX;
		for(i = 0; i < searchlen; ++i)
		{
			if(searchset[i].open)
			{
				openfound = true;
				if(searchset[i].f_score < fmin)
				{
					fmin = searchset[i].f_score;
					imin = i;
				}
			}
		}
		if(!openfound)
			break;
		searchset[imin].open = false;

		if(searchset[imin].tilex == exfrontx && searchset[imin].tiley == exity)
		{
			// found goal
			searchset[imin].next = -1;
			ifound = imin;
			for(imin = searchset[imin].prev; imin != -1; ifound = imin, imin = searchset[imin].prev)
				searchset[imin].next = ifound;
			return true;
		}
		
		for(j = 0; j < 4; ++j)
		{
			switch(j)
			{
			case 0:
				cx = searchset[imin].tilex + 1;
				cy = searchset[imin].tiley;
				break;
			case 1:
				cx = searchset[imin].tilex;
				cy = searchset[imin].tiley + 1;
				break;
			case 2:
				cx = searchset[imin].tilex - 1;
				cy = searchset[imin].tiley;
				break;
			case 3:
				cx = searchset[imin].tilex;
				cy = searchset[imin].tiley - 1;
				break;
			}
			check = actorat[cx][cy];
			byte door = tilemap[cx][cy];
			if(check && !ISPOINTER(check) && !(door & 0x80))
				continue;	// solid, can't be passed
			else if (door & 0x80)
			{
				door = door & ~0x80;
				byte lock = doorobjlist[door].lock;
				if (lock >= dr_lock1 && lock <= dr_lock4)
					if ( ! (gamestate.keys & (1 << (lock-dr_lock1) ) ) )
						continue;
			}
			neighinvalid = false;
			neighfound = false;
			for(i = searchlen - 1; i >= 0; --i)
			{
				if(searchset[i].tilex == cx && searchset[i].tiley == cy)
				{
					if(!searchset[i].open)
						neighinvalid = true;
					else
					{
						neighfound = true;
						ifound = i;
					}
					break;
				}
			}
			if(neighinvalid)
				continue;
			tentative_g_score = searchset[imin].g_score + 1;
			if(!neighfound)
			{
				data.g_score = tentative_g_score;
				data.h_score = ApproxDist(exfrontx - cx, exity - cy);
				data.f_score = data.g_score + data.h_score;
				data.open = true;
				data.tilex = cx;
				data.tiley = cy;
				data.prev = imin;
				AddToSet(data);
			}
			else if(tentative_g_score < searchset[ifound].g_score)
			{
				searchset[ifound].g_score = tentative_g_score;
				searchset[ifound].f_score = tentative_g_score + searchset[ifound].h_score;
				searchset[ifound].prev = imin;
			}
		}
	}

	return false;
}

//
// BotMan::ObjectOfInterest
//
// Pickable item, secret door, exit switch, exit pad
//
boolean BotMan::ObjectOfInterest(int tx, int ty)
{
	int i;
	exitx = -1;
	exity = -1;

	// items
	for(i = 0; &statobjlist[i] != laststatobj; ++i)
	{
		if(statobjlist[i].tilex == (byte)tx && statobjlist[i].tiley == (byte)ty && statobjlist[i].flags & FL_BONUS
			&& statobjlist[i].shapenum >= 0)
		{
			switch (statobjlist[i].itemnumber)
			{
			case    bo_firstaid:
				if (gamestate.health <= 75)
					return true;
				break;
			case    bo_food:
				if (gamestate.health <= 90)
					return true;
				break;
			case    bo_alpo:
				if (gamestate.health <= 96)
					return true;
				break;
			case    bo_key1:
			case    bo_key2:
			case    bo_key3:
			case    bo_key4:
			case    bo_cross:
			case    bo_chalice:
			case    bo_bible:
			case    bo_crown:
			case    bo_machinegun:
			case    bo_chaingun:
			case    bo_fullheal:
				return true;
			case    bo_clip:
				if(gamestate.ammo <= 91)
					return true;
				break;
			case    bo_clip2:
				if (gamestate.ammo <= 95)
					return true;
				break;
#ifdef SPEAR
			case    bo_25clip:
				if (gamestate.ammo < 74)
					return true;
				break;
			case    bo_spear:
				return true;
#endif
			case    bo_gibs:
				if (gamestate.health <= 10)
					return true;
				break;
			}
		}
	}

	objtype *check = actorat[tx][ty];
	if(gamestate.health > 75 && gamestate.ammo > 75 &&
		check && ISPOINTER(check) && Basic::IsEnemy(check->obclass) && check->flags & FL_SHOOTABLE)
	{
		nothingleft = 0;	// reset counter if enemies here
		return true;
	}

	// secret door
	if(ty + 3 < MAPSIZE && *(mapsegs[1]+((ty + 1)<<mapshift)+tx) == PUSHABLETILE)
	{
		if(!actorat[tx][ty+2] && !actorat[tx][ty+3])
		{
			exity = ty + 1;
			exitx = tx;
			return true;
		}
	}
	if(ty - 3 >= 0 && *(mapsegs[1]+((ty - 1)<<mapshift)+tx) == PUSHABLETILE)
	{
		if(!actorat[tx][ty-2] && !actorat[tx][ty-3])
		{
			exity = ty - 1;
			exitx = tx;
			return true;
		}
	}
	if(tx + 3 < MAPSIZE && *(mapsegs[1]+(ty<<mapshift)+tx + 1) == PUSHABLETILE)
	{
		if(!actorat[tx+2][ty] && !actorat[tx+3][ty])
		{
			exity = ty;
			exitx = tx + 1;
			return true;
		}
	}
	if(tx - 3 >= 0 && *(mapsegs[1]+(ty<<mapshift)+tx - 1) == PUSHABLETILE)
	{
		if(!actorat[tx-2][ty] && !actorat[tx-3][ty])
		{
			exity = ty;
			exitx = tx - 1;
			return true;
		}
	}

	// exit switch
	if(nothingleft >= 1)
	{
		if(tx - 1 >= 0 && tilemap[tx - 1][ty] == ELEVATORTILE) 
		{
			exitx = tx - 1;
			exity = ty;
			if(*(mapsegs[0]+(ty<<mapshift)+tx) == ALTELEVATORTILE || nothingleft >= 2)
				return true;
		}
		if(tx + 1 < MAPSIZE && tilemap[tx + 1][ty] == ELEVATORTILE)
		{
			exitx = tx + 1;
			exity = ty;
			if(*(mapsegs[0]+(ty<<mapshift)+tx) == ALTELEVATORTILE || nothingleft >= 2)
				return true;
		}

		// exit pad
		if(nothingleft >= 2 && *(mapsegs[1]+(ty<<mapshift)+tx) == EXITTILE)
			return true;
	}
	return false;
}

//
// BotMan::FindPathToExit
//
// Finds the path to an exit (A*)
//
boolean BotMan::FindRandomPath(boolean ignoreproj, boolean mindnazis)
{
	int i, j;

	SData data;

	data.g_score = 0;
	data.f_score = 0;
	data.open = true;
	data.tilex = player->tilex;
	data.tiley = player->tiley;
	data.prev = -1;

	EmptySet();
	AddToSet(data);

	boolean openfound;
	int fmin, imin, ifound;
	int cx, cy;
	objtype *check;
	boolean neighinvalid, neighfound;
	int tentative_g_score;
	while(1)
	{
		// look for open sets
		openfound = false;
		fmin = INT_MAX;
		for(i = 0; i < searchlen; ++i)
		{
			if(searchset[i].open)
			{
				openfound = true;
				if(searchset[i].f_score < fmin)
				{
					fmin = searchset[i].f_score;
					imin = i;
				}
			}
		}
		if(!openfound)
			break;
		searchset[imin].tilex;
		searchset[imin].tiley;
		searchset[imin].open = false;

		if(ObjectOfInterest(searchset[imin].tilex, searchset[imin].tiley))
		{
			// found goal
			searchset[imin].next = -1;
			ifound = imin;
			for(imin = searchset[imin].prev; imin != -1; ifound = imin, imin = searchset[imin].prev)
				searchset[imin].next = ifound;
			pathexists = true;
			return true;
		}
		
		for(j = 0; j < 4; ++j)
		{
			switch(j)
			{
			case 0:
				cx = searchset[imin].tilex + 1;
				cy = searchset[imin].tiley;
				break;
			case 1:
				cx = searchset[imin].tilex;
				cy = searchset[imin].tiley + 1;
				break;
			case 2:
				cx = searchset[imin].tilex - 1;
				cy = searchset[imin].tiley;
				break;
			case 3:
				cx = searchset[imin].tilex;
				cy = searchset[imin].tiley - 1;
				break;
			}
			check = actorat[cx][cy];
			byte door = tilemap[cx][cy];
			if(check && !ISPOINTER(check) && !(door & 0x80)
				|| 
				(abs(cx - player->tilex) > 1 || abs(cy - player->tiley) > 1) && IsProjectile(cx, cy, 1) && !ignoreproj ||
				IsEnemyAround(cx, cy, 1) && mindnazis)
				continue;	// solid, can't be passed
			else if (door & 0x80)
			{
				door = door & ~0x80;
				byte lock = doorobjlist[door].lock;
				if (lock >= dr_lock1 && lock <= dr_lock4)
					if (doorobjlist[door].action != dr_open &&
						doorobjlist[door].action != dr_opening && !(gamestate.keys & (1 << (lock-dr_lock1) ) ) )
						continue;
			}

			neighinvalid = false;
			neighfound = false;
			for(i = searchlen - 1; i >= 0; --i)
			{
				if(searchset[i].tilex == cx && searchset[i].tiley == cy)
				{
					if(!searchset[i].open)
						neighinvalid = true;
					else
					{
						neighfound = true;
						ifound = i;
					}
					break;
				}
			}
			if(neighinvalid)
				continue;
			tentative_g_score = searchset[imin].g_score + 1;
			if(!neighfound)
			{
				data.g_score = tentative_g_score;
				data.h_score = 0;
				data.f_score = data.g_score;
				data.open = true;
				data.tilex = cx;
				data.tiley = cy;
				data.prev = imin;
				AddToSet(data);
			}
			else if(tentative_g_score < searchset[ifound].g_score)
			{
				searchset[ifound].g_score = tentative_g_score;
				searchset[ifound].f_score = tentative_g_score;
				searchset[ifound].prev = imin;
			}
		}
	}

	if(!pwallstate)
		nothingleft++;

	return false;
}

//
// BotMan::EnemyOnTarget
//
// True if can shoot
//
objtype *BotMan::EnemyOnTarget()
{
	objtype *check,*closest,*oldclosest;
	int32_t  viewdist;

	//
	// find potential targets
	//
	viewdist = 0x7fffffffl;
	closest = NULL;

	while (1)
	{
		oldclosest = closest;

		for (check=player->next ; check ; check=check->next)
		{
			if ((check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
				&& abs(check->viewx-centerx) < shootdelta)
			{
				if (check->transx < viewdist)
				{
					viewdist = check->transx;
					closest = check;
				}
			}
		}

		if (closest == oldclosest)
			return NULL;                                         // no more targets, all missed

		//
		// trace a line from player to enemey
		//

		if (CheckLine(closest))
			return closest;
	}
	return NULL;
}

//
// BotMan::EnemyVisible
//
// True if an enemy is in sight
//
objtype *BotMan::EnemyVisible(short *angle, int *distance)
{
	int tx = player->tilex, ty = player->tiley;
	int i, j, k, distmin;
	objtype *ret, *retmin = NULL;
	short angmin;
	static objtype *oldret;
	double dby, dbx;

	distmin = INT_MAX;

	for(ret = lastobj; ret; ret = ret->prev)
	{
		k = abs(ret->tilex - tx);
		j = abs(ret->tiley - ty);
		i = k > j ? k : j;

		if(k > 15 || j > 15)
			continue;
		if(!Basic::IsEnemy(ret->obclass) || !(ret->flags & FL_SHOOTABLE) || !CheckLine(ret))
			continue;

		if(abs(*distance - i) >= 2 && ret != oldret || ret == oldret || oldret && !(oldret->flags & FL_SHOOTABLE))
		{
			if(i <= distmin)
			{
				dby = -((double)(ret->y) - (double)(player->y));
				dbx = (double)(ret->x) - (double)(player->x);
				distmin = i;
				angmin = (short)(180.0/PI*atan2(dby, dbx));

				if(angmin >= 360)
					angmin -= 360;
				if(angmin < 0)
					angmin += 360;

				retmin = ret;
			}
		}
	}

	if(retmin)
	{
		oldret = retmin;
		*angle = angmin;
		*distance = distmin;
	}

	return retmin;
}

//
// BotMan::GenericEnemyVisible
//
// True if an enemy is in sight
//
objtype *BotMan::GenericEnemyVisible(int tx, int ty)
{
	int x = Basic::Major(tx);
	int y = Basic::Major(ty);
	int j, k;
	objtype *ret;

	for(j = -15; j <= 15; ++j)
	{
		for(k = -15; k <= 15; ++k)
		{
			if(ty + j < 0 || ty + j >= MAPSIZE || tx + k < 0 || tx + k >= MAPSIZE)
				continue;
			ret = actorat[tx + k][ty + j];
			if(ret && ISPOINTER(ret) && Basic::IsEnemy(ret->obclass) && ret->flags & FL_SHOOTABLE 
				&& Basic::GenericCheckLine(ret->x, ret->y, x, y))
			{
				return ret;
			}
		}
	}
	return NULL;
}

//
// BotMan::EnemyEager
//
// True if 1 non-ambush enemies are afoot 
//
objtype *BotMan::EnemyEager()
{
	objtype *ret;

	for(ret = lastobj; ret; ret = ret->prev)
	{
		if(Basic::IsEnemy(ret->obclass) && ret->flags & FL_SHOOTABLE && areabyplayer[ret->areanumber] &&
			!(ret->flags & (FL_AMBUSH | FL_ATTACKMODE)))
			return ret;
	}
	return NULL;
}

//
// BotMan::DamageThreat
//
// true if damage is imminent and retreat should be done
//
objtype *BotMan::DamageThreat(objtype *targ)
{
	int tx = player->tilex, ty = player->tiley;
	int j, k, dist;
	objtype *ret;

	// Threat types
	// Guard: dist 2, shooting
	// Officer: dist 4, shooting
	// SS: dist 7, shooting
	// Mutant: dist 4, just being there
	// Hans: dist 8, shooting
	// Monster: dist 2, just being there
	// Hitler: dist 6, shooting
	// Gretel: dist 7, shooting
	// Fat: dist 6, shooting
	// Trans: dist 7, shooting
	// Will: dist 6, shooting
	// Uber: dist 6, shooting
	// Death: dist 6, shooting

	for(ret = lastobj; ret; ret = ret->prev)
	{
		k = ret->tilex - tx;
		j = ret->tiley - ty;
		dist = abs(j) > abs(k) ? abs(j) : abs(k);
		if(dist > 16 || !Basic::IsEnemy(ret->obclass) || !(ret->flags & FL_SHOOTABLE) || !CheckLine(ret) 
			|| ret == targ && !Basic::IsBoss(ret->obclass))
			continue;

		if(Basic::IsDamaging(ret, dist))
			return ret;
		
	}
	return NULL;
}

//
// BotMan::Crossfire
//
// Returns true if there's a crossfire in that spot
//
objtype *BotMan::Crossfire(int tx, int ty)
{
	int j, k, dist;
	objtype *ret;

	for(ret = lastobj; ret; ret = ret->prev)
	{
		k = ret->tilex - tx;
		j = ret->tiley - ty;
		dist = abs(j) > abs(k) ? abs(j) : abs(k);
		if(dist > 16 || !Basic::IsEnemy(ret->obclass) || !(ret->flags & FL_SHOOTABLE)
			|| !Basic::GenericCheckLine(ret->x, ret->y, 
			Basic::Major(tx),
			Basic::Major(ty)))
			continue;

		if(Basic::IsDamaging(ret, dist))
			return ret;
		
	}
	return NULL;
}

//
// BotMan::DoRetreat
//
// Retreats the bot sliding off walls
//
void BotMan::DoRetreat(boolean forth, objtype *cause)
{
	int neg = forth? -1 : 1;
	controly = neg * RUNMOVE * tics;
	int j, backx, backy, sidex, sidey, tx = player->tilex, ty = player->tiley, dir;
	if(player->angle > 0 && player->angle <= 45)
	{
		backx = -1*neg;
		backy = 0;
		sidex = 0;
		sidey = 1*neg;
		dir = RUNMOVE*neg;
	}
	else if(player->angle > 45 && player->angle <= 90)
	{
		backx = 0;
		backy = 1*neg;
		sidex = -1*neg;
		sidey = 0;
		dir = -RUNMOVE*neg;
	}
	else if(player->angle > 90 && player->angle <= 135)
	{
		backx = 0;
		backy = 1*neg;
		sidex = 1*neg;
		sidey = 0;
		dir = RUNMOVE*neg;
	}
	else if(player->angle > 135 && player->angle <= 180)
	{
		backx = 1*neg;
		backy = 0;
		sidex = 0;
		sidey = 1*neg;
		dir = -RUNMOVE*neg;
	}
	else if(player->angle > 180 && player->angle <= 225)
	{
		backx = 1*neg;
		backy = 0;
		sidex = 0;
		sidey = -1*neg;
		dir = RUNMOVE*neg;
	}
	else if(player->angle > 225 && player->angle <= 270)
	{
		backx = 0;
		backy = -1*neg;
		sidex = 1*neg;
		sidey = 0;
		dir = -RUNMOVE*neg;
	}
	else if(player->angle > 270 && player->angle <= 315)
	{
		backx = 0;
		backy = -1*neg;
		sidex = -1*neg;
		sidey = 0;
		dir = RUNMOVE*neg;
	}
	else
	{
		backx = -1*neg;
		backy = 0;
		sidex = 0;
		sidey = -1*neg;
		dir = -RUNMOVE*neg;
	}
	if(tx <= 0 || tx >= MAPSIZE - 1 || ty <= 0 || ty >= MAPSIZE - 1)
		return;
//	backx += tx;
//	backy += ty;
//	sidex += tx;
//	sidey += ty;

	objtype *check1 = actorat[tx + backx][ty + backy], *check2;
	if(check1 && !ISPOINTER(check1) || check1 && ISPOINTER(check1) && check1->flags & FL_SHOOTABLE)
	{
		// Look for way backwards
		if(sidex)	// strafing east or west
		{
			for(j = tx + sidex; ; j += sidex)
			{
				if(j >= MAPSIZE)
					break;
				check2 = actorat[j][ty];
				if(check2 && !ISPOINTER(check2) || check2 && ISPOINTER(check2) && check2->flags & FL_SHOOTABLE)
					break;	// stopped

				check2 = actorat[j][ty + backy];
				if(!check2 || check2 && ISPOINTER(check2) && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
					goto solved;// found here
			}
			for(j = tx - sidex; ; j -= sidex)
			{
				if(j < 0)
					break;
				check2 = actorat[j][ty];
				if(check2 && !ISPOINTER(check2) || check2 && ISPOINTER(check2) && check2->flags & FL_SHOOTABLE)
					break;

				check2 = actorat[j][ty + backy];
				if(!check2 || check2 && ISPOINTER(check2) && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
				{
					dir = -dir;// found here
					sidex = -sidex;
					sidey = -sidey;
					controly = 0;
					break;
				}
			}
		}
		else if(sidey)
		{
			for(j = ty + sidey; ; j += sidey)
			{
				if(j >= MAPSIZE)
					break;
				check2 = actorat[tx][j];
				if(check2 && !ISPOINTER(check2) || check2 && ISPOINTER(check2) && check2->flags & FL_SHOOTABLE)
					break;	// stopped

				check2 = actorat[tx + backx][j];
				if(!check2 || check2 && ISPOINTER(check2) && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
					goto solved;// found here
			}
			for(j = ty - sidey; ; j -= sidey)
			{
				if(j < 0)
					break;
				check2 = actorat[tx][j];
				if(check2 && !ISPOINTER(check2) || check2 && ISPOINTER(check2) && check2->flags & FL_SHOOTABLE)
					break;

				check2 = actorat[tx + backx][j];
				if(!check2 || check2 && ISPOINTER(check2) && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
				{
					dir = -dir;// found here
					sidex = -sidex;
					sidey = -sidey;
					controly = 0;
					break;
				}
			}
		}
solved:
		check2 = actorat[tx + sidex][ty + sidey];
		if(check2 && !ISPOINTER(check2) || check2 && ISPOINTER(check2) && check2->flags & FL_SHOOTABLE)
			return;
		buttonstate[bt_strafe] = true;
		controlx = dir*tics;
	}
	else
		buttonstate[bt_strafe] = false;
}

//
// BotMan::IsProjectile
//
// True if a flying projectile exists in this place
//
objtype *BotMan::IsProjectile(int tx, int ty, int dist, short *angle, int *distance)
{
	objtype *ret = lastobj;

	while(ret)
	{
		if(abs(ret->tilex - tx) <= dist && abs(ret->tiley - ty) <= dist)
		{
			switch(ret->obclass)
			{
#ifndef SPEAR
			case needleobj:
			case fireobj:
			case ghostobj:	// monsters count as projectiles
				goto retok;
			case rocketobj:
				if(ret->state != &s_boom1 && ret->state != &s_boom2 && ret->state != &s_boom3)
					goto retok;
				break;
#endif
#ifdef SPEAR
			case hrocketobj:
				if(ret->state != &s_hboom1 && ret->state != &s_hboom2 && ret->state != &s_hboom3)
					goto retok;
				break;
			case sparkobj:
				goto retok;
#endif
			}
		}
		ret = ret->prev;
	}
	return NULL;
retok:


	if(angle && distance)
	{
		
		double dby = -((double)(ret->y) - (double)(player->y));
		double dbx = (double)(ret->x) - (double)(player->x);

		*angle = (short)(180.0/PI*atan2(dby, dbx));
		*distance = dist;
					
		if(*angle >= 360)
			*angle -= 360;
		if(*angle < 0)
			*angle += 360;
	}

	return ret;
}

//
// BotMan::IsEnemyAround
//
// True if a living enemy exists around
//
objtype *BotMan::IsEnemyAround(int tx, int ty, int dist)
{
	objtype *ret = lastobj;

	while(ret)
	{
		if(abs(ret->tilex - tx) <= dist && abs(ret->tiley - ty) <= dist)
			if(Basic::IsEnemy(ret->obclass) && ret->flags & FL_SHOOTABLE && ret->flags & FL_ATTACKMODE)
				return ret;
		ret = ret->prev;
	}
	return NULL;
}

//
// BotMan::FindPath
//
// Finds the path to walk through
//
void BotMan::DoCommand()
{
	static short tangle, dangle;
	static byte pressuse, retreat;

	++pressuse;	// key press timer
	static short eangle = -1;
	static int edist = -1;

	objtype *check0, *check, *check2;

	check0 = EnemyVisible(&eangle, &edist);
	if(check0)
	{
		nothingleft = 0;	// reset elevator counter if distracted

		if(gamestate.weapon == wp_knife)
			if(FindRandomPath(false, true))
			{
				pathexists = false;
				goto disengage;
			}

		pathexists = false;
		// Enemy visible mode
		// centred angle here
		dangle = eangle - player->angle;
		if(dangle > 180)
			dangle -= 360;
		else if(dangle <= -180)
			dangle += 360;

		buttonstate[bt_strafe] = false;
		// turn towards nearest target
			
		if(dangle > 15)
			controlx = -RUNMOVE * tics;
		else if(dangle > 0)
			controlx = -BASEMOVE * tics;
		else if(dangle < -15)
			controlx = +RUNMOVE * tics;
		else if(dangle < 0)
			controlx = +BASEMOVE * tics;
	
		// FIXME: work around the numbness bug
		check = EnemyOnTarget();
		check2 = DamageThreat(check);
		short pangle, epangle; int proj = 0;
		if(!check2)
		{
			check2 = IsProjectile(player->tilex, player->tiley, 2, &pangle, &proj);
			proj = 1;
		}

		if(check2 && (check2 != check || Basic::IsBoss(check->obclass)) && gamestate.weapon != wp_knife)
		{
			DoRetreat(false, check2);
			
			if(proj)
			{
				epangle = pangle - player->angle;
				if(epangle > 180)
					epangle -= 360;
				else if(epangle <= -180)
					epangle += 360;
				buttonstate[bt_strafe] = true;
				if(epangle >= 0)	// strafe right
					controlx = RUNMOVE * tics;
				else
					controlx = -RUNMOVE * tics;
			}
			retreat = 10;
			if(check2->obclass == mutantobj)
				retreat = 5;
		}
		else
			retreat = 0;

		if(!check && dangle >= -15 && dangle <= 15 && !retreat)
			controly = -BASEMOVE * tics;	// something's wrong, so move a bit

		if(check)
		{
			// shoot according to how the weapon works
			if((gamestate.weapon <= wp_pistol && pressuse % 4 == 0 || gamestate.weapon > wp_pistol) && edist <= 10)
				buttonstate[bt_attack] = true;
			else
				buttonstate[bt_attack] = false;

			// TODO: don't always charge when using the knife!

			if(!retreat && (!check2 || check2 == check) && edist > 6 || dangle > -45 && dangle < 45 && gamestate.weapon == wp_knife)
			{
				// otherwise
				// TODO: use DoCharge here, to maintain slaloming 
				if(gamestate.weapon == wp_knife)
					controly = -RUNMOVE * tics;
				else
					DoRetreat(true);
			}
		}
	}
	else if(retreat)	// standard retreat, still moving
	{
		// retreat mode (to be removed?)
		edist = -1;
		retreat--;
		DoRetreat();
	}
	else
	{
disengage:
			// no path got defined
		if(!pathexists)
		{
	//		if(!FindPathToExit())
	//			return;
			if(!FindRandomPath(false, gamestate.weapon == wp_knife))
			{
				if(!FindRandomPath(true, gamestate.weapon == wp_knife))
					return;
			}
		}

		static boolean waitpwall;
		if(pwallstate)
			waitpwall = true;

		// found path to exit
		int searchpos = 0;
		int nowon = -1;

		// look if it's there
		for(; searchpos != -1; searchpos = searchset[searchpos].next)
		{
			if(player->tilex == searchset[searchpos].tilex && player->tiley == searchset[searchpos].tiley)
			{
				nowon = searchpos;
//				break;
			}
			if(nowon > -1 && IsProjectile(searchset[searchpos].tilex, searchset[searchpos].tiley)
				&& (abs(searchset[searchpos].tilex - player->tilex) > 1 || 
				abs(searchset[searchpos].tiley - player->tiley) > 1))
			{
				nothingleft = 0;
				pathexists = false;
				if(!FindRandomPath())
					FindRandomPath(true);
				return;
			}
		}

		if(nowon < 0 || !pwallstate && waitpwall)
		{
			// Reset if out of the path, or if a pushwall stopped moving
			waitpwall = false;
			pathexists = false;
			return;
		}

		int nx, ny, mx, my;
		boolean tryuse = false;	// whether to try using

		mx = player->tilex;
		my = player->tiley;
		
		// elevator
		byte tile;
		if(searchset[nowon].next == -1)
		{
			// end of path; start a new search
			pathexists = false;
			if(exitx >= 0 && exity >= 0)
			{
				// elevator switch: press it now
				nx = exitx;
				ny = exity;
				tryuse = true;
			}
			else if(searchset[nowon].prev >= 0)
			{
				// just go forward
				nx = 2*mx - searchset[searchset[nowon].prev].tilex;
				ny = 2*my - searchset[searchset[nowon].prev].tiley;
			}
			else
			{
				// if undefined, just go east
				nx = mx + 1;
				ny = my;
			}
		}
		else
		{
			// in the path
			nx = searchset[searchset[nowon].next].tilex;
			ny = searchset[searchset[nowon].next].tiley;
			tile = tilemap[nx][ny];
			// whether to press to open a door
			tryuse = (tile & 0x80) == 0x80 
				&& (doorobjlist[tile & ~0x80].action == dr_closed || doorobjlist[tile & ~0x80].action == dr_closing);
		}

		// set up the target angle
		if(nx > mx)
			tangle = 0;
		else if(ny > my)
			tangle = 270;
		else if(nx < mx)
			tangle = 180;
		else if(ny < my)
			tangle = 90;

		dangle = tangle - player->angle;
		if(dangle > 180)	// centred angle
			dangle -= 360;
		else if(dangle <= -180)
			dangle += 360;

		edist = -1;
		// Non-combat mode
		if(EnemyEager() && gamestate.weapon >= wp_pistol && gamestate.ammo >= 20 && pressuse % 64 == 0)
		{
			// shoot something to alert nazis
			// TODO: more stealth gameplay considerations?
			buttonstate[bt_attack] = true;
		}
		else
			buttonstate[bt_attack] = false;

		wakeupfire = 0;	// near dead

		// Move forward only if it's safe (replace this bloatness with a function)

		// What if there are projectiles
//		if(searchset[nowon].next >= 0 && IsProjectile(searchset[searchset[nowon].next].tilex,
//			searchset[searchset[nowon].next].tiley, 2))
//		{
//			pathexists = false;
//			FindRandomPath();
//			return;
//		}

		if(dangle > -45 && dangle < 45 && (Crossfire(player->tilex, player->tiley)
			|| !(searchset[nowon].next >= 0 
			&& Crossfire(searchset[searchset[nowon].next].tilex, searchset[searchset[nowon].next].tiley))
			&& !(searchset[nowon].next >= 0 && searchset[searchset[nowon].next].next >= 0
			&& Crossfire(searchset[searchset[searchset[nowon].next].next].tilex, 
			searchset[searchset[searchset[nowon].next].next].tiley))))
		{
			// So move
			controly = -RUNMOVE * tics;
			// Press if there's an obstacle ahead
			if(tryuse && (actorat[nx][ny] && !ISPOINTER(actorat[nx][ny])) && pressuse % 4 == 0)
				buttonstate[bt_use] = true;
			else
				buttonstate[bt_use] = false;
		}

		// normally just turn (it's non-combat, no problem)
		buttonstate[bt_strafe] = false;

		if(dangle > 15)
			controlx = -RUNMOVE * tics;
		else if(dangle > 0)
			controlx = -BASEMOVE * tics;
		else if(dangle < -15)
			controlx = +RUNMOVE * tics;
		else if(dangle < 0)
			controlx = +BASEMOVE * tics;
		else
		{
			// straight line: can strafe now
			buttonstate[bt_strafe] = true;
			fixed centx, centy, cento, plro;
			centx = (player->tilex << TILESHIFT);
			centy = (player->tiley << TILESHIFT);
			
			switch(tangle)
			{
			case 0:
				cento = -centy;
				plro = -player->y + (1<<(TILESHIFT - 1));
				break;
			case 90:
				cento = -centx;
				plro = -player->x + (1<<(TILESHIFT - 1));
				break;
			case 180:
				cento = centy;
				plro = player->y - (1<<(TILESHIFT - 1));
				break;
			case 270:
				cento = centx;
				plro = player->x - (1<<(TILESHIFT - 1));
				break;
			}
			if(plro - cento > 4096)
				controlx = BASEMOVE * tics;
			else if(plro - cento < -4096)
				controlx = -BASEMOVE * tics;
		}
	}
}