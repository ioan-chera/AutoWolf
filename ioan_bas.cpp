//
// IOAN_BAS.CPP
//
// File by Ioan Chera, started 17.05.2012
//
// Basic general setup. New functions and such placed here, instead of randomly through the exe
//

#include "ioan_bas.h"

// static members definition
boolean Basic::nonazis, Basic::secretstep3;
List<void *> Basic::livingNazis, Basic::thrownProjectiles;
List<byte> Basic::itemList[MAPSIZE][MAPSIZE];

int Basic::markov[27][27] =
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

int Basic::marktot[27] =
	{950,214,536,476,1644,365,269,599,1131,9,56,424,350,919,1315,381,10,1090,880,1350,434,120,228,26,353,0,2950};

//
// Basic::IsDangerous
//
// Check if the Nazi is ready to fire or firing (important for fighting SS and bosses)
//
boolean Basic::IsDamaging(objtype *ret, int dist)
{
	switch(ret->obclass)
	{
	case guardobj:
		if(dist <= 12 && (ret->state == &s_grdshoot2 || ret->state == &s_grdshoot3))
			return true;
		break;
	case officerobj:
		if(dist <= 14 && (ret->state == &s_ofcshoot2 || ret->state == &s_ofcshoot3))
			return true;
		break;
	case ssobj:
		if(dist <= 16 && (ret->state == &s_ssshoot2 || ret->state == &s_ssshoot3 || ret->state == &s_ssshoot4
			|| ret->state == &s_ssshoot5 || ret->state == &s_ssshoot6 || ret->state == &s_ssshoot7
			|| ret->state == &s_ssshoot8 || ret->state == &s_ssshoot9))
			return true;
		break;
	case mutantobj:
		if((dist <= 2 && ret->flags & FL_ATTACKMODE)
			|| (dist <= 14 && (ret->state == &s_mutshoot1 || ret->state == &s_mutshoot2 || ret->state == &s_mutshoot3 || 
			ret->state == &s_mutshoot4)))
			return true;
		break;
#ifndef SPEAR
	case bossobj:
		if(dist <= 16 && (ret->state == &s_bossshoot1 || ret->state == &s_bossshoot2 || ret->state == &s_bossshoot3 || ret->state == &s_bossshoot4
			|| ret->state == &s_bossshoot5 || ret->state == &s_bossshoot6 || ret->state == &s_bossshoot7
			|| ret->state == &s_bossshoot8))
			return true;
		break;
	case ghostobj:
		if(dist <= 2 && ret->flags & FL_ATTACKMODE)
			return true;
		break;
	case mechahitlerobj:
		if(dist <= 16 && (ret->state == &s_mechashoot1 || ret->state == &s_mechashoot2 || ret->state == &s_mechashoot3 || ret->state == &s_mechashoot4
			|| ret->state == &s_mechashoot5 || ret->state == &s_mechashoot6))
			return true;
		break;
	case realhitlerobj:
		if(dist <= 16 && (ret->state == &s_hitlershoot1 || ret->state == &s_hitlershoot2 || ret->state == &s_hitlershoot3 || ret->state == &s_hitlershoot4
			|| ret->state == &s_hitlershoot5 || ret->state == &s_hitlershoot6))
			return true;
		break;
	case gretelobj:
		if(dist <= 16 && (ret->state == &s_gretelshoot1 || ret->state == &s_gretelshoot2 || ret->state == &s_gretelshoot3 
			|| ret->state == &s_gretelshoot4
			|| ret->state == &s_gretelshoot5 || ret->state == &s_gretelshoot6 || ret->state == &s_gretelshoot7
			|| ret->state == &s_gretelshoot8))
			return true;
		break;
	case fatobj:
		if(dist <= 16 && (ret->state == &s_fatshoot1 || ret->state == &s_fatshoot2 || ret->state == &s_fatshoot3 || ret->state == &s_fatshoot4
			|| ret->state == &s_fatshoot5 || ret->state == &s_fatshoot6))
			return true;
		break;
#else
	case transobj:
		if(dist <= 16 && (ret->state == &s_transshoot1 || ret->state == &s_transshoot2 || ret->state == &s_transshoot3 
			|| ret->state == &s_transshoot4
			|| ret->state == &s_transshoot5 || ret->state == &s_transshoot6 || ret->state == &s_transshoot7
			|| ret->state == &s_transshoot8))
			return true;
		break;
	case willobj:
		if(dist <= 16 && (ret->state == &s_willshoot1 || ret->state == &s_willshoot2 || ret->state == &s_willshoot3 || ret->state == &s_willshoot4
			|| ret->state == &s_willshoot5 || ret->state == &s_willshoot6))
			return true;
		break;
	case uberobj:
		if(dist <= 16 && (ret->state == &s_ubershoot1 || ret->state == &s_ubershoot2 || ret->state == &s_ubershoot3 || ret->state == &s_ubershoot4
			|| ret->state == &s_ubershoot5 || ret->state == &s_ubershoot6 || ret->state == &s_ubershoot7))
			return true;
		break;
	case deathobj:
		if(dist <= 16 && (ret->state == &s_deathshoot1 || ret->state == &s_deathshoot2 || ret->state == &s_deathshoot3 || ret->state == &s_deathshoot4
			|| ret->state == &s_deathshoot5))
			return true;
		break;
#endif
		default:
			;
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
// Basic::SpawnStand
//
// Made protected.
//
// IOAN 29.06.2012: made objtype *
objtype *Basic::SpawnStand (enemy_t which, int tilex, int tiley, int dir)
{
    word *map;
    word tile;

    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_ssstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
		default:
			;
    }


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

    newobj->obclass = (classtype)(guardobj + which);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->dir = (dirtype)(dir * 2);
    newobj->flags |= FL_SHOOTABLE;
	
	return newobj;
}


//
// Basic::SpawnPatrol
//
// IOAN 29.06.2012: made objtype *
objtype *Basic::SpawnPatrol (enemy_t which, int tilex, int tiley, int dir)
{
    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_sspath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_dog:
            SpawnNewObj (tilex,tiley,&s_dogpath1);
            newobj->speed = SPDDOG;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
		default:
			;
    }

    newobj->obclass = (classtype)(guardobj+which);
    newobj->dir = (dirtype)(dir*2);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
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

//
// Basic::SpawnBoss
//
// IOAN 29.06.2012: made objtype *
objtype *Basic::SpawnBoss (enemy_t which, int tilex, int tiley)
{
	// IOAN 29.06.2012: update this for ANY boss
	statetype *spawnstate;
	classtype obclass;
	boolean setspeed = false, setdir = false, setbonus = false;
	switch(which)
	{
#ifndef SPEAR
		case en_boss:
			spawnstate = &s_bossstand;
			obclass = bossobj;
			setspeed = setdir = true;
			break;
		case en_gretel:
			spawnstate = &s_gretelstand;
			obclass = gretelobj;
			setspeed = setdir = true;
			break;
		case en_schabbs:
			spawnstate = &s_schabbstand;
			obclass = schabbobj;
			setspeed = setdir = true;
			// TODO: this should be elsewhere methinks
			if (DigiMode != sds_Off)
				s_schabbdie2.tictime = 140;
			else
				s_schabbdie2.tictime = 5;
			break;
		case en_gift:
			spawnstate = &s_giftstand;
			obclass = giftobj;
			setspeed = setdir = true;
			if (DigiMode != sds_Off)
				s_giftdie2.tictime = 140;
			else
				s_giftdie2.tictime = 5;
			break;
		case en_fat:
			spawnstate = &s_fatstand;
			obclass = fatobj;
			setspeed = setdir = true;
			if (DigiMode != sds_Off)
				s_fatdie2.tictime = 140;
			else
				s_fatdie2.tictime = 5;
			break;
		case en_fake:
			spawnstate = &s_fakestand;
			obclass = fakeobj;
			setspeed = setdir = true;
			break;
		case en_hitler:
			spawnstate = &s_mechastand;
			obclass = mechahitlerobj;
			setspeed = setdir = true;
			if (DigiMode != sds_Off)
				s_hitlerdie2.tictime = 140;
			else
				s_hitlerdie2.tictime = 5;
			break;
#else
		case en_trans:
			spawnstate = &s_transstand;
			obclass = transobj;
			if (SoundBlasterPresent && DigiMode != sds_Off)
				s_transdie01.tictime = 105;
			break;
		case en_uber:
			spawnstate = &s_uberstand;
			obclass = uberobj;
			if (SoundBlasterPresent && DigiMode != sds_Off)
				s_uberdie01.tictime = 70;
			break;
		case en_will:
			spawnstate = &s_willstand;
			obclass = willobj;
			if (SoundBlasterPresent && DigiMode != sds_Off)
				s_willdie2.tictime = 70;
			break;
		case en_death:
			spawnstate = &s_deathstand;
			obclass = deathobj;
			if (SoundBlasterPresent && DigiMode != sds_Off)
				s_deathdie2.tictime = 105;
			break;
		case en_angel:
			spawnstate = &s_angelstand;
			obclass = angelobj;
			if (SoundBlasterPresent && DigiMode != sds_Off)
				s_angeldie11.tictime = 105;
			break;
		case en_spectre:
			spawnstate = &s_spectrewait1;
			obclass = spectreobj;
			setbonus = true;
			break;
			
#endif
		default:
			;
	}
	
    SpawnNewObj (tilex,tiley,spawnstate);
	if(setspeed)
		newobj->speed = SPDPATROL;

    newobj->obclass = obclass;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
	if(setdir)
		newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
	if(setbonus)
		newobj->flags |= FL_BONUS;
    if (!loadedgame)
        gamestate.killtotal++;
	
	return newobj;
}

#ifndef SPEAR
//
// Basic::SpawnGhosts
//
void Basic::SpawnGhosts (int which, int tilex, int tiley)
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

#endif

//
// Basic::SpawnEnemy
//
// Generically spawns a Nazi, which can be a stander, patroller or boss. Applies common changers to them.
//
void Basic::SpawnEnemy(enemy_t which, int tilex, int tiley, int dir, boolean patrol)
{
	if(nonazis)
		return;	// don't spawn anything if --nonazis was defined
	
	objtype *newenemy = NULL;

	switch(which)
	{
	case en_guard:
	case en_officer:
	case en_ss:
	case en_mutant:
	case en_dog:
		if(patrol)
			newenemy = SpawnPatrol(which, tilex, tiley, dir);
		else
			newenemy = SpawnStand(which, tilex, tiley, dir);
		break;
	case en_boss:
	case en_schabbs:
	case en_fake:
	case en_hitler:
	case en_gretel:
	case en_gift:
	case en_fat:
	case en_spectre:
	case en_angel:
	case en_trans:
	case en_uber:
	case en_will:
	case en_death:
		newenemy = SpawnBoss(which, tilex, tiley);
		break;
#ifndef SPEAR
	case en_blinky:
	case en_clyde:
	case en_pinky:
	case en_inky:
		SpawnGhosts(which, tilex, tiley);
		break;
#endif
	}
	if(newenemy)
		livingNazis.add(newenemy);
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
		r = rand()%marktot[j];
		s = 0;
		for(i = 0; i < 27; i++)
		{
			s += markov[j][i];
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
// Basic::EmptyItemList
//
// empty itemList
//
void Basic::EmptyItemList()
{
	int i, j;
	for(i = 0; i < MAPSIZE; ++i)
		for(j = 0; j < MAPSIZE; ++j)
			itemList[i][j].removeAll();
}