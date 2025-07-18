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
// Wolfenstein developer debug routines
//
////////////////////////////////////////////////////////////////////////////////

// WL_DEBUG.C

#include "wl_def.h"
#include "version.h"
#include "wl_act1.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_play.h"
#include "Config.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_us.h"
#include "id_vh.h"
#include "ioan_bot.h"
#ifdef USE_CLOUDSKY
#include "wl_cloudsky.h"
#endif

#include "FileSystem.h"
#include "StdStringExtensions.h"

/*
=============================================================================

                                                 LOCAL CONSTANTS

=============================================================================
*/

#define VIEWTILEX       (viewwidth/16)
#define VIEWTILEY       (viewheight/16)

/*
=============================================================================

                                                 GLOBAL VARIABLES

=============================================================================
*/

#ifdef DEBUGKEYS

int DebugKeys ();

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/

int     maporgx;
int     maporgy;
// IOANCH 11.06.2012: added name to made compiler stop complaining
enum _viewtype_t {mapview,tilemapview,actoratview,visview}  viewtype;

void ViewMap ();

//===========================================================================

/*
==================
=
= CountObjects
=
==================
*/

void CountObjects ()
{
    int     i,total,count,active,inactive,doors;
    objtype *obj;

    CenterWindow (17,7);
    active = inactive = count = doors = 0;

    US_Print ("Total statics :");
    total = (int)(laststatobj-&statobjlist[0]);
    US_PrintUnsigned (total);

    char str[60];
    snprintf(str, sizeof(str), "\nlaststatobj=%.8X", (int32_t)(uintptr_t)laststatobj);
    US_Print(str);

    US_Print ("\nIn use statics:");
    for (i=0;i<total;i++)
    {
        if (statobjlist[i].shapenum != -1)
            count++;
        else
            doors++;        //debug
    }
    US_PrintUnsigned (count);

    US_Print ("\nDoors         :");
    US_PrintUnsigned (doornum);

    for (obj=player->next;obj;obj=obj->next)
    {
        if (obj->active)
            active++;
        else
            inactive++;
    }

    US_Print ("\nTotal actors  :");
    US_PrintUnsigned (active+inactive);

    US_Print ("\nActive actors :");
    US_PrintUnsigned (active);

    I_UpdateScreen();
    myInput.ack ();
}


//===========================================================================

/*
===================
=
= PictureGrabber
=
===================
*/
void PictureGrabber ()
{
    static char fname[] = "WSHOT000.BMP";
	std::string path;

    for(int i = 0; i < 1000; i++)
    {
        fname[7] = i % 10 + '0';
        fname[6] = (i / 10) % 10 + '0';
        fname[5] = i / 100 + '0';
		
		path = cfg_dir;
		ConcatSubpath(path, fname);
		
		if(!FileSystem::FileExists(path.c_str()))
			break;
    }

    // overwrites WSHOT999.BMP if all wshot files exist

   I_SaveBufferBMP(path.c_str());


    CenterWindow (18,2);
    US_PrintCentered ("Screenshot taken");
    I_UpdateScreen();
    myInput.ack();
}


//===========================================================================

/*
===================
=
= BasicOverhead
=
===================
*/

void BasicOverhead ()
{
    int x, y, z, offx, offy;

    z = 128/MAPSIZE; // zoom scale
    offx = LOGIC_WIDTH/2;
    offy = (160-MAPSIZE*z)/2;

#ifdef MAPBORDER
    int temp = viewsize;
    main_NewViewSize(16);
    DrawPlayBorder();
#endif

    // right side (raw)

    for(x=0;x<MAPSIZE;x++)
        for(y=0;y<MAPSIZE;y++)
            VL_Bar(x*z+offx, y*z+offy,z,z,(unsigned)(uintptr_t)actorat[x][y]);

    // left side (filtered)

    uintptr_t tile;
    int color = 0;
    offx -= 128;

    for(x=0;x<MAPSIZE;x++)
    {
        for(y=0;y<MAPSIZE;y++)
        {
            tile = (uintptr_t)actorat[x][y];
            if (ISPOINTER(tile) && ((objtype *)tile)->flags&FL_SHOOTABLE) color = 72;  // enemy
            else if (!tile || ISPOINTER(tile))
            {
                if (spotvis[x][y]) color = 111;  // visable
                else color = 0;  // nothing
            }
            else if (MAPSPOT(x,y,1) == PUSHABLETILE) color = 171;  // pushwall
            else if (tile == 64) color = 158; // solid obj
            else if (tile < 128) color = 154;  // walls
            else if (tile < 256) color = 146;  // doors

            VL_Bar(x*z+offx, y*z+offy,z,z,color);
        }
    }

    VL_Bar(player->tilex*z+offx,player->tiley*z+offy,z,z,15); // player

    // resize the border to match

    I_UpdateScreen();
    myInput.ack();

#ifdef MAPBORDER
    main_NewViewSize(temp);
    DrawPlayBorder();
#endif
}


//===========================================================================

/*
================
=
= ShapeTest
=
================
*/

void ShapeTest ()
{
    //TODO

}


//===========================================================================


/*
================
=
= DebugKeys
=
================
*/

int DebugKeys ()
{
    Boolean8 esc;
    int level;

    if (myInput.keyboard(sc_B))             // B = border color
    {
        CenterWindow(20,3);
        PrintY+=6;
        US_Print(" Border color (0-56): ");
        I_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,2,0);
        if (!esc)
        {
            level = atoi (str);
            if (level>=0 && level<=99)
            {
                if (level<30) level += 31;
                else
                {
                    if (level > 56) level=31;
                    else level -= 26;
                }

                bordercol=level*4+3;

                if (bordercol == VIEWCOLOR)
                    DrawStatusBorder(bordercol);

                DrawPlayBorder();

                return 0;
            }
        }
        return 1;
    }
    if (myInput.keyboard(sc_C))             // C = count objects
    {
        CountObjects();
        return 1;
    }
    if (myInput.keyboard(sc_D))             // D = Darkone's FPS counter
    {
        CenterWindow (22,2);
        if (fpscounter)
            US_PrintCentered ("Darkone's FPS Counter OFF");
        else
            US_PrintCentered ("Darkone's FPS Counter ON");
        I_UpdateScreen();
        myInput.ack();
        fpscounter ^= 1;
        return 1;
    }
	if (myInput.keyboard(sc_E))             // E = quit level
	{
		SaveFullInstanceState();
		playstate = ex_completed;
	}

    if (myInput.keyboard(sc_F))             // F = facing spot
    {
        char str[60];
        CenterWindow (14,6);
        US_Print ("x:");     US_PrintUnsigned (player->x);
        US_Print (" (");     US_PrintUnsigned (player->x%65536);
        US_Print (")\ny:");  US_PrintUnsigned (player->y);
        US_Print (" (");     US_PrintUnsigned (player->y%65536);
        US_Print (")\nA:");  US_PrintUnsigned (player->angle);
        US_Print (" X:");    US_PrintUnsigned (player->tilex);
        US_Print (" Y:");    US_PrintUnsigned (player->tiley);
        US_Print ("\n1:");   US_PrintUnsigned (tilemap[player->tilex][player->tiley]);
        snprintf(str, sizeof(str), " 2:%.8X", (unsigned)(uintptr_t)actorat[player->tilex][player->tiley]); US_Print(str);
        US_Print ("\nf 1:"); US_PrintUnsigned (player->areanumber);
        US_Print (" 2:");    US_PrintUnsigned (MAPSPOT(player->tilex,player->tiley,1));
        US_Print (" 3:");
        if ((unsigned)(uintptr_t)actorat[player->tilex][player->tiley] < 256)
            US_PrintUnsigned (spotvis[player->tilex][player->tiley]);
        else
            US_PrintUnsigned (actorat[player->tilex][player->tiley]->flags);
        I_UpdateScreen();
        myInput.ack();
        return 1;
    }

    if (myInput.keyboard(sc_G))             // G = god mode
    {
        CenterWindow (12,2);
        if (godmode == 0)
            US_PrintCentered ("God mode ON");
        else if (godmode == 1)
            US_PrintCentered ("God (no flash)");
        else if (godmode == 2)
            US_PrintCentered ("God mode OFF");

        I_UpdateScreen();
        myInput.ack();
        if (godmode != 2)
            godmode++;
        else
            godmode = 0;
        return 1;
    }
    if (myInput.keyboard(sc_H))             // H = hurt self
    {
        myInput.clearKeysDown ();
        TakeDamage (16,NULL);
    }
    else if (myInput.keyboard(sc_I))        // I = item cheat
    {
        CenterWindow (12,3);
        US_PrintCentered ("Free items!");
        I_UpdateScreen();
        GivePoints (100000);
        HealSelf (I_PLAYERHEALTH - 1);	// IOANCH 25.10.2012: named constants
        if (gamestate.bestweapon<wp_chaingun)
            GiveWeapon (gamestate.bestweapon+1);
        gamestate.ammo += 50;
        if (gamestate.ammo > I_MAXAMMO)	// IOANCH
            gamestate.ammo = I_MAXAMMO;
        DrawAmmo ();
        myInput.ack ();
        return 1;
    }
    else if (myInput.keyboard(sc_K))        // K = give keys
    {
        CenterWindow(16,3);
        PrintY+=6;
        US_Print("  Give Key (1-4): ");
        I_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,1,0);
        if (!esc)
        {
            level = atoi (str);
            if (level>0 && level<5)
                GiveKey(level-1);
        }
        return 1;
    }
    else if (myInput.keyboard(sc_L))        // L = level ratios
    {
        byte x,start,end=LRpack;

        if (end == 8)   // wolf3d
        {
            CenterWindow(17,10);
            start = 0;
        }
        else            // sod
        {
            CenterWindow(17,12);
            start = 0; end = 10;
        }
again:
        for(x=start;x<end;x++)
        {
            US_PrintUnsigned(x+1);
            US_Print(" ");
            US_PrintUnsigned(LevelRatios[x].time/60);
            US_Print(":");
            if (LevelRatios[x].time%60 < 10)
                US_Print("0");
            US_PrintUnsigned(LevelRatios[x].time%60);
            US_Print(" ");
            US_PrintUnsigned(LevelRatios[x].kill);
            US_Print("% ");
            US_PrintUnsigned(LevelRatios[x].secret);
            US_Print("% ");
            US_PrintUnsigned(LevelRatios[x].treasure);
            US_Print("%\n");
        }
        I_UpdateScreen();
        myInput.ack();
        if (end == 10 && gamestate.mapon > 9)
        {
            start = 10; end = 20;
            CenterWindow(17,12);
            goto again;
        }

        return 1;
    }
    else if (myInput.keyboard(sc_N))        // N = no clip
    {
        noclip^=1;
        CenterWindow (18,3);
        if (noclip)
            US_PrintCentered ("No clipping ON");
        else
            US_PrintCentered ("No clipping OFF");
        I_UpdateScreen();
        myInput.ack ();
        return 1;
    }
    else if (myInput.keyboard(sc_O))        // O = basic overhead
    {
        BasicOverhead();
        return 1;
    }
    else if(myInput.keyboard(sc_P))         // P = Ripper's picture grabber
    {
        PictureGrabber();
        return 1;
    }
    else if (myInput.keyboard(sc_Q))        // Q = fast quit
	{ 
		if (ingame && cfg_botActive)
			bot.SaveData();
		DestroySavedInstance();
        Quit ();
	}
    else if (myInput.keyboard(sc_S))        // S = slow motion
    {
        CenterWindow(30,3);
        PrintY+=6;
        US_Print(" Slow Motion steps (default 14): ");
        I_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,2,0);
        if (!esc)
        {
            level = atoi (str);
            if (level>=0 && level<=50)
                singlestep = level;
        }
        return 1;
    }
    else if (myInput.keyboard(sc_T))        // T = shape test
    {
        ShapeTest ();
        return 1;
    }
    else if (myInput.keyboard(sc_V))        // V = extra VBLs
    {
        CenterWindow(30,3);
        PrintY+=6;
        US_Print("  Add how many extra VBLs(0-8): ");
        I_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,1,0);
        if (!esc)
        {
            level = atoi (str);
            if (level>=0 && level<=8)
               cfg_extravbls = level;
        }
        return 1;
    }
    else if (myInput.keyboard(sc_W))        // W = warp to level
    {
        CenterWindow(26,3);
        PrintY+=6;
        // IOANCH 20130202: unification process
        if(!SPEAR::flag)
            US_Print("  Warp to which level(1-10): ");
        else
            US_Print("  Warp to which level(1-21): ");

        I_UpdateScreen();
        esc = !US_LineInput (px,py,str,NULL,true,2,0);
        if (!esc)
        {
            level = atoi (str);
            // IOANCH 20130202: unification process
            if ((!SPEAR::flag && level>0 && level<11) || (SPEAR::flag && level > 0 && level < 22))
            {
                gamestate.mapon = level-1;
                playstate = ex_warped;
            }
        }
        return 1;
    }
    else if (myInput.keyboard(sc_X))        // X = item cheat
    {
        CenterWindow (12,3);
        US_PrintCentered ("Extra stuff!");
        I_UpdateScreen();
        // DEBUG: put stuff here
        myInput.ack ();
        return 1;
    }
#ifdef USE_CLOUDSKY
    else if(myInput.keyboard(sc_Z))
    {
        char defstr[15];

        CenterWindow(34,4);
        PrintY+=6;
        US_Print("  Recalculate sky with seek: ");
        int seekpx = px, seekpy = py;
        US_PrintUnsigned(curSky->seed);
        US_Print("\n  Use color map (0-");
        US_PrintUnsigned(numColorMaps - 1);
        US_Print("): ");
        int mappx = px, mappy = py;
        US_PrintUnsigned(curSky->colorMapIndex);
        I_UpdateScreen();

        snprintf(defstr, sizeof(defstr), "%u", curSky->seed);
        esc = !US_LineInput(seekpx, seekpy, str, defstr, true, 10, 0);
        if(esc) return 0;
        curSky->seed = (uint32_t) atoi(str);

        snprintf(defstr, sizeof(defstr), "%u", curSky->colorMapIndex);
        esc = !US_LineInput(mappx, mappy, str, defstr, true, 10, 0);
        if(esc) return 0;
        uint32_t newInd = (uint32_t) atoi(str);
        if(newInd < (uint32_t) numColorMaps)
        {
            curSky->colorMapIndex = newInd;
            InitSky();
        }
        else
        {
            CenterWindow (18,3);
            US_PrintCentered ("Illegal color map!");
            I_UpdateScreen();
            myInput.ack ();
        }
    }
#endif

    return 0;
}

#endif
