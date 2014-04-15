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
// Agent Blazkowicz routines
//
////////////////////////////////////////////////////////////////////////////////

// WL_AGENT.C

#include "wl_def.h"
#include "version.h"
#include "wl_act1.h"
#include "wl_act2.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_play.h"
#include "wl_state.h"
#include "id_ca.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ioan_bas.h"
#include "ioan_bot.h"	// IOANCH
#include "HistoryRatio.h"
#include "Config.h"
#include "obattrib.h"

/*
=============================================================================

                                LOCAL CONSTANTS

=============================================================================
*/

#define MAXMOUSETURN    10


#define MOVESCALE       150l
#define BACKMOVESCALE   100l
#define ANGLESCALE      20

/*
=============================================================================

                                GLOBAL VARIABLES

=============================================================================
*/



//
// player state info
//
int32_t         thrustspeed;

short           anglefrac;

objtype        *LastAttacker;

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/


void    T_Player (objtype *ob);
void    T_Attack (objtype *ob);

statetype   s_player = {false,0,0,(statefunc) T_Player,NULL,NULL};
statetype   s_attack = {false,0,0,(statefunc) T_Attack,NULL,NULL};

struct atkinf
{
    int8_t    tics,attack,frame;              // attack is 1 for gun, 2 for knife
};

atkinf attackinfo[4][14] =
{
    { {6,0,1},{6,2,2},{6,0,3},{6,-1,4} },
    { {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },
    { {6,0,1},{6,1,2},{6,3,3},{6,-1,4} },
    { {6,0,1},{6,1,2},{6,4,3},{6,-1,4} },
};

//===========================================================================

Boolean8 TryMove (objtype *ob);
void T_Player (objtype *ob);

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove);

/*
=============================================================================

                                CONTROL STUFF

=============================================================================
*/

/*
======================
=
= CheckWeaponChange
=
= Keys 1-4 change weapons
=
======================
*/

void CheckWeaponChange ()
{
    int newWeapon = -1;

    if (!gamestate.ammo)            // must use knife with no ammo
        return;

#ifdef _arch_dreamcast
    int joyx, joyy;
    myInput.getJoyFineDelta (&joyx, &joyy);
    if(joyx < -64)
        buttonstate[bt_prevweapon] = true;
    else if(joyx > 64)
        buttonstate[bt_nextweapon] = true;
#endif

    if(buttonstate[bt_nextweapon] && !buttonheld[bt_nextweapon])
    {
        newWeapon = gamestate.weapon + 1;
        if(newWeapon > gamestate.bestweapon) newWeapon = 0;
    }
    else if(buttonstate[bt_prevweapon] && !buttonheld[bt_prevweapon])
    {
        newWeapon = gamestate.weapon - 1;
        if(newWeapon < 0) newWeapon = gamestate.bestweapon;
    }
    else
    {
        for(int i = wp_knife; i <= gamestate.bestweapon; i++)
        {
            if (buttonstate[bt_readyknife + i - wp_knife])
            {
                newWeapon = i;
                break;
            }
        }
    }

    if(newWeapon != -1)
    {
        gamestate.weapon = gamestate.chosenweapon = (weapontype) newWeapon;
        DrawWeapon();
    }
}


/*
=======================
=
= ControlMovement
=
= Takes controlx,controly, and buttonstate[bt_strafe]
=
= Changes the player's angle and position
=
= There is an angle hack because when going 70 fps, the roundoff becomes
= significant
=
=======================
*/

void ControlMovement (objtype *ob)
{
    int     angle;
    int     angleunits;

    thrustspeed = 0;

    if(buttonstate[bt_strafeleft])
    {
        angle = ob->angle + ANGLES/4;
        if(angle >= ANGLES)
            angle -= ANGLES;
        if(buttonstate[bt_run])
            Thrust(angle, RUNMOVE * MOVESCALE * tics);
        else
            Thrust(angle, BASEMOVE * MOVESCALE * tics);
    }

    if(buttonstate[bt_straferight])
    {
        angle = ob->angle - ANGLES/4;
        if(angle < 0)
            angle += ANGLES;
        if(buttonstate[bt_run])
            Thrust(angle, RUNMOVE * MOVESCALE * tics );
        else
            Thrust(angle, BASEMOVE * MOVESCALE * tics);
    }

    //
    // side to side move
    //
    if (buttonstate[bt_strafe])
    {
        //
        // strafing
        //
        //
        if (controlx > 0)
        {
            angle = ob->angle - ANGLES/4;
            if (angle < 0)
                angle += ANGLES;
            Thrust (angle,controlx*MOVESCALE);      // move to left
        }
        else if (controlx < 0)
        {
            angle = ob->angle + ANGLES/4;
            if (angle >= ANGLES)
                angle -= ANGLES;
            Thrust (angle,-controlx*MOVESCALE);     // move to right
        }
    }
    else
    {
        //
        // not strafing
        //
        anglefrac += controlx;
        angleunits = anglefrac/ANGLESCALE;
        anglefrac -= angleunits*ANGLESCALE;
        ob->angle -= angleunits;

        if (ob->angle >= ANGLES)
            ob->angle -= ANGLES;
        if (ob->angle < 0)
            ob->angle += ANGLES;

    }

    //
    // forward/backwards move
    //
    if (controly < 0)
    {
        Thrust (ob->angle,-controly*MOVESCALE); // move forwards
    }
    else if (controly > 0)
    {
        angle = ob->angle + ANGLES/2;
        if (angle >= ANGLES)
            angle -= ANGLES;
        Thrust (angle,controly*BACKMOVESCALE);          // move backwards
    }

    if (gamestate.victoryflag)              // watching the BJ actor
        return;
}

/*
=============================================================================

                            STATUS WINDOW STUFF

=============================================================================
*/


/*
==================
=
= StatusDrawPic
=
==================
*/

void StatusDrawPic (unsigned x, unsigned y, unsigned picnum)
{
    LatchDrawPicScaledCoord ((cfg_screenWidth-vid_scaleFactor*LOGIC_WIDTH)/16 + vid_scaleFactor*x,
                             cfg_screenHeight-vid_scaleFactor*(STATUSLINES-y),picnum);
}

void StatusDrawFace(unsigned picnum)
{
    StatusDrawPic(17, 4, picnum);

#ifdef _arch_dreamcast
    DC_StatusDrawLCD(picnum);
#endif
}


/*
==================
=
= DrawFace
=
==================
*/

void DrawFace ()
{
    if(viewsize == 21 && ingame) return;
    if (SD_SoundPlaying() == SPEAR::sd(GETGATLINGSND))
        StatusDrawFace(SPEAR::g(GOTGATLINGPIC));
    else if (gamestate.health)
    {
        // IOANCH 20130202: unification process
        if (SPEAR::flag && godmode)
            StatusDrawFace(SPEAR::g(GODMODEFACE1PIC)+gamestate.faceframe);
        else
            StatusDrawFace(SPEAR::g(FACE1APIC)+3*((I_PLAYERHEALTH-gamestate.health)/16)+gamestate.faceframe);
    }	// IOANCH 25.10.2012: named constant
    else
    {
        // IOANCH 20130202: unification process
        if (LastAttacker && LastAttacker->obclass == needleobj)
            StatusDrawFace(SPEAR::g(MUTANTBJPIC));
        else
            StatusDrawFace(SPEAR::g(FACE8APIC));
    }
}

/*
===============
=
= UpdateFace
=
= Calls draw face if time to change
=
===============
*/

int facecount = 0;
int facetimes = 0;

void UpdateFace ()
{
    // don't make demo depend on sound playback
    if(demoplayback || demorecord)
    {
        if(facetimes > 0)
        {
            facetimes--;
            return;
        }
    }
    else if(SD_SoundPlaying() == SPEAR::sd(GETGATLINGSND))
        return;

    facecount += tics;
    if (facecount > wolfRnd())
    {
        gamestate.faceframe = (wolfRnd()>>6);
        if (gamestate.faceframe==3)
            gamestate.faceframe = 1;

        facecount = 0;
        DrawFace ();
    }
}



/*
===============
=
= LatchNumber
=
= right justifies and pads with blanks
=
===============
*/

static void LatchNumber (int x, int y, unsigned width, int32_t number)
{
    unsigned length,c;
    char    str[20];

    ltoanoreturn ((long int)number,str,10);

    length = (unsigned) strlen (str);

    while (length<width)
    {
        StatusDrawPic (x,y,SPEAR::g(N_BLANKPIC));
        x++;
        width--;
    }

    c = length <= width ? 0 : length-width;

    while (c<length)
    {
        StatusDrawPic (x,y,str[c]-'0'+ SPEAR::g(N_0PIC));
        x++;
        c++;
    }
}


/*
===============
=
= DrawHealth
=
===============
*/

void DrawHealth ()
{
    if(viewsize == 21 && ingame) return;
    LatchNumber (21,16,3,gamestate.health);
}


/*
===============
=
= TakeDamage
=
===============
*/

void TakeDamage (int points,objtype *attacker)
{
    LastAttacker = attacker;

    if (gamestate.victoryflag)
        return;
    if (gamestate.difficulty==gd_baby)
        points>>=2;
	
	// IOANCH 02.07.2012: alert the bot that damage was taken
	bot.damagetaken = attacker;

    if (!godmode)
        gamestate.health -= points;

    if (gamestate.health<=0)
    {
        gamestate.health = 0;
        playstate = ex_died;
        killerobj = attacker;
    }

    if (godmode != 2)
        StartDamageFlash (points);

    DrawHealth ();
    DrawFace ();

    //
    // MAKE BJ'S EYES BUG IF MAJOR DAMAGE!
    //
    // IOANCH 20130202: unification process
    if (SPEAR::flag && points > 30 && gamestate.health!=0 && !godmode && viewsize != 21)
    {
        // IOANCH 20130302: unification
        StatusDrawFace(SPEAR::g(BJOUCHPIC));
        facecount = 0;
    }
}

/*
===============
=
= HealSelf
=
===============
*/

void HealSelf (int points)
{
    gamestate.health += points;
    if (gamestate.health>I_PLAYERHEALTH)
        gamestate.health = I_PLAYERHEALTH;	// IOANCH 25.10.2012: named constants

    DrawHealth ();
    DrawFace ();
}


//===========================================================================


/*
===============
=
= DrawLevel
=
===============
*/

void DrawLevel ()
{
    if(viewsize == 21 && ingame) return;
    // IOANCH 20130202: unification process
    if (SPEAR::flag && gamestate.mapon == 20)
        LatchNumber (2,16,2,18);
    else
        LatchNumber (2,16,2,gamestate.mapon+1);
}

//===========================================================================


/*
===============
=
= DrawLives
=
===============
*/

void DrawLives ()
{
    if(viewsize == 21 && ingame) return;
    LatchNumber (14,16,1,gamestate.lives);
}


/*
===============
=
= GiveExtraMan
=
===============
*/

void GiveExtraMan ()
{
    if (gamestate.lives<9)
        gamestate.lives++;
    DrawLives ();
    Sound::Play (BONUS1UPSND);
}

//===========================================================================

/*
===============
=
= DrawScore
=
===============
*/

void DrawScore ()
{
    if(viewsize == 21 && ingame) return;
    LatchNumber (6,16,6,gamestate.score);
}

/*
===============
=
= GivePoints
=
===============
*/

void GivePoints (int32_t points)
{
    gamestate.score += points;
    while (gamestate.score >= gamestate.nextextra)
    {
        gamestate.nextextra += EXTRAPOINTS;
        GiveExtraMan ();
    }
    DrawScore ();
}

//===========================================================================

/*
==================
=
= DrawWeapon
=
==================
*/

void DrawWeapon ()
{
    if(viewsize == 21 && ingame) return;
    StatusDrawPic (32,8,SPEAR::g(KNIFEPIC)+gamestate.weapon);
}


/*
==================
=
= DrawKeys
=
==================
*/

void DrawKeys ()
{
    if(viewsize == 21 && ingame) return;
    if (gamestate.keys & 1)
        StatusDrawPic (30,4,SPEAR::g(GOLDKEYPIC));
    else
        StatusDrawPic (30,4,SPEAR::g(NOKEYPIC));

    if (gamestate.keys & 2)
        StatusDrawPic (30,20,SPEAR::g(SILVERKEYPIC));
    else
        StatusDrawPic (30,20,SPEAR::g(NOKEYPIC));
}

/*
==================
=
= GiveWeapon
=
==================
*/

void GiveWeapon (int weapon)
{
    GiveAmmo (I_GUNAMMO);	// IOANCH 25.10.2012: named constants

    if (gamestate.bestweapon<weapon)
        gamestate.bestweapon = gamestate.weapon
        = gamestate.chosenweapon = (weapontype) weapon;

    DrawWeapon ();
}

//===========================================================================

/*
===============
=
= DrawAmmo
=
===============
*/

void DrawAmmo ()
{
    if(viewsize == 21 && ingame) return;
    LatchNumber (27,16,2,gamestate.ammo);
}

/*
===============
=
= GiveAmmo
=
===============
*/

void GiveAmmo (int ammo)
{
    if (!gamestate.ammo)                            // knife was out
    {
        if (!gamestate.attackframe)
        {
            gamestate.weapon = gamestate.chosenweapon;
            DrawWeapon ();
        }
    }
    gamestate.ammo += ammo;
    if (gamestate.ammo > I_MAXAMMO)	// IOANCH 25.10.2012: named constants
        gamestate.ammo = I_MAXAMMO;
    DrawAmmo ();
}

//===========================================================================

/*
==================
=
= GiveKey
=
==================
*/

void GiveKey (int key)
{
    gamestate.keys |= (1<<key);
    DrawKeys ();
}



////////////////////////////////////////////////////////////////////////////////
//
//                                MOVEMENT
//
////////////////////////////////////////////////////////////////////////////////

//
// =
// = GetBonus
// =
//

void GetBonus (statobj_t *check)
{
    switch (check->itemnumber)
    {
        case    bo_firstaid:
            if (gamestate.health == I_PLAYERHEALTH)	// IOANCH 25.10.2012: named constant
                return;

            Sound::Play (HEALTH2SND);
            HealSelf (I_FIRSTAIDHEALTH);	// IOANCH
            break;

        case    bo_key1:
        case    bo_key2:
        case    bo_key3:
        case    bo_key4:
            GiveKey (check->itemnumber - bo_key1);
            Sound::Play (GETKEYSND);
            break;

            // IOANCH 20130601: use centralized atr:: attrib
        case    bo_cross:
        case    bo_chalice:
        case    bo_bible:
        case    bo_crown:
            Sound::Play(atr::treasures[check->itemnumber - bo_cross].
                         pickupsound);
            GivePoints(atr::treasures[check->itemnumber - bo_cross].points);
            gamestate.treasurecount++;
            break;

        case    bo_clip:
            if (gamestate.ammo == I_MAXAMMO)	// IOANCH
                return;

            Sound::Play (GETAMMOSND);
            GiveAmmo (I_CLIPAMMO);	// IOANCH
            break;
        case    bo_clip2:
            if (gamestate.ammo == I_MAXAMMO)	// IOANCH
                return;

            Sound::Play (GETAMMOSND);
            GiveAmmo (I_SEMICLIPAMMO);	// IOANCH
            break;

            // IOANCH 20130202: unification process
        case    bo_25clip:
            if (gamestate.ammo == I_MAXAMMO)	// IOANCH
                return;

            Sound::Play (GETAMMOBOXSND);
            GiveAmmo (I_BOXAMMO);	// IOANCH
            break;

        case    bo_machinegun:
            Sound::Play (GETMACHINESND);
            GiveWeapon (wp_machinegun);
            break;
        case    bo_chaingun:
            Sound::Play (GETGATLINGSND);
            facetimes = 38;
            GiveWeapon (wp_chaingun);

            if(viewsize != 21)
                StatusDrawFace (SPEAR::g(GOTGATLINGPIC));
            facecount = 0;
            break;

        case    bo_fullheal:
            Sound::Play (BONUS1UPSND);
            HealSelf (I_PLAYERHEALTH - 1);	// IOANCH
            GiveAmmo (I_BONUSAMMO);	// IOANCH
            GiveExtraMan ();
            gamestate.treasurecount++;
            break;

        case    bo_food:
            if (gamestate.health == I_PLAYERHEALTH)	// IOANCH
                return;

            Sound::Play (HEALTH1SND);
            HealSelf (I_FOODHEALTH);	// IOANCH
            break;

        case    bo_alpo:
            if (gamestate.health == I_PLAYERHEALTH)	// IOANCH
                return;

            Sound::Play (HEALTH1SND);
            HealSelf (I_DOGFOODHEALTH);	// IOANCH
            break;

        case    bo_gibs:
            if (gamestate.health > I_BLOODHEALTHTHRESHOLD)	// IOANCH
                return;

            Sound::Play (SLURPIESND);
            HealSelf (1);
            break;

            // IOANCH 20130202: unification process
        case    bo_spear:
            spearflag = true;
            spearx = player->x;
            speary = player->y;
            spearangle = player->angle;
            playstate = ex_completed;
    }

    StartBonusFlash ();
    check->shapenum = -1;                   // remove from list
	
	// IOANCH 26.10.2012: remove item from list
	Basic::RemoveItemFromList(check->tilex, check->tiley, check->itemnumber);
}

/*
===================
=
= TryMove
=
= returns true if move ok
= debug: use pointers to optimize
===================
*/

Boolean8 TryMove (objtype *ob)
{
    int         xl,yl,xh,yh,x,y;
    objtype    *check;
    int32_t     deltax,deltay;

    xl = (ob->x-PLAYERSIZE) >>TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >>TILESHIFT;

    xh = (ob->x+PLAYERSIZE) >>TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >>TILESHIFT;

#define PUSHWALLMINDIST PLAYERSIZE

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
            {
                if(tilemap[x][y]==64 && x==pwallx && y==pwally)   // back of moving pushwall?
                {
                    switch(pwalldir)
                    {
                        case di_north:
                            if(ob->y-PUSHWALLMINDIST<=(pwally<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_west:
                            if(ob->x-PUSHWALLMINDIST<=(pwallx<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_east:
                            if(ob->x+PUSHWALLMINDIST>=(pwallx<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                        case di_south:
                            if(ob->y+PUSHWALLMINDIST>=(pwally<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                    }
                }
                else return false;
            }
        }
    }

    //
    // check for actors
    //
    if (yl>0)
        yl--;
    if (yh<MAPSIZE-1)
        yh++;
    if (xl>0)
        xl--;
    if (xh<MAPSIZE-1)
        xh++;

    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (ISPOINTER(check) && check != player && (check->flags & FL_SHOOTABLE) )
            {
                deltax = ob->x - check->x;
                if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
                    continue;
                deltay = ob->y - check->y;
                if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
                    continue;

                return false;
            }
        }
    }

    return true;
}


/*
===================
=
= ClipMove
=
===================
*/

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove)
{
    int32_t    basex,basey;

    basex = ob->x;
    basey = ob->y;

    ob->x = basex+xmove;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

#ifndef REMDEBUG
    if (noclip && ob->x > 2*TILEGLOBAL && ob->y > 2*TILEGLOBAL
        && ob->x < (((int32_t)(mapwidth-1))<<TILESHIFT)
        && ob->y < (((int32_t)(mapheight-1))<<TILESHIFT) )
        return;         // walk through walls
#endif

    if (!SD_SoundPlaying())
        Sound::Play (HITWALLSND);

    ob->x = basex+xmove;
    ob->y = basey;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey;
}

//==========================================================================

/*
===================
=
= VictoryTile
=
===================
*/

void VictoryTile ()
{
    // IOANCH 20130202: unification process
    if(!SPEAR::flag)
        SpawnBJVictory ();

    gamestate.victoryflag = true;
}

/*
===================
=
= Thrust
=
===================
*/

// For player movement in demos exactly as in the original Wolf3D v1.4 source code
static fixed FixedByFracOrig(fixed a, fixed b)
{
    int sign = 0;
    if(b == 65536) b = 65535;
    else if(b == -65536) b = 65535, sign = 1;
    else if(b < 0) b = (-b), sign = 1;

    if(a < 0)
    {
        a = -a;
        sign = !sign;
    }
    fixed res = (fixed)(((int64_t) a * b) >> 16);
    if(sign)
        res = -res;
    return res;
}

void Thrust (int angle, int32_t speed)
{
    int32_t xmove,ymove;
    unsigned offset;


    //
    // ZERO FUNNY COUNTER IF MOVED!
    //
    // IOANCH 20130202: unification process
    if (SPEAR::flag && speed)
        funnyticount = 0;

    thrustspeed += speed;
    //
    // moving bounds speed
    //
    if (speed >= MINDIST*2)
        speed = MINDIST*2-1;

    xmove = DEMOCHOOSE_ORIG_SDL(
                FixedByFracOrig(speed, costable[angle]),
                FixedMul(speed,costable[angle]));
    ymove = DEMOCHOOSE_ORIG_SDL(
                -FixedByFracOrig(speed, sintable[angle]),
                -FixedMul(speed,sintable[angle]));

    ClipMove(player,xmove,ymove);

    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    offset = (player->tiley<<mapshift)+player->tilex;
    player->areanumber = *(mapSegs[0] + offset) -AREATILE;

    if (*(mapSegs[1] + offset) == EXITTILE)
        VictoryTile ();
}


/*
=============================================================================

                                ACTIONS

=============================================================================
*/


/*
===============
=
= Cmd_Fire
=
===============
*/

void Cmd_Fire ()
{
    buttonheld[bt_attack] = true;

    gamestate.weaponframe = 0;

    player->state = &s_attack;

    gamestate.attackframe = 0;
    gamestate.attackcount =
        attackinfo[gamestate.weapon][gamestate.attackframe].tics;
    gamestate.weaponframe =
        attackinfo[gamestate.weapon][gamestate.attackframe].frame;
}

//===========================================================================

/*
===============
=
= Cmd_Use
=
===============
*/

void Cmd_Use ()
{
    int     checkx,checky,doornum,dir;
    Boolean8 elevatorok;

    //
    // find which cardinal direction the player is facing
    //
    if (player->angle < ANGLES/8 || player->angle > 7*ANGLES/8)
    {
        checkx = player->tilex + 1;
        checky = player->tiley;
        dir = di_east;
        elevatorok = true;
    }
    else if (player->angle < 3*ANGLES/8)
    {
        checkx = player->tilex;
        checky = player->tiley-1;
        dir = di_north;
        elevatorok = false;
    }
    else if (player->angle < 5*ANGLES/8)
    {
        checkx = player->tilex - 1;
        checky = player->tiley;
        dir = di_west;
        elevatorok = true;
    }
    else
    {
        checkx = player->tilex;
        checky = player->tiley + 1;
        dir = di_south;
        elevatorok = false;
    }

    doornum = tilemap[checkx][checky];
    if (mapSegs(1, checkx, checky) == PUSHABLETILE)
    {
        //
        // pushable wall
        //

        PushWall (checkx,checky,dir);
        return;
    }
    if (!buttonheld[bt_use] && doornum == ELEVATORTILE && elevatorok)
    {
        //
        // use elevator
        //
        buttonheld[bt_use] = true;

        tilemap[checkx][checky]++;              // flip switch
        if (mapSegs(0, player->tilex, player->tiley) == ALTELEVATORTILE)
            playstate = ex_secretlevel;
        else
            playstate = ex_completed;
        Sound::Play (LEVELDONESND);
        SD_WaitSoundDone();
    }
    else if (!buttonheld[bt_use] && doornum & 0x80)
    {
        buttonheld[bt_use] = true;
        OperateDoor (doornum & ~0x80);
    }
    else
        Sound::Play (DONOTHINGSND);
}

/*
=============================================================================

                                PLAYER CONTROL

=============================================================================
*/



/*
===============
=
= SpawnPlayer
=
===============
*/

void SpawnPlayer (int tilex, int tiley, int dir)
{
    player->obclass = playerobj;
    player->active = ac_yes;
    player->tilex = tilex;
    player->tiley = tiley;
   player->areanumber = (byte) mapSegs(0, player->tilex, player->tiley);
    player->x = ((int32_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
    player->y = ((int32_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
    player->state = &s_player;
    player->angle = (1-dir)*90;
    if (player->angle<0)
        player->angle += ANGLES;
    player->flags = FL_NEVERMARK;
    Thrust (0,0);                           // set some variables

    InitAreas ();
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = T_KnifeAttack
// =
// = Update player hands, and try to do damage when the proper frame is reached
// =
//
////////////////////////////////////////////////////////////////////////////////


// IOANCH: modified to use another function
void    KnifeAttack (objtype *ob)
{
    objtype *closest;
    
    Sound::Play (ATKKNIFESND);

    closest = Basic::CheckKnifeEnemy();
    
    // hit something
    if(closest)
        closest->DamageActor (wolfRnd() >> 4);
}



void    GunAttack (objtype *ob)
{
    objtype *check,*closest,*oldclosest;
    int      damage;
    int      dx,dy,dist;
    int32_t  viewdist;

    switch (gamestate.weapon)
    {
        case wp_pistol:
            Sound::Play (ATKPISTOLSND);
            break;
        case wp_machinegun:
            Sound::Play (ATKMACHINEGUNSND);
            break;
        case wp_chaingun:
            Sound::Play (ATKGATLINGSND);
            break;
		default:
			;
    }

    madenoise = true;

    //
    // find potential targets
    //
    viewdist = 0x7fffffffl;
    closest = NULL;

    while (1)
    {
        oldclosest = closest;

        for (check=ob->next ; check ; check=check->next)
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
		{
			// IOANCH 26.06.2012: efficiency
			bot.shootRatio.addFail();
            return;                                         // no more targets, all missed
		}

        //
        // trace a line from player to enemey
        //

        if (closest->CheckLine())
            break;
    }

    //
    // hit something
    //
    dx = ABS(closest->tilex - player->tilex);
    dy = ABS(closest->tiley - player->tiley);
    dist = dx>dy ? dx:dy;
    if (dist<2)
        damage = wolfRnd() / 4;
    else if (dist<4)
        damage = wolfRnd() / 6;
    else
    {
        if ( (wolfRnd() / 12) < dist)           // missed
		{
			// IOANCH 26.06.2012: efficiency
			bot.shootRatio.addFail();
            return;
		}
        damage = wolfRnd() / 6;
    }
	// IOANCH 26.06.2012: efficiency
	bot.shootRatio.addSuccess();
    closest->DamageActor (damage);
}

//===========================================================================

/*
===============
=
= VictorySpin
=
===============
*/

void VictorySpin ()
{
    int32_t    desty;

    if (player->angle > 270)
    {
        player->angle -= (short)(tics * 3);
        if (player->angle < 270)
            player->angle = 270;
    }
    else if (player->angle < 270)
    {
        player->angle += (short)(tics * 3);
        if (player->angle > 270)
            player->angle = 270;
    }

    desty = (((int32_t)player->tiley-5)<<TILESHIFT)-0x3000;

    if (player->y > desty)
    {
        player->y -= tics*4096;
        if (player->y < desty)
            player->y = desty;
    }
}


//===========================================================================

/*
===============
=
= T_Attack
=
===============
*/

void    T_Attack (objtype *ob)
{
    struct  atkinf  *cur;

    UpdateFace ();

    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }

    if ( buttonstate[bt_use] && !buttonheld[bt_use] )
        buttonstate[bt_use] = false;

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack])
        buttonstate[bt_attack] = false;

    ControlMovement (ob);
    if (gamestate.victoryflag)              // watching the BJ actor
        return;
    
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    //
    // change frame and fire
    //
    gamestate.attackcount -= (short) tics;
    while (gamestate.attackcount <= 0)
    {
        cur = &attackinfo[gamestate.weapon][gamestate.attackframe];
        switch (cur->attack)
        {
            case -1:
                ob->state = &s_player;
                if (!gamestate.ammo)
                {
                    gamestate.weapon = wp_knife;
                    DrawWeapon ();
                }
                else
                {
                    if (gamestate.weapon != gamestate.chosenweapon)
                    {
                        gamestate.weapon = gamestate.chosenweapon;
                        DrawWeapon ();
                    }
                }
                gamestate.attackframe = gamestate.weaponframe = 0;
                return;

            case 4:
                if (!gamestate.ammo)
                    break;
                if (buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
            case 1:
                if (!gamestate.ammo)
                {       // can only happen with chain gun
                    gamestate.attackframe++;
                    break;
                }
                GunAttack (ob);
                if (!ammocheat)
                    gamestate.ammo--;
                DrawAmmo ();
                break;

            case 2:
                KnifeAttack (ob);
                break;

            case 3:
                if (gamestate.ammo && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
                break;
        }

        gamestate.attackcount += cur->tics;
        gamestate.attackframe++;
        gamestate.weaponframe =
            attackinfo[gamestate.weapon][gamestate.attackframe].frame;
    }
}



//===========================================================================

/*
===============
=
= T_Player
=
===============
*/

void    T_Player (objtype *ob)
{
    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }

    UpdateFace ();
    CheckWeaponChange ();

    if ( buttonstate[bt_use] )
        Cmd_Use ();

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack])
        Cmd_Fire ();

    ControlMovement (ob);
    if (gamestate.victoryflag)              // watching the BJ actor
        return;
    
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);
}
