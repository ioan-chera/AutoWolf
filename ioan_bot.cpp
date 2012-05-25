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
		searchset[imin].tilex; 
		searchset[imin].tiley;
		searchset[imin].open = false;
		if(searchset[imin].prev > -1)
			searchset[searchset[imin].prev].next = imin;

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
			if(check && !ISPOINTER(check) && !(tilemap[cx][cy] & 0x80))
				continue;	// solid, can't be passed
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
// BotMan::FindPath
//
// Finds the path to walk through
//
void BotMan::DoCommand()
{
	// no path got defined
	if(!pathexists)
		if(!FindPathToExit())
			return;

	// found path to exit
	static int searchpos = 0;
	int nowon = -1;

	// look if it's there
	for(; searchpos != -1; searchpos = searchset[searchpos].next)
	{
		if(player->tilex == searchset[searchpos].tilex && player->tiley == searchset[searchpos].tiley)
		{
			nowon = searchpos;
			break;
		}
	}
	if(nowon < 0)
	{
		pathexists = false;
		return;
	}

	// now move!
	int delta = buttonstate[bt_run] ? RUNMOVE * tics : BASEMOVE * tics;

	short tangle, dangle;
	int nx, ny, mx, my;
	boolean tryuse = false;

	mx = player->tilex;
	my = player->tiley;
	
	// elevator
	if(searchset[nowon].next == -1)
	{
		nx = exitx;
		ny = exity;
		tryuse = true;
	}
	else
	{
		nx = searchset[searchset[nowon].next].tilex;
		ny = searchset[searchset[nowon].next].tiley;
		tryuse = (tilemap[nx][ny] & 0x80) == 0x80;
	}

	if(nx > mx)
		tangle = 0;
	else if(ny > my)
		tangle = 270;
	else if(nx < mx)
		tangle = 180;
	else if(ny < my)
		tangle = 90;

	if(tangle == 0)
	{
		if(player->angle > 345 || player->angle < 15)
			controly -= delta;
	}
	else
		if(tangle - player->angle < 15 || player->angle - tangle < 15)
			controly -= delta;

	dangle = tangle - player->angle;
	if(dangle > 180)
		dangle -= 360;
	else if(dangle <= -180)
		dangle += 360;

	buttonstate[bt_strafe] = false;

	if(dangle > 0)
		controlx -= delta;
	else if(dangle < 0)
		controlx += delta;
	else
	{
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
		{
			controlx += delta;
		}
		else if(plro - cento < -4096)
		{
			controlx -= delta;
		}
	}

	if(tryuse)
		buttonstate[bt_use] = !buttonstate[bt_use];
	else
		buttonstate[bt_use] = false;
}