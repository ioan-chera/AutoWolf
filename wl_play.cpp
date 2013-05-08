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

// WL_PLAY.C

#include "wl_def.h"
#include "wl_act1.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_inter.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_play.h"
#pragma hdrstop
#include "SODFlag.h"

#include "wl_cloudsky.h"
#include "wl_shade.h"
// IOANCH 17.05.2012
#include "ioan_bot.h"
#include "Exception.h"
#include "PString.h"

/*
=============================================================================

                                                 LOCAL CONSTANTS

=============================================================================
*/

#define sc_Question     0x35

/*
=============================================================================

                                                 GLOBAL VARIABLES

=============================================================================
*/

boolean madenoise;              // true when shooting or screaming

exit_t playstate;

static int lastmusicchunk = 0;

static int DebugOk;

objtype objlist[MAXACTORS];
objtype *newobj, *obj, *player, *lastobj, *objfreelist, *killerobj;

boolean noclip, ammocheat;
int godmode, singlestep, extravbls = 0;

byte tilemap[MAPSIZE][MAPSIZE]; // wall values only
byte spotvis[MAPSIZE][MAPSIZE];
objtype *actorat[MAPSIZE][MAPSIZE];

//
// replacing refresh manager
//
unsigned tics;

//
// control info
//
boolean mouseenabled, joystickenabled;
int dirscan[4] = { sc_UpArrow, sc_RightArrow, sc_DownArrow, sc_LeftArrow };
int buttonscan[NUMBUTTONS] = { sc_Control, sc_Alt, sc_LShift, sc_Space, sc_1, sc_2, sc_3, sc_4 };
int buttonmouse[4] = { bt_attack, bt_strafe, bt_use, bt_nobutton };
int buttonjoy[32] = {
#ifdef _arch_dreamcast
    bt_attack, bt_strafe, bt_use, bt_run, bt_esc, bt_prevweapon, bt_nobutton, bt_nextweapon,
    bt_pause, bt_strafeleft, bt_straferight, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
#else
    bt_attack, bt_strafe, bt_use, bt_run, bt_strafeleft, bt_straferight, bt_esc, bt_pause,
    bt_prevweapon, bt_nextweapon, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
#endif
    bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
    bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton
};

int viewsize;

boolean buttonheld[NUMBUTTONS];

boolean demorecord, demoplayback;
int8_t *demoptr, *lastdemoptr;
memptr demobuffer;

//
// current user input
//
int controlx, controly;         // range from -100 to 100 per tic
boolean buttonstate[NUMBUTTONS];

int lastgamemusicoffset = 0;


//===========================================================================


void CenterWindow (word w, word h);
void InitObjList (void);
void RemoveObj (objtype * gone);
void PollControls (void);
int StopMusic (void);
void StartMusic (void);
void ContinueMusic (int offs);
void PlayLoop (void);

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/


objtype dummyobj;

//
// LIST OF SONGS FOR EACH VERSION
//

// IOANCH 20130301: unification
int songs_wl6[] = {
    //
    // Episode One
    //
    GETTHEM_MUS_wl6,
    SEARCHN_MUS_wl6,
    POW_MUS_wl6,
    SUSPENSE_MUS_wl6,
    GETTHEM_MUS_wl6,
    SEARCHN_MUS_wl6,
    POW_MUS_wl6,
    SUSPENSE_MUS_wl6,
	
    WARMARCH_MUS_wl6,               // Boss level
    CORNER_MUS_wl6,                 // Secret level
	
    //
    // Episode Two
    //
    NAZI_OMI_MUS_wl6,
    PREGNANT_MUS_wl6,
    GOINGAFT_MUS_wl6,
    HEADACHE_MUS_wl6,
    NAZI_OMI_MUS_wl6,
    PREGNANT_MUS_wl6,
    HEADACHE_MUS_wl6,
    GOINGAFT_MUS_wl6,
	
    WARMARCH_MUS_wl6,               // Boss level
    DUNGEON_MUS_wl6,                // Secret level
	
    //
    // Episode Three
    //
    INTROCW3_MUS_wl6,
    NAZI_RAP_MUS_wl6,
    TWELFTH_MUS_wl6,
    ZEROHOUR_MUS_wl6,
    INTROCW3_MUS_wl6,
    NAZI_RAP_MUS_wl6,
    TWELFTH_MUS_wl6,
    ZEROHOUR_MUS_wl6,
	
    ULTIMATE_MUS_wl6,               // Boss level
    PACMAN_MUS_wl6,                 // Secret level
	
    //
    // Episode Four
    //
    GETTHEM_MUS_wl6,
    SEARCHN_MUS_wl6,
    POW_MUS_wl6,
    SUSPENSE_MUS_wl6,
    GETTHEM_MUS_wl6,
    SEARCHN_MUS_wl6,
    POW_MUS_wl6,
    SUSPENSE_MUS_wl6,
	
    WARMARCH_MUS_wl6,               // Boss level
    CORNER_MUS_wl6,                 // Secret level
	
    //
    // Episode Five
    //
    NAZI_OMI_MUS_wl6,
    PREGNANT_MUS_wl6,
    GOINGAFT_MUS_wl6,
    HEADACHE_MUS_wl6,
    NAZI_OMI_MUS_wl6,
    PREGNANT_MUS_wl6,
    HEADACHE_MUS_wl6,
    GOINGAFT_MUS_wl6,
	
    WARMARCH_MUS_wl6,               // Boss level
    DUNGEON_MUS_wl6,                // Secret level
	
    //
    // Episode Six
    //
    INTROCW3_MUS_wl6,
    NAZI_RAP_MUS_wl6,
    TWELFTH_MUS_wl6,
    ZEROHOUR_MUS_wl6,
    INTROCW3_MUS_wl6,
    NAZI_RAP_MUS_wl6,
    TWELFTH_MUS_wl6,
    ZEROHOUR_MUS_wl6,
	
    ULTIMATE_MUS_wl6,               // Boss level
    FUNKYOU_MUS_wl6                 // Secret level
};
int songs_sod[] = {

    //////////////////////////////////////////////////////////////
    //
    // SPEAR.Flag() OF DESTINY TRACKS
    //
    //////////////////////////////////////////////////////////////
    XTIPTOE_MUS_sod,
    XFUNKIE_MUS_sod,
    XDEATH_MUS_sod,
    XGETYOU_MUS_sod,                // DON'T KNOW
    ULTIMATE_MUS_sod,               // Trans Gr”sse
	
    DUNGEON_MUS_sod,
    GOINGAFT_MUS_sod,
    POW_MUS_sod,
    TWELFTH_MUS_sod,
    ULTIMATE_MUS_sod,               // Barnacle Wilhelm BOSS
	
    NAZI_OMI_MUS_sod,
    GETTHEM_MUS_sod,
    SUSPENSE_MUS_sod,
    SEARCHN_MUS_sod,
    ZEROHOUR_MUS_sod,
    ULTIMATE_MUS_sod,               // Super Mutant BOSS
	
    XPUTIT_MUS_sod,
    ULTIMATE_MUS_sod,               // Death Knight BOSS
	
    XJAZNAZI_MUS_sod,               // Secret level
    XFUNKIE_MUS_sod,                // Secret level (DON'T KNOW)
	
    XEVIL_MUS_sod                   // Angel of Death BOSS
};

/*
=============================================================================

                               USER CONTROL

=============================================================================
*/

/*
===================
=
= PollKeyboardButtons
=
===================
*/

void PollKeyboardButtons (void)
{
    int i;

    for (i = 0; i < NUMBUTTONS; i++)
        if (Keyboard[buttonscan[i]])
            buttonstate[i] = true;
}


/*
===================
=
= PollMouseButtons
=
===================
*/

void PollMouseButtons (void)
{
    int buttons = IN_MouseButtons ();

    if (buttons & 1)
        buttonstate[buttonmouse[0]] = true;
    if (buttons & 2)
        buttonstate[buttonmouse[1]] = true;
    if (buttons & 4)
        buttonstate[buttonmouse[2]] = true;
}



/*
===================
=
= PollJoystickButtons
=
===================
*/

void PollJoystickButtons (void)
{
    int buttons = IN_JoyButtons();

    for(int i = 0, val = 1; i < JoyNumButtons; i++, val <<= 1)
    {
        if(buttons & val)
            buttonstate[buttonjoy[i]] = true;
    }
}


/*
===================
=
= PollKeyboardMove
=
===================
*/

void PollKeyboardMove (void)
{
    int delta = buttonstate[bt_run] ? RUNMOVE * tics : BASEMOVE * tics;

    if (Keyboard[dirscan[di_north]])
        controly -= delta;
    if (Keyboard[dirscan[di_south]])
        controly += delta;
    if (Keyboard[dirscan[di_west]])
        controlx -= delta;
    if (Keyboard[dirscan[di_east]])
        controlx += delta;
}


/*
===================
=
= PollMouseMove
=
===================
*/

void PollMouseMove (void)
{
    int mousexmove, mouseymove;

    SDL_GetMouseState(&mousexmove, &mouseymove);
    if(IN_IsInputGrabbed())
        IN_CenterMouse();

    mousexmove -= screenWidth / 2;
    mouseymove -= screenHeight / 2;

    controlx += mousexmove * 10 / (13 - mouseadjustment);
    controly += mouseymove * 20 / (13 - mouseadjustment);
}


/*
===================
=
= PollJoystickMove
=
===================
*/

void PollJoystickMove (void)
{
    int joyx, joyy;

    IN_GetJoyDelta (&joyx, &joyy);

    int delta = buttonstate[bt_run] ? RUNMOVE * tics : BASEMOVE * tics;

    if (joyx > 64 || buttonstate[bt_turnright])
        controlx += delta;
    else if (joyx < -64  || buttonstate[bt_turnleft])
        controlx -= delta;
    if (joyy > 64 || buttonstate[bt_movebackward])
        controly += delta;
    else if (joyy < -64 || buttonstate[bt_moveforward])
        controly -= delta;
}

/*
===================
=
= PollControls
=
= Gets user or demo input, call once each frame
=
= controlx              set between -100 and 100 per tic
= controly
= buttonheld[]  the state of the buttons LAST frame
= buttonstate[] the state of the buttons THIS frame
=
===================
*/

void PollControls (void)
{
    int max, min, i;
    byte buttonbits;

    IN_ProcessEvents();

//
// get timing info for last frame
//
    if (demoplayback || demorecord)   // demo recording and playback needs to be constant
    {
        // wait up to DEMOTICS Wolf tics
        uint32_t curtime = SDL_GetTicks();
        lasttimecount += DEMOTICS;
        int32_t timediff = (lasttimecount * 100) / 7 - curtime;
        if(timediff > 0)
            SDL_Delay(timediff);

        if(timediff < -2 * DEMOTICS)       // more than 2-times DEMOTICS behind?
            lasttimecount = (curtime * 7) / 100;    // yes, set to current timecount

        tics = DEMOTICS;
    }
    else
        CalcTics ();

    controlx = 0;
    controly = 0;
    memcpy (buttonheld, buttonstate, sizeof (buttonstate));
    memset (buttonstate, 0, sizeof (buttonstate));

    if (demoplayback)
    {
        //
        // read commands from demo buffer
        //
        buttonbits = *demoptr++;
        for (i = 0; i < NUMBUTTONS; i++)
        {
            buttonstate[i] = buttonbits & 1;
            buttonbits >>= 1;
        }

        controlx = *demoptr++;
        controly = *demoptr++;

        if (demoptr == lastdemoptr)
            playstate = ex_completed;   // demo is done

        controlx *= (int) tics;
        controly *= (int) tics;

        return;
    }


	//
	// get button states
	//
     PollKeyboardButtons ();

     if (mouseenabled && IN_IsInputGrabbed())
          PollMouseButtons ();

     if (joystickenabled)
          PollJoystickButtons ();

	//
	// get movements
	//
     PollKeyboardMove ();

     if (mouseenabled && IN_IsInputGrabbed())
          PollMouseMove ();

     if (joystickenabled)
          PollJoystickMove ();

     if(BotMan::active)	// bot active: operate
	 {
		 // Find A* path
         try
         {
             BotMan::DoCommand();
         }
         catch (const Exception &exc)
         {
             PString errormsg(exc.info());
             Quit(errormsg.buffer());
         }
	 }

//
// bound movement to a maximum
//
    max = 100 * tics;
    min = -max;
    if (controlx > max)
        controlx = max;
    else if (controlx < min)
        controlx = min;

    if (controly > max)
        controly = max;
    else if (controly < min)
        controly = min;

    if (demorecord)
    {
        //
        // save info out to demo buffer
        //
        controlx /= (int) tics;
        controly /= (int) tics;

        buttonbits = 0;

        // TODO: Support 32-bit buttonbits
        for (i = NUMBUTTONS - 1; i >= 0; i--)
        {
            buttonbits <<= 1;
            if (buttonstate[i])
                buttonbits |= 1;
        }

        *demoptr++ = buttonbits;
        *demoptr++ = controlx;
        *demoptr++ = controly;

        if (demoptr >= lastdemoptr - 8)
            playstate = ex_completed;
        else
        {
            controlx *= (int) tics;
            controly *= (int) tics;
        }
    }
}



//==========================================================================



///////////////////////////////////////////////////////////////////////////
//
//      CenterWindow() - Generates a window of a given width & height in the
//              middle of the screen
//
///////////////////////////////////////////////////////////////////////////
#define MAXX    320
#define MAXY    160

void CenterWindow (word w, word h)
{
    US_DrawWindow (((MAXX / 8) - w) / 2, ((MAXY / 8) - h) / 2, w, h);
}

//===========================================================================


/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys (void)
{
    ScanCode scan;


    if (screenfaded || demoplayback)    // don't do anything with a faded screen
        return;

    scan = LastScan;


    if(SPEAR.Flag())
    {
        //
        // SECRET CHEAT CODE: TAB-G-F10
        //
        if (Keyboard[sc_Tab] && Keyboard[sc_G] && Keyboard[sc_F10])
        {
            WindowH = 160;
            if (godmode)
            {
                Message ("God mode OFF");
                SD_PlaySound (NOBONUSSND);
            }
            else
            {
                Message ("God mode ON");
                SD_PlaySound (ENDBONUS2SND);
            }

            IN_Ack ();
            godmode ^= 1;
            DrawPlayBorderSides ();
            IN_ClearKeysDown ();
            return;
        }
    }


    //
    // SECRET CHEAT CODE: 'MLI'
    //
    if (Keyboard[sc_M] && Keyboard[sc_L] && Keyboard[sc_I])
    {
        gamestate.health = I_PLAYERHEALTH;	// IOANCH 25.10.2012: named constants
        gamestate.ammo = I_MAXAMMO;	// IOANCH
        gamestate.keys = 3;
        gamestate.score = 0;
        gamestate.TimeCount += 42000L;
        GiveWeapon (wp_chaingun);
        DrawWeapon ();
        DrawHealth ();
        DrawKeys ();
        DrawAmmo ();
        DrawScore ();

        ClearMemory ();
        CA_CacheGrChunk (gfxvmap[STARTFONT][SPEAR.Flag()] + 1);
        ClearSplitVWB ();

        Message (STR_CHEATER1 "\n"
                 STR_CHEATER2 "\n\n" STR_CHEATER3 "\n" STR_CHEATER4 "\n" STR_CHEATER5);

        UNCACHEGRCHUNK (gfxvmap[STARTFONT][SPEAR.Flag()] + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        if (viewsize < 17)
            DrawPlayBorder ();
    }

    //
    // OPEN UP DEBUG KEYS
    //
#ifdef DEBUGKEYS
    if (Keyboard[sc_BackSpace] && Keyboard[sc_LShift] && Keyboard[sc_Alt] && param_debugmode)
    {
        ClearMemory ();
        CA_CacheGrChunk (gfxvmap[STARTFONT][SPEAR.Flag()] + 1);
        ClearSplitVWB ();

        Message ("Debugging keys are\nnow available!");
        UNCACHEGRCHUNK (gfxvmap[STARTFONT][SPEAR.Flag()] + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        DrawPlayBorderSides ();
        DebugOk = 1;
    }
#endif

    //
    // TRYING THE KEEN CHEAT CODE!
    //
    if (Keyboard[sc_B] && Keyboard[sc_A] && Keyboard[sc_T])
    {
        ClearMemory ();
        CA_CacheGrChunk (gfxvmap[STARTFONT][SPEAR.Flag()] + 1);
        ClearSplitVWB ();

        Message ("Commander Keen is also\n"
                 "available from Apogee, but\n"
                 "then, you already know\n" "that - right, Cheatmeister?!");

        UNCACHEGRCHUNK (gfxvmap[STARTFONT][SPEAR.Flag()] + 1);
        IN_ClearKeysDown ();
        IN_Ack ();

        if (viewsize < 18)
            DrawPlayBorder ();
    }

//
// pause key weirdness can't be checked as a scan code
//
    if(buttonstate[bt_pause]) Paused = true;
    if(Paused)
    {
        int lastoffs = StopMusic();
        LatchDrawPic (20 - 4, 80 - 2 * 8, gfxvmap[PAUSEDPIC][SPEAR.Flag()]);
        VH_UpdateScreen();
        IN_Ack ();
        Paused = false;
        ContinueMusic(lastoffs);
        if (MousePresent && IN_IsInputGrabbed())
            IN_CenterMouse();     // Clear accumulated mouse movement
        lasttimecount = GetTimeCount();
        return;
    }

//
// F1-F7/ESC to enter control panel
//
    if (
#ifndef DEBCHECK
           scan == sc_F10 ||
#endif
           scan == sc_F9 || scan == sc_F7 || scan == sc_F8)     // pop up quit dialog
    {
		// IOANCH 11.06.2012: commented to made compiler stop complaining
        //short oldmapon = gamestate.mapon;
        //short oldepisode = gamestate.episode;
        ClearMemory ();
        ClearSplitVWB ();
        US_ControlPanel (scan);

        DrawPlayBorderSides ();

        SETFONTCOLOR (0, 15);
        IN_ClearKeysDown ();
        return;
    }

    if ((scan >= sc_F1 && scan <= sc_F9) || scan == sc_Escape || buttonstate[bt_esc])
    {
        int lastoffs = StopMusic ();
        ClearMemory ();
        VW_FadeOut ();

        US_ControlPanel (buttonstate[bt_esc] ? sc_Escape : scan);

        SETFONTCOLOR (0, 15);
        IN_ClearKeysDown ();
        VW_FadeOut();
        if(viewsize != 21)
            DrawPlayScreen ();
        if (!startgame && !loadedgame)
            ContinueMusic (lastoffs);
        if (loadedgame)
            playstate = ex_abort;
        lasttimecount = GetTimeCount();
        if (MousePresent && IN_IsInputGrabbed())
            IN_CenterMouse();     // Clear accumulated mouse movement
        return;
    }

//
// TAB-? debug keys
//
#ifdef DEBUGKEYS
    if (Keyboard[sc_Tab] && DebugOk)
    {
        CA_CacheGrChunk (gfxvmap[STARTFONT][SPEAR.Flag()]);
        fontnumber = 0;
        SETFONTCOLOR (0, 15);
        if (DebugKeys () && viewsize < 20)
            DrawPlayBorder ();       // dont let the blue borders flash

        if (MousePresent && IN_IsInputGrabbed())
            IN_CenterMouse();     // Clear accumulated mouse movement

        lasttimecount = GetTimeCount();
        return;
    }
#endif
}


//===========================================================================

/*
#############################################################################

                                  The objlist data structure

#############################################################################

objlist containt structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawn another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitActorList
=
= Call to clear out the actor object lists returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

int objcount;

void InitActorList (void)
{
    int i;

//
// init the actor lists
//
    for (i = 0; i < MAXACTORS; i++)
    {
        objlist[i].prev = &objlist[i + 1];
        objlist[i].next = NULL;
    }

    objlist[MAXACTORS - 1].prev = NULL;

    objfreelist = &objlist[0];
    lastobj = NULL;

    objcount = 0;

//
// give the player the first free spots
//
    GetNewActor ();
    player = newobj;

}

//===========================================================================

/*
=========================
=
= GetNewActor
=
= Sets the global variable new to point to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

void GetNewActor (void)
{
    if (!objfreelist)
        Quit ("GetNewActor: No free spots in objlist!");

    newobj = objfreelist;
    objfreelist = newobj->prev;
    memset (newobj, 0, sizeof (*newobj));

    if (lastobj)
        lastobj->next = newobj;
    newobj->prev = lastobj;     // new->next is allready NULL from memset

    newobj->active = ac_no;
    lastobj = newobj;

    objcount++;
}

//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype * gone)
{
    if (gone == player)
        Quit ("RemoveObj: Tried to remove the player!");

    gone->state = NULL;

//
// fix the next object's back link
//
    if (gone == lastobj)
        lastobj = (objtype *) gone->prev;
    else
        gone->next->prev = gone->prev;

//
// fix the previous object's forward link
//
    gone->prev->next = gone->next;

//
// add it back in to the free list
//
    gone->prev = objfreelist;
    objfreelist = gone;

    objcount--;
}

/*
=============================================================================

                                                MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/
int StopMusic (void)
{
    int lastoffs = SD_MusicOff ();

	// IOANCH 20130301: unification
    UNCACHEAUDIOCHUNK (IMPALE(STARTMUSIC) + lastmusicchunk);

    return lastoffs;
}

//==========================================================================


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic ()
{
    SD_MusicOff ();
	// IOANCH 20130301: unification
	int *songs = IMPALE(songs);
    lastmusicchunk = songs[gamestate.mapon + gamestate.episode * 10];
	
	// IOANCH 20130301: unification
    SD_StartMusic(IMPALE(STARTMUSIC) + lastmusicchunk);
}

void ContinueMusic (int offs)
{
    SD_MusicOff ();
	// IOANCH 20130301: unification
	int *songs = IMPALE(songs);
    lastmusicchunk = songs[gamestate.mapon + gamestate.episode * 10];
	
	// IOANCH 20130301: unification
    SD_ContinueMusic(IMPALE(STARTMUSIC) + lastmusicchunk, 
					 offs);
}

/*
=============================================================================

                                        PALETTE SHIFTING STUFF

=============================================================================
*/

#define NUMREDSHIFTS    6
#define REDSTEPS        8

#define NUMWHITESHIFTS  3
#define WHITESTEPS      20
#define WHITETICS       6


SDL_Color redshifts[NUMREDSHIFTS][256];
SDL_Color whiteshifts[NUMWHITESHIFTS][256];

int damagecount, bonuscount;
boolean palshifted;

/*
=====================
=
= InitRedShifts
=
=====================
*/

void InitRedShifts (void)
{
    SDL_Color *workptr, *baseptr;
    int i, j, delta;


//
// fade through intermediate frames
//
    for (i = 1; i <= NUMREDSHIFTS; i++)
    {
        workptr = redshifts[i - 1];
        // IOANCH 20130202: unification process
        baseptr = IMPALE(palette);

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / REDSTEPS;
            delta = -baseptr->g;
            workptr->g = baseptr->g + delta * i / REDSTEPS;
            delta = -baseptr->b;
            workptr->b = baseptr->b + delta * i / REDSTEPS;
            baseptr++;
            workptr++;
        }
    }

    for (i = 1; i <= NUMWHITESHIFTS; i++)
    {
        workptr = whiteshifts[i - 1];
        // IOANCH 20130202: unification process
        baseptr = IMPALE(palette);

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / WHITESTEPS;
            delta = 248 - baseptr->g;
            workptr->g = baseptr->g + delta * i / WHITESTEPS;
            delta = 0-baseptr->b;
            workptr->b = baseptr->b + delta * i / WHITESTEPS;
            baseptr++;
            workptr++;
        }
    }
}


/*
=====================
=
= ClearPaletteShifts
=
=====================
*/

void ClearPaletteShifts (void)
{
    bonuscount = damagecount = 0;
    palshifted = false;
}


/*
=====================
=
= StartBonusFlash
=
=====================
*/

void StartBonusFlash (void)
{
    bonuscount = NUMWHITESHIFTS * WHITETICS;    // white shift palette
}


/*
=====================
=
= StartDamageFlash
=
=====================
*/

void StartDamageFlash (int damage)
{
    damagecount += damage;
}


/*
=====================
=
= UpdatePaletteShifts
=
=====================
*/

void UpdatePaletteShifts (void)
{
    int red, white;

    if (bonuscount)
    {
        white = bonuscount / WHITETICS + 1;
        if (white > NUMWHITESHIFTS)
            white = NUMWHITESHIFTS;
        bonuscount -= tics;
        if (bonuscount < 0)
            bonuscount = 0;
    }
    else
        white = 0;


    if (damagecount)
    {
        red = damagecount / 10 + 1;
        if (red > NUMREDSHIFTS)
            red = NUMREDSHIFTS;

        damagecount -= tics;
        if (damagecount < 0)
            damagecount = 0;
    }
    else
        red = 0;

    if (red)
    {
        VL_SetPalette (redshifts[red - 1], false);
        palshifted = true;
    }
    else if (white)
    {
        VL_SetPalette (whiteshifts[white - 1], false);
        palshifted = true;
    }
    else if (palshifted)
    {
        // IOANCH 20130202: unification process
        VL_SetPalette (IMPALE(palette), false);        // back to normal
        palshifted = false;
    }
}


/*
=====================
=
= FinishPaletteShifts
=
= Resets palette to normal if needed
=
=====================
*/

void FinishPaletteShifts (void)
{
    if (palshifted)
    {
        palshifted = 0;
        // IOANCH 20130202: unification process
        VL_SetPalette (IMPALE(palette), true);
    }
}


/*
=============================================================================

                                                CORE PLAYLOOP

=============================================================================
*/


/*
=====================
=
= DoActor
=
=====================
*/

void DoActor (objtype * ob)
{
    void (*think) (objtype *);

    if (!ob->active && !areabyplayer[ob->areanumber])
        return;

    if (!(ob->flags & (FL_NONMARK | FL_NEVERMARK)))
        actorat[ob->tilex][ob->tiley] = NULL;

//
// non transitional object
//

    if (!ob->ticcount)
    {
        think = (void (*)(objtype *)) ob->state->think;
        if (think)
        {
            think (ob);
            if (!ob->state)
            {
                RemoveObj (ob);
                return;
            }
        }

        if (ob->flags & FL_NEVERMARK)
            return;

        if ((ob->flags & FL_NONMARK) && actorat[ob->tilex][ob->tiley])
            return;

        actorat[ob->tilex][ob->tiley] = ob;
        return;
    }

//
// transitional object
//
    ob->ticcount -= (short) tics;
    while (ob->ticcount <= 0)
    {
        think = (void (*)(objtype *)) ob->state->action;        // end of state action
        if (think)
        {
            think (ob);
            if (!ob->state)
            {
                RemoveObj (ob);
                return;
            }
        }

        ob->state = ob->state->next;

        if (!ob->state)
        {
            RemoveObj (ob);
            return;
        }

        if (!ob->state->tictime)
        {
            ob->ticcount = 0;
            goto think;
        }

        ob->ticcount += ob->state->tictime;
    }

think:
    //
    // think
    //
    think = (void (*)(objtype *)) ob->state->think;
    if (think)
    {
        think (ob);
        if (!ob->state)
        {
            RemoveObj (ob);
            return;
        }
    }

    if (ob->flags & FL_NEVERMARK)
        return;

    if ((ob->flags & FL_NONMARK) && actorat[ob->tilex][ob->tiley])
        return;

    actorat[ob->tilex][ob->tiley] = ob;
}

//==========================================================================


/*
===================
=
= PlayLoop
=
===================
*/
int32_t funnyticount;


void PlayLoop (void)
{
#if defined(USE_FEATUREFLAGS) && defined(USE_CLOUDSKY)
    if(GetFeatureFlags() & FF_CLOUDSKY)
        InitSky();
#endif

#ifdef USE_SHADING
    InitLevelShadeTable();
#endif

    playstate = ex_stillplaying;
    lasttimecount = GetTimeCount();
    frameon = 0;
    anglefrac = 0;
    facecount = 0;
    funnyticount = 0;
    memset (buttonstate, 0, sizeof (buttonstate));
    ClearPaletteShifts ();

    if (MousePresent && IN_IsInputGrabbed())
        IN_CenterMouse();         // Clear accumulated mouse movement

    if (demoplayback)
        IN_StartAck ();

    do
    {
        PollControls ();

//
// actor thinking
//
        madenoise = false;

        MoveDoors ();
        MovePWalls ();

        for (obj = player; obj; obj = obj->next)
            DoActor (obj);

        UpdatePaletteShifts ();

        ThreeDRefresh ();

        //
        // MAKE FUNNY FACE IF BJ DOESN'T MOVE FOR AWHILE
        //
        if(SPEAR.Flag())
        {
            funnyticount += tics;
            if (funnyticount > 30l * 70)
            {
                funnyticount = 0;
                // IOANCH 20130302: unification
                if(viewsize != 21)
                    StatusDrawFace(gfxvmap[BJWAITING1PIC][SPEAR.Flag()] + (US_RndT () & 1));
                facecount = 0;
            }
        }

        gamestate.TimeCount += tics;

        UpdateSoundLoc ();      // JAB
        if (screenfaded)
            VW_FadeIn ();

        CheckKeys ();

//
// debug aids
//
        if (singlestep)
        {
            VW_WaitVBL (singlestep);
            lasttimecount = GetTimeCount();
        }
        if (extravbls)
            VW_WaitVBL (extravbls);

        if (demoplayback)
        {
            if (IN_CheckAck ())
            {
                IN_ClearKeysDown ();
                playstate = ex_abort;
            }
        }
    }
    while (!playstate && !startgame);
	
	// IOANCH 20121215
	// Now is a time to save explored map data
	if(ingame)
		BotMan::SaveData();

    if (playstate != ex_died)
        FinishPaletteShifts ();
}
