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
// Various actor controlling functions
//
////////////////////////////////////////////////////////////////////////////////

// WL_STATE.C

#include "wl_def.h"
#include "version.h"
#include "wl_act1.h"
#include "wl_act2.h"
#include "wl_agent.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_play.h"
#include "id_ca.h"
#include "id_us.h"
#include "ioan_bas.h"	// IOANCH 29.06.2012
#include "List.h"
#include "obattrib.h"
#include "ActorStates.h"

////////////////////////////////////////////////////////////////////////////////
//
//                            LOCAL CONSTANTS
//
//                            GLOBAL VARIABLES
//
////////////////////////////////////////////////////////////////////////////////



static const dirtype opposite[9] =
    {west,southwest,south,southeast,east,northeast,north,northwest,nodir};

static const dirtype diagonal[9][9] =
{
    /* east */  {nodir,nodir,northeast,nodir,nodir,nodir,southeast,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    /* north */ {northeast,nodir,nodir,nodir,northwest,nodir,nodir,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    /* west */  {nodir,nodir,northwest,nodir,nodir,nodir,southwest,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    /* south */ {southeast,nodir,nodir,nodir,southwest,nodir,nodir,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
                {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir}
};



void    SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state);
void    NewState (objtype *ob, statetype *state);

Boolean8 TryWalk (objtype *ob);
void    MoveObj (objtype *ob, int32_t move);

void    KillActor (objtype *ob);
void    DamageActor (objtype *ob, unsigned damage);


void    FirstSighting (objtype *ob);
Boolean8 CheckSight (objtype *ob);

////////////////////////////////////////////////////////////////////////////////
//
//                                LOCAL VARIABLES
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// =
// = SpawnNewObj
// =
// = Spaws a new actor at the given TILE coordinates, with the given state, and
// = the given size in GLOBAL units.
// =
// = newobj = a pointer to an initialized new actor
// =
//
////////////////////////////////////////////////////////////////////////////////


void SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state)
{
    GetNewActor ();
    newobj->state = state;
    if (state->tictime)
        newobj->ticcount = DEMOCHOOSE_ORIG_SDL(
                wolfRnd () % state->tictime,
                wolfRnd () % state->tictime + 1);     // Chris' moonwalk bugfix ;D
    else
        newobj->ticcount = 0;

    newobj->tilex = (short) tilex;
    newobj->tiley = (short) tiley;
    newobj->x = ((int32_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
    newobj->y = ((int32_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
    newobj->dir = nodir;

    actorat[tilex][tiley] = newobj;
   newobj->areanumber = mapSegs(0, newobj->tilex, newobj->tiley) - AREATILE;
}



////////////////////////////////////////////////////////////////////////////////
//
// =
// = NewState
// =
// = Changes ob to a new state, setting ticcount to the max for that state
// =
//
////////////////////////////////////////////////////////////////////////////////

// IOANCH 20130305: made it const-correct
void NewState (objtype *ob, statetype *state)
{
    ob->state = state;
    ob->ticcount = state->tictime;
}

////////////////////////////////////////////////////////////////////////////////
//
//                        ENEMY TILE WORLD MOVEMENT CODE
//
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
// =
// = TryWalk
// =
// = Attempts to move ob in its current (ob->dir) direction.
// =
// = If blocked by either a wall or an actor returns FALSE
// =
// = If move is either clear or blocked only by a door, returns TRUE and sets
// =
// = ob->tilex         = new destination
// = ob->tiley
// = ob->areanumber    = the floor tile number (0-(NUMAREAS-1)) of destination
// = ob->distance      = TILEGLOBAl, or -doornumber if a door is blocking the 
// way
// =
// = If a door is in the way, an OpenDoor call is made to start it opening.
// = The actor code should wait until
// =       doorobjlist[-ob->distance].action = dr_open, meaning the door has 
// been
// =       fully opened
// =
//
////////////////////////////////////////////////////////////////////////////////


#define CHECKDIAG(x,y)                              \
{                                                   \
    temp=(uintptr_t)actorat[x][y];                  \
    if (temp)                                       \
    {                                               \
        if (temp<256)                               \
            return false;                           \
        if (((objtype *)temp)->flags&FL_SHOOTABLE)  \
            return false;                           \
    }                                               \
}

#ifdef PLAYDEMOLIKEORIGINAL
    #define DOORCHECK                                   \
            if(DEMOCOND_ORIG)                           \
                doornum = temp&63;                      \
            else                                        \
            {                                           \
                doornum = (int) temp & 127;             \
                OpenDoor(doornum);                      \
                ob->distance = -doornum - 1;            \
                return true;                            \
            }
#else
    #define DOORCHECK                                   \
            doornum = (int) temp & 127;                 \
            OpenDoor(doornum);                          \
            ob->distance = -doornum - 1;                \
            return true;
#endif

#define CHECKSIDE(x,y)                                  \
{                                                       \
    temp=(uintptr_t)actorat[x][y];                      \
    if (temp)                                           \
    {                                                   \
        if (temp<128)                                   \
            return false;                               \
        if (temp<256)                                   \
        {                                               \
            DOORCHECK                                   \
        }                                               \
        else if (((objtype *)temp)->flags&FL_SHOOTABLE) \
            return false;                               \
    }                                                   \
}


Boolean8 TryWalk (objtype *ob)
{
    int       doornum = -1;
    uintptr_t temp;

    if (ob->obclass == inertobj)
    {
        switch (ob->dir)
        {
            case north:
                ob->tiley--;
                break;

            case northeast:
                ob->tilex++;
                ob->tiley--;
                break;

            case east:
                ob->tilex++;
                break;

            case southeast:
                ob->tilex++;
                ob->tiley++;
                break;

            case south:
                ob->tiley++;
                break;

            case southwest:
                ob->tilex--;
                ob->tiley++;
                break;

            case west:
                ob->tilex--;
                break;

            case northwest:
                ob->tilex--;
                ob->tiley--;
                break;
			default:
				;
        }
    }
    else
    {
        switch (ob->dir)
        {
            case north:
                if (ob->obclass == dogobj || ob->obclass == fakeobj
                    || ob->obclass == ghostobj || ob->obclass == spectreobj)
                {
                    CHECKDIAG(ob->tilex,ob->tiley-1);
                }
                else
                {
                    CHECKSIDE(ob->tilex,ob->tiley-1);
                }
                ob->tiley--;
                break;

            case northeast:
                CHECKDIAG(ob->tilex+1,ob->tiley-1);
                CHECKDIAG(ob->tilex+1,ob->tiley);
                CHECKDIAG(ob->tilex,ob->tiley-1);
                ob->tilex++;
                ob->tiley--;
                break;

            case east:
                if (ob->obclass == dogobj || ob->obclass == fakeobj
                    || ob->obclass == ghostobj || ob->obclass == spectreobj)
                {
                    CHECKDIAG(ob->tilex+1,ob->tiley);
                }
                else
                {
                    CHECKSIDE(ob->tilex+1,ob->tiley);
                }
                ob->tilex++;
                break;

            case southeast:
                CHECKDIAG(ob->tilex+1,ob->tiley+1);
                CHECKDIAG(ob->tilex+1,ob->tiley);
                CHECKDIAG(ob->tilex,ob->tiley+1);
                ob->tilex++;
                ob->tiley++;
                break;

            case south:
                if (ob->obclass == dogobj || ob->obclass == fakeobj
                    || ob->obclass == ghostobj || ob->obclass == spectreobj)
                {
                    CHECKDIAG(ob->tilex,ob->tiley+1);
                }
                else
                {
                    CHECKSIDE(ob->tilex,ob->tiley+1);
                }
                ob->tiley++;
                break;

            case southwest:
                CHECKDIAG(ob->tilex-1,ob->tiley+1);
                CHECKDIAG(ob->tilex-1,ob->tiley);
                CHECKDIAG(ob->tilex,ob->tiley+1);
                ob->tilex--;
                ob->tiley++;
                break;

            case west:
                if (ob->obclass == dogobj || ob->obclass == fakeobj
                    || ob->obclass == ghostobj || ob->obclass == spectreobj)
                {
                    CHECKDIAG(ob->tilex-1,ob->tiley);
                }
                else
                {
                    CHECKSIDE(ob->tilex-1,ob->tiley);
                }
                ob->tilex--;
                break;

            case northwest:
                CHECKDIAG(ob->tilex-1,ob->tiley-1);
                CHECKDIAG(ob->tilex-1,ob->tiley);
                CHECKDIAG(ob->tilex,ob->tiley-1);
                ob->tilex--;
                ob->tiley--;
                break;

            case nodir:
                return false;

            default:
                Quit ("Walk: Bad dir");
        }
    }

#ifdef PLAYDEMOLIKEORIGINAL
    if (DEMOCOND_ORIG && doornum != -1)
    {
        OpenDoor(doornum);
        ob->distance = -doornum-1;
        return true;
    }
#endif

   ob->areanumber = mapSegs(0, ob->tilex, ob->tiley) - AREATILE;

    ob->distance = TILEGLOBAL;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = SelectDodgeDir
// =
// = Attempts to choose and initiate a movement for ob that sends it towards
// = the player while dodging
// =
// = If there is no possible move (ob is totally surrounded)
// =
// = ob->dir           =       nodir
// =
// = Otherwise
// =
// = ob->dir           = new direction to follow
// = ob->distance      = TILEGLOBAL or -doornumber
// = ob->tilex         = new destination
// = ob->tiley
// = ob->areanumber    = the floor tile number (0-(NUMAREAS-1)) of destination
// =
//
////////////////////////////////////////////////////////////////////////////////


void SelectDodgeDir (objtype *ob)
{
    int         deltax,deltay,i;
    unsigned    absdx,absdy;
    dirtype     dirtry[5];
    dirtype     turnaround,tdir;

    if (ob->flags & FL_FIRSTATTACK)
    {
        //
        // turning around is only ok the very first time after noticing the
        // player
        //
        turnaround = nodir;
        ob->flags &= ~FL_FIRSTATTACK;
    }
    else
        turnaround=opposite[ob->dir];

    deltax = player->tilex - ob->tilex;
    deltay = player->tiley - ob->tiley;

    //
    // arange 5 direction choices in order of preference
    // the four cardinal directions plus the diagonal straight towards
    // the player
    //

    if (deltax>0)
    {
        dirtry[1]= east;
        dirtry[3]= west;
    }
    else
    {
        dirtry[1]= west;
        dirtry[3]= east;
    }

    if (deltay>0)
    {
        dirtry[2]= south;
        dirtry[4]= north;
    }
    else
    {
        dirtry[2]= north;
        dirtry[4]= south;
    }

    //
    // randomize a bit for dodging
    //
    absdx = abs(deltax);
    absdy = abs(deltay);

    if (absdx > absdy)
    {
        tdir = dirtry[1];
        dirtry[1] = dirtry[2];
        dirtry[2] = tdir;
        tdir = dirtry[3];
        dirtry[3] = dirtry[4];
        dirtry[4] = tdir;
    }

    if (wolfRnd() < 128)
    {
        tdir = dirtry[1];
        dirtry[1] = dirtry[2];
        dirtry[2] = tdir;
        tdir = dirtry[3];
        dirtry[3] = dirtry[4];
        dirtry[4] = tdir;
    }

    dirtry[0] = diagonal [ dirtry[1] ] [ dirtry[2] ];

    //
    // try the directions util one works
    //
    for (i=0;i<5;i++)
    {
        if ( dirtry[i] == nodir || dirtry[i] == turnaround)
            continue;

        ob->dir = dirtry[i];
        if (TryWalk(ob))
            return;
    }

    //
    // turn around only as a last resort
    //
    if (turnaround != nodir)
    {
        ob->dir = turnaround;

        if (TryWalk(ob))
            return;
    }

    ob->dir = nodir;
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = SelectChaseDir
// =
// = As SelectDodgeDir, but doesn't try to dodge
// =
//
////////////////////////////////////////////////////////////////////////////////


void SelectChaseDir (objtype *ob)
{
    int     deltax,deltay;
    dirtype d[3];
    dirtype tdir, olddir, turnaround;


    olddir=ob->dir;
    turnaround=opposite[olddir];

    deltax=player->tilex - ob->tilex;
    deltay=player->tiley - ob->tiley;

    d[1]=nodir;
    d[2]=nodir;

    if (deltax>0)
        d[1]= east;
    else if (deltax<0)
        d[1]= west;
    if (deltay>0)
        d[2]=south;
    else if (deltay<0)
        d[2]=north;

    if (abs(deltay)>abs(deltax))
    {
        tdir=d[1];
        d[1]=d[2];
        d[2]=tdir;
    }

    if (d[1]==turnaround)
        d[1]=nodir;
    if (d[2]==turnaround)
        d[2]=nodir;


    if (d[1]!=nodir)
    {
        ob->dir=d[1];
        if (TryWalk(ob))
            return;     /*either moved forward or attacked*/
    }

    if (d[2]!=nodir)
    {
        ob->dir=d[2];
        if (TryWalk(ob))
            return;
    }

    /* there is no direct path to the player, so pick another direction */

    if (olddir!=nodir)
    {
        ob->dir=olddir;
        if (TryWalk(ob))
            return;
    }

    if (wolfRnd()>128)      /*randomly determine direction of search*/
    {
        for (tdir=north; tdir<=west; tdir=(dirtype)(tdir+1))
        {
            if (tdir!=turnaround)
            {
                ob->dir=tdir;
                if ( TryWalk(ob) )
                    return;
            }
        }
    }
    else
    {
        for (tdir=west; tdir>=north; tdir=(dirtype)(tdir-1))
        {
            if (tdir!=turnaround)
            {
                ob->dir=tdir;
                if ( TryWalk(ob) )
                    return;
            }
        }
    }

    if (turnaround !=  nodir)
    {
        ob->dir=turnaround;
        if (ob->dir != nodir)
        {
            if ( TryWalk(ob) )
                return;
        }
    }

    ob->dir = nodir;                // can't move
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = SelectRunDir
// =
// = Run Away from player
// =
//
////////////////////////////////////////////////////////////////////////////////


void SelectRunDir (objtype *ob)
{
    int deltax,deltay;
    dirtype d[3];
    dirtype tdir;


    deltax=player->tilex - ob->tilex;
    deltay=player->tiley - ob->tiley;

    if (deltax<0)
        d[1]= east;
    else
        d[1]= west;
    if (deltay<0)
        d[2]=south;
    else
        d[2]=north;

    if (abs(deltay)>abs(deltax))
    {
        tdir=d[1];
        d[1]=d[2];
        d[2]=tdir;
    }

    ob->dir=d[1];
    if (TryWalk(ob))
        return;     /*either moved forward or attacked*/

    ob->dir=d[2];
    if (TryWalk(ob))
        return;

    /* there is no direct path to the player, so pick another direction */

    if (wolfRnd()>128)      /*randomly determine direction of search*/
    {
        for (tdir=north; tdir<=west; tdir=(dirtype)(tdir+1))
        {
            ob->dir=tdir;
            if ( TryWalk(ob) )
                return;
        }
    }
    else
    {
        for (tdir=west; tdir>=north; tdir=(dirtype)(tdir-1))
        {
            ob->dir=tdir;
            if ( TryWalk(ob) )
                return;
        }
    }

    ob->dir = nodir;                // can't move
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = MoveObj
// =
// = Moves ob be move global units in ob->dir direction
// = Actors are not allowed to move inside the player
// = Does NOT check to see if the move is tile map valid
// =
// = ob->x                 = adjusted for new position
// = ob->y
// =
//
////////////////////////////////////////////////////////////////////////////////


void MoveObj (objtype *ob, int32_t move)
{
    int32_t    deltax,deltay;

    switch (ob->dir)
    {
        case north:
            ob->y -= move;
            break;
        case northeast:
            ob->x += move;
            ob->y -= move;
            break;
        case east:
            ob->x += move;
            break;
        case southeast:
            ob->x += move;
            ob->y += move;
            break;
        case south:
            ob->y += move;
            break;
        case southwest:
            ob->x -= move;
            ob->y += move;
            break;
        case west:
            ob->x -= move;
            break;
        case northwest:
            ob->x -= move;
            ob->y -= move;
            break;

        case nodir:
            return;

        default:
            Quit ("MoveObj: bad dir!");
    }

    //
    // check to make sure it's not on top of player
    //
    if (areabyplayer[ob->areanumber])
    {
        deltax = ob->x - player->x;
        if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
            goto moveok;
        deltay = ob->y - player->y;
        if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
            goto moveok;

        if (ob->hidden)          // move closer until he meets CheckLine
            goto moveok;

        if (ob->obclass == ghostobj || ob->obclass == spectreobj)
            TakeDamage (tics*2,ob);

        //
        // back up
        //
        switch (ob->dir)
        {
            case north:
                ob->y += move;
                break;
            case northeast:
                ob->x -= move;
                ob->y += move;
                break;
            case east:
                ob->x -= move;
                break;
            case southeast:
                ob->x -= move;
                ob->y -= move;
                break;
            case south:
                ob->y -= move;
                break;
            case southwest:
                ob->x += move;
                ob->y -= move;
                break;
            case west:
                ob->x += move;
                break;
            case northwest:
                ob->x += move;
                ob->y += move;
                break;

            case nodir:
                return;
        }
        return;
    }
moveok:
    ob->distance -=move;
}

////////////////////////////////////////////////////////////////////////////////
//
//                                STUFF
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// =
// = DropItem
// =
// = Tries to drop a bonus item somewhere in the tiles surrounding the
// = given tilex/tiley
// =
//
////////////////////////////////////////////////////////////////////////////////


void DropItem (wl_stat_t itemtype, int tilex, int tiley)
{
    int     x,y,xl,xh,yl,yh;

    //
    // find a free spot to put it in
    //
    if (!actorat[tilex][tiley])
    {
        PlaceItemType (itemtype, tilex,tiley);
        return;
    }

    xl = tilex-1;
    xh = tilex+1;
    yl = tiley-1;
    yh = tiley+1;

    for (x=xl ; x<= xh ; x++)
    {
        for (y=yl ; y<= yh ; y++)
        {
            if (!actorat[x][y])
            {
                PlaceItemType (itemtype, x,y);
                return;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = KillActor
// =
//
////////////////////////////////////////////////////////////////////////////////


void KillActor (objtype *ob)
{
    int     tilex,tiley;

    tilex = ob->tilex = (word)(ob->x >> TILESHIFT);         // drop item on center
    tiley = ob->tiley = (word)(ob->y >> TILESHIFT);

    switch (ob->obclass)
    {
        case guardobj:
            GivePoints (I_GUARDSCORE);	// IOANCH 25.10.2012: all constants started with I_
            NewState (ob,&s_grddie1);
            PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case officerobj:
            GivePoints (I_OFFICERSCORE);
            NewState (ob,&s_ofcdie1);
            PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case mutantobj:
            GivePoints (I_MUTANTSCORE);
            NewState (ob,&s_mutdie1);
            PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case ssobj:
            GivePoints (I_SSSCORE);
            NewState (ob,&s_ssdie1);
            if (gamestate.bestweapon < wp_machinegun)
                PlaceItemType (bo_machinegun,tilex,tiley);
            else
                PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case dogobj:
            GivePoints (I_DOGSCORE);
            NewState (ob,&s_dogdie1);
            break;
            // IOANCH 20130303: unification
        case bossobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_bossdie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case gretelobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_greteldie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case giftobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (ob,&s_giftdie1);
            break;

        case fatobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (ob,&s_fatdie1);
            break;

        case schabbobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (ob,&s_schabbdie1);
            break;
        case fakeobj:
            GivePoints (I_FAKEHITLERSCORE);
            NewState (ob,&s_fakedie1);
            break;

        case mechahitlerobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_mechadie1);
            break;
        case realhitlerobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (ob,&s_hitlerdie1);
            break;
        case spectreobj:
            if (ob->flags&FL_BONUS)
            {
                GivePoints (I_SPECTRESCORE);       // Get points once for each
                ob->flags &= ~FL_BONUS;
            }
            NewState (ob,&s_spectredie1);
            break;

        case angelobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_angeldie1);
            break;

        case transobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_transdie0);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case uberobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_uberdie0);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case willobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_willdie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case deathobj:
            GivePoints (I_BOSSSCORE);
            NewState (ob,&s_deathdie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;
		default:
			;
    }

    gamestate.killcount++;
	// IOANCH 29.06.2012: remove object from living set
	Basic::livingNazis.remove(ob);
	
    ob->flags &= ~FL_SHOOTABLE;
    actorat[ob->tilex][ob->tiley] = NULL;
    ob->flags |= FL_NONMARK;
}
////////////////////////////////////////////////////////////////////////////////
//
// =
// = DamageActor
// =
// = Called when the player succesfully hits an enemy.
// =
// = Does damage points to enemy ob, either putting it into a stun frame or
// = killing it.
// =
//
////////////////////////////////////////////////////////////////////////////////


void DamageActor (objtype *ob, unsigned damage)
{
    madenoise = true;

    //
    // do double damage if shooting a non attack mode actor
    //
    if ( !(ob->flags & FL_ATTACKMODE) )
        damage <<= 1;

    ob->hitpoints -= (short)damage;

    if (ob->hitpoints<=0)
        KillActor (ob);
    else
    {
        if (! (ob->flags & FL_ATTACKMODE) )
            FirstSighting (ob);             // put into combat mode
        
        // IOANCH 20130305: wrap them
        statetype *pain = atr::states[ob->obclass].pain,
               *altpain = atr::states[ob->obclass].altpain;
        
        if(pain && ob->hitpoints & 1)
            NewState(ob, pain);
        else if(altpain)
            NewState(ob, altpain);

    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                                CHECKSIGHT
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// =
// = CheckLine
// =
// = Returns true if a straight line between the player and ob is unobstructed
// =
//
////////////////////////////////////////////////////////////////////////////////


Boolean8 CheckLine (objtype *ob, Boolean8 solidActors)
{
    // IOANCH 20130305: DRY
    return Basic::GenericCheckLine(ob->x, ob->y, player->x, player->y, solidActors);
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = CheckSight
// =
// = Checks a straight line between player and current object
// =
// = If the sight is ok, check alertness and angle to see if they notice
// =
// = returns true if the player has been spoted
// =
//
////////////////////////////////////////////////////////////////////////////////


#define MINSIGHT        0x18000l

Boolean8 CheckSight (objtype *ob)
{
    int32_t deltax,deltay;

    //
    // don't bother tracing a line if the area isn't connected to the player's
    //
    if (!areabyplayer[ob->areanumber])
        return false;

    //
    // if the player is real close, sight is automatic
    //
    deltax = player->x - ob->x;
    deltay = player->y - ob->y;

    if (deltax > -MINSIGHT && deltax < MINSIGHT
        && deltay > -MINSIGHT && deltay < MINSIGHT)
        return true;

    //
    // see if they are looking in the right direction
    //
    switch (ob->dir)
    {
        case north:
            if (deltay > 0)
                return false;
            break;

        case east:
            if (deltax < 0)
                return false;
            break;

        case south:
            if (deltay < 0)
                return false;
            break;

        case west:
            if (deltax > 0)
                return false;
            break;

        // check diagonal moving guards fix

        case northwest:
            if (DEMOCOND_SDL && deltay > -deltax)
                return false;
            break;

        case northeast:
            if (DEMOCOND_SDL && deltay > deltax)
                return false;
            break;

        case southwest:
            if (DEMOCOND_SDL && deltax > deltay)
                return false;
            break;

        case southeast:
            if (DEMOCOND_SDL && -deltax > deltay)
                return false;
            break;
		default:
			;
    }

    //
    // trace a line to check for blocking tiles (corners)
    //

    return CheckLine (ob);
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = FirstSighting
// =
// = Puts an actor into attack mode and possibly reverses the direction
// = if the player is behind it
// =
//
////////////////////////////////////////////////////////////////////////////////


void FirstSighting (objtype *ob)
{
    //
    // react to the player
    //
    
    // IOANCH 20130305: wrap them to objattribs
    classtype &cls = ob->obclass;
    if(atr::sounds[cls].sight >= 0)
    {
        if(atr::flags[cls] & ATR_BOSS_SOUNDS)
            SD_PlaySound((soundnames)atr::sounds[cls].sight);
        else
            PlaySoundLocActor(atr::sounds[cls].sight, ob);
    }
    if(atr::states[cls].chase)
        NewState(ob, atr::states[cls].chase);
    if(atr::speeds[cls].chase >= 0)
        ob->speed = atr::speeds[cls].chase;

    if (ob->distance < 0)
        ob->distance = 0;       // ignore the door opening command

    ob->flags |= FL_ATTACKMODE|FL_FIRSTATTACK;
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = SightPlayer
// =
// = Called by actors that ARE NOT chasing the player.  If the player
// = is detected (by sight, noise, or proximity), the actor is put into
// = it's combat frame and true is returned.
// =
// = Incorporates a random reaction delay
// =
//
////////////////////////////////////////////////////////////////////////////////


Boolean8 SightPlayer (objtype *ob)
{
    if (ob->flags & FL_ATTACKMODE)
        Quit ("An actor in ATTACKMODE called SightPlayer!");

    if (ob->temp2)
    {
        //
        // count down reaction time
        //
        ob->temp2 -= (short) tics;
        if (ob->temp2 > 0)
            return false;
        ob->temp2 = 0;                                  // time to react
    }
    else
    {
        if (!areabyplayer[ob->areanumber])
            return false;

        if (ob->flags & FL_AMBUSH)
        {
            if (!CheckSight (ob))
                return false;
            ob->flags &= ~FL_AMBUSH;
        }
        else
        {
            if (!madenoise && !CheckSight (ob))
                return false;
        }


        switch (ob->obclass)
        {
            case guardobj:
                ob->temp2 = 1+wolfRnd()/4;
                break;
            case officerobj:
                ob->temp2 = 2;
                break;
            case mutantobj:
                ob->temp2 = 1+wolfRnd()/6;
                break;
            case ssobj:
                ob->temp2 = 1+wolfRnd()/6;
                break;
            case dogobj:
                ob->temp2 = 1+wolfRnd()/8;
                break;

            case bossobj:
            case schabbobj:
            case fakeobj:
            case mechahitlerobj:
            case realhitlerobj:
            case gretelobj:
            case giftobj:
            case fatobj:
            case spectreobj:
            case angelobj:
            case transobj:
            case uberobj:
            case willobj:
            case deathobj:
                ob->temp2 = 1;
                break;
			default:
				;
        }
        return false;
    }

    FirstSighting (ob);

    return true;
}
