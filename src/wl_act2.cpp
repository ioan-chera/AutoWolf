//
// Copyright (C) 1991-1992  id Software
// Copyright (C) 2007-2011  Moritz Kroll
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
// Routines dealing mostly with sentient actors
//
////////////////////////////////////////////////////////////////////////////////

// WL_ACT2.C

#include "wl_def.h"
#include "foreign.h"
#include "version.h"
#include "wl_act1.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_play.h"
#include "wl_state.h"
#include "ActorStates.h"
#include "Config.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_vh.h"
#include "ioan_bas.h"
#include "ioan_bot.h"	// include bot
#include "List.h"
#include "obattrib.h"   // IOANCH 20130306

//
// IOANCH 20130311: corrected T_ names to A_ names when used as one-shot
// actions
//

////////////////////////////////////////////////////////////////////////////////
//
// LOCAL CONSTANTS
//
////////////////////////////////////////////////////////////////////////////////

#define PROJECTILESIZE  0xc000l

#define BJRUNSPEED      2048
#define BJJUMPSPEED     680

//
// A_Smoke
//

void A_Smoke (objtype *ob)
{
    GetNewActor ();
    // IOANCH 20130202: unification process
    if (ob->obclass == hrocketobj)
        newobj->state = &s_hsmoke1;
    else
        newobj->state = &s_smoke1;
    newobj->ticcount = 6;
    
    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = inertobj;
    newobj->active = ac_yes;
    
    newobj->flags = FL_NEVERMARK;
}

//
// ProjectileTryMove
//
// returns true if move ok
//

#define PROJSIZE        0x2000

static Boolean8 ProjectileTryMove (objtype *ob)
{
    int      xl,yl,xh,yh,x,y;
    objtype *check;
    
    xl = (ob->x-PROJSIZE) >> TILESHIFT;
    yl = (ob->y-PROJSIZE) >> TILESHIFT;
    
    xh = (ob->x+PROJSIZE) >> TILESHIFT;
    yh = (ob->y+PROJSIZE) >> TILESHIFT;
    
    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
                return false;
        }
    
    return true;
}

//
// T_Projectile
//
void T_Projectile (objtype *ob)
{
    int32_t deltax,deltay;
    int     damage = 0; // IOANCH 20130202: set damage here to something
                        // whatever, to prevent undefined behaviour
    int32_t speed;
    
    speed = (int32_t)ob->speed*tics;
    
    deltax = FixedMul(speed,costable[ob->angle]);
    deltay = -FixedMul(speed,sintable[ob->angle]);
    
    if (deltax>0x10000l)
        deltax = 0x10000l;
    if (deltay>0x10000l)
        deltay = 0x10000l;
    
    ob->x += deltax;
    ob->y += deltay;
    
    deltax = LABS(ob->x - player->x);
    deltay = LABS(ob->y - player->y);
    
    if (!ProjectileTryMove (ob))
    {
		// IOANCH 30.06.2012: remove link
		Basic::thrownProjectiles.remove(ob);
		
        // IOANCH 20130301: unification culling
        
        if (ob->obclass == rocketobj)
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_boom1;
        } // IOANCH 20130202: unification process
        else if (ob->obclass == hrocketobj)
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_hboom1;
        }
        else
            ob->state = NULL;               // mark for removal
        
        return;
    }
    
    if (deltax < PROJECTILESIZE && deltay < PROJECTILESIZE)
    {       // hit the player
		// IOANCH 30.06.2012: remove link
		Basic::thrownProjectiles.remove(ob);
		
        switch (ob->obclass)
        {
            case needleobj:
                damage = (wolfRnd() >>3) + 20;
                break;
            case rocketobj:
            case hrocketobj:
            case sparkobj:
                damage = (wolfRnd() >>3) + 30;
                break;
            case fireobj:
                damage = (wolfRnd() >>3);
                break;
            default:
                ;
        }
        
        TakeDamage (damage,ob);
        ob->state = NULL;               // mark for removal
        return;
    }
    
    ob->tilex = (short)(ob->x >> TILESHIFT);
    ob->tiley = (short)(ob->y >> TILESHIFT);
}

//
// SpawnDeadGuard
//
void SpawnDeadGuard (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_grddie4);
    DEMOIF_SDL
    {
        newobj->flags |= FL_NONMARK;    // walk through moving enemy fix
    }
    newobj->obclass = inertobj;
}

//
// A_DeathScream
//
void A_DeathScream (objtype *ob)
{
    // IOANCH 20130301: unification culling
    
    // IOANCH 20130202: unification process
    if (!wolfRnd() && ((!SPEAR() && mapSegs.map()==9) ||
                       (SPEAR() && (mapSegs.map() == 18 || mapSegs.map() == 19))))
    {
        switch(ob->obclass)
        {
            case mutantobj:
            case guardobj:
            case officerobj:
            case ssobj:
            case dogobj:
                PlaySoundLocActor(DEATHSCREAM6SND,ob);  // fart
                return;
			default:
				;
        }
    }
    
    switch (ob->obclass)
    {
        case mutantobj:
            PlaySoundLocActor(AHHHGSND,ob);
            break;
            
        case guardobj:
        {
            int sounds[9]={ DEATHSCREAM1SND,
                DEATHSCREAM2SND,
                DEATHSCREAM3SND,
                // IOANCH 20130301: unification culling
                
                DEATHSCREAM4SND,
                DEATHSCREAM5SND,
                DEATHSCREAM7SND,
                DEATHSCREAM8SND,
                DEATHSCREAM9SND
                
            };
            // IOANCH 20130301: unification culling
            
            PlaySoundLocActor(sounds[wolfRnd()%8],ob);
            break;
        }
        default:
            if(atr::sounds[ob->obclass].death >= 0)
            {
                if(atr::flags[ob->obclass] & ATR_BOSS_SOUNDS)
                    SD_PlaySound((soundnames)atr::sounds[ob->obclass].death);
                else
                    PlaySoundLocActor(atr::sounds[ob->obclass].death, ob);
            }
            
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// SPEAR() ACTORS
//
////////////////////////////////////////////////////////////////////////////////

//
// A_UShoot
//
void A_UShoot (objtype *ob)
{
    int     dx,dy,dist;
    
    A_Shoot (ob);
    
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;
    if (dist <= 1)
        TakeDamage (10,ob);
}

//
// A_Launch
//
void A_Launch (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;
    
    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2 ((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);
    if (ob->obclass == deathobj)
    {
        A_Shoot (ob);
        if (ob->state == &s_deathshoot2)
        {
            iangle-=4;
            if (iangle<0)
                iangle+=ANGLES;
        }
        else
        {
            iangle+=4;
            if (iangle>=ANGLES)
                iangle-=ANGLES;
        }
    }
    
    GetNewActor ();
    newobj->state = &s_rocket;
    newobj->ticcount = 1;
    
    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = rocketobj;
    switch(ob->obclass)
    {
        case deathobj:
            newobj->state = &s_hrocket;
            newobj->obclass = hrocketobj;
            PlaySoundLocActor (KNIGHTMISSILESND,newobj);
            break;
        case angelobj:
            newobj->state = &s_spark1;
            newobj->obclass = sparkobj;
            PlaySoundLocActor (ANGELFIRESND,newobj);
            break;
        default:
            PlaySoundLocActor (MISSILEFIRESND,newobj);
    }
    
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;
	
	// IOANCH 30.06.2012: link proj
	Basic::thrownProjectiles.add(newobj);
}

//
// angel
//
void A_Slurpie (objtype *)
{
    SD_PlaySound(SLURPIESND);
}

void A_Breathing (objtype *)
{
    SD_PlaySound(ANGELTIREDSND);
}

//
// A_Victory
//
void A_Victory (objtype *)
{
    playstate = ex_victorious;
}

//
// =================
// =
// = A_StartAttack
// =
// =================
// */
//
void A_StartAttack (objtype *ob)
{
    ob->temp1 = 0;
}

//
// =================
// =
// = A_Relaunch
// =
// =================
// */
//
void A_Relaunch (objtype *ob)
{
    if (++ob->temp1 == 3)
    {
        NewState (ob,&s_angeltired);
        return;
    }
    
    if (wolfRnd()&1)
    {
        NewState (ob,&s_angelchase1);
        return;
    }
}

//
// spectre
//

//
// ===============
// =
// = A_Dormant
// =
// ===============
// */
//
void A_Dormant (objtype *ob)
{
    int32_t     deltax,deltay;
    int         xl,xh,yl,yh;
    int         x,y;
    uintptr_t   tile;
    
    deltax = ob->x - player->x;
    if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
        goto moveok;
    deltay = ob->y - player->y;
    if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
        goto moveok;
    
    return;
moveok:
    
    xl = (ob->x-MINDIST) >> TILESHIFT;
    xh = (ob->x+MINDIST) >> TILESHIFT;
    yl = (ob->y-MINDIST) >> TILESHIFT;
    yh = (ob->y+MINDIST) >> TILESHIFT;
    
    for (y=yl ; y<=yh ; y++)
        for (x=xl ; x<=xh ; x++)
        {
            tile = (uintptr_t)actorat[x][y];
            if (!tile)
                continue;
            if (!ISPOINTER(tile))
                return;
            if (((objtype *)tile)->flags&FL_SHOOTABLE)
                return;
        }
    
    ob->flags |= FL_AMBUSH | FL_SHOOTABLE;
    ob->flags &= ~FL_ATTACKMODE;
    ob->flags &= ~FL_NONMARK;      // stuck bugfix 1
    ob->dir = nodir;
    NewState (ob,&s_spectrewait1);
    // IOANCH reregister it
    Basic::livingNazis.add(ob);
}

//
// =================
// =
// = IOANCH 20121204
// = Merged T_Will, T_Schabb and T_Gift here
// =
// = T_ProjectileBossChase
// =
// =================
// */
//
void T_ProjectileBossChase(objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    Boolean8 dodge;
	
    dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;
	
    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) wolfRnd() < (tics<<3) && objfreelist)
        {
            //
            // go into attack frame
            //
			
			switch(ob->obclass)
			{
                    // IOANCH 20130202: unification process
				case willobj:
					NewState (ob,&s_willshoot1);
					break;
				case angelobj:
					NewState (ob,&s_angelshoot1);
					break;
				case deathobj:
					NewState (ob,&s_deathshoot1);
					break;
				case schabbobj:
					NewState(ob, &s_schabbshoot1);
					break;
				case giftobj:
					NewState(ob, &s_giftshoot1);
					break;
				case fatobj:
					NewState(ob, &s_fatshoot1);
					break;
					
				default:
					;
			}
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;
	
    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
	
    move = ob->speed*tics;
	
    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }
		
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }
		
        //
        // reached goal tile, so select another one
        //
		
        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		
        move -= ob->distance;
		
        if (dist <4)
            SelectRunDir (ob);
        else if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
		
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// SCHABBS / GIFT / FAT
//
////////////////////////////////////////////////////////////////////////////////

//
// _MissileThrow
//
// IOANCH 20130306: generic missile throw
//
static void _MissileThrow(objtype *ob, statetype *state, classtype cl,
                   soundnames snd)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;
    
    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);
    
    GetNewActor ();
    newobj->state = state;
    newobj->ticcount = 1;
    
    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = cl;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;
	
	// IOANCH 30.06.2012: link to projectile list
	Basic::thrownProjectiles.add(newobj);
    
    // IOANCH 20130301: unification culling
    
    PlaySoundLocActor (snd,newobj);
}

//
// A_SchabbThrow
//
void A_SchabbThrow (objtype *ob)
{
    _MissileThrow(ob, &s_needle1, needleobj, SCHABBSTHROWSND);
}

//
// A_GiftThrow
//
void A_GiftThrow (objtype *ob)
{
    _MissileThrow(ob, &s_rocket, rocketobj, MISSILEFIRESND);
}

////////////////////////////////////////////////////////////////////////////////
//
// HITLERS
//
////////////////////////////////////////////////////////////////////////////////

//
// A_HitlerMorph
//
void A_HitlerMorph (objtype *ob)
{
    short hitpoints[4]={500,700,800,900};
    
    SpawnNewObj (ob->tilex,ob->tiley,&s_hitlerchase1);
    newobj->speed = SPDPATROL*5;
    
    newobj->x = ob->x;
    newobj->y = ob->y;
    
    newobj->distance = ob->distance;
    newobj->dir = ob->dir;
    newobj->flags = ob->flags | FL_SHOOTABLE;
    newobj->flags &= ~FL_NONMARK;   // hitler stuck with nodir fix
    
    newobj->obclass = realhitlerobj;
    newobj->hitpoints = hitpoints[gamestate.difficulty];
    
    // IOANCH 20130303: add him to livingNazis list
    Basic::livingNazis.add(newobj);
}

//
// A_MechaSound
// A_Slurpie
//
void A_MechaSound (objtype *ob)
{
    if (areabyplayer[ob->areanumber])
        PlaySoundLocActor (MECHSTEPSND,ob);
}

//
// =================
// =
// = A_FakeFire
// =
// =================
// */
void A_FakeFire (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;
    
    if (!objfreelist)       // stop shooting if over MAXACTORS
    {
        NewState (ob,&s_fakechase1);
        return;
    }
    
    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float)(M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);
    
    GetNewActor ();
    newobj->state = &s_fire1;
    newobj->ticcount = 1;
    
    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->obclass = fireobj;
    newobj->speed = 0x1200l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;
	
	// IOANCH 30.06.2012: link fire
	Basic::thrownProjectiles.add(newobj);
    
    PlaySoundLocActor (FLAMETHROWERSND,newobj);
}

//
// =================
// =
// = T_Fake
// =
// =================
// */
//
void T_Fake (objtype *ob)
{
    int32_t move;
    
    // IOANCH 20.05.2012: modif func
    if (CheckLine(ob))                      // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) wolfRnd() < (tics<<1) && objfreelist)
        {
            //
            // go into attack frame
            //
            NewState (ob,&s_fakeshoot1);
            return;
        }
    }
    else
        ob->hidden = true;
    
    if (ob->dir == nodir)
    {
        SelectDodgeDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
    
    move = ob->speed*tics;
    
    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }
        
        //
        // reached goal tile, so select another one
        //
        
        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        
        move -= ob->distance;
        
        SelectDodgeDir (ob);
        
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// ============================================================================
//
// STAND
//
// ============================================================================
// */
//
////////////////////////////////////////////////////////////////////////////////

//
// ===============
// =
// = T_Stand
// =
// ===============
// */
//
void T_Stand (objtype *ob)
{
    SightPlayer (ob);
}

////////////////////////////////////////////////////////////////////////////////
//
// ============================================================================
//
// CHASE
//
// ============================================================================
// */
//
////////////////////////////////////////////////////////////////////////////////

//
// =================
// =
// = T_Chase
// =
// =================
// */
//

void T_Chase (objtype *ob)
{
    int32_t move,target;
    int     dx,dy,dist,chance;
    Boolean8 dodge;
    
    if (gamestate.victoryflag)
        return;
    
    dodge = false;
    // IOANCH 20.05.2012: modif func
    if (CheckLine(ob))      // got a shot at player?
    {
        ob->hidden = false;
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx : dy;
        
#ifdef PLAYDEMOLIKEORIGINAL
        if(DEMOCOND_ORIG)
        {
            if(!dist || (dist == 1 && ob->distance < 0x4000))
                chance = 300;
            else
                chance = (tics<<4)/dist;
        }
        else
#endif
        {
            if (dist)
                chance = (tics<<4)/dist;
            else
                chance = 300;
            
            if (dist == 1)
            {
                target = abs(ob->x - player->x);
                if (target < 0x14000l)
                {
                    target = abs(ob->y - player->y);
                    if (target < 0x14000l)
                        chance = 300;
                }
            }
        }
        
        if ( wolfRnd()<chance)
        {
            //
            // go into attack frame
            //
            // IOANCH 20130306: wrap
            if(atr::states[ob->obclass].shoot)
                NewState(ob, atr::states[ob->obclass].shoot);
            
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;
    
    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
    
    move = ob->speed*tics;
    
    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }
        
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }
        
        //
        // reached goal tile, so select another one
        //
        
        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        
        move -= ob->distance;
        
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
 =================
 =
 = T_Ghosts
 =
 =================
 */

void T_Ghosts (objtype *ob)
{
    int32_t move;
    
    if (ob->dir == nodir)
    {
        SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
    
    move = ob->speed*tics;
    
    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }
        
        //
        // reached goal tile, so select another one
        //
        
        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        
        move -= ob->distance;
        
        SelectChaseDir (ob);
        
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
 =================
 =
 = T_DogChase
 =
 =================
 */

void T_DogChase (objtype *ob)
{
    int32_t    move;
    int32_t    dx,dy;
    
    if (ob->dir == nodir)
    {
        SelectDodgeDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
    
    move = ob->speed*tics;
    
    while (move)
    {
        //
        // check for byte range
        //
        dx = player->x - ob->x;
        if (dx<0)
            dx = -dx;
        dx -= move;
        if (dx <= MINACTORDIST)
        {
            dy = player->y - ob->y;
            if (dy<0)
                dy = -dy;
            dy -= move;
            if (dy <= MINACTORDIST)
            {
                NewState (ob,&s_dogjump1);
                return;
            }
        }
        
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }
        
        //
        // reached goal tile, so select another one
        //
        
        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        
        move -= ob->distance;
        
        SelectDodgeDir (ob);
        
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
 ============================================================================
 
 PATH
 
 ============================================================================
 */

/*
 ===============
 =
 = SelectPathDir
 =
 ===============
 */

static void SelectPathDir (objtype *ob)
{
    unsigned spot;
    
    spot = MAPSPOT(ob->tilex,ob->tiley,1)-ICONARROWS;
    
    if (spot<8)
    {
        // new direction
        ob->dir = (dirtype)(spot);
    }
    
    ob->distance = TILEGLOBAL;
    
    if (!TryWalk (ob))
        ob->dir = nodir;
}

/*
 ===============
 =
 = T_Path
 =
 ===============
 */

void T_Path (objtype *ob)
{
    int32_t    move;
    
    if (SightPlayer (ob))
        return;
    
    if (ob->dir == nodir)
    {
        SelectPathDir (ob);
        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }
    
    move = ob->speed*tics;
    
    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }
        
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }
        
        if (ob->tilex>MAPSIZE || ob->tiley>MAPSIZE)
        {
            sprintf (str, "T_Path hit a wall at %u,%u, dir %u",
                     ob->tilex,ob->tiley,ob->dir);
            Quit (str);
        }
        
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;
        
        SelectPathDir (ob);
        
        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }
}

/*
 =============================================================================
 
 FIGHT
 
 =============================================================================
 */

/*
 ===============
 =
 = A_Shoot
 =
 = Try to damage the player, based on skill level and player's speed
 =
 ===============
 */

void A_Shoot (objtype *ob)
{
    int     dx,dy,dist;
    int     hitchance,damage;
    
    hitchance = 128;
    
    if (!areabyplayer[ob->areanumber])
        return;
    
    // IOANCH 20130306: restored old behaviour, now shot sound means hit.
    if (!CheckLine (ob))                    // player is not behind a wall
        return;
    
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx:dy;
    
    if (ob->obclass == ssobj || ob->obclass == bossobj)
        dist = dist*2/3;                                        // ss are better shots
    
    if (thrustspeed >= RUNSPEED)
    {
        if (ob->flags&FL_VISABLE)
            hitchance = 160-dist*16;                // player can see to dodge
        else
            hitchance = 160-dist*8;
    }
    else
    {
        if (ob->flags&FL_VISABLE)
            hitchance = 256-dist*16;                // player can see to dodge
        else
            hitchance = 256-dist*8;
    }
    
    // see if the shot was a hit
    
    if (wolfRnd()<hitchance)
    {
        if (dist<2)
            damage = wolfRnd()>>2;
        else if (dist<4)
            damage = wolfRnd()>>3;
        else
            damage = wolfRnd()>>4;
        
        TakeDamage (damage,ob);
    }
    
    
    switch(ob->obclass)
    {
        case ssobj:
            PlaySoundLocActor(SSFIRESND,ob);
            break;
            // IOANCH 20130202: unification process
            // IOANCH 20130301: unification culling
            
        case giftobj:
        case fatobj:
            PlaySoundLocActor(MISSILEFIRESND,ob);
            break;
            
        case mechahitlerobj:
        case realhitlerobj:
        case bossobj:
            PlaySoundLocActor(BOSSFIRESND,ob);
            break;
        case schabbobj:
            PlaySoundLocActor(SCHABBSTHROWSND,ob);
            break;
        case fakeobj:
            PlaySoundLocActor(FLAMETHROWERSND,ob);
            break;
        default:
            PlaySoundLocActor(NAZIFIRESND,ob);
    }
}

/*
 ===============
 =
 = A_Bite
 =
 ===============
 */

void A_Bite (objtype *ob)
{
    int32_t    dx,dy;
    
    PlaySoundLocActor(DOGATTACKSND,ob);     // JAB
    
    dx = player->x - ob->x;
    if (dx<0)
        dx = -dx;
    dx -= TILEGLOBAL;
    if (dx <= MINACTORDIST)
    {
        dy = player->y - ob->y;
        if (dy<0)
            dy = -dy;
        dy -= TILEGLOBAL;
        if (dy <= MINACTORDIST)
        {
            if (wolfRnd()<180)
            {
                TakeDamage (wolfRnd()>>4,ob);
                return;
            }
        }
    }
}

// IOANCH 20130202: unification process
/*
 ============================================================================
 
 BJ VICTORY
 
 ============================================================================
 */

/*
 ===============
 =
 = SpawnBJVictory
 =
 ===============
 */

void SpawnBJVictory ()
{
    SpawnNewObj (player->tilex,player->tiley+1,&s_bjrun1);
    newobj->x = player->x;
    newobj->y = player->y;
    newobj->obclass = bjobj;
    newobj->dir = north;
    newobj->temp1 = 6;                      // tiles to run forward
}

/*
 ===============
 =
 = T_BJRun
 =
 ===============
 */

void T_BJRun (objtype *ob)
{
    int32_t    move;
    
    move = BJRUNSPEED*tics;
    
    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }
        
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;
        
        SelectPathDir (ob);
        
        if ( !(--ob->temp1) )
        {
            NewState (ob,&s_bjjump1);
            return;
        }
    }
}

/*
 ===============
 =
 = T_BJJump
 =
 ===============
 */

void T_BJJump (objtype *ob)
{
    int32_t    move;
    
    move = BJJUMPSPEED*tics;
    MoveObj (ob,move);
}

/*
 ===============
 =
 = A_BJYell
 =
 ===============
 */

void A_BJYell (objtype *ob)
{
    PlaySoundLocActor(YEAHSND,ob);  // JAB
}

/*
 ===============
 =
 = A_BJDone
 =
 ===============
 */

void A_BJDone (objtype *)
{
    playstate = ex_victorious;                              // exit castle tile
}

//===========================================================================

/*
 ===============
 =
 = CheckPosition
 =
 ===============
 */

static Boolean8 CheckPosition (objtype *ob)
{
    int     x,y,xl,yl,xh,yh;
    objtype *check;
    
    xl = (ob->x-PLAYERSIZE) >> TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >> TILESHIFT;
    
    xh = (ob->x+PLAYERSIZE) >> TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >> TILESHIFT;
    
    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
                return false;
        }
    }
    
    return true;
}

/*
 ===============
 =
 = A_StartDeathCam
 =
 ===============
 */

void    A_StartDeathCam (objtype *ob)
{
    int32_t dx,dy;
    float   fangle;
    int32_t xmove,ymove;
    int32_t dist;
    
    FinishPaletteShifts ();
    
    VL_WaitVBL (100);
    
    if (gamestate.victoryflag)
    {
        playstate = ex_victorious;                              // exit castle tile
        return;
    }
    
    if(cfg_usedoublebuffering) I_UpdateScreen();
    
    gamestate.victoryflag = true;
    unsigned fadeheight = viewsize != 21 ? cfg_screenHeight-vid_scaleFactor*STATUSLINES : cfg_screenHeight;
    VL_BarScaledCoord (0, 0, cfg_screenWidth, fadeheight, bordercol);
    FizzleFade(0, 0, cfg_screenWidth, fadeheight, 70, false);
    
    if (bordercol != VIEWCOLOR)
    {
        graphSegs.cacheChunk (SPEAR.g(STARTFONT)+1);
        fontnumber = 1;
        SETFONTCOLOR(15,bordercol);
        PrintX = 68; PrintY = 45;
        US_Print (STR_SEEAGAIN);
        graphSegs.uncacheChunk(SPEAR.g(STARTFONT)+1);
    }
    else
    {
        CacheLump(SPEAR.g(LEVELEND_LUMP_START),SPEAR.g(LEVELEND_LUMP_END));
        // IOANCH 20130301: unification culling
        Write(0,7,STR_SEEAGAIN);
    }
    
    I_UpdateScreen ();
    if(cfg_usedoublebuffering) I_UpdateScreen();
    
    myInput.userInput(300);
    
    //
    // line angle up exactly
    //
    NewState (player,&s_deathcam);
    
    player->x = gamestate.killx;
    player->y = gamestate.killy;
    
    dx = ob->x - player->x;
    dy = player->y - ob->y;
    
    fangle = (float) atan2((float) dy, (float) dx);          // returns -pi to pi
    if (fangle<0)
        fangle = (float) (M_PI*2+fangle);
    
    player->angle = (short) (fangle/(M_PI*2)*ANGLES);
    
    //
    // try to position as close as possible without being in a wall
    //
    dist = 0x14000l;
    do
    {
        xmove = FixedMul(dist,costable[player->angle]);
        ymove = -FixedMul(dist,sintable[player->angle]);
        
        player->x = ob->x - xmove;
        player->y = ob->y - ymove;
        dist += 0x1000;
        
    } while (!CheckPosition (player));
    
    player->tilex = (word)(player->x >> TILESHIFT);         // scale to tile values
    player->tiley = (word)(player->y >> TILESHIFT);
    
    //
    // go back to the game
    //
    
    DrawPlayBorder ();
    
    fizzlein = true;
    
    switch (ob->obclass)
    {
            // IOANCH 20130202: unification process
        case schabbobj:
            NewState (ob,&s_schabbdeathcam);
            break;
        case realhitlerobj:
            NewState (ob,&s_hitlerdeathcam);
            break;
        case giftobj:
            NewState (ob,&s_giftdeathcam);
            break;
        case fatobj:
            NewState (ob,&s_fatdeathcam);
            break;
		default:
			;
    }
}
