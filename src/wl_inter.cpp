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
// Routines dealing with intermission
//
////////////////////////////////////////////////////////////////////////////////

// WL_INTER.C

#include "wl_def.h"
#include "foreign.h"
#include "f_spear.h"
#include "wl_agent.h"
#include "wl_game.h"
#include "wl_menu.h"
#include "wl_text.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_vh.h"
#include "ioan_bot.h"	// IOANCH 27.05.2012
#include "ioan_bas.h"
#include "Config.h"
// IOANCH 20130302: unification
LRstruct LevelRatios[LRpack_wl6 > LRpack_sod ? LRpack_wl6 : LRpack_sod];
int32_t lastBreathTime = 0;



void Write (int x, int y, const char *string);

//==========================================================================

/*
==================
=
= CLearSplitVWB
=
==================
*/

void ClearSplitVWB ()
{
    WindowX = 0;
    WindowY = 0;
    WindowW = 320;
    WindowH = 160;
}


//==========================================================================


// IOANCH 20130301: unification culling
////////////////////////////////////////////////////////
//
// End of Spear of Destiny
//
////////////////////////////////////////////////////////

static void EndScreen (int palette, int screen)
{
    SDL_Color pal[256];
    graphSegs.cacheScreen (screen);
    I_UpdateScreen ();
    graphSegs.cacheChunk (palette);
    VL_ConvertPalette(graphSegs[palette], pal, 256);
    VL_FadeIn (0, 255, pal, 30);
    graphSegs.uncacheChunk (palette);
    myInput.clearKeysDown ();
    myInput.ack ();
    VW_FadeOut ();
}


static void EndSpear ()
{
    SDL_Color pal[256];

    EndScreen (SPEAR::g(END1PALETTE), SPEAR::g(ENDSCREEN11PIC));

    graphSegs.cacheScreen (SPEAR::g(ENDSCREEN3PIC));
    I_UpdateScreen ();
    graphSegs.cacheChunk (SPEAR::g(END3PALETTE));
    VL_ConvertPalette(graphSegs[SPEAR::g(END3PALETTE)], pal, 256);
    VL_FadeIn (0, 255, pal, 30);
    graphSegs.uncacheChunk (SPEAR::g(END3PALETTE));
    fontnumber = 0;
    g_fontcolor = 0xd0;
    WindowX = 0;
    WindowW = 320;
    PrintX = 0;
    PrintY = 180;
    US_CPrint (STR_ENDGAME1 "\n");
    US_CPrint (STR_ENDGAME2);
    I_UpdateScreen ();
    myInput.userInput(700);

    PrintX = 0;
    PrintY = 180;
    VL_Bar (0, 180, 320, 20, 0);
    US_CPrint (STR_ENDGAME3 "\n");
    US_CPrint (STR_ENDGAME4);
    I_UpdateScreen ();
    myInput.userInput(700);

    VW_FadeOut ();

    EndScreen (SPEAR::g(END4PALETTE), SPEAR::g(ENDSCREEN4PIC));
    EndScreen (SPEAR::g(END5PALETTE), SPEAR::g(ENDSCREEN5PIC));
    EndScreen (SPEAR::g(END6PALETTE), SPEAR::g(ENDSCREEN6PIC));
    EndScreen (SPEAR::g(END7PALETTE), SPEAR::g(ENDSCREEN7PIC));
    EndScreen (SPEAR::g(END8PALETTE), SPEAR::g(ENDSCREEN8PIC));
    EndScreen (SPEAR::g(END9PALETTE), SPEAR::g(ENDSCREEN9PIC));

    EndScreen (SPEAR::g(END2PALETTE), SPEAR::g(ENDSCREEN12PIC));

    MainMenu[savegame].active = 0;
}



//==========================================================================

/*
==================
=
= Victory
=
==================
*/

void Victory ()
{
// IOANCH 20130301: unification culling
    int32_t sec;
    int i, min, kr, sr, tr, x;
    char tempstr[8];

#define RATIOX  6
#define RATIOY  14
#define TIMEX   14
#define TIMEY   8

    // IOANCH 20130302: unification
    if(SPEAR::flag)
    {
        // IOANCH 20130301: unification music
        StartCPMusic (XTHEEND_MUS_sod);

        // IOANCH 20130302: unification
        graphSegs.cacheChunk (SPEAR::g(BJCOLLAPSE1PIC));
        graphSegs.cacheChunk (SPEAR::g(BJCOLLAPSE2PIC));
        graphSegs.cacheChunk (SPEAR::g(BJCOLLAPSE3PIC));
        graphSegs.cacheChunk (SPEAR::g(BJCOLLAPSE4PIC));

        VL_Bar (0, 0, 320, 200, VIEWCOLOR);
        VWB_DrawPic (124, 44, SPEAR::g(BJCOLLAPSE1PIC));
        I_UpdateScreen ();
        VW_FadeIn ();
        VL_WaitVBL (2 * 70);
        VWB_DrawPic (124, 44, SPEAR::g(BJCOLLAPSE2PIC));
        I_UpdateScreen ();
        VL_WaitVBL (105);
        VWB_DrawPic (124, 44, SPEAR::g(BJCOLLAPSE3PIC));
        I_UpdateScreen ();
        VL_WaitVBL (105);
        VWB_DrawPic (124, 44, SPEAR::g(BJCOLLAPSE4PIC));
        I_UpdateScreen ();
        VL_WaitVBL (3 * 70);

        graphSegs.uncacheChunk (SPEAR::g(BJCOLLAPSE1PIC));
        graphSegs.uncacheChunk (SPEAR::g(BJCOLLAPSE2PIC));
        graphSegs.uncacheChunk (SPEAR::g(BJCOLLAPSE3PIC));
        graphSegs.uncacheChunk (SPEAR::g(BJCOLLAPSE4PIC));
        VL_FadeOut (0, 255, 0, 17, 17, 5);
    }
            // IOANCH 20130301: unification music
    StartCPMusic (IMPALE((int)URAHERO_MUS));
    ClearSplitVWB ();
    CacheLump (SPEAR::g(LEVELEND_LUMP_START), SPEAR::g(LEVELEND_LUMP_END));
    graphSegs.cacheChunk (SPEAR::g(STARTFONT));

    if(!SPEAR::flag)
        graphSegs.cacheChunk (SPEAR::g(C_TIMECODEPIC));

    VL_Bar (0, 0, 320, cfg_screenHeight / vid_scaleFactor - STATUSLINES + 1, VIEWCOLOR);
    if (bordercol != VIEWCOLOR)
        DrawStatusBorder (VIEWCOLOR);
// IOANCH 20130301: unification culling
    Write (18, 2, STR_YOUWIN);

    Write (TIMEX, TIMEY - 2, STR_TOTALTIME);

    Write (12, RATIOY - 2, "averages");

#ifdef SPANISH
    Write (RATIOX + 2, RATIOY, STR_RATKILL);
    Write (RATIOX + 2, RATIOY + 2, STR_RATSECRET);
    Write (RATIOX + 2, RATIOY + 4, STR_RATTREASURE);
#else
    Write (RATIOX + 8, RATIOY, STR_RATKILL);
    Write (RATIOX + 4, RATIOY + 2, STR_RATSECRET);
    Write (RATIOX, RATIOY + 4, STR_RATTREASURE);
#endif



    VWB_DrawPic (8, 4, SPEAR::g(L_BJWINSPIC));


    for (kr = sr = tr = sec = i = 0; i < LRpack; i++)
    {
        sec += LevelRatios[i].time;
        kr += LevelRatios[i].kill;
        sr += LevelRatios[i].secret;
        tr += LevelRatios[i].treasure;
    }

    if(!SPEAR::flag)
    {
        kr /= LRpack;
        sr /= LRpack;
        tr /= LRpack;
    }
    else
    {
        kr /= 14;
        sr /= 14;
        tr /= 14;
    }

    min = sec / 60;
    sec %= 60;

    if (min > 99)
        min = sec = 99;

    i = TIMEX * 8 + 1;
    VWB_DrawPic (i, TIMEY * 8, SPEAR::g(L_NUM0PIC) + (min / 10));
    i += 2 * 8;
    VWB_DrawPic (i, TIMEY * 8, SPEAR::g(L_NUM0PIC) + (min % 10));
    i += 2 * 8;
    Write (i / 8, TIMEY, ":");
    i += 1 * 8;
    VWB_DrawPic (i, TIMEY * 8, SPEAR::g(L_NUM0PIC) + (sec / 10));
    i += 2 * 8;
    VWB_DrawPic (i, TIMEY * 8, SPEAR::g(L_NUM0PIC) + (sec % 10));
    I_UpdateScreen ();

    itoanoreturn (kr, tempstr, 10);
    x = RATIOX + 24 - (int) strlen(tempstr) * 2;
    Write (x, RATIOY, tempstr);

    itoanoreturn (sr, tempstr, 10);
    x = RATIOX + 24 - (int) strlen(tempstr) * 2;
    Write (x, RATIOY + 2, tempstr);

    itoanoreturn (tr, tempstr, 10);
    x = RATIOX + 24 - (int) strlen(tempstr) * 2;
    Write (x, RATIOY + 4, tempstr);

#ifndef SPANISH
    // IOANCH 20130301: unification culling


    if(!SPEAR::flag)
    {
        //
        // TOTAL TIME VERIFICATION CODE
        //
        if (gamestate.difficulty >= gd_medium)
        {
            VWB_DrawPic (30 * 8, TIMEY * 8, SPEAR::g(C_TIMECODEPIC));
            fontnumber = 0;
            g_fontcolor = READHCOLOR;
            PrintX = 30 * 8 - 3;
            PrintY = TIMEY * 8 + 8;
            PrintX += 4;
            tempstr[0] = (((min / 10) ^ (min % 10)) ^ 0xa) + 'A';
            tempstr[1] = (int) ((((sec / 10) ^ (sec % 10)) ^ 0xa) + 'A');
            tempstr[2] = (tempstr[0] ^ tempstr[1]) + 'A';
            tempstr[3] = 0;
            US_Print (tempstr);
        }
    }


#endif

    fontnumber = 1;

    I_UpdateScreen ();
    VW_FadeIn ();

    myInput.ack ();

    VW_FadeOut ();
    if(cfg_screenHeight % 200 != 0)
        I_ClearScreen(0);

    if(!SPEAR::flag)
        graphSegs.uncacheChunk (SPEAR::g(C_TIMECODEPIC));
    UnCacheLump (SPEAR::g(LEVELEND_LUMP_START), SPEAR::g(LEVELEND_LUMP_END));


    if(!SPEAR::flag)
        EndText ();
    else
        EndSpear ();



}


//==========================================================================
// IOANCH 20130301: unification culling




//==========================================================================

void Write (int x, int y, const char *string)
{
    static const unsigned int alpha[] = { SPEAR::g(L_NUM0PIC),
        SPEAR::g(L_NUM1PIC), SPEAR::g(L_NUM2PIC), 
        SPEAR::g(L_NUM3PIC), SPEAR::g(L_NUM4PIC), 
        SPEAR::g(L_NUM5PIC), SPEAR::g(L_NUM6PIC), 
        SPEAR::g(L_NUM7PIC), SPEAR::g(L_NUM8PIC), 
        SPEAR::g(L_NUM9PIC), SPEAR::g(L_COLONPIC), 0, 0, 0, 0, 0, 0, 
        SPEAR::g(L_APIC), SPEAR::g(L_BPIC), SPEAR::g(L_CPIC), 
        SPEAR::g(L_DPIC), SPEAR::g(L_EPIC), SPEAR::g(L_FPIC), 
        SPEAR::g(L_GPIC), SPEAR::g(L_HPIC), SPEAR::g(L_IPIC),
        SPEAR::g(L_JPIC), SPEAR::g(L_KPIC), SPEAR::g(L_LPIC), 
        SPEAR::g(L_MPIC), SPEAR::g(L_NPIC), SPEAR::g(L_OPIC), 
        SPEAR::g(L_PPIC), SPEAR::g(L_QPIC), SPEAR::g(L_RPIC), 
        SPEAR::g(L_SPIC), SPEAR::g(L_TPIC), SPEAR::g(L_UPIC), 
        SPEAR::g(L_VPIC), SPEAR::g(L_WPIC), SPEAR::g(L_XPIC), 
        SPEAR::g(L_YPIC), SPEAR::g(L_ZPIC)
    };

    int i, ox, nx, ny, len = (int) strlen(string);
    char ch;

    ox = nx = x * 8;
    ny = y * 8;
    for (i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            nx = ox;
            ny += 16;
        }
        else
        {
            ch = string[i];
            if (ch >= 'a')
                ch -= ('a' - 'A');
            ch -= '0';

            switch (string[i])
            {
                case '!':
                    VWB_DrawPic (nx, ny, SPEAR::g(L_EXPOINTPIC));
                    nx += 8;
                    continue;
// IOANCH 20130301: unification culling

                case '\'':
                    VWB_DrawPic (nx, ny, SPEAR::g(L_APOSTROPHEPIC));
                    nx += 8;
                    continue;


                case ' ':
                    break;

                case 0x3a:     // ':'
                    VWB_DrawPic (nx, ny, SPEAR::g(L_COLONPIC));
                    nx += 8;
                    continue;

                case '%':
                    VWB_DrawPic (nx, ny, SPEAR::g(L_PERCENTPIC));
                    break;

                default:
                    VWB_DrawPic (nx, ny, alpha[ch]);
            }
            nx += 16;
        }
    }
}


//
// Breathe Mr. BJ!!!
//
static void BJ_Breathe ()
{
    static int which = 0, max = 10;
    int pics[2] = { static_cast<int>(SPEAR::g(L_GUYPIC)), static_cast<int>(SPEAR::g(L_GUY2PIC)) };

    I_Delay(5);

    if ((int32_t) GetTimeCount () - lastBreathTime > max)
    {
        which ^= 1;
        VWB_DrawPic (0, 16, pics[which]);
        I_UpdateScreen ();
        lastBreathTime = GetTimeCount();
        max = 35;
    }
}



/*
==================
=
= LevelCompleted
=
= Entered with the screen faded out
= Still in split screen mode with the status bar
=
= Exit with the screen faded out
=
==================
*/

void LevelCompleted ()
{
#define VBLWAIT 30
#define PAR_AMOUNT      500
#define PERCENT100AMT   10000
   struct times
    {
        float time;
        char timestr[6];
    } ;

    int x, i, min, sec, ratio, kr, sr, tr;
    char tempstr[10];
    int32_t bonus, timeleft = 0;
    // IOANCH 20130302: unification
    times parTimes_wl6[] = {
        //
        // Episode One Par Times
        //
        {1.5, "01:30"},
        {2, "02:00"},
        {2, "02:00"},
        {3.5, "03:30"},
        {3, "03:00"},
        {3, "03:00"},
        {2.5, "02:30"},
        {2.5, "02:30"},
        {0, "??:??"},           // Boss level
        {0, "??:??"},           // Secret level

        //
        // Episode Two Par Times
        //
        {1.5, "01:30"},
        {3.5, "03:30"},
        {3, "03:00"},
        {2, "02:00"},
        {4, "04:00"},
        {6, "06:00"},
        {1, "01:00"},
        {3, "03:00"},
        {0, "??:??"},
        {0, "??:??"},

        //
        // Episode Three Par Times
        //
        {1.5, "01:30"},
        {1.5, "01:30"},
        {2.5, "02:30"},
        {2.5, "02:30"},
        {3.5, "03:30"},
        {2.5, "02:30"},
        {2, "02:00"},
        {6, "06:00"},
        {0, "??:??"},
        {0, "??:??"},

        //
        // Episode Four Par Times
        //
        {2, "02:00"},
        {2, "02:00"},
        {1.5, "01:30"},
        {1, "01:00"},
        {4.5, "04:30"},
        {3.5, "03:30"},
        {2, "02:00"},
        {4.5, "04:30"},
        {0, "??:??"},
        {0, "??:??"},

        //
        // Episode Five Par Times
        //
        {2.5, "02:30"},
        {1.5, "01:30"},
        {2.5, "02:30"},
        {2.5, "02:30"},
        {4, "04:00"},
        {3, "03:00"},
        {4.5, "04:30"},
        {3.5, "03:30"},
        {0, "??:??"},
        {0, "??:??"},

        //
        // Episode Six Par Times
        //
        {6.5, "06:30"},
        {4, "04:00"},
        {4.5, "04:30"},
        {6, "06:00"},
        {5, "05:00"},
        {5.5, "05:30"},
        {5.5, "05:30"},
        {8.5, "08:30"},
        {0, "??:??"},
        {0, "??:??"}
    };
    times parTimes_sod[] = {

        //
        // SPEAR::flag OF DESTINY TIMES
        //
        {1.5, "01:30"},
        {3.5, "03:30"},
        {2.75, "02:45"},
        {3.5, "03:30"},
        {0, "??:??"},           // Boss 1
        {4.5, "04:30"},
        {3.25, "03:15"},
        {2.75, "02:45"},
        {4.75, "04:45"},
        {0, "??:??"},           // Boss 2
        {6.5, "06:30"},
        {4.5, "04:30"},
        {2.75, "02:45"},
        {4.5, "04:30"},
        {6, "06:00"},
        {0, "??:??"},           // Boss 3
        {6, "06:00"},
        {0, "??:??"},           // Boss 4
        {0, "??:??"},           // Secret level 1
        {0, "??:??"},           // Secret level 2
    };
    times *parTimes = IMPALE(parTimes);

    CacheLump (SPEAR::g(LEVELEND_LUMP_START), SPEAR::g(LEVELEND_LUMP_END));
    ClearSplitVWB ();           // set up for double buffering in split screen
    VL_Bar (0, 0, 320, cfg_screenHeight / vid_scaleFactor - STATUSLINES + 1, VIEWCOLOR);

    if (bordercol != VIEWCOLOR)
        DrawStatusBorder (VIEWCOLOR);
            // IOANCH 20130301: unification music
    StartCPMusic (IMPALE((int)ENDLEVEL_MUS));

//
// do the intermission
//
    myInput.clearKeysDown ();
    myInput.startAck ();
// IOANCH 20130301: unification culling
    VWB_DrawPic (0, 16, SPEAR::g(L_GUYPIC));

    if ((!SPEAR::flag && mapSegs.map() < 8) || (SPEAR::flag && mapSegs.map() != 4 && mapSegs.map() != 9 && mapSegs.map() != 15 && mapSegs.map() < 17))
    {
        // IOANCH 20130301: unification culling
#ifdef SPANISH
        Write (14, 2, "piso\ncompletado");
#else
        Write (14, 2, "floor\ncompleted");
#endif

        Write (14, 7, STR_BONUS "     0");
        Write (16, 10, STR_TIME);
        Write (16, 12, STR_PAR);

#ifdef SPANISH
        Write (11, 14, STR_RAT2KILL);
        Write (11, 16, STR_RAT2SECRET);
        Write (11, 18, STR_RAT2TREASURE);
#else
        Write (9, 14, STR_RAT2KILL);
        Write (5, 16, STR_RAT2SECRET);
        Write (1, 18, STR_RAT2TREASURE);
#endif

        Write (26, 2, itoa (gamestate.mapon + 1, tempstr, 10));


#ifdef SPANISH
        Write (30, 12, parTimes[gamestate.episode * 10 + mapSegs.map()].timestr);
#else
        Write (26, 12, parTimes[gamestate.episode * 10 + mapSegs.map()].timestr);
#endif

        //
        // PRINT TIME
        //
        sec = gamestate.TimeCount / 70;

        if (sec > 99 * 60)      // 99 minutes max
            sec = 99 * 60;

        if (gamestate.TimeCount < parTimes[gamestate.episode * 10 + mapSegs.map()].time * 4200)
            timeleft = (int32_t) ((parTimes[gamestate.episode * 10 + mapSegs.map()].time * 4200) / 70 - sec);

        min = sec / 60;
        sec %= 60;

#ifdef SPANISH
        i = 30 * 8;
#else
        i = 26 * 8;
#endif
        VWB_DrawPic (i, 10 * 8, SPEAR::g(L_NUM0PIC) + (min / 10));
        i += 2 * 8;
        VWB_DrawPic (i, 10 * 8, SPEAR::g(L_NUM0PIC) + (min % 10));
        i += 2 * 8;
        Write (i / 8, 10, ":");
        i += 1 * 8;
        VWB_DrawPic (i, 10 * 8, SPEAR::g(L_NUM0PIC) + (sec / 10));
        i += 2 * 8;
        VWB_DrawPic (i, 10 * 8, SPEAR::g(L_NUM0PIC) + (sec % 10));

        I_UpdateScreen ();
        VW_FadeIn ();


        //
        // FIGURE RATIOS OUT BEFOREHAND
        //
        kr = sr = tr = 0;
        if (gamestate.killtotal)
            kr = (gamestate.killcount * 100) / gamestate.killtotal;
        if (gamestate.secrettotal)
            sr = (gamestate.secretcount * 100) / gamestate.secrettotal;
        if (gamestate.treasuretotal)
            tr = (gamestate.treasurecount * 100) / gamestate.treasuretotal;


        //
        // PRINT TIME BONUS
        //
        bonus = timeleft * PAR_AMOUNT;
        if (bonus)
        {
            for (i = 0; i <= timeleft; i++)
            {
                ltoanoreturn ((long int)((int32_t) i * PAR_AMOUNT), tempstr, 10);
                x = 36 - (int) strlen(tempstr) * 2;
                Write (x, 7, tempstr);
                if (!(i % (PAR_AMOUNT / 10)))
                    Sound::Play (ENDBONUS1SND);
                if(!cfg_usedoublebuffering || !(i % (PAR_AMOUNT / 50))) I_UpdateScreen ();
                while(SD_SoundPlaying ())
                    BJ_Breathe ();
                if (myInput.checkAck ())
                    goto done;
            }

            I_UpdateScreen ();

            Sound::Play (ENDBONUS2SND);
            while (SD_SoundPlaying ())
                BJ_Breathe ();
        }


#ifdef SPANISH
#define RATIOXX                33
#else
#define RATIOXX                37
#endif
        //
        // KILL RATIO
        //
        ratio = kr;
        for (i = 0; i <= ratio; i++)
        {
            itoanoreturn (i, tempstr, 10);
            x = RATIOXX - (int) strlen(tempstr) * 2;
            Write (x, 14, tempstr);
            if (!(i % 10))
                Sound::Play (ENDBONUS1SND);
            if(!cfg_usedoublebuffering || !(i & 1)) I_UpdateScreen ();
            while (SD_SoundPlaying ())
                BJ_Breathe ();

            if (myInput.checkAck ())
                goto done;
        }
        if (ratio >= 100)
        {
            VL_WaitVBL (VBLWAIT);
            SD_StopSound ();
            bonus += PERCENT100AMT;
            ltoanoreturn ((long int)bonus, tempstr, 10);
            x = (RATIOXX - 1) - (int) strlen(tempstr) * 2;
            Write (x, 7, tempstr);
            I_UpdateScreen ();
            Sound::Play (PERCENT100SND);
        }
        else if (!ratio)
        {
            VL_WaitVBL (VBLWAIT);
            SD_StopSound ();
            Sound::Play (NOBONUSSND);
        }
        else
            Sound::Play (ENDBONUS2SND);

        I_UpdateScreen ();
        while (SD_SoundPlaying ())
            BJ_Breathe ();

        //
        // SECRET RATIO
        //
        ratio = sr;
        for (i = 0; i <= ratio; i++)
        {
            itoanoreturn (i, tempstr, 10);
            x = RATIOXX - (int) strlen(tempstr) * 2;
            Write (x, 16, tempstr);
            if (!(i % 10))
                Sound::Play (ENDBONUS1SND);
            if(!cfg_usedoublebuffering || !(i & 1)) I_UpdateScreen ();
            while (SD_SoundPlaying ())
                BJ_Breathe ();

            if (myInput.checkAck ())
                goto done;
        }
        if (ratio >= 100)
        {
            VL_WaitVBL (VBLWAIT);
            SD_StopSound ();
            bonus += PERCENT100AMT;
            ltoanoreturn ((long int)bonus, tempstr, 10);
            x = (RATIOXX - 1) - (int) strlen(tempstr) * 2;
            Write (x, 7, tempstr);
            I_UpdateScreen ();
            Sound::Play (PERCENT100SND);
        }
        else if (!ratio)
        {
            VL_WaitVBL (VBLWAIT);
            SD_StopSound ();
            Sound::Play (NOBONUSSND);
        }
        else
            Sound::Play (ENDBONUS2SND);
        I_UpdateScreen ();
        while (SD_SoundPlaying ())
            BJ_Breathe ();

        //
        // TREASURE RATIO
        //
        ratio = tr;
        for (i = 0; i <= ratio; i++)
        {
            itoanoreturn (i, tempstr, 10);
            x = RATIOXX - (int) strlen(tempstr) * 2;
            Write (x, 18, tempstr);
            if (!(i % 10))
                Sound::Play (ENDBONUS1SND);
            if(!cfg_usedoublebuffering || !(i & 1)) I_UpdateScreen ();
            while (SD_SoundPlaying ())
                BJ_Breathe ();
            if (myInput.checkAck ())
                goto done;
        }
        if (ratio >= 100)
        {
            VL_WaitVBL (VBLWAIT);
            SD_StopSound ();
            bonus += PERCENT100AMT;
            ltoanoreturn ((long int)bonus, tempstr, 10);
            x = (RATIOXX - 1) - (int) strlen(tempstr) * 2;
            Write (x, 7, tempstr);
            I_UpdateScreen ();
            Sound::Play (PERCENT100SND);
        }
        else if (!ratio)
        {
            VL_WaitVBL (VBLWAIT);
            SD_StopSound ();
            Sound::Play (NOBONUSSND);
        }
        else
            Sound::Play (ENDBONUS2SND);
        I_UpdateScreen ();
        while (SD_SoundPlaying ())
            BJ_Breathe ();


        //
        // JUMP STRAIGHT HERE IF KEY PRESSED
        //
done:   itoanoreturn (kr, tempstr, 10);
        x = RATIOXX - (int) strlen(tempstr) * 2;
        Write (x, 14, tempstr);

        itoanoreturn (sr, tempstr, 10);
        x = RATIOXX - (int) strlen(tempstr) * 2;
        Write (x, 16, tempstr);

        itoanoreturn (tr, tempstr, 10);
        x = RATIOXX - (int) strlen(tempstr) * 2;
        Write (x, 18, tempstr);

        bonus = (int32_t) timeleft *PAR_AMOUNT +
            (PERCENT100AMT * (kr >= 100)) +
            (PERCENT100AMT * (sr >= 100)) + (PERCENT100AMT * (tr >= 100));

        GivePoints (bonus);
        ltoanoreturn ((long int)bonus, tempstr, 10);
        x = 36 - (int) strlen(tempstr) * 2;
        Write (x, 7, tempstr);

        //
        // SAVE RATIO INFORMATION FOR ENDGAME
        //
        LevelRatios[mapSegs.map()].kill = kr;
        LevelRatios[mapSegs.map()].secret = sr;
        LevelRatios[mapSegs.map()].treasure = tr;
        LevelRatios[mapSegs.map()].time = min * 60 + sec;
    }
    else
    {
// IOANCH 20130301: unification culling
        if(SPEAR::flag) 
        {
            switch (mapSegs.map())
            {
                case 4:
                    Write (14, 4, " trans\n" " grosse\n" STR_DEFEATED);
                    break;
                case 9:
                    Write (14, 4, "barnacle\n" "wilhelm\n" STR_DEFEATED);
                    break;
                case 15:
                    Write (14, 4, "ubermutant\n" STR_DEFEATED);
                    break;
                case 17:
                    Write (14, 4, " death\n" " knight\n" STR_DEFEATED);
                    break;
                case 18:
                    Write (13, 4, "secret tunnel\n" "    area\n" "  completed!");
                    break;
                case 19:
                    Write (13, 4, "secret castle\n" "    area\n" "  completed!");
                    break;
            }
        }
        else
            Write (14, 4, "secret floor\n completed!");

        Write (10, 16, "15000 bonus!");

        I_UpdateScreen ();
        VW_FadeIn ();

        GivePoints (15000);
    }


    DrawScore ();
    I_UpdateScreen ();

    lastBreathTime = GetTimeCount();
    myInput.startAck ();

	 // IOANCH 27.05.2012: let bots automatically hit after 1 second
	int botcount = 0;
    while (!myInput.checkAck ())
	{
        BJ_Breathe ();
        if(cfg_botActive)
		{
			  if(++botcount == 200) // IOANCH 20130607: reduced to 1 second
				  break;
		  }
	 }

//
// done
//
    // IOANCH 20130301: unification culling



    VW_FadeOut ();
    DrawPlayBorder();

    UnCacheLump (SPEAR::g(LEVELEND_LUMP_START), SPEAR::g(LEVELEND_LUMP_END));
}



//==========================================================================


/*
=================
=
= PreloadGraphics
=
= Fill the cache up
=
=================
*/

static bool PreloadUpdate (unsigned current, unsigned total)
{
    unsigned w = WindowW - vid_scaleFactor * 10;

    VL_BarScaledCoord (WindowX + vid_scaleFactor * 5, WindowY + WindowH - vid_scaleFactor * 3,
        w, vid_scaleFactor * 2, BLACK);
    w = ((int32_t) w * current) / total;
    if (w)
    {
        VL_BarScaledCoord (WindowX + vid_scaleFactor * 5, WindowY + WindowH - vid_scaleFactor * 3,
            w, vid_scaleFactor * 2, 0x37);       //SECONDCOLOR);
        VL_BarScaledCoord (WindowX + vid_scaleFactor * 5, WindowY + WindowH - vid_scaleFactor * 3,
            w - vid_scaleFactor * 1, vid_scaleFactor * 1, 0x32);

    }
    I_UpdateScreen ();
//      if (myInput.lastScan() == sc_Escape)
//      {
//              myInput.clearKeysDown();
//              return(true);
//      }
//      else
    return (false);
}

void PreloadGraphics ()
{
    DrawLevel ();
    ClearSplitVWB ();           // set up for double buffering in split screen

    VL_BarScaledCoord (0, 0, cfg_screenWidth, cfg_screenHeight - vid_scaleFactor * (STATUSLINES - 1), bordercol);
    LatchDrawPicScaledCoord ((cfg_screenWidth-vid_scaleFactor*224)/16,
        (cfg_screenHeight-vid_scaleFactor*(STATUSLINES+48))/2, SPEAR::g(GETPSYCHEDPIC));

    WindowX = (cfg_screenWidth - vid_scaleFactor*224)/2;
    WindowY = (cfg_screenHeight - vid_scaleFactor*(STATUSLINES+48))/2;
    WindowW = vid_scaleFactor * 28 * 8;
    WindowH = vid_scaleFactor * 48;

    I_UpdateScreen ();
    VW_FadeIn ();

//      PM_Preload (PreloadUpdate);
    PreloadUpdate (10, 10);
    myInput.userInput (70);
    VW_FadeOut ();

    DrawPlayBorder ();
    I_UpdateScreen ();
}


//==========================================================================

/*
==================
=
= DrawHighScores
=
==================
*/

void DrawHighScores ()
{
    char buffer[16];

    char *str;
    // IOANCH 20130301: unification culling

    char buffer1[5];


    word i, w, h;
    HighScore *s;

    if(!SPEAR::flag)
    {
        graphSegs.cacheChunk (SPEAR::g(HIGHSCORESPIC));
        graphSegs.cacheChunk (SPEAR::g(STARTFONT));
        // IOANCH 20130301: unification culling

        graphSegs.cacheChunk (SPEAR::g(C_LEVELPIC));
        graphSegs.cacheChunk (SPEAR::g(C_SCOREPIC));
        graphSegs.cacheChunk (SPEAR::g(C_NAMEPIC));




        ClearMScreen ();
        DrawStripes (10);

        VWB_DrawPic (48, 0, SPEAR::g(HIGHSCORESPIC));
        graphSegs.uncacheChunk (SPEAR::g(HIGHSCORESPIC));

        // IOANCH 20130301: unification culling

        VWB_DrawPic (4 * 8, 68, SPEAR::g(C_NAMEPIC));
        VWB_DrawPic (20 * 8, 68, SPEAR::g(C_LEVELPIC));
        VWB_DrawPic (28 * 8, 68, SPEAR::g(C_SCOREPIC));



        fontnumber = 0;
    }
    else
    {
        CacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
        ClearMScreen ();
        DrawStripes (10);
        UnCacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));

        CacheLump (SPEAR::g(HIGHSCORES_LUMP_START), SPEAR::g(HIGHSCORES_LUMP_END));
        graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);
        VWB_DrawPic (0, 0, SPEAR::g(HIGHSCORESPIC));

        fontnumber = 1;
    }

    if(!SPEAR::flag)
    {
        SETFONTCOLOR (15, 0x29);
    }
    else
    {
        SETFONTCOLOR (HIGHLIGHT, 0x29);
    }

    for (i = 0, s = Scores; i < MaxScores; i++, s++)
    {
        PrintY = 76 + (16 * i);

        //
        // name
        //
        if(!SPEAR::flag)
            PrintX = 4 * 8;
        else
            PrintX = 16;

        US_Print (s->name);

        //
        // level
        //
        itoanoreturn (s->completed, buffer, 10);
        // IOANCH 20130303
        if(!SPEAR::flag)
        {
            for (str = buffer; *str; str++)
                *str = *str + (129 - '0');  // Used fixed-width numbers (129...)
            USL_MeasureString (buffer, &w, &h);
            PrintX = (22 * 8) - w;
        }
        else
        {
            USL_MeasureString (buffer, &w, &h);
            PrintX = 194 - w;
        }
// IOANCH 20130301: unification culling

        if(!SPEAR::flag)
        {
            PrintX -= 6;
            itoanoreturn (s->episode + 1, buffer1, 10);
            US_Print ("E");
            US_Print (buffer1);
            US_Print ("/L");
        }

        if(SPEAR::flag)
        {
            if (s->completed == 21)
                VWB_DrawPic (PrintX + 8, PrintY - 1, SPEAR::g(C_WONSPEARPIC));
            else
                US_Print (buffer);
        }
        else
            US_Print (buffer);

        //
        // score
        //
        itoanoreturn (s->score, buffer, 10);
        if(!SPEAR::flag)
        {
            for (str = buffer; *str; str++)
                *str = *str + (129 - '0');  // Used fixed-width numbers (129...)
            USL_MeasureString (buffer, &w, &h);
            PrintX = (34 * 8) - 8 - w;
        }
        else
        {
            USL_MeasureString (buffer, &w, &h);
            PrintX = 292 - w;
        }
        US_Print (buffer);

        // IOANCH 20130301: unification culling

    }

    I_UpdateScreen ();

    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(HIGHSCORES_LUMP_START), SPEAR::g(HIGHSCORES_LUMP_END));
        fontnumber = 0;
    }
}

//===========================================================================


/*
=======================
=
= CheckHighScore
=
=======================
*/

void CheckHighScore (int32_t score, word other)
{
    word i, j;
    int n;
    HighScore myscore;

    strcpy (myscore.name, "");
    myscore.score = score;
    myscore.episode = gamestate.episode;
    myscore.completed = other;

    for (i = 0, n = -1; i < MaxScores; i++)
    {
        if ((myscore.score > Scores[i].score)
            || ((myscore.score == Scores[i].score) && (myscore.completed > Scores[i].completed)))
        {
            for (j = MaxScores; --j > i;)
                Scores[j] = Scores[j - 1];
            Scores[i] = myscore;
            n = i;
            break;
        }
    }
            // IOANCH 20130301: unification music
    if (SPEAR::flag)
        StartCPMusic (XAWARD_MUS_sod);
    else
        StartCPMusic (ROSTER_MUS_wl6);

    DrawHighScores ();

    VW_FadeIn ();

    if (n != -1)
    {
        //
        // got a high score
        //
        PrintY = 76 + (16 * n);

        if(!SPEAR::flag)
        {
            PrintX = 4 * 8;
            g_backcolor = Menu::g_bordColor;
            g_fontcolor = 15;
		      // IOANCH 27.05.2012: let the bot write his random name
		      if(cfg_botActive)
		      {
			      Basic::MarkovWrite(Scores[n].name, 10);	// maximum 10 chars
			      US_Print(Scores[n].name);
			      I_UpdateScreen();

			      myInput.clearKeysDown ();
			      myInput.userInput (500);
		      }
		      else
			    US_LineInput (PrintX, PrintY, Scores[n].name, 0, true, MaxHighName, 100);
        }
        else
        {

            PrintX = 16;
            fontnumber = 1;
            VL_Bar (PrintX - 2, PrintY - 2, 145, 15, 0x9c);
            I_UpdateScreen ();
            g_backcolor = 0x9c;
            g_fontcolor = 15;
		      // IOANCH 27.05.2012: let the bot write his random name
		      if(cfg_botActive)
		      {
			      Basic::MarkovWrite(Scores[n].name, 10);	// maximum 10 chars
			      US_Print(Scores[n].name);
			      I_UpdateScreen();

			      myInput.clearKeysDown ();
			      myInput.userInput (500);
		      }
		      else
			    US_LineInput (PrintX, PrintY, Scores[n].name, 0, true, MaxHighName, 130);
        }

    }
    else
    {
        myInput.clearKeysDown ();
        myInput.userInput (500);
    }

}

// IOANCH 20130301: unification culling

// IOANCH 20130301: unification culling
//===========================================================================
