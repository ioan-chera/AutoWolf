//
// IOAN_BOT.CPP
//
// File by Ioan Chera, started 17.05.2012
//
// Basic bot setup. I'm working with classes here!
//

#include "WL_DEF.H"
#pragma hdrstop

#define SET_SIZE 2048

// protected ones
static boolean pathexists, exitfound;
static byte nothingleft, wakeupfire;
static char exitx, exity, exfrontx;
//static SData *searchset;
static SData far searchset[SET_SIZE];
static int searchsize, searchlen;

extern objtype *lastobj;

static objtype *IsEnemyAround(char tx, char ty, char dist);
static objtype *IsProjectile(char tx, char ty, char dist, short *angle,
                             char *distance);
static void EmptySet()
{
	 searchsize = searchlen = 0;
//	 if(searchset)
//	 {
//		  MM_FreePtr(&(memptr)searchset);
//		  searchset = NULL;
//	 }
}

void Bot_MapInit(void)
{
	 system("pause");
	 exitfound = pathexists = false;
	 nothingleft = wakeupfire = 0;
	 EmptySet();
}


//
// BotMan::ApproxDist
//
// Approx. distance (too lazy to use sqrt)
//
static long ApproxDist(long dx, long dy)
{
	dx = ABS(dx);
	dy = ABS(dy);
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
static boolean AddToSet(const SData *data)
{
	++searchlen;
	if(searchlen >= SET_SIZE)
	{
		--searchlen;
		return false;
	}
	searchset[searchlen - 1] = *data;
	return true;
}


//
// BotMan::ObjectOfInterest
//
// Pickable item, secret door, exit switch, exit pad
//
static boolean ObjectOfInterest(char tx, char ty)
{
	short i;
	objtype *check;
	exitx = -1;
	exity = -1;

	// items
	for(i = 0; &statobjlist[i] != laststatobj; ++i)
	{
		if(statobjlist[i].tilex == (byte)tx && statobjlist[i].tiley == (byte)ty
           && statobjlist[i].flags & FL_BONUS
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
						  if (gamestate.ammo <= 74)
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

	check = actorat[tx][ty];
	if(gamestate.health > 75 && gamestate.ammo > 75 &&
       check >= objlist && Basic_IsEnemy(check->obclass)
       && check->flags & FL_SHOOTABLE)
	{
		nothingleft = 0;	// reset counter if enemies here
		return true;
	}

	// secret door
	if(ty + 3 < MAPSIZE && *(mapsegs[1]+farmapylookup[ty + 1]+tx) == PUSHABLETILE)
	{
		if(!actorat[tx][ty+2] && !actorat[tx][ty+3])
		{
			exity = ty + 1;
			exitx = tx;
			return true;
		}
	}
	if(ty - 3 >= 0 && *(mapsegs[1]+farmapylookup[ty - 1]+tx) == PUSHABLETILE)
	{
		if(!actorat[tx][ty-2] && !actorat[tx][ty-3])
		{
			exity = ty - 1;
			exitx = tx;
			return true;
		}
	}
	if(tx + 3 < MAPSIZE && *(mapsegs[1]+farmapylookup[ty]+tx + 1) == PUSHABLETILE)
	{
		if(!actorat[tx+2][ty] && !actorat[tx+3][ty])
		{
			exity = ty;
			exitx = tx + 1;
			return true;
		}
	}
	if(tx - 3 >= 0 && *(mapsegs[1]+farmapylookup[ty]+tx - 1) == PUSHABLETILE)
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
			if(*(mapsegs[0]+farmapylookup[ty]+tx) == ALTELEVATORTILE
               || nothingleft >= 2)
            {
				return true;
            }
		}
		if(tx + 1 < MAPSIZE && tilemap[tx + 1][ty] == ELEVATORTILE)
		{
			exitx = tx + 1;
			exity = ty;
			if(*(mapsegs[0]+farmapylookup[ty]+tx) == ALTELEVATORTILE
               || nothingleft >= 2)
            {
				return true;
            }
		}

		// exit pad
		if(nothingleft >= 2 && *(mapsegs[1]+farmapylookup[ty]+tx) == EXITTILE)
			return true;
	}
	return false;
}

//
// FindRandomPath
//
// Finds a path to any object
//
short far pathused[MAPSIZE][MAPSIZE];
static boolean FindRandomPath(boolean ignoreproj, boolean mindnazis)
{
    short i, k;
    char j;
	boolean openfound;
	short fmin;
    short imin, ifound;
	char cx, cy;
	objtype *check;
	boolean neighinvalid, neighfound;
	short tentative_g_score;
    short numopen = 1;

	SData data;

	data.g_score = 0;
	data.f_score = 0;
	data.open = true;
	data.tilex = player->tilex;
	data.tiley = player->tiley;
	data.prev = -1;
    _fmemset(pathused, 0, sizeof(pathused));
    pathused[data.tilex][data.tiley] = 1;

	EmptySet();

	if(!AddToSet(&data))
        goto bail;

	for(;;)
	{
		// look for open sets
		openfound = false;
		fmin = 0x7fff;
		for(k = numopen, i = searchlen - 1; k > 0 && i >= 0; --i)
		{
			if(searchset[i].open)
			{
                --k;
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
        --numopen;

		if(ObjectOfInterest(searchset[imin].tilex, searchset[imin].tiley))
		{
			// found goal
			searchset[imin].next = -1;
			ifound = imin;
			for(imin = searchset[imin].prev; imin != -1; ifound = imin,
                imin = searchset[imin].prev)
            {
				searchset[imin].next = ifound;
            }
			pathexists = true;
			return true;
		}

		for(j = 0; j < 4; ++j)
		{
			byte door;
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
			door = tilemap[cx][cy];
			if(check && check < objlist && !(door & 0x80)
               || (ABS(cx - player->tilex) > 1 || ABS(cy - player->tiley) > 1)
               && IsProjectile(cx, cy, 1, NULL, NULL) != NULL && !ignoreproj ||
               IsEnemyAround(cx, cy, 1) != NULL && mindnazis)
            {
				continue;	// solid, can't be passed
            }
			else if (door & 0x80)
			{
				byte lock;
				door = door & ~0x80;
				lock = doorobjlist[door].lock;
				if (lock >= dr_lock1 && lock <= dr_lock4)
                {
					if (doorobjlist[door].action != dr_open &&
						doorobjlist[door].action != dr_opening &&
                        !(gamestate.keys & (1 << (lock-dr_lock1) ) ) )
                    {
						continue;
                    }
                }
			}

			neighinvalid = false;
			neighfound = false;
//            for(i = searchlen - 1; i >= 0; --i)
//			{
//				if(searchset[i].tilex == cx && searchset[i].tiley == cy)
//				{
//					if(!searchset[i].open)
//						neighinvalid = true;
//					else
//					{
//						neighfound = true;
//						ifound = i;
//					}
//					break;
//				}
//			}
            i = pathused[cx][cy];
            if(i >= 1)
            {
                --i;
                if(!searchset[i].open)
                    neighinvalid = true;
                else
                {
                    neighfound = true;
                    ifound = i;
                }
            }
			if(neighinvalid)
				continue;
			tentative_g_score = searchset[imin].g_score + 1;
			if(!neighfound)
			{
                ++numopen;
				data.g_score = tentative_g_score;
				data.h_score = 0;
				data.f_score = data.g_score;
				data.open = true;
				data.tilex = cx;
				data.tiley = cy;
				data.prev = imin;
				if(!AddToSet(&data))
                    goto bail;
                pathused[cx][cy] = searchlen;
			}
			else if(tentative_g_score < searchset[ifound].g_score)
			{
				searchset[ifound].g_score = tentative_g_score;
				searchset[ifound].f_score = tentative_g_score;
				searchset[ifound].prev = imin;
			}
		}
	}
bail:
	if(!pwallstate)
		nothingleft++;

	return false;
}

//
// BotMan::EnemyOnTarget
//
// True if can shoot
//
static objtype *EnemyOnTarget(void)
{
	objtype *check,*closest,*oldclosest;
	long  viewdist;
    
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
	//return NULL;
}

//
// BotMan::EnemyVisible
//
// True if an enemy is in sight
//
static objtype *EnemyVisible(short *angle, char *distance)
{
	char tx = player->tilex, ty = player->tiley;
	char i, j, k, distmin;
	objtype *ret, *retmin = NULL;
	short angmin;
	static objtype *oldret;
	float dby, dbx;
    
	distmin = 0x7f;
    
	for(ret = lastobj; ret; ret = ret->prev)
	{
		k = abs(ret->tilex - tx);
		j = abs(ret->tiley - ty);
		i = k > j ? k : j;
        
		if(k > 15 || j > 15)
			continue;
		if(!Basic_IsEnemy(ret->obclass) || !(ret->flags & FL_SHOOTABLE)
           || !CheckLine(ret))
        {
			continue;
        }
        
		if(abs(*distance - i) >= 2 && ret != oldret || ret == oldret
           || oldret && !(oldret->flags & FL_SHOOTABLE))
		{
			if(i <= distmin)
			{
				dby = -((float)(ret->y) - (float)(player->y));
				dbx = (float)(ret->x) - (float)(player->x);
				distmin = i;
				angmin = (short)(180.0f/PI*atan2(dby, dbx));
                
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
// BotMan::EnemyEager
//
// True if 1 non-ambush enemies are afoot
//
static objtype *EnemyEager(void)
{
	objtype *ret;
    
	for(ret = lastobj; ret; ret = ret->prev)
	{
		if(Basic_IsEnemy(ret->obclass) && ret->flags & FL_SHOOTABLE && areabyplayer[ret->areanumber] &&
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
static objtype *DamageThreat(objtype *targ)
{
	char tx = player->tilex, ty = player->tiley;
	char j, k, dist;
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
		if(dist > 16 || !Basic_IsEnemy(ret->obclass)
           || !(ret->flags & FL_SHOOTABLE) || !CheckLine(ret)
           || ret == targ && !Basic_IsBoss(ret->obclass))
			continue;
        
		if(Basic_IsDamaging(ret, dist))
			return ret;
		
	}
	return NULL;
}

//
// BotMan::Crossfire
//
// Returns true if there's a crossfire in that spot
//
static objtype *Crossfire(int tx, int ty)
{
	int j, k, dist;
	objtype *ret;
    
	for(ret = lastobj; ret; ret = ret->prev)
	{
		k = ret->tilex - tx;
		j = ret->tiley - ty;
		dist = abs(j) > abs(k) ? abs(j) : abs(k);
		if(dist > 16 || !Basic_IsEnemy(ret->obclass) || !(ret->flags & FL_SHOOTABLE)
           || !Basic_GenericCheckLine(ret->x, ret->y,
                                       Basic_Major(tx),
													Basic_Major(ty)))
			continue;
        
		if(Basic_IsDamaging(ret, dist))
			return ret;
		
	}
	return NULL;
}

//
// BotMan::DoRetreat
//
// Retreats the bot sliding off walls
//
static void DoRetreat(boolean forth, objtype *cause)
{
	int neg = forth? -1 : 1;
	int j, backx, backy, sidex, sidey, tx = player->tilex, ty = player->tiley, dir;
	objtype *check1, *check2;

	controly = neg * RUNMOVE * tics;
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

	check1 = actorat[tx + backx][ty + backy];
	if(check1 && check1 < objlist || check1 >= objlist && check1->flags & FL_SHOOTABLE)
	{
		// Look for way backwards
		if(sidex)	// strafing east or west
		{
			for(j = tx + sidex; ; j += sidex)
			{
				if(j >= MAPSIZE)
					break;
				check2 = actorat[j][ty];
				if(check2 && check2 < objlist || check2 >= objlist && check2->flags & FL_SHOOTABLE)
					break;	// stopped
                
				check2 = actorat[j][ty + backy];
				if(!check2 || check2 >= objlist && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
					goto solved;// found here
			}
			for(j = tx - sidex; ; j -= sidex)
			{
				if(j < 0)
					break;
				check2 = actorat[j][ty];
				if(check2 && check2 < objlist || check2 >= objlist && check2->flags & FL_SHOOTABLE)
					break;
                
				check2 = actorat[j][ty + backy];
				if(!check2 || check2 >= objlist && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
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
				if(check2 && check2 < objlist || check2 >= objlist && check2->flags & FL_SHOOTABLE)
					break;	// stopped
                
				check2 = actorat[tx + backx][j];
				if(!check2 || check2 >= objlist && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
					goto solved;// found here
			}
			for(j = ty - sidey; ; j -= sidey)
			{
				if(j < 0)
					break;
				check2 = actorat[tx][j];
				if(check2 && check2 < objlist || check2 >= objlist && check2->flags & FL_SHOOTABLE)
					break;
                
				check2 = actorat[tx + backx][j];
				if(!check2 || check2 >= objlist && (check2->obclass == inertobj || !(check2->flags & FL_SHOOTABLE)))
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
		if(check2 && check2 < objlist || check2 >= objlist && check2->flags & FL_SHOOTABLE)
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
objtype *IsProjectile(char tx, char ty, char dist, short *angle, char *distance)
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
						  if(ret->state != &s_boom1 && ret->state != &s_boom2
                             && ret->state != &s_boom3)
                        goto retok;
                    break;
#endif
#ifdef SPEAR
                case hrocketobj:
                    if(ret->state != &s_hboom1 && ret->state != &s_hboom2
                       && ret->state != &s_hboom3)
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
		
		float dby = -((float)(ret->y) - (float)(player->y));
		float dbx = (float)(ret->x) - (float)(player->x);
        
		*angle = (short)(180.0f/PI*atan2(dby, dbx));
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
static objtype *IsEnemyAround(char tx, char ty, char dist)
{
	objtype *ret = lastobj;

	while(ret)
	{
		if(abs(ret->tilex - tx) <= dist && abs(ret->tiley - ty) <= dist)
			if(Basic_IsEnemy(ret->obclass) && ret->flags & FL_SHOOTABLE
               && ret->flags & FL_ATTACKMODE)
				return ret;
		ret = ret->prev;
	}
	return NULL;
}

//
// DoCombatAI
//
// Branch of DoCommand that does NAZI fighting
//
static byte retreat, pressuse;
static boolean DoCombatAI(short eangle, char edist)
{
    short dangle, pangle, epangle;
    objtype *check, *check2;
    char proj = 0;
    
    nothingleft = 0;	// reset elevator counter if distracted
    
    if(gamestate.weapon == wp_knife)
    {
        if(FindRandomPath(false, true))
        {
            pathexists = false;
            return false;
        }
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
    if(!check2)
    {
        check2 = IsProjectile(player->tilex, player->tiley, 2, &pangle, &proj);
        proj = 1;
    }
    
    if(check2 && (check2 != check || Basic_IsBoss(check->obclass))
       && gamestate.weapon != wp_knife)
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
        if((gamestate.weapon <= wp_pistol && pressuse % 4 == 0
            || gamestate.weapon > wp_pistol) && edist <= 10)
        {
            buttonstate[bt_attack] = true;
        }
        else
            buttonstate[bt_attack] = false;
        
        // TODO: don't always charge when using the knife!
        
        if(!retreat && (!check2 || check2 == check) && edist > 6
           || dangle > -45 && dangle < 45 && gamestate.weapon == wp_knife)
        {
            // otherwise
            // TODO: use DoCharge here, to maintain slaloming
            if(gamestate.weapon == wp_knife)
                controly = -RUNMOVE * tics;
            else
                DoRetreat(true, NULL);
        }
    }
    return true;
}

//
// Bot_DoCommand
//
// Starting point for bot execution
//
void Bot_DoCommand(void)
{
	static short tangle, dangle;

	static short eangle = -1;
	static char edist = -2;
    short pangle, epangle;
    static boolean waitpwall;
	// found path to exit
    short searchpos = 0;
	short nowon = -1;
	char nx, ny, mx, my;
	boolean tryuse = false;	// whether to try using
	byte tile;

	objtype *check0, *check, *check2;
	++pressuse;	// key press timer

	check0 = EnemyVisible(&eangle, &edist);

	if(check0)
	{
		if(!DoCombatAI(eangle, edist))
            goto disengage;
	}
	else if(retreat)	// standard retreat, still moving
	{
		// retreat mode (to be removed?)
		edist = -2;
		retreat--;
		DoRetreat(false, NULL);
	}
	else
	{
	 disengage:
		  // no path got defined
		;

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
		if(pwallstate)
			waitpwall = true;


		// look if it's there
		for(; searchpos != -1; searchpos = searchset[searchpos].next)
		{
			if(player->tilex == searchset[searchpos].tilex
               && player->tiley == searchset[searchpos].tiley)
			{
				nowon = searchpos;
					 //				break;
			}
			if(nowon > -1 && IsProjectile(searchset[searchpos].tilex,
                    searchset[searchpos].tiley,1,NULL,NULL) != NULL
					&& (abs(searchset[searchpos].tilex - player->tilex) > 1 ||
						 abs(searchset[searchpos].tiley - player->tiley) > 1))
			{
				nothingleft = 0;
				pathexists = false;
				if(!FindRandomPath(false,false))
					FindRandomPath(true,false);
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


		mx = player->tilex;
		my = player->tiley;

		// elevator
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
			if(tryuse && (actorat[nx][ny] && actorat[nx][ny] < objlist) && pressuse % 4 == 0)
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
						fixed centx, centy, cento, plro;

			// straight line: can strafe now
			buttonstate[bt_strafe] = true;
			centx = ((fixed)player->tilex << TILESHIFT);
			centy = ((fixed)player->tiley << TILESHIFT);

			switch(tangle)
			{
					 case 0:
						  cento = -centy;
						  plro = -player->y + (1l<<(TILESHIFT - 1));
						  break;
					 case 90:
						  cento = -centx;
						  plro = -player->x + (1l<<(TILESHIFT - 1));
						  break;
					 case 180:
						  cento = centy;
						  plro = player->y - (1l<<(TILESHIFT - 1));
						  break;
					 case 270:
						  cento = centx;
						  plro = player->x - (1l<<(TILESHIFT - 1));
						  break;
			}
			if(plro - cento > 4096)
				controlx = BASEMOVE * tics;
			else if(plro - cento < -4096)
				controlx = -BASEMOVE * tics;
		}
	}
}
