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
#include "Exception.h"

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
void objtype::NewState (statetype *state)
{
    this->state = state;
    this->ticcount = state->tictime;
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
                Act1::OpenDoor(doornum);                      \
                distance = -doornum - 1;                \
                return true;                            \
            }
#else
    #define DOORCHECK                                   \
            doornum = (int) temp & 127;                 \
            Act1::OpenDoor(doornum);                          \
            distance = -doornum - 1;                    \
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


Boolean8 objtype::TryWalk ()
{
    int       doornum = -1;
    uintptr_t temp;

    if (obclass == inertobj)
    {
        switch (dir)
        {
            case north:
                tiley--;
                break;

            case northeast:
                tilex++;
                tiley--;
                break;

            case east:
                tilex++;
                break;

            case southeast:
                tilex++;
                tiley++;
                break;

            case south:
                tiley++;
                break;

            case southwest:
                tilex--;
                tiley++;
                break;

            case west:
                tilex--;
                break;

            case northwest:
                tilex--;
                tiley--;
                break;
			default:
				;
        }
    }
    else
    {
        switch (dir)
        {
            case north:
                if (obclass == dogobj || obclass == fakeobj
                    || obclass == ghostobj || obclass == spectreobj)
                {
                    CHECKDIAG(tilex,tiley-1);
                }
                else
                {
                    CHECKSIDE(tilex,tiley-1);
                }
                tiley--;
                break;

            case northeast:
                CHECKDIAG(tilex+1,tiley-1);
                CHECKDIAG(tilex+1,tiley);
                CHECKDIAG(tilex,tiley-1);
                tilex++;
                tiley--;
                break;

            case east:
                if (obclass == dogobj || obclass == fakeobj
                    || obclass == ghostobj || obclass == spectreobj)
                {
                    CHECKDIAG(tilex+1,tiley);
                }
                else
                {
                    CHECKSIDE(tilex+1,tiley);
                }
                tilex++;
                break;

            case southeast:
                CHECKDIAG(tilex+1,tiley+1);
                CHECKDIAG(tilex+1,tiley);
                CHECKDIAG(tilex,tiley+1);
                tilex++;
                tiley++;
                break;

            case south:
                if (obclass == dogobj || obclass == fakeobj
                    || obclass == ghostobj || obclass == spectreobj)
                {
                    CHECKDIAG(tilex,tiley+1);
                }
                else
                {
                    CHECKSIDE(tilex,tiley+1);
                }
                tiley++;
                break;

            case southwest:
                CHECKDIAG(tilex-1,tiley+1);
                CHECKDIAG(tilex-1,tiley);
                CHECKDIAG(tilex,tiley+1);
                tilex--;
                tiley++;
                break;

            case west:
                if (obclass == dogobj || obclass == fakeobj
                    || obclass == ghostobj || obclass == spectreobj)
                {
                    CHECKDIAG(tilex-1,tiley);
                }
                else
                {
                    CHECKSIDE(tilex-1,tiley);
                }
                tilex--;
                break;

            case northwest:
                CHECKDIAG(tilex-1,tiley-1);
                CHECKDIAG(tilex-1,tiley);
                CHECKDIAG(tilex,tiley-1);
                tilex--;
                tiley--;
                break;

            case nodir:
                return false;

            default:
                throw Exception ("Walk: Bad dir");
        }
    }

#ifdef PLAYDEMOLIKEORIGINAL
    if (DEMOCOND_ORIG && doornum != -1)
    {
        Act1::OpenDoor(doornum);
        distance = -doornum-1;
        return true;
    }
#endif

   areanumber = mapSegs(0, tilex, tiley) - AREATILE;

    distance = TILEGLOBAL;
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


void objtype::SelectDodgeDir ()
{
    int         deltax,deltay,i;
    unsigned    absdx,absdy;
    dirtype     dirtry[5];
    dirtype     turnaround,tdir;

    if (flags & FL_FIRSTATTACK)
    {
        //
        // turning around is only ok the very first time after noticing the
        // player
        //
        turnaround = nodir;
        flags &= ~FL_FIRSTATTACK;
    }
    else
        turnaround=opposite[dir];

    deltax = player->tilex - tilex;
    deltay = player->tiley - tiley;

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

        dir = dirtry[i];
        if (TryWalk())
            return;
    }

    //
    // turn around only as a last resort
    //
    if (turnaround != nodir)
    {
        dir = turnaround;

        if (TryWalk())
            return;
    }

    dir = nodir;
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


void objtype::SelectChaseDir ()
{
    int     deltax,deltay;
    dirtype d[3];
    dirtype tdir, olddir, turnaround;


    olddir=dir;
    turnaround=opposite[olddir];

    deltax=player->tilex - tilex;
    deltay=player->tiley - tiley;

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
        dir=d[1];
        if (TryWalk())
            return;     /*either moved forward or attacked*/
    }

    if (d[2]!=nodir)
    {
        dir=d[2];
        if (TryWalk())
            return;
    }

    /* there is no direct path to the player, so pick another direction */

    if (olddir!=nodir)
    {
        dir=olddir;
        if (TryWalk())
            return;
    }

    if (wolfRnd()>128)      /*randomly determine direction of search*/
    {
        for (tdir=north; tdir<=west; tdir=(dirtype)(tdir+1))
        {
            if (tdir!=turnaround)
            {
                dir=tdir;
                if ( TryWalk() )
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
                dir=tdir;
                if ( TryWalk() )
                    return;
            }
        }
    }

    if (turnaround !=  nodir)
    {
        dir=turnaround;
        if (dir != nodir)
        {
            if ( TryWalk() )
                return;
        }
    }

    dir = nodir;                // can't move
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
    if (ob->TryWalk())
        return;     /*either moved forward or attacked*/

    ob->dir=d[2];
    if (ob->TryWalk())
        return;

    /* there is no direct path to the player, so pick another direction */

    if (wolfRnd()>128)      /*randomly determine direction of search*/
    {
        for (tdir=north; tdir<=west; tdir=(dirtype)(tdir+1))
        {
            ob->dir=tdir;
            if ( ob->TryWalk() )
                return;
        }
    }
    else
    {
        for (tdir=west; tdir>=north; tdir=(dirtype)(tdir-1))
        {
            ob->dir=tdir;
            if ( ob->TryWalk() )
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


void objtype::MoveObj (int32_t move)
{
    int32_t    deltax,deltay;

    switch (dir)
    {
        case north:
            y -= move;
            break;
        case northeast:
            x += move;
            y -= move;
            break;
        case east:
            x += move;
            break;
        case southeast:
            x += move;
            y += move;
            break;
        case south:
            y += move;
            break;
        case southwest:
            x -= move;
            y += move;
            break;
        case west:
            x -= move;
            break;
        case northwest:
            x -= move;
            y -= move;
            break;

        case nodir:
            return;

        default:
            throw Exception ("MoveObj: bad dir!");
    }

    //
    // check to make sure it's not on top of player
    //
    if (areabyplayer[areanumber])
    {
        deltax = x - player->x;
        if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
            goto moveok;
        deltay = y - player->y;
        if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
            goto moveok;

        if (hidden)          // move closer until he meets CheckLine
            goto moveok;

        if (obclass == ghostobj || obclass == spectreobj)
            TakeDamage (tics*2,this);

        //
        // back up
        //
        switch (dir)
        {
            case north:
                y += move;
                break;
            case northeast:
                x -= move;
                y += move;
                break;
            case east:
                x -= move;
                break;
            case southeast:
                x -= move;
                y -= move;
                break;
            case south:
                y -= move;
                break;
            case southwest:
                x += move;
                y -= move;
                break;
            case west:
                x += move;
                break;
            case northwest:
                x += move;
                y += move;
                break;

            case nodir:
                return;
        }
        return;
    }
moveok:
    distance -=move;
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


void objtype::KillActor ()
{
    this->tilex = (word)(this->x >> TILESHIFT);         // drop item on center
    this->tiley = (word)(this->y >> TILESHIFT);

    switch (obclass)
    {
        case guardobj:
            GivePoints (I_GUARDSCORE);	// IOANCH 25.10.2012: all constants started with I_
            NewState (&s_grddie1);
            PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case officerobj:
            GivePoints (I_OFFICERSCORE);
            NewState (&s_ofcdie1);
            PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case mutantobj:
            GivePoints (I_MUTANTSCORE);
            NewState (&s_mutdie1);
            PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case ssobj:
            GivePoints (I_SSSCORE);
            NewState (&s_ssdie1);
            if (gamestate.bestweapon < wp_machinegun)
                PlaceItemType (bo_machinegun,tilex,tiley);
            else
                PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case dogobj:
            GivePoints (I_DOGSCORE);
            NewState (&s_dogdie1);
            break;
            // IOANCH 20130303: unification
        case bossobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_bossdie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case gretelobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_greteldie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case giftobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (&s_giftdie1);
            break;

        case fatobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (&s_fatdie1);
            break;

        case schabbobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (&s_schabbdie1);
            break;
        case fakeobj:
            GivePoints (I_FAKEHITLERSCORE);
            NewState (&s_fakedie1);
            break;

        case mechahitlerobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_mechadie1);
            break;
        case realhitlerobj:
            GivePoints (I_BOSSSCORE);
            gamestate.killx = player->x;
            gamestate.killy = player->y;
            NewState (&s_hitlerdie1);
            break;
        case spectreobj:
            if (flags&FL_BONUS)
            {
                GivePoints (I_SPECTRESCORE);       // Get points once for each
                flags &= ~FL_BONUS;
            }
            NewState (&s_spectredie1);
            break;

        case angelobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_angeldie1);
            break;

        case transobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_transdie0);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case uberobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_uberdie0);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case willobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_willdie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;

        case deathobj:
            GivePoints (I_BOSSSCORE);
            NewState (&s_deathdie1);
            PlaceItemType (bo_key1,tilex,tiley);
            break;
		default:
			;
    }

    gamestate.killcount++;
	// IOANCH 29.06.2012: remove object from living set
	Basic::livingNazis.remove(this);
	
    flags &= ~FL_SHOOTABLE;
    actorat[this->tilex][this->tiley] = NULL;
    flags |= FL_NONMARK;
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


void objtype::DamageActor (unsigned damage)
{
    madenoise = true;

    //
    // do double damage if shooting a non attack mode actor
    //
    if ( !(flags & FL_ATTACKMODE) )
        damage <<= 1;

    hitpoints -= (short)damage;

    if (hitpoints<=0)
        KillActor ();
    else
    {
        if (! (flags & FL_ATTACKMODE) )
            FirstSighting ();             // put into combat mode
        
        // IOANCH 20130305: wrap them
        statetype *pain = atr::states[obclass].pain,
               *altpain = atr::states[obclass].altpain;
        
        if(pain && hitpoints & 1)
            NewState(pain);
        else if(altpain)
            NewState(altpain);

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


Boolean8 objtype::CheckLine (Boolean8 solidActors)
{
    // IOANCH 20130305: DRY
    return Basic::GenericCheckLine(x, y, player->x, player->y, solidActors);
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

Boolean8 objtype::CheckSight ()
{
    int32_t deltax,deltay;

    //
    // don't bother tracing a line if the area isn't connected to the player's
    //
    if (!areabyplayer[areanumber])
        return false;

    //
    // if the player is real close, sight is automatic
    //
    deltax = player->x - x;
    deltay = player->y - y;

    if (deltax > -MINSIGHT && deltax < MINSIGHT
        && deltay > -MINSIGHT && deltay < MINSIGHT)
        return true;

    //
    // see if they are looking in the right direction
    //
    switch (dir)
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

    return CheckLine ();
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


void objtype::FirstSighting ()
{
    //
    // react to the player
    //
    
    // IOANCH 20130305: wrap them to objattribs
    classtype &cls = obclass;
    if(atr::sounds[cls].sight >= 0)
    {
        if(atr::flags[cls] & ATR_BOSS_SOUNDS)
            Sound::Play((soundnames)atr::sounds[cls].sight);
        else
            PlaySoundLocActor(atr::sounds[cls].sight, this, this);
    }
    if(atr::states[cls].chase)
        NewState(atr::states[cls].chase);
    if(atr::speeds[cls].chase >= 0)
        speed = atr::speeds[cls].chase;

    if (distance < 0)
        distance = 0;       // ignore the door opening command

    flags |= FL_ATTACKMODE|FL_FIRSTATTACK;
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


Boolean8 objtype::SightPlayer ()
{
    if (flags & FL_ATTACKMODE)
        throw Exception ("An actor in ATTACKMODE called SightPlayer!");

    if (temp2)
    {
        //
        // count down reaction time
        //
        temp2 -= (short) tics;
        if (temp2 > 0)
            return false;
        temp2 = 0;                                  // time to react
    }
    else
    {
        if (!areabyplayer[areanumber])
            return false;

        if (flags & FL_AMBUSH)
        {
            if (!CheckSight ())
                return false;
            flags &= ~FL_AMBUSH;
        }
        else
        {
            if (!madenoise && !CheckSight ())
                return false;
        }


        switch (obclass)
        {
            case guardobj:
                temp2 = 1+wolfRnd()/4;
                break;
            case officerobj:
                temp2 = 2;
                break;
            case mutantobj:
                temp2 = 1+wolfRnd()/6;
                break;
            case ssobj:
                temp2 = 1+wolfRnd()/6;
                break;
            case dogobj:
                temp2 = 1+wolfRnd()/8;
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
                temp2 = 1;
                break;
			default:
				;
        }
        return false;
    }

    FirstSighting ();

    return true;
}
