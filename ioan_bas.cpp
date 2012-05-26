//
// IOAN_BAS.CPP
//
// File by Ioan Chera, started 17.05.2012
//
// Basic general setup. New functions and such placed here, instead of randomly through the exe
//

#include "ioan_bas.h"

// static members definition
boolean Basic::nonazis;

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
		if(dist <= 6 && (ret->state == &s_grdshoot2 || ret->state == &s_grdshoot3))
			return true;
		break;
	case officerobj:
		if(dist <= 8 && (ret->state == &s_ofcshoot2 || ret->state == &s_ofcshoot3))
			return true;
		break;
	case ssobj:
		if(dist <= 11 && (ret->state == &s_ssshoot2 || ret->state == &s_ssshoot3 || ret->state == &s_ssshoot4
			|| ret->state == &s_ssshoot5 || ret->state == &s_ssshoot6 || ret->state == &s_ssshoot7
			|| ret->state == &s_ssshoot8 || ret->state == &s_ssshoot9))
			return true;
		break;
	case mutantobj:
		if(dist <= 2 && ret->flags & FL_ATTACKMODE)
			return true;
		break;
#ifndef SPEAR
	case bossobj:
		if(dist <= 12 && (ret->state == &s_bossshoot1 || ret->state == &s_bossshoot2 || ret->state == &s_bossshoot3 || ret->state == &s_bossshoot4
			|| ret->state == &s_bossshoot5 || ret->state == &s_bossshoot6 || ret->state == &s_bossshoot7
			|| ret->state == &s_bossshoot8))
			return true;
		break;
	case ghostobj:
		if(dist <= 2)
			return true;
		break;
	case mechahitlerobj:
		if(dist <= 10 && (ret->state == &s_mechashoot1 || ret->state == &s_mechashoot2 || ret->state == &s_mechashoot3 || ret->state == &s_mechashoot4
			|| ret->state == &s_mechashoot5 || ret->state == &s_mechashoot6))
			return true;
		break;
	case realhitlerobj:
		if(dist <= 10 && (ret->state == &s_hitlershoot1 || ret->state == &s_hitlershoot2 || ret->state == &s_hitlershoot3 || ret->state == &s_hitlershoot4
			|| ret->state == &s_hitlershoot5 || ret->state == &s_hitlershoot6))
			return true;
		break;
	case gretelobj:
		if(dist <= 11 && (ret->state == &s_gretelshoot1 || ret->state == &s_gretelshoot2 || ret->state == &s_gretelshoot3 
			|| ret->state == &s_gretelshoot4
			|| ret->state == &s_gretelshoot5 || ret->state == &s_gretelshoot6 || ret->state == &s_gretelshoot7
			|| ret->state == &s_gretelshoot8))
			return true;
		break;
	case fatobj:
		if(dist <= 10 && (ret->state == &s_fatshoot1 || ret->state == &s_fatshoot2 || ret->state == &s_fatshoot3 || ret->state == &s_fatshoot4
			|| ret->state == &s_fatshoot5 || ret->state == &s_fatshoot6))
			return true;
		break;
#else
	case transobj:
		if(dist <= 11 && (ret->state == &s_transshoot1 || ret->state == &s_transshoot2 || ret->state == &s_transshoot3 
			|| ret->state == &s_transshoot4
			|| ret->state == &s_transshoot5 || ret->state == &s_transshoot6 || ret->state == &s_transshoot7
			|| ret->state == &s_transshoot8))
			return true;
		break;
	case willobj:
		if(dist <= 10 && (ret->state == &s_willshoot1 || ret->state == &s_willshoot2 || ret->state == &s_willshoot3 || ret->state == &s_willshoot4
			|| ret->state == &s_willshoot5 || ret->state == &s_willshoot6))
			return true;
		break;
	case uberobj:
		if(dist <= 10 && (ret->state == &s_ubershoot1 || ret->state == &s_ubershoot2 || ret->state == &s_ubershoot3 || ret->state == &s_ubershoot4
			|| ret->state == &s_ubershoot5 || ret->state == &s_ubershoot6 || ret->state == &s_ubershoot7))
			return true;
		break;
	case deathobj:
		if(dist <= 10 && (ret->state == &s_deathshoot1 || ret->state == &s_deathshoot2 || ret->state == &s_deathshoot3 || ret->state == &s_deathshoot4
			|| ret->state == &s_deathshoot5))
			return true;
		break;
#endif
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
void Basic::SpawnStand (enemy_t which, int tilex, int tiley, int dir)
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
}


//
// Basic::SpawnPatrol
//
void Basic::SpawnPatrol (enemy_t which, int tilex, int tiley, int dir)
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
}

#ifndef SPEAR
//
// Basic::SpawnBoss
//
void Basic::SpawnBoss (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_bossstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = bossobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_boss];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnGretel
//
void Basic::SpawnGretel (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_gretelstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = gretelobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_gretel];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnSchabbs
//
void Basic::SpawnSchabbs (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_schabbdie2.tictime = 140;
    else
        s_schabbdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_schabbstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = schabbobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_schabbs];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

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

//
// Basic::SpawnGift
//
void Basic::SpawnGift (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_giftdie2.tictime = 140;
    else
        s_giftdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_giftstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = giftobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_gift];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnFat
//
void Basic::SpawnFat (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_fatdie2.tictime = 140;
    else
        s_fatdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_fatstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = fatobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_fat];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnFakeHitler
//
void Basic::SpawnFakeHitler (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_hitlerdie2.tictime = 140;
    else
        s_hitlerdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_fakestand);
    newobj->speed = SPDPATROL;

    newobj->obclass = fakeobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_fake];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnHitler
//
void Basic::SpawnHitler (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_hitlerdie2.tictime = 140;
    else
        s_hitlerdie2.tictime = 5;


    SpawnNewObj (tilex,tiley,&s_mechastand);
    newobj->speed = SPDPATROL;

    newobj->obclass = mechahitlerobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_hitler];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

#else
//
// Basic::SpawnTrans
//
void Basic::SpawnTrans (int tilex, int tiley)
{
    //        word *map;
    //        word tile;

    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_transdie01.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_transstand);
    newobj->obclass = transobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_trans];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnUber
//
void Basic::SpawnUber (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_uberdie01.tictime = 70;

    SpawnNewObj (tilex,tiley,&s_uberstand);
    newobj->obclass = uberobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_uber];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnWill
//
void Basic::SpawnWill (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_willdie2.tictime = 70;

    SpawnNewObj (tilex,tiley,&s_willstand);
    newobj->obclass = willobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_will];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnDeath
//
void Basic::SpawnDeath (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_deathdie2.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_deathstand);
    newobj->obclass = deathobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_death];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnAngel
//
void Basic::SpawnAngel (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_angeldie11.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_angelstand);
    newobj->obclass = angelobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_angel];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

//
// Basic::SpawnSpectre
//
void Basic::SpawnSpectre (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_spectrewait1);
    newobj->obclass = spectreobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BONUS; // |FL_NEVERMARK|FL_NONMARK;
    if (!loadedgame)
        gamestate.killtotal++;
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

	switch(which)
	{
	case en_guard:
	case en_officer:
	case en_ss:
	case en_mutant:
	case en_dog:
		if(patrol)
			SpawnPatrol(which, tilex, tiley, dir);
		else
			SpawnStand(which, tilex, tiley, dir);
		break;
#ifndef SPEAR
	case en_boss:
		SpawnBoss(tilex, tiley);
		break;
	case en_schabbs:
		SpawnSchabbs(tilex, tiley);
		break;
	case en_fake:
		SpawnFakeHitler(tilex, tiley);
		break;
	case en_hitler:
		SpawnHitler(tilex, tiley);
		break;
	case en_blinky:
	case en_clyde:
	case en_pinky:
	case en_inky:
		SpawnGhosts(which, tilex, tiley);
		break;
	case en_gretel:
		SpawnGretel(tilex, tiley);
		break;
	case en_gift:
		SpawnGift(tilex, tiley);
		break;
	case en_fat:
		SpawnFat(tilex, tiley);
		break;
#else
	case en_spectre:
		SpawnSpectre(tilex, tiley);
		break;
	case en_angel:
		SpawnAngel(tilex, tiley);
		break;
	case en_trans:
		SpawnTrans(tilex, tiley);
		break;
	case en_uber:
		SpawnUber(tilex, tiley);
		break;
	case en_will:
		SpawnWill(tilex, tiley);
		break;
	case en_death:
		SpawnDeath(tilex, tiley);
		break;
#endif
	}
}