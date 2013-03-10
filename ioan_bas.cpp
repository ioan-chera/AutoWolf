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


#include "ioan_bas.h"
#include "ioan_bot.h"
#include "obattrib.h"
#include "wl_act1.h"
#include "wl_act2.h"

// static members definition
boolean Basic::nonazis, Basic::secretstep3;
List<void *> Basic::livingNazis, Basic::thrownProjectiles;
static List<byte> _itemList[MAPSIZE][MAPSIZE];

//
// _markov
//
// Structure for random text generation.
// Markov transition matrix.
// Each column is A to Z then space (' '), each row likewise. Each cell 
// describes the probability weight of going from letter of row to letter of 
// column.
//
static int _markov[27][27] =
{
	{0,26,45,16,0,2,8,0,25,1,14,74,93,202,0,20,0,120,47,137,13,13,5,0,25,0,64},
	{16,0,0,0,26,0,0,0,8,5,0,46,0,0,10,0,0,7,3,1,60,0,0,0,29,0,3},
	{35,0,11,0,144,0,0,55,22,0,2,18,0,0,149,0,0,1,0,53,18,0,0,0,0,0,28},
	{19,0,0,5,96,0,1,0,114,0,0,1,0,0,20,0,0,2,6,0,4,1,0,0,2,0,205},
	{31,1,67,120,49,14,5,0,21,0,0,22,24,165,5,17,9,229,112,20,1,20,7,26,7,0,672},
	{3,0,0,0,21,11,0,0,22,0,0,1,0,0,67,0,0,45,0,36,5,0,0,0,14,0,140},
	{6,0,0,0,44,0,1,34,11,0,0,0,1,17,1,0,0,87,1,0,4,0,0,0,0,0,62},
	{79,0,0,0,284,0,0,0,74,0,0,1,0,0,60,0,0,5,0,29,4,0,0,0,3,0,60},
	{14,62,132,23,46,59,41,0,0,0,1,32,28,179,121,6,0,21,192,133,4,37,0,0,0,0,0},
	{1,0,0,0,5,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,0,0,0,0,0,0},
	{0,0,0,0,15,0,0,0,4,0,0,0,0,3,0,0,0,0,6,0,0,0,0,0,0,0,28},
	{31,0,1,15,60,2,0,0,126,0,0,46,0,0,15,0,0,0,12,4,14,1,0,0,36,0,61},
	{57,4,0,0,44,0,0,0,24,0,0,0,6,0,37,29,0,0,33,0,10,0,0,0,0,0,106},
	{20,0,31,141,56,6,79,0,7,0,1,7,0,8,70,0,0,0,122,114,12,2,0,0,45,0,198},
	{1,9,2,43,6,147,79,1,4,0,0,23,49,219,6,64,0,198,26,79,164,19,24,0,3,0,149},
	{37,0,0,0,33,0,0,3,19,0,0,39,0,0,24,17,0,125,1,13,29,0,0,0,38,0,3},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0},
	{141,4,27,9,194,4,5,0,137,0,29,0,44,3,125,7,0,23,28,35,6,3,3,0,17,0,246},
	{13,0,7,0,169,3,0,28,46,0,3,6,0,0,74,13,0,0,26,110,27,0,0,0,5,0,350},
	{51,0,0,0,162,0,0,402,166,0,0,3,0,2,121,2,0,72,38,11,3,0,37,0,29,0,251},
	{4,27,14,9,6,0,5,0,10,0,0,18,12,47,1,1,0,55,50,90,0,0,0,0,0,0,85},
	{8,0,0,0,90,0,0,0,19,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0},
	{58,0,0,0,15,0,0,42,40,0,0,0,0,0,33,0,0,10,0,0,0,0,0,0,0,0,30},
	{2,0,10,0,9,0,0,0,0,0,0,0,0,0,0,3,0,0,0,2,0,0,0,0,0,0,0},
	{1,0,0,0,4,0,0,0,8,0,0,0,0,1,103,3,0,16,7,1,0,0,0,0,0,0,209},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{322,81,189,95,66,117,45,34,224,3,6,87,93,73,269,199,1,74,170,482,44,24,152,0,100,0,0,}
};

//
// _marktot
//
// Sum of each row from _markov. Required to calculate the normalized 
// probabilities.
//
static int _marktot[27] =
	{950,214,536,476,1644,365,269,599,1131,9,56,424,350,919,1315,381,10,1090,880,1350,434,120,228,26,353,0,2950};

//
// Basic::EmptyItemList
//
// empty itemList
//
void Basic::EmptyItemList()
{
	int i, j;
	for(i = 0; i < MAPSIZE; ++i)
		for(j = 0; j < MAPSIZE; ++j)
			_itemList[i][j].removeAll();
}

//
// Basic::AddItemToList
//
void Basic::AddItemToList(int tx, int ty, byte itemtype)
{
    _itemList[tx][ty].add(itemtype);
}

//
// Basic::RemoveItemFromList
//
void Basic::RemoveItemFromList(int tx, int ty, byte itemtype)
{
	_itemList[tx][ty].remove(itemtype);
}

//
// Basic::FirstObjectAt
//
byte Basic::FirstObjectAt(int tx, int ty)
{
	return _itemList[tx][ty].firstObject();
}

//
// Basic::NextObjectAt
//
byte Basic::NextObjectAt(int tx, int ty)
{
	return _itemList[tx][ty].nextObject();
}

//
// _SpawnStand
//
// Made protected.
//
// IOANCH 29.06.2012: made objtype *
//
static objtype *_SpawnStand (classtype which, int tilex, int tiley, int dir)
{
    word *map;
    word tile;
    
    if(atr::states[which].stand)
    {
        SpawnNewObj(tilex, tiley, atr::states[which].stand);
        if(atr::speeds[which].patrol >= 0)
            newobj->speed = atr::speeds[which].patrol;
        if(!loadedgame)
            gamestate.killtotal++;
    
        map = mapsegs[0]+(tiley<<mapshift)+tilex;
        tile = *map;
        if (tile == AMBUSHTILE)
        {
            tilemap[tilex][tiley] = 0;

            if (*(map+1) >= AREATILE)
                tile = *(map+1);
            if (*(map-mapwidth) >= AREATILE)
                tile = *(map-mapwidth);
            if (*(map+mapwidth) >= AREATILE)
                tile = *(map+mapwidth);
            if ( *(map-1) >= AREATILE)
                tile = *(map-1);

            *map = tile;
            newobj->areanumber = tile-AREATILE;

            newobj->flags |= FL_AMBUSH;
        }

        newobj->obclass = which;
        newobj->hitpoints = atr::hitpoints[which][gamestate.difficulty];
        newobj->dir = (dirtype)(dir * 2);
        newobj->flags |= FL_SHOOTABLE;
        
        return newobj;
    }
    return NULL;
}

//
// _SpawnPatrol
//
// IOANCH 29.06.2012: made objtype *
//
static objtype *_SpawnPatrol (classtype which, int tilex, int tiley, int dir)
{
    if(atr::states[which].patrol)
    {
        SpawnNewObj(tilex, tiley, atr::states[which].patrol);
        if(atr::speeds[which].patrol >= 0)
            newobj->speed = atr::speeds[which].patrol;
        if (!loadedgame)
            gamestate.killtotal++;
    
        newobj->obclass = which;
        newobj->dir = (dirtype)(dir*2);
        newobj->hitpoints = atr::hitpoints[which][gamestate.difficulty];
        newobj->distance = TILEGLOBAL;
        newobj->flags |= FL_SHOOTABLE;
        newobj->active = ac_yes;

        actorat[newobj->tilex][newobj->tiley] = NULL;           // don't use original spot

        switch (dir)
        {
            case 0:
                newobj->tilex++;
                break;
            case 1:
                newobj->tiley--;
                break;
            case 2:
                newobj->tilex--;
                break;
            case 3:
                newobj->tiley++;
                break;
        }

        actorat[newobj->tilex][newobj->tiley] = newobj;

        
        return newobj;
    }
    return NULL;
}

//
// _SpawnBoss
//
// IOANCH 29.06.2012: made objtype *
//
static objtype *_SpawnBoss (classtype which, int tilex, int tiley)
{
	// IOANCH 29.06.2012: update this for ANY boss
	statetype *spawnstate = NULL;   // IOANCH 20130202: set value here to something whatever, to prevent undefined behaviour

    if(atr::states[which].stand)
    {
        SpawnNewObj (tilex,tiley,atr::states[which].stand);

        spawnstate = atr::states[which].stand;
        atr::actions[which].spawn();
    
        newobj->obclass = which;
        newobj->hitpoints = atr::hitpoints[which][gamestate.difficulty];
        newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
        if (!loadedgame)
            gamestate.killtotal++;
        
        return newobj;
    }
    return NULL;
}

// IOANCH 20130202: unification process
//
// _SpawnGhosts
//
static void _SpawnGhosts (int which, int tilex, int tiley)
{
    switch(which)
    {
        case en_blinky:
            SpawnNewObj (tilex,tiley,&s_blinkychase1);
            break;
        case en_clyde:
            SpawnNewObj (tilex,tiley,&s_clydechase1);
            break;
        case en_pinky:
            SpawnNewObj (tilex,tiley,&s_pinkychase1);
            break;
        case en_inky:
            SpawnNewObj (tilex,tiley,&s_inkychase1);
            break;
    }

    newobj->obclass = ghostobj;
    newobj->speed = SPDDOG;

    newobj->dir = east;
    newobj->flags |= FL_AMBUSH;
    if (!loadedgame)
    {
        gamestate.killtotal++;
        gamestate.killcount++;
    }
}

//
// Basic::SpawnEnemy
//
// Generically spawns a Nazi, which can be a stander, patroller or boss. Applies common changers to them.
//
void Basic::SpawnEnemy(classtype which, int tilex, int tiley, int dir, 
                       boolean patrol, enemy_t ghosttype)
{
    // IOANCH 20130304: don't account for loaded game
	if(nonazis && !loadedgame)
		return;	// don't spawn anything if --nonazis was defined
	
	objtype *newenemy = NULL;

	switch(which)
	{
	case guardobj:
	case officerobj:
	case ssobj:
	case mutantobj:
	case dogobj:
		if(patrol)
			newenemy = _SpawnPatrol(which, tilex, tiley, dir);
		else
			newenemy = _SpawnStand(which, tilex, tiley, dir);
		break;
	case bossobj:
	case schabbobj:
	case fakeobj:
	case mechahitlerobj:
	case gretelobj:
	case giftobj:
	case fatobj:
	case spectreobj:
	case angelobj:
	case transobj:
	case uberobj:
	case willobj:
	case deathobj:
		newenemy = _SpawnBoss(which, tilex, tiley);
		break;
            // IOANCH 20130202: unification process
	case ghostobj:
		_SpawnGhosts(ghosttype, tilex, tiley);
		break;
    default:
        return;
	}
	
	// IOANCH 20121219: record enemy position
	BotMan::RecordEnemyPosition(newobj);
	
	if(newenemy)
		livingNazis.add(newenemy);
}

//
// Basic::IsEnemy
//
boolean Basic::IsEnemy(classtype cls)
{
    return atr::flags[cls] & ATR_ENEMY;
}

//
// Basic::IsBoss
//
boolean Basic::IsBoss(classtype cls)
{
    return atr::flags[cls] & ATR_BOSS;
}

//
// Basic::IsDangerous
//
// Check if the Nazi is ready to fire or firing (important for fighting SS and bosses)
//
boolean Basic::IsDamaging(objtype *ret, int dist)
{
    if((dist <= 2 && atr::flags[ret->obclass] & ATR_NEARBY_THREAT && 
        ret->flags & FL_ATTACKMODE) || 
        (dist <= atr::threatrange[ret->obclass] && ret->state->flags & STF_DAMAGING))
    {
        return true;
    }
    
	return false;
}

//
// Basic::GenericCheckLine
//
// Like CheckLine, but with user-settable coordinates
//
boolean Basic::GenericCheckLine (int x1, int y1, int x2, int y2)
{
    int         xt1,yt1,xt2,yt2;
    int         x,y;
    int         xdist,ydist,xstep,ystep;
    int         partial,delta;
    int32_t     ltemp;
    int         xfrac,yfrac,deltafrac;
    unsigned    value,intercept;

    x1 >>= UNSIGNEDSHIFT;            // 1/256 tile precision
    y1 >>= UNSIGNEDSHIFT;
    xt1 = x1 >> 8;
    yt1 = y1 >> 8;

	 x2 >>= UNSIGNEDSHIFT;
	 y2 >>= UNSIGNEDSHIFT;
    xt2 = x2 >> 8;
    yt2 = y2 >> 8;

    xdist = abs(xt2-xt1);

    if (xdist > 0)
    {
        if (xt2 > xt1)
        {
            partial = 256-(x1&0xff);
            xstep = 1;
        }
        else
        {
            partial = x1&0xff;
            xstep = -1;
        }

        deltafrac = abs(x2-x1);
        delta = y2-y1;
        ltemp = ((int32_t)delta<<8)/deltafrac;
        if (ltemp > 0x7fffl)
            ystep = 0x7fff;
        else if (ltemp < -0x7fffl)
            ystep = -0x7fff;
        else
            ystep = ltemp;
        yfrac = y1 + (((int32_t)ystep*partial) >>8);

        x = xt1+xstep;
        xt2 += xstep;
        do
        {
            y = yfrac>>8;
            yfrac += ystep;

            value = (unsigned)tilemap[x][y];
            x += xstep;

            if (!value)
                continue;

            if (value<128 || value>256)
                return false;

            //
            // see if the door is open enough
            //
            value &= ~0x80;
            intercept = yfrac-ystep/2;

            if (intercept>doorposition[value])
                return false;

        } while (x != xt2);
    }

    ydist = abs(yt2-yt1);

    if (ydist > 0)
    {
        if (yt2 > yt1)
        {
            partial = 256-(y1&0xff);
            ystep = 1;
        }
        else
        {
            partial = y1&0xff;
            ystep = -1;
        }

        deltafrac = abs(y2-y1);
        delta = x2-x1;
        ltemp = ((int32_t)delta<<8)/deltafrac;
        if (ltemp > 0x7fffl)
            xstep = 0x7fff;
        else if (ltemp < -0x7fffl)
            xstep = -0x7fff;
        else
            xstep = ltemp;
        xfrac = x1 + (((int32_t)xstep*partial) >>8);

        y = yt1 + ystep;
        yt2 += ystep;
        do
        {
            x = xfrac>>8;
            xfrac += xstep;

            value = (unsigned)tilemap[x][y];
            y += ystep;

            if (!value)
                continue;

            if (value<128 || value>256)
                return false;

            //
            // see if the door is open enough
            //
            value &= ~0x80;
            intercept = xfrac-xstep/2;

            if (intercept>doorposition[value])
                return false;
        } while (y != yt2);
    }

    return true;
}



//
// Basic::MarkovWrite
//
// Write a random name
//
void Basic::MarkovWrite(char *c, int nmax)
{
	int oldindex = rndindex;	// don't let random table affect demos and such

	int i,j,r,s, n = 0;

	srand((unsigned)US_RndT());

	rndindex = oldindex;

	j = 26;
	
	do
	{
		r = rand()%_marktot[j];
		s = 0;
		for(i = 0; i < 27; i++)
		{
			s += _markov[j][i];
			if(s > r)
				break;
		}
		if(i < 26)
			c[n++] = (char)(i + 'a');
		j = i;
	}
	while((i < 26 && n < nmax) || n < 6);
	c[n] = '\0';
}

//
// Basic::ApproxDist
//
// Approx. distance (too lazy to use sqrt)
//
int Basic::ApproxDist(int dx, int dy)
{
	dx = abs(dx);
	dy = abs(dy);
	if(dx < dy)
		return dx + dy - (dx >> 1);
	return dx + dy - (dy >> 1);
}

//
// Basic::NewStringTildeExpand
//     
// Create new string with 
//
char *Basic::NewStringTildeExpand(const char *basedir)
{
    if(basedir[0] == '~')
    {
        const char *home = getenv("HOME");
        if(home)
        {
            size_t newlen = strlen(home) + strlen(basedir);
            char *newalloc = (char *)malloc(newlen * sizeof(char));

            strcat(newalloc, home);
            strcat(newalloc, basedir + 1);
                        
            return newalloc;
        }
    }
    
    // Make sure it is always new
    return strdup(basedir);
}

