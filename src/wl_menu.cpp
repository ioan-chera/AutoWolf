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
// WL_MENU.C
// by John Romero (C) 1992 Id Software, Inc.
//
// IOANCH: menu control
//
////////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include "Logger.h"
#include "MasterDirectoryFile.h"
#include "PString.h"
#include "SODFlag.h"
#include "ShellUnicode.h"
#include "foreign.h"
#include "i_system.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_sd.h"
#include "id_vh.h"
#include "ioan_bot.h"	// IOANCH 20121217: bot
#include "wl_agent.h"
#include "wl_def.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_inter.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "wl_play.h"
#include "wl_text.h"

int Menu::g_bkgdColor;
int Menu::g_bordColor;

static const int DEACTIVE_wl6 = 0x2b;
static const int DEACTIVE_sod = 0x9b;
static const int STRIPE = 0x2c;

static int s_bord2Color;
static int s_deactive;

// IOANCH 20130303: Cocoa functions for Apple computers
#include "macosx/CocoaFun.h"

enum {MOUSE,JOYSTICK,KEYBOARDBTNS,KEYBOARDMOVE};        // FOR INPUT TYPES

struct CustomCtrls
{
    short allowed[4];
} ;

extern int lastgamemusicoffset;
// IOANCH: made external

// IOANCH 20130301: unification culling
#define STARTITEM       newgame


// ENDSTRx constants are defined in foreign.h
// IOANCH 20130202: unification process
char endStrings[18][80] = {
    ENDSTR1,
    ENDSTR2,
    ENDSTR3,
    ENDSTR4,
    ENDSTR5,
    ENDSTR6,
    ENDSTR7,
    ENDSTR8,
    ENDSTR9,
    ENDSTR1SPEAR,
    ENDSTR2SPEAR,
    ENDSTR3SPEAR,
    ENDSTR4SPEAR,
    ENDSTR5SPEAR,
    ENDSTR6SPEAR,
    ENDSTR7SPEAR,
    ENDSTR8SPEAR,
    ENDSTR9SPEAR
};

static int CP_NewGame(int);
static int CP_Sound(int);
static int CP_LoadGame(int quick);
static int CP_SaveGame(int quick);
static int CP_Control(int);
static int CP_ChangeView(int);

CP_itemtype MainMenu[] = {
    // IOANCH 20130301: unification culling

    {1, STR_NG, CP_NewGame},
    {1, STR_SD, CP_Sound},
    {1, STR_CL, CP_Control},
    {1, STR_LG, CP_LoadGame},
    {0, STR_SG, CP_SaveGame},
    {1, STR_CV, CP_ChangeView},

    // IOANCH 20130301: unification culling

    {1, STR_VS, CP_ViewScores},
    {1, STR_BD, 0},
    {1, STR_QT, 0}

};

CP_itemtype SndMenu[] = {
    // IOANCH 20130301: unification culling
    {1, STR_NONE, 0},
    {0, STR_PC, 0},
    {1, STR_ALSB, 0},
    {0, "", 0},
    {0, "", 0},
    {1, STR_NONE, 0},
    {0, STR_DISNEY, 0},
    {1, STR_SB, 0},
    {0, "", 0},
    {0, "", 0},
    {1, STR_NONE, 0},
    {1, STR_ALSB, 0}
};
// IOANCH 20130301: unification culling
enum { CTL_MOUSEENABLE, CTL_MOUSESENS, CTL_JOYENABLE, CTL_CUSTOMIZE };

static int CustomControls(int);
static int MouseSensitivity(int);

CP_itemtype CtlMenu[] = {
    // IOANCH 20130301: unification culling
    {0, STR_MOUSEEN, 0},
    {0, STR_SENS, MouseSensitivity},
    {0, STR_JOYEN, 0},
    {1, STR_CUSTOM, CustomControls}
};

// IOANCH 20130303: unification
CP_itemtype menu_newep[] = {
    // IOANCH 20130301: unification culling
#ifdef SPANISH
    {1, "Episodio 1\n" "Fuga desde Wolfenstein", 0},
    {0, "", 0},
    {3, "Episodio 2\n" "Operacion Eisenfaust", 0},
    {0, "", 0},
    {3, "Episodio 3\n" "Muere, Fuhrer, Muere!", 0},
    {0, "", 0},
    {3, "Episodio 4\n" "Un Negro Secreto", 0},
    {0, "", 0},
    {3, "Episodio 5\n" "Huellas del Loco", 0},
    {0, "", 0},
    {3, "Episodio 6\n" "Confrontacion", 0}
#else
    {1, "Episode 1\n" "Escape from Wolfenstein", 0},
    {0, "", 0},
    {3, "Episode 2\n" "Operation: Eisenfaust", 0},
    {0, "", 0},
    {3, "Episode 3\n" "Die, Fuhrer, Die!", 0},
    {0, "", 0},
    {3, "Episode 4\n" "A Dark Secret", 0},
    {0, "", 0},
    {3, "Episode 5\n" "Trail of the Madman", 0},
    {0, "", 0},
    {3, "Episode 6\n" "Confrontation", 0}
#endif
};



CP_itemtype NewMenu[] = {
    // IOANCH 20130301: unification culling
    {1, STR_DADDY, 0},
    {1, STR_HURTME, 0},
    {1, STR_BRINGEM, 0},
    {1, STR_DEATH, 0}
};

CP_itemtype LSMenu[] = {
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0}
};

CP_itemtype CusMenu[] = {
    {1, "", 0},
    {0, "", 0},
    {0, "", 0},
    {1, "", 0},
    {0, "", 0},
    {0, "", 0},
    {1, "", 0},
    {0, "", 0},
    {1, "", 0}
};

// CP_iteminfo struct format: short x, y, amount, curpos, indent;
// IOANCH 20130303: unification
CP_iteminfo MainItems = { MENU_X, MENU_Y, lengthof(MainMenu), STARTITEM, 24 },
            SndItems  = { SM_X, SM_Y1, lengthof(SndMenu), 0, 52 },
            LSItems   = { LSM_X, LSM_Y, lengthof(LSMenu), 0, 24 },
            CtlItems  = { CTL_X, CTL_Y, lengthof(CtlMenu), -1, 56 },
            CusItems  = { 8, CST_Y + 13 * 2, lengthof(CusMenu), -1, 0},
            NewEitems = { NE_X, NE_Y, lengthof(menu_newep), 0, 88 },
            NewItems  = { NM_X, NM_Y, lengthof(NewMenu), 2, 24 };
// IOANCH 20130302: unification
int color_hlite_wl6[] = {
    DEACTIVE_wl6,
    HIGHLIGHT,
    READHCOLOR,
    0x67
};
int color_hlite_sod[] = {
    DEACTIVE_sod,
    HIGHLIGHT,
    READHCOLOR,
    0x67
};
int color_norml_wl6[] = {
    DEACTIVE_wl6,
    TEXTCOLOR,
    READCOLOR,
    0x6b
};
int color_norml_sod[] = {
    DEACTIVE_sod,
    TEXTCOLOR,
    READCOLOR,
    0x6b
};

int menu_epselect[6] = { 1 };


static int SaveGamesAvail[10];
static int s_StartGame;
static int SoundStatus = 1;
static int pickquick;
static char SaveGameNames[10][32];
std::string cfg_savename("SAVEGAM?.");


////////////////////////////////////////////////////////////////////
//
// INPUT MANAGER SCANCODE TABLES
//
////////////////////////////////////////////////////////////////////

// IOANCH 20130801: added meta/command/windows/super key to the names
static std::unordered_map<unsigned, std::string> ScanNames;

/*
 "?","?","?","?","?","?","?","Enter",                            // 264
 "?","Up","Down","Right","Left","Ins","Home","End",              // 272
 "PgUp","PgDn","F1","F2","F3","F4","F5","F6",                    // 280
 "F7","F8","F9","F10","F11","F12","?","?",                       // 288
 "?","?","?","?","NumLk","CapsLk","ScrlLk","RShft",              // 296
 "Shift","RCtrl","Ctrl","RAlt","Alt","RMeta","Meta","?",         // 304
 "?","?","?","?","PrtSc","?","?","?",                            // 312
 "?","?"                                                         // 320

 */

void US_SetScanNames()
{
	ScanNames[sc_Enter] = "Enter";
	ScanNames[SDLK_UP] = "Up";
	ScanNames[SDLK_DOWN] = "Down";
	ScanNames[SDLK_RIGHT] = "Right";
	ScanNames[SDLK_LEFT] = "Left";
	ScanNames[SDLK_INSERT] = "Ins";
	ScanNames[SDLK_HOME] = "Home";
	ScanNames[SDLK_END] = "End";
	ScanNames[SDLK_PAGEUP] = "PgUp";
	ScanNames[SDLK_PAGEDOWN] = "PgDn";
	ScanNames[SDLK_F1] = "F1";
	ScanNames[SDLK_F2] = "F2";
	ScanNames[SDLK_F3] = "F3";
	ScanNames[SDLK_F4] = "F4";
	ScanNames[SDLK_F5] = "F5";
	ScanNames[SDLK_F6] = "F6";
	ScanNames[SDLK_F7] = "F7";
	ScanNames[SDLK_F8] = "F8";
	ScanNames[SDLK_F9] = "F9";
	ScanNames[SDLK_F10] = "F10";
	ScanNames[SDLK_F11] = "F11";
	ScanNames[SDLK_F12] = "F12";
	ScanNames[SDLK_F13] = "F13";
	ScanNames[SDLK_F14] = "F14";
	ScanNames[SDLK_F15] = "F15";
	ScanNames[SDLK_F16] = "F16";
	ScanNames[SDLK_F17] = "F17";
	ScanNames[SDLK_F18] = "F18";
	ScanNames[SDLK_F19] = "F19";
	ScanNames[SDLK_NUMLOCKCLEAR] = "NumLk";
	ScanNames[SDLK_CAPSLOCK] = "CapsLk";
	ScanNames[SDLK_SCROLLLOCK] = "ScrlLk";
	ScanNames[SDLK_RSHIFT] = "RShft";
	ScanNames[SDLK_LSHIFT] = "Shift";
	ScanNames[SDLK_RCTRL] = "RCtrl";
	ScanNames[SDLK_LCTRL] = "LCtrl";
	ScanNames[SDLK_RALT] = "RAlt";
	ScanNames[SDLK_LALT] = "Alt";
	ScanNames[SDLK_RGUI] = "RMeta";
	ScanNames[SDLK_LGUI] = "LMeta";
	ScanNames[SDLK_PRINTSCREEN] = "PrtSc";
	ScanNames[sc_BackSpace] = "BkSp";
	ScanNames[sc_Tab] = "Tab";
	ScanNames[sc_Return] = "Return";
	ScanNames[SDLK_PAUSE] = "Pause";
	ScanNames[sc_Escape] = "Esc";
	ScanNames[sc_Space] = "Space";
	ScanNames[SDLK_EXCLAIM] = "!";
	ScanNames[SDLK_QUOTEDBL] = "\"";
	ScanNames[SDLK_HASH] = "#";
	ScanNames[SDLK_DOLLAR] = "$";
	ScanNames[SDLK_QUESTION] = "?";
	ScanNames[SDLK_AMPERSAND] = "&";
	ScanNames[SDLK_QUOTE] = "'";
	ScanNames[SDLK_LEFTPAREN] = "(";
	ScanNames[SDLK_RIGHTPAREN] = ")";
	ScanNames[SDLK_ASTERISK] = "*";
	ScanNames[SDLK_PLUS] = "+";
	ScanNames[SDLK_COMMA] = ",";
	ScanNames[SDLK_MINUS] = "-";
	ScanNames[SDLK_PERIOD] = ".";
	ScanNames[SDLK_SLASH] = "/";
	ScanNames[SDLK_0] = "0";
	ScanNames[SDLK_1] = "1";
	ScanNames[SDLK_2] = "2";
	ScanNames[SDLK_3] = "3";
	ScanNames[SDLK_4] = "4";
	ScanNames[SDLK_5] = "5";
	ScanNames[SDLK_6] = "6";
	ScanNames[SDLK_7] = "7";
	ScanNames[SDLK_8] = "8";
	ScanNames[SDLK_9] = "9";
	ScanNames[SDLK_COLON] = ":";
	ScanNames[SDLK_SEMICOLON] = ";";
	ScanNames[SDLK_LESS] = "<";
	ScanNames[SDLK_EQUALS] = "=";
	ScanNames[SDLK_GREATER] = ">";
	ScanNames[SDLK_AT] = "@";
	ScanNames[SDLK_a] = "A";
	ScanNames[SDLK_b] = "B";
	ScanNames[SDLK_c] = "C";
	ScanNames[SDLK_d] = "D";
	ScanNames[SDLK_e] = "E";
	ScanNames[SDLK_f] = "F";
	ScanNames[SDLK_g] = "G";
	ScanNames[SDLK_h] = "H";
	ScanNames[SDLK_i] = "I";
	ScanNames[SDLK_j] = "J";
	ScanNames[SDLK_k] = "K";
	ScanNames[SDLK_l] = "L";
	ScanNames[SDLK_m] = "M";
	ScanNames[SDLK_n] = "N";
	ScanNames[SDLK_o] = "O";
	ScanNames[SDLK_p] = "P";
	ScanNames[SDLK_q] = "Q";
	ScanNames[SDLK_r] = "R";
	ScanNames[SDLK_s] = "S";
	ScanNames[SDLK_t] = "T";
	ScanNames[SDLK_u] = "U";
	ScanNames[SDLK_v] = "V";
	ScanNames[SDLK_w] = "W";
	ScanNames[SDLK_x] = "X";
	ScanNames[SDLK_y] = "Y";
	ScanNames[SDLK_z] = "Z";
	ScanNames[SDLK_LEFTBRACKET] = "[";
	ScanNames[SDLK_BACKSLASH] = "\\";
	ScanNames[SDLK_RIGHTBRACKET] = "]";
}

////////////////////////////////////////////////////////////////////
//
// Wolfenstein Control Panel!  Ta Da!
//
////////////////////////////////////////////////////////////////////
static void CleanupControlPanel ();
static int CP_CheckQuick (ScanCode scancode);
static void DrawMainMenu ();
static void SetupControlPanel ();
static int CP_Quit(int);
static int CP_EndGame(int);
static int HandleMenu(CP_iteminfo *item_i, const CP_itemtype *items, void (*routine)(int w));


void Menu::ControlPanel (ScanCode scancode)
{
    int which;

#ifdef _arch_dreamcast
    DC_StatusClearLCD();
#endif

    if (ingame)
    {
        if (CP_CheckQuick (scancode))
            return;
        lastgamemusicoffset = StartCPMusic (MENUSONG);
    }
    else
        StartCPMusic (MENUSONG);
    SetupControlPanel ();

    //
    // F-KEYS FROM WITHIN GAME
    //
    switch (scancode)
    {
        case sc_F1:
			// Used to be a boss key here, it never did anything past the DOS days
            goto finishup;

        case sc_F2:
            CP_SaveGame (0);
            goto finishup;

        case sc_F3:
            CP_LoadGame (0);
            goto finishup;

        case sc_F4:
            CP_Sound (0);
            goto finishup;

        case sc_F5:
            CP_ChangeView (0);
            goto finishup;

        case sc_F6:
            CP_Control (0);
            goto finishup;

        finishup:
            CleanupControlPanel ();
            // IOANCH 20130303: unification
            if(SPEAR::flag) 
                UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
            return;
          default:
          ;
    }

    if(SPEAR::flag)
        CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));

    DrawMainMenu ();
    MenuFadeIn ();
    s_StartGame = 0;

    //
    // MAIN MENU LOOP
    //
    do
    {
        which = HandleMenu (&MainItems, &MainMenu[0], NULL);

// IOANCH 20130301: unification culling
        if(SPEAR::flag)
        {
            myInput.processEvents();

            //
            // EASTER EGG FOR SPEAR::flag OF DESTINY!
            //
            if (myInput.keyboard(sc_I) && myInput.keyboard(sc_D))
            {
                VW_FadeOut ();
                            // IOANCH 20130301: unification music
                StartCPMusic (XJAZNAZI_MUS_sod);
                UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
                UnCacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
                Sound::StopDigitized ();


                graphSegs.cacheChunk (SPEAR::g(IDGUYS1PIC));
                VWB_DrawPic (0, 0, SPEAR::g(IDGUYS1PIC));
                graphSegs.uncacheChunk (SPEAR::g(IDGUYS1PIC));

                graphSegs.cacheChunk (SPEAR::g(IDGUYS2PIC));
                VWB_DrawPic (0, 80, SPEAR::g(IDGUYS2PIC));
                graphSegs.uncacheChunk (SPEAR::g(IDGUYS2PIC));

                I_UpdateScreen ();

                SDL_Color pal[256];
                graphSegs.cacheChunk (SPEAR::g(IDGUYSPALETTE));
                VL_ConvertPalette(graphSegs[SPEAR::g(IDGUYSPALETTE)], pal, 256);
                VL_FadeIn (0, 255, pal, 30);
                graphSegs.uncacheChunk (SPEAR::g(IDGUYSPALETTE));

                while (myInput.keyboard(sc_I) || myInput.keyboard(sc_D))
                    myInput.waitAndProcessEvents();
                myInput.clearKeysDown ();
                myInput.ack ();

                VW_FadeOut ();

                CacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
                CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
                DrawMainMenu ();
                StartCPMusic (MENUSONG);
                MenuFadeIn ();
            }
        }

        switch (which)
        {
            case viewscores:
                if (MainMenu[viewscores].routine == NULL)
                {
                    if (CP_EndGame (0))
                        s_StartGame = 1;
                }
                else
                {
                    DrawMainMenu();
                    MenuFadeIn ();
                }
                break;

            case backtodemo:
                s_StartGame = 1;
                if (!ingame)
                    StartCPMusic (INTROSONG);
                VL_FadeOut (0, 255, 0, 0, 0, 10);
                break;

            case -1:
            case quit:
                CP_Quit (0);
                break;

            default:
                if (!s_StartGame)
                {
                    DrawMainMenu ();
                    MenuFadeIn ();
                }
        }

        //
        // "EXIT OPTIONS" OR "NEW GAME" EXITS
        //
    }
    while (!s_StartGame);

    //
    // DEALLOCATE EVERYTHING
    //
    CleanupControlPanel ();

    //
    // CHANGE MAINMENU ITEM
    //
    if (startgame || loadedgame)
        EnableEndGameMenuItem();

    // RETURN/START GAME EXECUTION

    if(SPEAR::flag)
        UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
}

void EnableEndGameMenuItem()
{
    MainMenu[viewscores].routine = NULL;
    // IOANCH 20130301: unification culling

    strcpy (MainMenu[viewscores].string, STR_EG);

}

////////////////////////
//
// DRAW MAIN MENU SCREEN
//
static void DrawWindow(int x,int y,int w,int h,int wcolor);

static void DrawMainMenu ()
{
    // IOANCH 20130301: unification culling
    ClearMScreen ();

    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));
    DrawStripes (10);
    VWB_DrawPic (84, 0, SPEAR::g(C_OPTIONSPIC));

#ifdef SPANISH
    DrawWindow (MENU_X - 8, MENU_Y - 3, MENU_W + 8, MENU_H, Menu::g_bkgdColor);
#else
    DrawWindow (MENU_X - 8, MENU_Y - 3, MENU_W, MENU_H, Menu::g_bkgdColor);
#endif

    //
    // CHANGE "GAME" AND "DEMO"
    //
    if (ingame)
    {
        // IOANCH 20130301: unification culling


#ifdef SPANISH
        strcpy (&MainMenu[backtodemo].string, STR_GAME);
#else
        strcpy (&MainMenu[backtodemo].string[8], STR_GAME);
#endif

        MainMenu[backtodemo].active = 2;
    }
    else
    {
        // IOANCH 20130301: unification culling

#ifdef SPANISH
        strcpy (&MainMenu[backtodemo].string, STR_BD);
#else
        strcpy (&MainMenu[backtodemo].string[8], STR_DEMO);
#endif
        MainMenu[backtodemo].active = 1;
    }

    DrawMenu (&MainItems, &MainMenu[0]);
    I_UpdateScreen ();
}

// IOANCH 20130301: unification culling

////////////////////////////////////////////////////////////////////
//
// CHECK QUICK-KEYS & QUIT (WHILE IN A GAME)
//
////////////////////////////////////////////////////////////////////
static int Confirm(const char *string);

static int CP_CheckQuick (ScanCode scancode)
{
    switch (scancode)
    {
        //
        // END GAME
        //
        case sc_F7:
            graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);

            WindowH = 160;
            // IOANCH 20130301: unification culling
            if (Confirm (ENDGAMESTR))
            {
                playstate = ex_died;
                killerobj = NULL;
                pickquick = gamestate.lives = 0;
            }

            WindowH = LOGIC_HEIGHT;
            fontnumber = 0;
            MainMenu[savegame].active = 0;
            return 1;

        //
        // QUICKSAVE
        //
        case sc_F8:
            if (SaveGamesAvail[LSItems.curpos] && pickquick)
            {
                graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);
                fontnumber = 1;
                Message (STR_SAVING "...");
                CP_SaveGame (1);
                fontnumber = 0;
            }
            else
            {

                // IOANCH 20130302: unification
                if(!SPEAR::flag)
                {
                    graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);
                    graphSegs.cacheChunk (SPEAR::g(C_CURSOR1PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_CURSOR2PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_DISKLOADING1PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_DISKLOADING2PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_SAVEGAMEPIC));
                    graphSegs.cacheChunk (SPEAR::g(C_MOUSELBACKPIC));
                }
                else
                {
                    CacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
                    graphSegs.cacheChunk (SPEAR::g(C_CURSOR1PIC));
                }

                VW_FadeOut ();
                if(cfg_screenHeight % LOGIC_HEIGHT != 0)
                    I_ClearScreen(0);

                lastgamemusicoffset = StartCPMusic (MENUSONG);
                pickquick = CP_SaveGame (0);

                SETFONTCOLOR (0, 15);
                myInput.clearKeysDown ();
                VW_FadeOut();
                if(viewsize != 21)
                    DrawPlayScreen ();

                if (!startgame && !loadedgame)
                    ContinueMusic (lastgamemusicoffset);

                if (loadedgame)
                    playstate = ex_abort;
                lasttimecount = GetTimeCount ();

                if (myInput.mousePresent() && myInput.inputGrabbed())
                    myInput.centreMouse();     // Clear accumulated mouse movement

                if(!SPEAR::flag)
                {
                    // IOANCH 20130302: unification
                    graphSegs.uncacheChunk (SPEAR::g(C_CURSOR1PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_CURSOR2PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_DISKLOADING1PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_DISKLOADING2PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_SAVEGAMEPIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_MOUSELBACKPIC));
                }
                else
                {
                    UnCacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
                }
            }
            return 1;

        //
        // QUICKLOAD
        //
        case sc_F9:
            if (SaveGamesAvail[LSItems.curpos] && pickquick)
            {
                char string[100] = STR_LGC;


                graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);
                fontnumber = 1;

                strcat (string, SaveGameNames[LSItems.curpos]);
                strcat (string, "\"?");

                if (Confirm (string))
                    CP_LoadGame (1);

                fontnumber = 0;
            }
            else
            {
                if(!SPEAR::flag)
                {
                    graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);
                    // IOANCH 20130302: unification
                    graphSegs.cacheChunk (SPEAR::g(C_CURSOR1PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_CURSOR2PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_DISKLOADING1PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_DISKLOADING2PIC));
                    graphSegs.cacheChunk (SPEAR::g(C_LOADGAMEPIC));
                    graphSegs.cacheChunk (SPEAR::g(C_MOUSELBACKPIC));
                }
                else
                {
                    graphSegs.cacheChunk (SPEAR::g(C_CURSOR1PIC));
                    CacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
                }

                VW_FadeOut ();
                if(cfg_screenHeight % LOGIC_HEIGHT != 0)
                    I_ClearScreen(0);

                lastgamemusicoffset = StartCPMusic (MENUSONG);
                pickquick = CP_LoadGame (0);    // loads lastgamemusicoffs

                SETFONTCOLOR (0, 15);
                myInput.clearKeysDown ();
                VW_FadeOut();
                if(viewsize != 21)
                    DrawPlayScreen ();

                if (!startgame && !loadedgame)
                    ContinueMusic (lastgamemusicoffset);

                if (loadedgame)
                    playstate = ex_abort;

                lasttimecount = GetTimeCount ();

                if (myInput.mousePresent() && myInput.inputGrabbed())
                    myInput.centreMouse();     // Clear accumulated mouse movement

                // IOANCH 20130302: unification
                if(!SPEAR::flag)
                {
                    graphSegs.uncacheChunk (SPEAR::g(C_CURSOR1PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_CURSOR2PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_DISKLOADING1PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_DISKLOADING2PIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_LOADGAMEPIC));
                    graphSegs.uncacheChunk (SPEAR::g(C_MOUSELBACKPIC));
                }
                else
                {
                    UnCacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
                }
            }
            return 1;

        //
        // QUIT
        //
        case sc_F10:
            graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);

            WindowX = WindowY = 0;
            WindowW = LOGIC_WIDTH;
            WindowH = 160;
            // IOANCH 20130301: unification culling
#ifdef SPANISH
            if (Confirm (ENDGAMESTR))
#else
            // IOANCH 20130202: unification process
            if (Confirm (endStrings[wolfRnd () & 0x7 + (wolfRnd () & 1) + (SPEAR::flag ? 9 : 0)]))
#endif
            {
				if(ingame)
					bot.SaveData();

                I_UpdateScreen ();
                Sound::MusicOff ();
				Sound::Stop();
                MenuFadeOut ();

                Quit ();
            }

            DrawPlayBorder ();
            WindowH = LOGIC_HEIGHT;
            fontnumber = 0;
            return 1;
          default:
          ;
    }

    return 0;
}


////////////////////////////////////////////////////////////////////
//
// END THE CURRENT GAME
//
////////////////////////////////////////////////////////////////////
static int CP_EndGame (int)
{
    int res;
    // IOANCH 20130301: unification culling

    res = Confirm (ENDGAMESTR);
    DrawMainMenu();
    if(!res) return 0;

    pickquick = gamestate.lives = 0;
    playstate = ex_died;
    killerobj = NULL;

    MainMenu[savegame].active = 0;
    MainMenu[viewscores].routine = CP_ViewScores;

    strcpy (MainMenu[viewscores].string, STR_VS);


    return 1;
}


////////////////////////////////////////////////////////////////////
//
// VIEW THE HIGH SCORES
//
////////////////////////////////////////////////////////////////////
int CP_ViewScores (int)
{
    fontnumber = 0;

    // IOANCH 20130303: unification
    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
                    // IOANCH 20130301: unification music
        StartCPMusic (XAWARD_MUS_sod);
    }

    else
    {
                    // IOANCH 20130301: unification music
        StartCPMusic (ROSTER_MUS_wl6);
    }

    DrawHighScores ();
    I_UpdateScreen ();
    MenuFadeIn ();
    fontnumber = 1;

    myInput.ack ();

    StartCPMusic (MENUSONG);
    MenuFadeOut ();

    if(SPEAR::flag)
    {
        CacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));
        CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
    }
    return 0;
}


////////////////////////////////////////////////////////////////////
//
// START A NEW GAME
//
////////////////////////////////////////////////////////////////////
static void ShootSnd();
static void DrawNewEpisode();
static void DrawNewGame();
static void DrawNewGameDiff(int w);

static int CP_NewGame (int)
{
    int which, episode;

    if(SPEAR::flag)
        UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));


    if(!SPEAR::flag)
    {
firstpart:

        DrawNewEpisode ();
        do
        {
            which = HandleMenu (&NewEitems, &menu_newep[0], NULL);
            switch (which)
            {
                case -1:
                    MenuFadeOut ();
                    return 0;

                default:
                    if (!menu_epselect[which / 2])
                    {
                        Sound::Play (NOWAYSND);
                        Message ("Please select \"Read This!\"\n"
                                 "from the Options menu to\n"
                                 "find out how to order this\n" "episode from Apogee.");
                        myInput.clearKeysDown ();
                        myInput.ack ();
                        DrawNewEpisode ();
                        which = 0;
                    }
                    else
                    {
                        episode = which / 2;
                        which = 1;
                    }
                    break;
            }

        }
        while (!which);

        ShootSnd ();

        //
        // ALREADY IN A GAME?
        //
        if (ingame)
            // IOANCH 20130301: unification culling



            if (!Confirm (CURGAME))
            {
                MenuFadeOut ();
                return 0;
            }

        MenuFadeOut ();
    }
    else
    {
        episode = 0;

        //
        // ALREADY IN A GAME?
        //
        CacheLump (SPEAR::g(NEWGAME_LUMP_START), SPEAR::g(NEWGAME_LUMP_END));
        DrawNewGame ();
        if (ingame)
            if (!Confirm (CURGAME))
            {
                MenuFadeOut ();
                UnCacheLump (SPEAR::g(NEWGAME_LUMP_START), SPEAR::g(NEWGAME_LUMP_END));
                CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
                return 0;
            }
    }

    DrawNewGame ();
    which = HandleMenu (&NewItems, &NewMenu[0], DrawNewGameDiff);
    if (which < 0)
    {
        MenuFadeOut ();
        if(!SPEAR::flag)
            goto firstpart;
        else
        {
            UnCacheLump (SPEAR::g(NEWGAME_LUMP_START), SPEAR::g(NEWGAME_LUMP_END));
            CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
            return 0;
        }
    }

    ShootSnd ();
    NewGame (which, episode);
    s_StartGame = 1;
    MenuFadeOut ();

    //
    // CHANGE "READ THIS!" TO NORMAL COLOR
    //

    pickquick = 0;

    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(NEWGAME_LUMP_START), SPEAR::g(NEWGAME_LUMP_END));
        CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
    }

    return 0;
}


/////////////////////
//
// DRAW NEW EPISODE MENU
//
static void WaitKeyUp();

static void DrawNewEpisode ()
{
    int i;

    // IOANCH 20130301: unification culling
    ClearMScreen ();
    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));

    DrawWindow (NE_X - 4, NE_Y - 4, NE_W + 8, NE_H + 8, Menu::g_bkgdColor);
    SETFONTCOLOR (READHCOLOR, Menu::g_bkgdColor);
    PrintY = 2;
    WindowX = 0;
#ifdef SPANISH
    US_CPrint ("Cual episodio jugar?");
#else
    US_CPrint ("Which episode to play?");
#endif

    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
    DrawMenu (&NewEitems, &menu_newep[0]);
    // IOANCH 20130302: unification
    for (i = 0; i < 6; i++)
        VWB_DrawPic (NE_X + 32, NE_Y + i * 26, SPEAR::g(C_EPISODE1PIC) + i);

    I_UpdateScreen ();
    MenuFadeIn ();
    WaitKeyUp ();
}

/////////////////////
//
// DRAW NEW GAME MENU
//
static void DrawNewGame ()
{
    // IOANCH 20130301: unification culling
    ClearMScreen ();
    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));

    SETFONTCOLOR (READHCOLOR, Menu::g_bkgdColor);
    PrintX = NM_X + 20;
    PrintY = NM_Y - 32;

    if(!SPEAR::flag)
    {
#ifdef SPANISH
        US_Print ("Eres macho?");
#else
        US_Print ("How tough are you?");
#endif
    }
    else
    {
        // IOANCH 20130302: unification
        VWB_DrawPic (PrintX, PrintY, SPEAR::g(C_HOWTOUGHPIC));
    }

    DrawWindow (NM_X - 5, NM_Y - 10, NM_W, NM_H, Menu::g_bkgdColor);

    DrawMenu (&NewItems, &NewMenu[0]);
    DrawNewGameDiff (NewItems.curpos);
    I_UpdateScreen ();
    MenuFadeIn ();
    WaitKeyUp ();
}


////////////////////////
//
// DRAW NEW GAME GRAPHIC
//
static void DrawNewGameDiff (int w)
{
    // IOANCH 20130302: unification
    VWB_DrawPic (NM_X + 185, NM_Y + 7, w + SPEAR::g(C_BABYMODEPIC));
}


////////////////////////////////////////////////////////////////////
//
// HANDLE SOUND MENU
//
////////////////////////////////////////////////////////////////////
static void DrawSoundMenu();

static int CP_Sound (int)
{
    int which;


    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
        CacheLump (SPEAR::g(SOUND_LUMP_START), SPEAR::g(SOUND_LUMP_END));
    }

    DrawSoundMenu ();
    MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu (&SndItems, &SndMenu[0], NULL);
        //
        // HANDLE MENU CHOICES
        //
        switch (which)
        {
                //
                // SOUND EFFECTS
                //
            case 0:
                if (sd_soundMode != sdm_Off)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_Off);
                    DrawSoundMenu ();
                }
                break;
            case 1:
                if (sd_soundMode != sdm_PC)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_PC);
                    audioSegs.loadAllSounds(sd_soundMode);
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;
            case 2:
                if (sd_soundMode != sdm_AdLib)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_AdLib);
                    audioSegs.loadAllSounds(sd_soundMode);
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;

                //
                // DIGITIZED SOUND
                //
            case 5:
                if (sd_digiMode != sds_Off)
                {
                    SD_SetDigiDevice (sds_Off);
                    DrawSoundMenu ();
                }
                break;
            case 6:
/*                if (sd_digiMode != sds_SoundSource)
                {
                    SD_SetDigiDevice (sds_SoundSource);
                    DrawSoundMenu ();
                    ShootSnd ();
                }*/
                break;
            case 7:
                if (sd_digiMode != sds_SoundBlaster)
                {
                    SD_SetDigiDevice (sds_SoundBlaster);
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;

                //
                // MUSIC
                //
            case 10:
                if (sd_musicMode != smm_Off)
                {
                    SD_SetMusicMode (smm_Off);
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;
            case 11:
                if (sd_musicMode != smm_AdLib)
                {
                    SD_SetMusicMode (smm_AdLib);
                    DrawSoundMenu ();
                    ShootSnd ();
                    StartCPMusic (MENUSONG);
                }
                break;
        }
    }
    while (which >= 0);

    MenuFadeOut ();

    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(SOUND_LUMP_START), SPEAR::g(SOUND_LUMP_END));
        CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
    }
    return 0;
}


//////////////////////
//
// DRAW THE SOUND MENU
//
static void DrawMenuGun (const CP_iteminfo * iteminfo);

static void DrawSoundMenu ()
{
    int i, on;

// IOANCH 20130301: unification culling
    //
    // DRAW SOUND MENU
    //
    ClearMScreen ();
    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));

    DrawWindow (SM_X - 8, SM_Y1 - 3, SM_W, SM_H1, Menu::g_bkgdColor);
    DrawWindow (SM_X - 8, SM_Y2 - 3, SM_W, SM_H2, Menu::g_bkgdColor);
    DrawWindow (SM_X - 8, SM_Y3 - 3, SM_W, SM_H3, Menu::g_bkgdColor);

    //
    // IF NO ADLIB, NON-CHOOSENESS!
    //
    if (!sd_adLibPresent && !sd_soundBlasterPresent)
    {
        SndMenu[2].active = SndMenu[10].active = SndMenu[11].active = 0;
    }

    if (!sd_soundBlasterPresent)
        SndMenu[7].active = 0;

    if (!sd_soundBlasterPresent)
        SndMenu[5].active = 0;

    DrawMenu (&SndItems, &SndMenu[0]);
    // IOANCH 20130301: unification culling

    VWB_DrawPic (100, SM_Y1 - 20, SPEAR::g(C_FXTITLEPIC));
    VWB_DrawPic (100, SM_Y2 - 20, SPEAR::g(C_DIGITITLEPIC));
    VWB_DrawPic (100, SM_Y3 - 20, SPEAR::g(C_MUSICTITLEPIC));


    for (i = 0; i < SndItems.amount; i++)
        // IOANCH 20130301: unification culling
        if (SndMenu[i].string[0])
        {
            //
            // DRAW SELECTED/NOT SELECTED GRAPHIC BUTTONS
            //
            on = 0;
            switch (i)
            {
                    //
                    // SOUND EFFECTS
                    //
                case 0:
                    if (sd_soundMode == sdm_Off)
                        on = 1;
                    break;
                case 1:
                    if (sd_soundMode == sdm_PC)
                        on = 1;
                    break;
                case 2:
                    if (sd_soundMode == sdm_AdLib)
                        on = 1;
                    break;

                    //
                    // DIGITIZED SOUND
                    //
                case 5:
                    if (sd_digiMode == sds_Off)
                        on = 1;
                    break;
                case 6:
//                    if (sd_digiMode == sds_SoundSource)
//                        on = 1;
                    break;
                case 7:
                    if (sd_digiMode == sds_SoundBlaster)
                        on = 1;
                    break;

                    //
                    // MUSIC
                    //
                case 10:
                    if (sd_musicMode == smm_Off)
                        on = 1;
                    break;
                case 11:
                    if (sd_musicMode == smm_AdLib)
                        on = 1;
                    break;
            }

            if (on)
                VWB_DrawPic (SM_X + 24, SM_Y1 + i * 13 + 2, SPEAR::g(C_SELECTEDPIC));
            else
                VWB_DrawPic (SM_X + 24, SM_Y1 + i * 13 + 2, SPEAR::g(C_NOTSELECTEDPIC));
        }

    DrawMenuGun (&SndItems);
    I_UpdateScreen ();
}


//
// DRAW LOAD/SAVE IN PROGRESS
//
static void DrawOutline(int x,int y,int w,int h,int color1,int color2);

static void DrawLSAction (int which)
{
#define LSA_X   96
#define LSA_Y   80
#define LSA_W   130
#define LSA_H   42

    DrawWindow (LSA_X, LSA_Y, LSA_W, LSA_H, TEXTCOLOR);
    DrawOutline (LSA_X, LSA_Y, LSA_W, LSA_H, 0, HIGHLIGHT);
    // IOANCH 20130302: unification
    VWB_DrawPic (LSA_X + 8, LSA_Y + 5, SPEAR::g(C_DISKLOADING1PIC));

    fontnumber = 1;
    SETFONTCOLOR (0, TEXTCOLOR);
    PrintX = LSA_X + 46;
    PrintY = LSA_Y + 13;

    if (!which)
        US_Print (STR_LOADING "...");
    else
        US_Print (STR_SAVING "...");

    I_UpdateScreen ();
}


////////////////////////////////////////////////////////////////////
//
// LOAD SAVED GAMES
//
////////////////////////////////////////////////////////////////////
static void DrawLoadSaveScreen(int loadsave);
static void TrackWhichGame(int w);

static int CP_LoadGame (int quick)
{
    FILE *file;
    int which, exit = 0;
    std::string name;
    std::string loadpath;   // IOANCH 20130509: use PString for paths

    name = cfg_savename;

    //
    // QUICKLOAD?
    //
    if (quick)
    {
        which = LSItems.curpos;

        if (SaveGamesAvail[which])
        {
            name[7] = which + '0';

#ifdef _arch_dreamcast
            DC_LoadFromVMU(name.buffer());
#endif
			loadpath = cfg_dir;
			ConcatSubpath(loadpath, name);

			file = ShellUnicode::fopen(loadpath.c_str(), "rb");
			if(!file)
				return 0;	// god help you here. But DON'T crash.
            fseek (file, 32, SEEK_SET);
            loadedgame = true;
            LoadTheGame (file, 0, 0);
            loadedgame = false;
            fclose (file);

            DrawFace ();
            DrawHealth ();
            DrawLives ();
            DrawLevel ();
            DrawAmmo ();
            DrawKeys ();
            DrawWeapon ();
            DrawScore ();
            ContinueMusic (lastgamemusicoffset);
            return 1;
        }
    }


    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
        CacheLump (SPEAR::g(LOADSAVE_LUMP_START), SPEAR::g(LOADSAVE_LUMP_END));
    }

    DrawLoadSaveScreen (0);

    do
    {
        which = HandleMenu (&LSItems, &LSMenu[0], TrackWhichGame);
        if (which >= 0 && SaveGamesAvail[which])
        {
            ShootSnd ();
            name[7] = which + '0';

#ifdef _arch_dreamcast
            DC_LoadFromVMU(name.buffer());
#endif
			loadpath = cfg_dir;
			ConcatSubpath(loadpath, name);

			file = ShellUnicode::fopen(loadpath.c_str(), "rb");
			if(!file)
			{
				exit = 0;
				break;
			}
            fseek (file, 32, SEEK_SET);

            DrawLSAction (0);
            loadedgame = true;

            LoadTheGame (file, LSA_X + 8, LSA_Y + 5);
            fclose (file);

            s_StartGame = 1;
            ShootSnd ();
            //
            // CHANGE "READ THIS!" TO NORMAL COLOR
            //


            exit = 1;
            break;
        }

    }
    while (which >= 0);

    MenuFadeOut ();

    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(LOADSAVE_LUMP_START), SPEAR::g(LOADSAVE_LUMP_END));
        CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
    }

    return exit;
}


///////////////////////////////////
//
// HIGHLIGHT CURRENT SELECTED ENTRY
//
static void PrintLSEntry(int w,int color);

static void TrackWhichGame (int w)
{
    static int lastgameon = 0;

    PrintLSEntry (lastgameon, TEXTCOLOR);
    PrintLSEntry (w, HIGHLIGHT);

    lastgameon = w;
}


////////////////////////////
//
// DRAW THE LOAD/SAVE SCREEN
//
static void DrawLoadSaveScreen (int loadsave)
{
#define DISKX   100
#define DISKY   0

    int i;


    ClearMScreen ();
    fontnumber = 1;
    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));
    DrawWindow (LSM_X - 10, LSM_Y - 5, LSM_W, LSM_H, Menu::g_bkgdColor);
    DrawStripes (10);
    // IOANCH 20130302: unification
    if (!loadsave)
        VWB_DrawPic (60, 0, SPEAR::g(C_LOADGAMEPIC));
    else
        VWB_DrawPic (60, 0, SPEAR::g(C_SAVEGAMEPIC));

    for (i = 0; i < 10; i++)
        PrintLSEntry (i, TEXTCOLOR);

    DrawMenu (&LSItems, &LSMenu[0]);
    I_UpdateScreen ();
    MenuFadeIn ();
    WaitKeyUp ();
}


///////////////////////////////////////////
//
// PRINT LOAD/SAVE GAME ENTRY W/BOX OUTLINE
//
static void PrintLSEntry (int w, int color)
{
    SETFONTCOLOR (color, Menu::g_bkgdColor);
    DrawOutline (LSM_X + LSItems.indent, LSM_Y + w * 13, LSM_W - LSItems.indent - 15, 11, color,
                 color);
    PrintX = LSM_X + LSItems.indent + 2;
    PrintY = LSM_Y + w * 13 + 1;
    fontnumber = 0;

    if (SaveGamesAvail[w])
        US_Print (SaveGameNames[w]);
    else
        US_Print ("      - " STR_EMPTY " -");

    fontnumber = 1;
}


////////////////////////////////////////////////////////////////////
//
// SAVE CURRENT GAME
//
////////////////////////////////////////////////////////////////////
static int CP_SaveGame (int quick)
{
    int which, exit = 0;
    FILE *file;
    std::string name;
    std::string savepath;   // IOANCH 20130509: use PString for paths
    char input[32];

    name = cfg_savename;

    //
    // QUICKSAVE?
    //
    if (quick)
    {
        which = LSItems.curpos;

        if (SaveGamesAvail[which])
        {
            name[7] = which + '0';

			savepath = cfg_dir;
			ConcatSubpath(savepath, name);

			ShellUnicode::unlink(savepath.c_str());
			file = ShellUnicode::fopen(savepath.c_str(), "wb");
			if(!file)
				return 0;

            strcpy (input, &SaveGameNames[which][0]);

            fwrite (input, 1, 32, file);
            fseek (file, 32, SEEK_SET);
            SaveTheGame (file, 0, 0);
            fclose (file);

#ifdef _arch_dreamcast
            DC_SaveToVMU(name.buffer(), input);
#endif

            return 1;
        }
    }


    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
        CacheLump (SPEAR::g(LOADSAVE_LUMP_START), SPEAR::g(LOADSAVE_LUMP_END));
    }

    DrawLoadSaveScreen (1);

    do
    {
        which = HandleMenu (&LSItems, &LSMenu[0], TrackWhichGame);
        if (which >= 0)
        {
            //
            // OVERWRITE EXISTING SAVEGAME?
            //
            if (SaveGamesAvail[which])
            {
                // IOANCH 20130301: unification culling
                if (!Confirm (GAMESVD))
                {
                    DrawLoadSaveScreen (1);
                    continue;
                }
                else
                {
                    DrawLoadSaveScreen (1);
                    PrintLSEntry (which, HIGHLIGHT);
                    I_UpdateScreen ();
                }
            }

            ShootSnd ();

            strcpy (input, &SaveGameNames[which][0]);
            name[7] = which + '0';

            fontnumber = 0;
            if (!SaveGamesAvail[which])
                VL_Bar (LSM_X + LSItems.indent + 1, LSM_Y + which * 13 + 1,
                         LSM_W - LSItems.indent - 16, 10, Menu::g_bkgdColor);
            I_UpdateScreen ();

            if (US_LineInput
                (LSM_X + LSItems.indent + 2, LSM_Y + which * 13 + 1, input, input, true, 31,
                 LSM_W - LSItems.indent - 30))
            {
                SaveGamesAvail[which] = 1;
                strcpy (&SaveGameNames[which][0], input);

				savepath = cfg_dir;
				ConcatSubpath(savepath, name);

				ShellUnicode::unlink(savepath.c_str());
				file = ShellUnicode::fopen(savepath.c_str(), "wb");
				
                fwrite (input, 32, 1, file);
                fseek (file, 32, SEEK_SET);

                DrawLSAction (1);
                SaveTheGame (file, LSA_X + 8, LSA_Y + 5);

                fclose (file);

#ifdef _arch_dreamcast
                DC_SaveToVMU(name.buffer(), input);
#endif

                ShootSnd ();
                exit = 1;
            }
            else
            {
                VL_Bar (LSM_X + LSItems.indent + 1, LSM_Y + which * 13 + 1,
                         LSM_W - LSItems.indent - 16, 10, Menu::g_bkgdColor);
                PrintLSEntry (which, HIGHLIGHT);
                I_UpdateScreen ();
                Sound::Play (ESCPRESSEDSND);
                continue;
            }

            fontnumber = 1;
            break;
        }

    }
    while (which >= 0);

    MenuFadeOut ();

    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(LOADSAVE_LUMP_START), SPEAR::g(LOADSAVE_LUMP_END));
        CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
    }

    return exit;
}

////////////////////////////////////////////////////////////////////
//
// DEFINE CONTROLS
//
////////////////////////////////////////////////////////////////////
static void DrawCtlScreen();

static int CP_Control (int)
{
    int which;

    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
        CacheLump (SPEAR::g(CONTROL_LUMP_START), SPEAR::g(CONTROL_LUMP_END));
    }

    DrawCtlScreen ();
    MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu (&CtlItems, CtlMenu, NULL);
        switch (which)
        {
            case CTL_MOUSEENABLE:
                mouseenabled ^= 1;
                if(myInput.inputGrabbed())
                    myInput.centreMouse();
                DrawCtlScreen ();
                CusItems.curpos = -1;
                ShootSnd ();
                break;

            case CTL_JOYENABLE:
                joystickenabled ^= 1;
                DrawCtlScreen ();
                CusItems.curpos = -1;
                ShootSnd ();
                break;

            case CTL_MOUSESENS:
            case CTL_CUSTOMIZE:
                DrawCtlScreen ();
                MenuFadeIn ();
                WaitKeyUp ();
                break;
        }
    }
    while (which >= 0);

    MenuFadeOut ();

    if(SPEAR::flag)
    {
        UnCacheLump (SPEAR::g(CONTROL_LUMP_START), SPEAR::g(CONTROL_LUMP_END));
        CacheLump (SPEAR::g(OPTIONS_LUMP_START), SPEAR::g(OPTIONS_LUMP_END));
    }
    return 0;
}


////////////////////////////////
//
// DRAW MOUSE SENSITIVITY SCREEN
//
static void DrawMouseSens ()
{
    // IOANCH 20130301: unification culling
    ClearMScreen ();
    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));
#ifdef SPANISH
    DrawWindow (10, 80, 300, 43, Menu::g_bkgdColor);
#else
    DrawWindow (10, 80, 300, 30, Menu::g_bkgdColor);
#endif

    WindowX = 0;
    WindowW = LOGIC_WIDTH;
    PrintY = 82;
    SETFONTCOLOR (READCOLOR, Menu::g_bkgdColor);
    US_CPrint (STR_MOUSEADJ);

    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
#ifdef SPANISH
    PrintX = 14;
    PrintY = 95 + 13;
    US_Print (STR_SLOW);
    PrintX = 252;
    US_Print (STR_FAST);
#else
    PrintX = 14;
    PrintY = 95;
    US_Print (STR_SLOW);
    PrintX = 269;
    US_Print (STR_FAST);
#endif


    VL_Bar (60, 97, LOGIC_HEIGHT, 10, TEXTCOLOR);
    DrawOutline (60, 97, LOGIC_HEIGHT, 10, 0, HIGHLIGHT);
    DrawOutline (60 + 20 * mouseadjustment, 97, 20, 10, 0, READCOLOR);
    VL_Bar (61 + 20 * mouseadjustment, 98, 19, 9, READHCOLOR);

    I_UpdateScreen ();
    MenuFadeIn ();
}


///////////////////////////
//
// ADJUST MOUSE SENSITIVITY
//
int MouseSensitivity (int)
{
    CursorInfo ci;
    int exit = 0, oldMA;


    oldMA = mouseadjustment;
    DrawMouseSens ();
    do
    {
        I_Delay(5);
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
            case dir_North:
            case dir_West:
                if (mouseadjustment)
                {
                    mouseadjustment--;
                    VL_Bar (60, 97, LOGIC_HEIGHT, 10, TEXTCOLOR);
                    DrawOutline (60, 97, LOGIC_HEIGHT, 10, 0, HIGHLIGHT);
                    DrawOutline (60 + 20 * mouseadjustment, 97, 20, 10, 0, READCOLOR);
                    VL_Bar (61 + 20 * mouseadjustment, 98, 19, 9, READHCOLOR);
                    I_UpdateScreen ();
                    Sound::Play (MOVEGUN1SND);
                    TicDelay(20);
                }
                break;

            case dir_South:
            case dir_East:
                if (mouseadjustment < 9)
                {
                    mouseadjustment++;
                    VL_Bar (60, 97, LOGIC_HEIGHT, 10, TEXTCOLOR);
                    DrawOutline (60, 97, LOGIC_HEIGHT, 10, 0, HIGHLIGHT);
                    DrawOutline (60 + 20 * mouseadjustment, 97, 20, 10, 0, READCOLOR);
                    VL_Bar (61 + 20 * mouseadjustment, 98, 19, 9, READHCOLOR);
                    I_UpdateScreen ();
                    Sound::Play (MOVEGUN1SND);
                    TicDelay(20);
                }
                break;
			default:
				;
        }

        if (ci.button0 || myInput.keyboard(sc_Space) || myInput.keyboard(sc_Enter))
            exit = 1;
        else if (ci.button1 || myInput.keyboard(sc_Escape))
            exit = 2;

    }
    while (!exit);

    if (exit == 2)
    {
        mouseadjustment = oldMA;
        Sound::Play (ESCPRESSEDSND);
    }
    else
        Sound::Play (SHOOTSND);

    WaitKeyUp ();
    MenuFadeOut ();

    return 0;
}


///////////////////////////
//
// DRAW CONTROL MENU SCREEN
//
static void DrawCtlScreen ()
{
    int i, x, y;
// IOANCH 20130301: unification culling
    ClearMScreen ();
    DrawStripes (10);
    VWB_DrawPic (80, 0, SPEAR::g(C_CONTROLPIC));
    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));
    DrawWindow (CTL_X - 8, CTL_Y - 5, CTL_W, CTL_H, Menu::g_bkgdColor);
    WindowX = 0;
    WindowW = LOGIC_WIDTH;
    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);

    if (myInput.joyPresent())
        CtlMenu[CTL_JOYENABLE].active = 1;

    if (myInput.mousePresent())
    {
        CtlMenu[CTL_MOUSESENS].active = CtlMenu[CTL_MOUSEENABLE].active = 1;
    }

    CtlMenu[CTL_MOUSESENS].active = mouseenabled;


    DrawMenu (&CtlItems, CtlMenu);


    x = CTL_X + CtlItems.indent - 24;
    y = CTL_Y + 3;
    if (mouseenabled)
        VWB_DrawPic (x, y, SPEAR::g(C_SELECTEDPIC));
    else
        VWB_DrawPic (x, y, SPEAR::g(C_NOTSELECTEDPIC));

    y = CTL_Y + 29;
    if (joystickenabled)
        VWB_DrawPic (x, y, SPEAR::g(C_SELECTEDPIC));
    else
        VWB_DrawPic (x, y, SPEAR::g(C_NOTSELECTEDPIC));

    //
    // PICK FIRST AVAILABLE SPOT
    //
    if (CtlItems.curpos < 0 || !CtlMenu[CtlItems.curpos].active)
    {
        for (i = 0; i < CtlItems.amount; i++)
        {
            if (CtlMenu[i].active)
            {
                CtlItems.curpos = i;
                break;
            }
        }
    }

    DrawMenuGun (&CtlItems);
    I_UpdateScreen ();
}


////////////////////////////////////////////////////////////////////
//
// CUSTOMIZE CONTROLS
//
////////////////////////////////////////////////////////////////////
enum
{ FIRE, STRAFE, RUN, OPEN };
char mbarray[4][3] = { "b0", "b1", "b2", "b3" };
int8_t order[4] = { RUN, OPEN, FIRE, STRAFE };

static void DefineMouseBtns();
static void DefineJoyBtns();
static void DefineKeyBtns();
static void DefineKeyMove();
static void DrawCustomScreen();
static void DrawCustMouse(int hilight);
static void DrawCustJoy(int hilight);
static void DrawCustKeybd(int hilight);
static void DrawCustKeys(int hilight);
static void FixupCustom(int w);

static int CustomControls (int)
{
    int which;

    DrawCustomScreen ();
    do
    {
        which = HandleMenu (&CusItems, &CusMenu[0], FixupCustom);
        switch (which)
        {
            case 0:
                DefineMouseBtns ();
                DrawCustMouse (1);
                break;
            case 3:
                DefineJoyBtns ();
                DrawCustJoy (0);
                break;
            case 6:
                DefineKeyBtns ();
                DrawCustKeybd (0);
                break;
            case 8:
                DefineKeyMove ();
                DrawCustKeys (0);
        }
    }
    while (which >= 0);

    MenuFadeOut ();

    return 0;
}


////////////////////////
//
// DEFINE THE MOUSE BUTTONS
//

static void EnterCtrlData(int index,CustomCtrls *cust,void (*DrawRtn)(int),void (*PrintRtn)(int),int type);
static void PrintCustMouse(int i);

static void DefineMouseBtns ()
{
    CustomCtrls mouseallowed = { 0, 1, 1, 1 };
    EnterCtrlData (2, &mouseallowed, DrawCustMouse, PrintCustMouse, MOUSE);
}


////////////////////////
//
// DEFINE THE JOYSTICK BUTTONS
//
static void PrintCustJoy(int i);

static void DefineJoyBtns ()
{
    CustomCtrls joyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (5, &joyallowed, DrawCustJoy, PrintCustJoy, JOYSTICK);
}


////////////////////////
//
// DEFINE THE KEYBOARD BUTTONS
//
static void PrintCustKeybd(int i);

static void DefineKeyBtns ()
{
    CustomCtrls keyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (8, &keyallowed, DrawCustKeybd, PrintCustKeybd, KEYBOARDBTNS);
}


////////////////////////
//
// DEFINE THE KEYBOARD BUTTONS
//
static void PrintCustKeys(int i);

static void DefineKeyMove ()
{
    CustomCtrls keyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (10, &keyallowed, DrawCustKeys, PrintCustKeys, KEYBOARDMOVE);
}


////////////////////////
//
// ENTER CONTROL DATA FOR ANY TYPE OF CONTROL
//
enum
{ FWRD, RIGHT, BKWD, LEFT };
int moveorder[4] = { LEFT, RIGHT, FWRD, BKWD };

static void EnterCtrlData (int index, CustomCtrls * cust, void (*DrawRtn) (int), void (*PrintRtn) (int),
               int type)
{
    int j, exit, tick, redraw, which, x, picked, lastFlashTime;
    CursorInfo ci;


    ShootSnd ();
    PrintY = CST_Y + 13 * index;
    myInput.clearKeysDown ();
    exit = 0;
    redraw = 1;
    //
    // FIND FIRST SPOT IN ALLOWED ARRAY
    //
    for (j = 0; j < 4; j++)
        if (cust->allowed[j])
        {
            which = j;
            break;
        }

    do
    {
        if (redraw)
        {
            x = CST_START + CST_SPC * which;
            DrawWindow (5, PrintY - 1, 310, 13, Menu::g_bkgdColor);

            DrawRtn (1);
            DrawWindow (x - 2, PrintY, CST_SPC, 11, TEXTCOLOR);
            DrawOutline (x - 2, PrintY, CST_SPC, 11, 0, HIGHLIGHT);
            SETFONTCOLOR (0, TEXTCOLOR);
            PrintRtn (which);
            PrintX = x;
            SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
            I_UpdateScreen ();
            WaitKeyUp ();
            redraw = 0;
        }

        I_Delay(5);
        ReadAnyControl (&ci);

        if (type == MOUSE || type == JOYSTICK)
            if (myInput.keyboard(sc_Enter) || myInput.keyboard(sc_Control) || myInput.keyboard(sc_Alt))
            {
                myInput.clearKeysDown ();
                ci.button0 = ci.button1 = false;
            }

        //
        // CHANGE BUTTON VALUE?
        //
        if (((type != KEYBOARDBTNS && type != KEYBOARDMOVE) && (ci.button0 | ci.button1 | ci.button2 | ci.button3)) ||
            ((type == KEYBOARDBTNS || type == KEYBOARDMOVE) && myInput.lastScan() == sc_Enter))
        {
            lastFlashTime = GetTimeCount();
            tick = picked = 0;
            SETFONTCOLOR (0, TEXTCOLOR);

            if (type == KEYBOARDBTNS || type == KEYBOARDMOVE)
                myInput.clearKeysDown ();

            while(1)
            {
                int button, result = 0;

                //
                // FLASH CURSOR
                //
                if (GetTimeCount() - lastFlashTime > 10)
                {
                    switch (tick)
                    {
                        case 0:
                            VL_Bar (x, PrintY + 1, CST_SPC - 2, 10, TEXTCOLOR);
                            break;
                        case 1:
                            PrintX = x;
                            US_Print ("?");
                            Sound::Play (HITWALLSND);
                    }
                    tick ^= 1;
                    lastFlashTime = GetTimeCount();
                    I_UpdateScreen ();
                }
                else I_Delay(5);

                //
                // WHICH TYPE OF INPUT DO WE PROCESS?
                //
                switch (type)
                {
                    case MOUSE:
                        button = myInput.mouseButtons();
                        switch (button)
                        {
                            case 1:
                                result = 1;
                                break;
                            case 2:
                                result = 2;
                                break;
                            case 4:
                                result = 3;
                                break;
                        }

                        if (result)
                        {
                            for (int z = 0; z < 4; z++)
                                if (order[which] == buttonmouse[z])
                                {
                                    buttonmouse[z] = bt_nobutton;
                                    break;
                                }

                            buttonmouse[result - 1] = order[which];
                            picked = 1;
                            Sound::Play (SHOOTDOORSND);
                        }
                        break;

                    case JOYSTICK:
                        if (ci.button0)
                            result = 1;
                        else if (ci.button1)
                            result = 2;
                        else if (ci.button2)
                            result = 3;
                        else if (ci.button3)
                            result = 4;

                        if (result)
                        {
                            for (int z = 0; z < 4; z++)
                            {
                                if (order[which] == buttonjoy[z])
                                {
                                    buttonjoy[z] = bt_nobutton;
                                    break;
                                }
                            }

                            buttonjoy[result - 1] = order[which];
                            picked = 1;
                            Sound::Play (SHOOTDOORSND);
                        }
                        break;

                    case KEYBOARDBTNS:
                        if (myInput.lastScan() && myInput.lastScan() != sc_Escape)
                        {
                            buttonscan[order[which]] = myInput.lastScan();
                            picked = 1;
                            ShootSnd ();
                            myInput.clearKeysDown ();
                        }
                        break;

                    case KEYBOARDMOVE:
                        if (myInput.lastScan() && myInput.lastScan() != sc_Escape)
                        {
                            dirscan[moveorder[which]] = myInput.lastScan();
                            picked = 1;
                            ShootSnd ();
                            myInput.clearKeysDown ();
                        }
                        break;
                }

                //
                // EXIT INPUT?
                //
                if (myInput.keyboard(sc_Escape) || (type != JOYSTICK && ci.button1))
                {
                    picked = 1;
                    Sound::Play (ESCPRESSEDSND);
                }

                if(picked) break;

                ReadAnyControl (&ci);
            }

            SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
            redraw = 1;
            WaitKeyUp ();
            continue;
        }

        if (ci.button1 || myInput.keyboard(sc_Escape))
            exit = 1;

        //
        // MOVE TO ANOTHER SPOT?
        //
        switch (ci.dir)
        {
            case dir_West:
                do
                {
                    which--;
                    if (which < 0)
                        which = 3;
                }
                while (!cust->allowed[which]);
                redraw = 1;
                Sound::Play (MOVEGUN1SND);
                while (ReadAnyControl (&ci), ci.dir != dir_None) I_Delay(5);
                myInput.clearKeysDown ();
                break;

            case dir_East:
                do
                {
                    which++;
                    if (which > 3)
                        which = 0;
                }
                while (!cust->allowed[which]);
                redraw = 1;
                Sound::Play (MOVEGUN1SND);
                while (ReadAnyControl (&ci), ci.dir != dir_None) I_Delay(5);
                myInput.clearKeysDown ();
                break;
            case dir_North:
            case dir_South:
                exit = 1;
			default:
				;
        }
    }
    while (!exit);

    Sound::Play (ESCPRESSEDSND);
    WaitKeyUp ();
    DrawWindow (5, PrintY - 1, 310, 13, Menu::g_bkgdColor);
}


////////////////////////
//
// FIXUP GUN CURSOR OVERDRAW SHIT
//
static void FixupCustom (int w)
{
    static int lastwhich = -1;
    int y = CST_Y + 26 + w * 13;


    VWB_Hlin (7, 32, y - 1, s_deactive);
    VWB_Hlin (7, 32, y + 12, s_bord2Color);
    if(!SPEAR::flag)
    {
        VWB_Hlin (7, 32, y - 2, Menu::g_bordColor);
        VWB_Hlin (7, 32, y + 13, Menu::g_bordColor);
    }
    else
    {
        VWB_Hlin (7, 32, y - 2, s_bord2Color);
        VWB_Hlin (7, 32, y + 13, s_bord2Color);
    }

    switch (w)
    {
        case 0:
            DrawCustMouse (1);
            break;
        case 3:
            DrawCustJoy (1);
            break;
        case 6:
            DrawCustKeybd (1);
            break;
        case 8:
            DrawCustKeys (1);
    }


    if (lastwhich >= 0)
    {
        y = CST_Y + 26 + lastwhich * 13;
        VWB_Hlin (7, 32, y - 1, s_deactive);
        VWB_Hlin (7, 32, y + 12, s_bord2Color);
        if(!SPEAR::flag)
        {
            VWB_Hlin (7, 32, y - 2, Menu::g_bordColor);
            VWB_Hlin (7, 32, y + 13, Menu::g_bordColor);
        }
        else
        {
            VWB_Hlin (7, 32, y - 2, s_bord2Color);
            VWB_Hlin (7, 32, y + 13, s_bord2Color);
        }

        if (lastwhich != w)
            switch (lastwhich)
            {
                case 0:
                    DrawCustMouse (0);
                    break;
                case 3:
                    DrawCustJoy (0);
                    break;
                case 6:
                    DrawCustKeybd (0);
                    break;
                case 8:
                    DrawCustKeys (0);
            }
    }

    lastwhich = w;
}


////////////////////////
//
// DRAW CUSTOMIZE SCREEN
//
static void DrawCustomScreen ()
{
    int i;

// IOANCH 20130301: unification culling
    ClearMScreen ();
    WindowX = 0;
    WindowW = LOGIC_WIDTH;
    VWB_DrawPic (112, 184, SPEAR::g(C_MOUSELBACKPIC));
    DrawStripes (10);
    VWB_DrawPic (80, 0, SPEAR::g(C_CUSTOMIZEPIC));

    //
    // MOUSE
    //
    SETFONTCOLOR (READCOLOR, Menu::g_bkgdColor);
    WindowX = 0;
    WindowW = LOGIC_WIDTH;

    if(!SPEAR::flag)
    {
        PrintY = CST_Y;
        US_CPrint ("Mouse\n");
    }
    else
    {
        PrintY = CST_Y + 13;
        VWB_DrawPic (128, 48, SPEAR::g(C_MOUSEPIC));
    }

    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
#ifdef SPANISH
    PrintX = CST_START - 16;
    US_Print (STR_CRUN);
    PrintX = CST_START - 16 + CST_SPC * 1;
    US_Print (STR_COPEN);
    PrintX = CST_START - 16 + CST_SPC * 2;
    US_Print (STR_CFIRE);
    PrintX = CST_START - 16 + CST_SPC * 3;
    US_Print (STR_CSTRAFE "\n");
#else
    PrintX = CST_START;
    US_Print (STR_CRUN);
    PrintX = CST_START + CST_SPC * 1;
    US_Print (STR_COPEN);
    PrintX = CST_START + CST_SPC * 2;
    US_Print (STR_CFIRE);
    PrintX = CST_START + CST_SPC * 3;
    US_Print (STR_CSTRAFE "\n");
#endif

    DrawWindow (5, PrintY - 1, 310, 13, Menu::g_bkgdColor);
    DrawCustMouse (0);
    US_Print ("\n");


    //
    // JOYSTICK/PAD
    //
    if(!SPEAR::flag)
    {
        SETFONTCOLOR (READCOLOR, Menu::g_bkgdColor);
        US_CPrint ("Joystick/Gravis GamePad\n");
    }
    else
    {
        PrintY += 13;
        // IOANCH 20130302: unification
        VWB_DrawPic (40, 88, SPEAR::g(C_JOYSTICKPIC));
    }

    if(SPEAR::flag)
    {
        VWB_DrawPic (112, 120, SPEAR::g(C_KEYBOARDPIC));
    }

    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
#ifdef SPANISH
    PrintX = CST_START - 16;
    US_Print (STR_CRUN);
    PrintX = CST_START - 16 + CST_SPC * 1;
    US_Print (STR_COPEN);
    PrintX = CST_START - 16 + CST_SPC * 2;
    US_Print (STR_CFIRE);
    PrintX = CST_START - 16 + CST_SPC * 3;
    US_Print (STR_CSTRAFE "\n");
#else
    PrintX = CST_START;
    US_Print (STR_CRUN);
    PrintX = CST_START + CST_SPC * 1;
    US_Print (STR_COPEN);
    PrintX = CST_START + CST_SPC * 2;
    US_Print (STR_CFIRE);
    PrintX = CST_START + CST_SPC * 3;
    US_Print (STR_CSTRAFE "\n");
#endif
    DrawWindow (5, PrintY - 1, 310, 13, Menu::g_bkgdColor);
    DrawCustJoy (0);
    US_Print ("\n");


    //
    // KEYBOARD
    //
    if(!SPEAR::flag)
    {
        SETFONTCOLOR (READCOLOR, Menu::g_bkgdColor);
        US_CPrint ("Keyboard\n");
    }
    else
    {
        PrintY += 13;
    }
    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
#ifdef SPANISH
    PrintX = CST_START - 16;
    US_Print (STR_CRUN);
    PrintX = CST_START - 16 + CST_SPC * 1;
    US_Print (STR_COPEN);
    PrintX = CST_START - 16 + CST_SPC * 2;
    US_Print (STR_CFIRE);
    PrintX = CST_START - 16 + CST_SPC * 3;
    US_Print (STR_CSTRAFE "\n");
#else
    PrintX = CST_START;
    US_Print (STR_CRUN);
    PrintX = CST_START + CST_SPC * 1;
    US_Print (STR_COPEN);
    PrintX = CST_START + CST_SPC * 2;
    US_Print (STR_CFIRE);
    PrintX = CST_START + CST_SPC * 3;
    US_Print (STR_CSTRAFE "\n");
#endif
    DrawWindow (5, PrintY - 1, 310, 13, Menu::g_bkgdColor);
    DrawCustKeybd (0);
    US_Print ("\n");


    //
    // KEYBOARD MOVE KEYS
    //
    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
#ifdef SPANISH
    PrintX = 4;
    US_Print (STR_LEFT);
    US_Print ("/");
    US_Print (STR_RIGHT);
    US_Print ("/");
    US_Print (STR_FRWD);
    US_Print ("/");
    US_Print (STR_BKWD "\n");
#else
    PrintX = CST_START;
    US_Print (STR_LEFT);
    PrintX = CST_START + CST_SPC * 1;
    US_Print (STR_RIGHT);
    PrintX = CST_START + CST_SPC * 2;
    US_Print (STR_FRWD);
    PrintX = CST_START + CST_SPC * 3;
    US_Print (STR_BKWD "\n");
#endif
    DrawWindow (5, PrintY - 1, 310, 13, Menu::g_bkgdColor);
    DrawCustKeys (0);
    //
    // PICK STARTING POINT IN MENU
    //
    if (CusItems.curpos < 0)
        for (i = 0; i < CusItems.amount; i++)
            if (CusMenu[i].active)
            {
                CusItems.curpos = i;
                break;
            }


    I_UpdateScreen ();
    MenuFadeIn ();
}


static void PrintCustMouse (int i)
{
    int j;

    for (j = 0; j < 4; j++)
        if (order[i] == buttonmouse[j])
        {
            PrintX = CST_START + CST_SPC * i;
            US_Print (mbarray[j]);
            break;
        }
}

static void DrawCustMouse (int hilight)
{
    int i, color;


    color = TEXTCOLOR;
    if (hilight)
        color = HIGHLIGHT;
    SETFONTCOLOR (color, Menu::g_bkgdColor);

    if (!mouseenabled)
    {
        SETFONTCOLOR (s_deactive, Menu::g_bkgdColor);
        CusMenu[0].active = 0;
    }
    else
        CusMenu[0].active = 1;

    PrintY = CST_Y + 13 * 2;
    for (i = 0; i < 4; i++)
        PrintCustMouse (i);
}

static void PrintCustJoy (int i)
{
    for (int j = 0; j < 4; j++)
    {
        if (order[i] == buttonjoy[j])
        {
            PrintX = CST_START + CST_SPC * i;
            US_Print (mbarray[j]);
            break;
        }
    }
}

static void DrawCustJoy (int hilight)
{
    int i, color;

    color = TEXTCOLOR;
    if (hilight)
        color = HIGHLIGHT;
    SETFONTCOLOR (color, Menu::g_bkgdColor);

    if (!joystickenabled)
    {
        SETFONTCOLOR (s_deactive, Menu::g_bkgdColor);
        CusMenu[3].active = 0;
    }
    else
        CusMenu[3].active = 1;

    PrintY = CST_Y + 13 * 5;
    for (i = 0; i < 4; i++)
        PrintCustJoy (i);
}


static void PrintCustKeybd (int i)
{
    PrintX = CST_START + CST_SPC * i;
	if(ScanNames.count(buttonscan[order[i]]))
		US_Print (ScanNames[buttonscan[order[i]]].c_str());
	else
		US_Print("?");
}

static void DrawCustKeybd (int hilight)
{
    int i, color;


    color = TEXTCOLOR;
    if (hilight)
        color = HIGHLIGHT;
    SETFONTCOLOR (color, Menu::g_bkgdColor);

    PrintY = CST_Y + 13 * 8;
    for (i = 0; i < 4; i++)
        PrintCustKeybd (i);
}

static void PrintCustKeys (int i)
{
    PrintX = CST_START + CST_SPC * i;
	if(ScanNames.count(buttonscan[order[i]]))
		US_Print (ScanNames[dirscan[moveorder[i]]].c_str());
	else
		US_Print("?");
}

static void DrawCustKeys (int hilight)
{
    int i, color;


    color = TEXTCOLOR;
    if (hilight)
        color = HIGHLIGHT;
    SETFONTCOLOR (color, Menu::g_bkgdColor);

    PrintY = CST_Y + 13 * 10;
    for (i = 0; i < 4; i++)
        PrintCustKeys (i);
}


////////////////////////////////////////////////////////////////////
//
// CHANGE SCREEN VIEWING SIZE
//
////////////////////////////////////////////////////////////////////
static void CheckPause();
static void DrawChangeView(int view);

static int CP_ChangeView (int)
{
    int exit = 0, oldview, newview;
    CursorInfo ci;

    WindowX = WindowY = 0;
    WindowW = LOGIC_WIDTH;
    WindowH = LOGIC_HEIGHT;
    newview = oldview = viewsize;
    DrawChangeView (oldview);
    MenuFadeIn ();

    do
    {
        CheckPause ();
        I_Delay(5);
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
            case dir_South:
            case dir_West:
                newview--;
                if (newview < 4)
                    newview = 4;
                if(newview >= 19) DrawChangeView(newview);
                else ShowViewSize (newview);
                I_UpdateScreen ();
                Sound::Play (HITWALLSND);
                TicDelay (10);
                break;

            case dir_North:
            case dir_East:
                newview++;
                if (newview >= 21)
                {
                    newview = 21;
                    DrawChangeView(newview);
                }
                else ShowViewSize (newview);
                I_UpdateScreen ();
                Sound::Play (HITWALLSND);
                TicDelay (10);
                break;
			default:
				;
        }

        if (ci.button0 || myInput.keyboard(sc_Enter))
            exit = 1;
        else if (ci.button1 || myInput.keyboard(sc_Escape))
        {
            Sound::Play (ESCPRESSEDSND);
            MenuFadeOut ();
            if(cfg_screenHeight % LOGIC_HEIGHT != 0)
                I_ClearScreen(0);
            return 0;
        }
    }
    while (!exit);

    if (oldview != newview)
    {
        Sound::Play (SHOOTSND);
        Message (STR_THINK "...");
        main_NewViewSize (newview);
    }

    ShootSnd ();
    MenuFadeOut ();
    if(cfg_screenHeight % LOGIC_HEIGHT != 0)
        I_ClearScreen(0);

    return 0;
}


/////////////////////////////
//
// DRAW THE CHANGEVIEW SCREEN
//
static void DrawChangeView (int view)
{
    int rescaledHeight = cfg_screenHeight / vid_scaleFactor;
    if(view != 21) VL_Bar (0, rescaledHeight - 40, LOGIC_WIDTH, 40, bordercol);
// IOANCH 20130301: unification culling
    ShowViewSize (view);

    PrintY = (cfg_screenHeight / vid_scaleFactor) - 39;
    WindowX = 0;
    WindowY = LOGIC_WIDTH;                                  // TODO: Check this!
    SETFONTCOLOR (HIGHLIGHT, Menu::g_bkgdColor);

    US_CPrint (STR_SIZE1 "\n");
    US_CPrint (STR_SIZE2 "\n");
    US_CPrint (STR_SIZE3);
    I_UpdateScreen ();
}


////////////////////////////////////////////////////////////////////
//
// QUIT THIS INFERNAL GAME!
//
////////////////////////////////////////////////////////////////////
static int CP_Quit (int)
{
    // IOANCH 20130301: unification culling

#ifdef SPANISH
    if (Confirm (ENDGAMESTR))
#else
    // IOANCH 20130202: unification process
    if (Confirm (endStrings[wolfRnd () & 0x7 + (wolfRnd () & 1) + (SPEAR::flag ? 9 : 0)]))
#endif

    {
		// IOANCH 20121217: save data
		if(ingame)
			bot.SaveData();
		
        I_UpdateScreen ();
        Sound::MusicOff ();
		Sound::Stop();
        MenuFadeOut ();
        Quit ();
    }

    DrawMainMenu ();
    return 0;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
// SUPPORT ROUTINES
//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Clear Menu screens to dark red
//
////////////////////////////////////////////////////////////////////
void
ClearMScreen ()
{
    if(!SPEAR::flag)
        VL_Bar (0, 0, LOGIC_WIDTH, LOGIC_HEIGHT, Menu::g_bordColor);
    else
    // IOANCH 20130302: unification
        VWB_DrawPic (0, 0, SPEAR::g(C_BACKDROPPIC));
}


////////////////////////////////////////////////////////////////////
//
// Un/Cache a LUMP of graphics
//
////////////////////////////////////////////////////////////////////
void CacheLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        graphSegs.cacheChunk (i);
}


void UnCacheLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        if (graphSegs[i])
            graphSegs.uncacheChunk (i);
}


////////////////////////////////////////////////////////////////////
//
// Draw a window for a menu
//
////////////////////////////////////////////////////////////////////
static void DrawWindow (int x, int y, int w, int h, int wcolor)
{
    VL_Bar (x, y, w, h, wcolor);
    DrawOutline (x, y, w, h, s_bord2Color, s_deactive);
}


static void DrawOutline (int x, int y, int w, int h, int color1, int color2)
{
    VWB_Hlin (x, x + w, y, color2);
    VWB_Vlin (y, y + h, x, color2);
    VWB_Hlin (x, x + w, y + h, color1);
    VWB_Vlin (y, y + h, x + w, color1);
}


////////////////////////////////////////////////////////////////////
//
// Setup Control Panel stuff - graphics, etc.
//
////////////////////////////////////////////////////////////////////
static void SetupControlPanel ()
{
    //
    // CACHE GRAPHICS & SOUNDS
    //
    graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);
    if(!SPEAR::flag)
        CacheLump (SPEAR::g(CONTROLS_LUMP_START), SPEAR::g(CONTROLS_LUMP_END));
    else
        CacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));

    SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
    fontnumber = 1;
    WindowH = LOGIC_HEIGHT;
    if(cfg_screenHeight % LOGIC_HEIGHT != 0)
        I_ClearScreen(0);

    if (!ingame)
        audioSegs.loadAllSounds(sd_soundMode);
    else
        MainMenu[savegame].active = 1;

    //
    // CENTER MOUSE
    //
    if(myInput.inputGrabbed())
        myInput.centreMouse();
}

////////////////////////////////////////////////////////////////////
//
// SEE WHICH SAVE GAME FILES ARE AVAILABLE & READ STRING IN
//
////////////////////////////////////////////////////////////////////
void menu_SetupSaveGames()
{
    std::string name;
    std::string savepath;

    name = cfg_savename;
    for(int i = 0; i < 10; i++)
    {
        name[7] = '0' + i;
#ifdef _arch_dreamcast
        // Try to unpack file
        if(DC_LoadFromVMU(name.buffer()))
        {
#endif
			savepath = cfg_dir;
			ConcatSubpath(savepath, name);

			FILE* f = ShellUnicode::fopen(savepath.c_str(), "rb");

            if(f)
            {
                char temp[32];

                SaveGamesAvail[i] = 1;
               fread(temp, sizeof(*temp), lengthof(temp), f);
                fclose(f);
                strcpy(&SaveGameNames[i][0], temp);
            }
#ifdef _arch_dreamcast
            // Remove unpacked version of file
            fs_unlink(name.buffer());
        }
#endif
    }
}

////////////////////////////////////////////////////////////////////
//
// Clean up all the Control Panel stuff
//
////////////////////////////////////////////////////////////////////
static void CleanupControlPanel ()
{
    if(!SPEAR::flag)
        UnCacheLump (SPEAR::g(CONTROLS_LUMP_START), SPEAR::g(CONTROLS_LUMP_END));
    else
        UnCacheLump (SPEAR::g(BACKDROP_LUMP_START), SPEAR::g(BACKDROP_LUMP_END));

    fontnumber = 0;
}


////////////////////////////////////////////////////////////////////
//
// Handle moving gun around a menu
//
////////////////////////////////////////////////////////////////////
static void DrawGun(const CP_iteminfo *item_i, const CP_itemtype *items,int x,int *y,int which,int basey,void (*routine)(int w));
static void DrawHalfStep(int x,int y);
static void EraseGun(const CP_iteminfo *item_i, const CP_itemtype *items,int x,int y,int which);
static void SetTextColor(const CP_itemtype *items,int hlight);

static int HandleMenu (CP_iteminfo * item_i, const CP_itemtype * items, void (*routine) (int w))
{
    char key;
    static int redrawitem = 1, lastitem = -1;
    int i, x, y, basey, exit, which, shape;
    int32_t lastBlinkTime, timer;
    CursorInfo ci;


    which = item_i->curpos;
    x = item_i->x & -8;
    basey = item_i->y - 2;
    y = basey + which * 13;

    // IOANCH 20130302: unification
    VWB_DrawPic (x, y, SPEAR::g(C_CURSOR1PIC));
    SetTextColor (items + which, 1);
    if (redrawitem)
    {
        PrintX = item_i->x + item_i->indent;
        PrintY = item_i->y + which * 13;
        US_Print ((items + which)->string);
    }
    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    I_UpdateScreen ();

    // IOANCH 20130302: unification
    shape = SPEAR::g(C_CURSOR1PIC);
    timer = 8;
    exit = 0;
    lastBlinkTime = GetTimeCount ();
    myInput.clearKeysDown ();


    do
    {
        //
        // CHANGE GUN SHAPE
        //
        if ((int32_t)GetTimeCount () - lastBlinkTime > timer)
        {
            lastBlinkTime = GetTimeCount ();
            // IOANCH 20130302: unification
            if (shape == SPEAR::g(C_CURSOR1PIC))
            {
                shape = SPEAR::g(C_CURSOR2PIC);
                timer = 8;
            }
            else
            {
                shape = SPEAR::g(C_CURSOR1PIC);
                timer = 70;
            }
            VWB_DrawPic (x, y, shape);
            if (routine)
                routine (which);
            I_UpdateScreen ();
        }
        else I_Delay(5);

        CheckPause ();

        //
        // SEE IF ANY KEYS ARE PRESSED FOR INITIAL CHAR FINDING
        //
        key = myInput.lastASCII();
        if (key)
        {
            int ok = 0;

            if (key >= 'a')
                key -= 'a' - 'A';

            for (i = which + 1; i < item_i->amount; i++)
                if ((items + i)->active && (items + i)->string[0] == key)
                {
                    EraseGun (item_i, items, x, y, which);
                    which = i;
                    DrawGun (item_i, items, x, &y, which, basey, routine);
                    ok = 1;
                    myInput.clearKeysDown ();
                    break;
                }

            //
            // DIDN'T FIND A MATCH FIRST TIME THRU. CHECK AGAIN.
            //
            if (!ok)
            {
                for (i = 0; i < which; i++)
                    if ((items + i)->active && (items + i)->string[0] == key)
                    {
                        EraseGun (item_i, items, x, y, which);
                        which = i;
                        DrawGun (item_i, items, x, &y, which, basey, routine);
                        myInput.clearKeysDown ();
                        break;
                    }
            }
        }

        //
        // GET INPUT
        //
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
                ////////////////////////////////////////////////
                //
                // MOVE UP
                //
            case dir_North:

                EraseGun (item_i, items, x, y, which);

                //
                // ANIMATE HALF-STEP
                //
                if (which && (items + which - 1)->active)
                {
                    y -= 6;
                    DrawHalfStep (x, y);
                }

                //
                // MOVE TO NEXT AVAILABLE SPOT
                //
                do
                {
                    if (!which)
                        which = item_i->amount - 1;
                    else
                        which--;
                }
                while (!(items + which)->active);

                DrawGun (item_i, items, x, &y, which, basey, routine);
                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;

                ////////////////////////////////////////////////
                //
                // MOVE DOWN
                //
            case dir_South:

                EraseGun (item_i, items, x, y, which);
                //
                // ANIMATE HALF-STEP
                //
                if (which != item_i->amount - 1 && (items + which + 1)->active)
                {
                    y += 6;
                    DrawHalfStep (x, y);
                }

                do
                {
                    if (which == item_i->amount - 1)
                        which = 0;
                    else
                        which++;
                }
                while (!(items + which)->active);

                DrawGun (item_i, items, x, &y, which, basey, routine);

                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;
			default:
				;
        }
		
		if((ci.touch == touch_Hold || ci.touch == touch_Release) && vid_scaleFactor)
		{
			int touchx, touchy, newwhich;
			touchx = ci.x / vid_scaleFactor;
			touchy = ci.y / vid_scaleFactor;
			if(touchx >= x && touchx <= 320 - x &&
			   touchy >= basey && touchy <= basey + item_i->amount * 13)
			{
				if(ci.touch == touch_Hold)
				{
					newwhich = (touchy - basey) / 13;
					if(newwhich != which && newwhich < item_i->amount
					   && newwhich >= 0)
					{
						if((items + newwhich)->active)
						{
							EraseGun (item_i, items, x, y, which);
							which = newwhich;
							DrawGun (item_i, items, x, &y, which, basey, routine);
						}
						else if(items == &menu_newep[0] && newwhich > 0
								&& (items + newwhich - 1)->active
								&& which != newwhich - 1)
						{
							// HACK: menu_newep has apparently wider items,
							// which actually are one enabled one disabled.
							// But the user doesn't know that and thinks they're
							// big touchable items instead! So just for this
							// menu let's select previous entry if that's
							// enabled.
							EraseGun (item_i, items, x, y, which);
							which = newwhich - 1;
							DrawGun (item_i, items, x, &y, which, basey, routine);
						}
					}
				}
				else
				{
					exit = 1;
				}
			}
		}

        if (ci.button0 || myInput.keyboard(sc_Space) || myInput.keyboard(sc_Enter))
            exit = 1;

        if ((ci.button1 && !myInput.keyboard(sc_Alt)) || myInput.keyboard(sc_Escape))
            exit = 2;

    }
    while (!exit);


    myInput.clearKeysDown ();

    //
    // ERASE EVERYTHING
    //
    if (lastitem != which)
    {
        VL_Bar (x - 1, y, 25, 16, Menu::g_bkgdColor);
        PrintX = item_i->x + item_i->indent;
        PrintY = item_i->y + which * 13;
        US_Print ((items + which)->string);
        redrawitem = 1;
    }
    else
        redrawitem = 0;

    if (routine)
        routine (which);
    I_UpdateScreen ();

    item_i->curpos = which;

    lastitem = which;
    switch (exit)
    {
        case 1:
            //
            // CALL THE ROUTINE
            //
            if ((items + which)->routine != NULL)
            {
                ShootSnd ();
                MenuFadeOut ();
                (items + which)->routine (0);
            }
            return which;

        case 2:
            Sound::Play (ESCPRESSEDSND);
            return -1;
    }

    return 0;                   // JUST TO SHUT UP THE ERROR MESSAGES!
}


//
// ERASE GUN & DE-HIGHLIGHT STRING
//
void EraseGun (const CP_iteminfo * item_i, const CP_itemtype * items, int x, int y, int which)
{
    VL_Bar (x - 1, y, 25, 16, Menu::g_bkgdColor);
    SetTextColor (items + which, 0);

    PrintX = item_i->x + item_i->indent;
    PrintY = item_i->y + which * 13;
    US_Print ((items + which)->string);
    I_UpdateScreen ();
}


//
// DRAW HALF STEP OF GUN TO NEXT POSITION
//
void
DrawHalfStep (int x, int y)
{
    VWB_DrawPic (x, y, SPEAR::g(C_CURSOR1PIC));
    I_UpdateScreen ();
    Sound::Play (MOVEGUN1SND);
    I_Delay (8 * 100 / 7);
}


//
// DRAW GUN AT NEW POSITION
//
static void DrawGun (const CP_iteminfo * item_i, const CP_itemtype * items, int x, int *y, int which, int basey,
         void (*routine) (int w))
{
    VL_Bar (x - 1, *y, 25, 16, Menu::g_bkgdColor);
    *y = basey + which * 13;
    // IOANCH 20130302: unification
    VWB_DrawPic (x, *y, SPEAR::g(C_CURSOR1PIC));
    SetTextColor (items + which, 1);

    PrintX = item_i->x + item_i->indent;
    PrintY = item_i->y + which * 13;
    US_Print ((items + which)->string);

    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    I_UpdateScreen ();
	Sound::Play (MOVEGUN2SND);
}

////////////////////////////////////////////////////////////////////
//
// DELAY FOR AN AMOUNT OF TICS OR UNTIL CONTROLS ARE INACTIVE
//
////////////////////////////////////////////////////////////////////
void TicDelay (int count)
{
    CursorInfo ci;

    int32_t startTime = GetTimeCount ();
    do
    {
        I_Delay(5);
        ReadAnyControl (&ci);
    }
    while ((int32_t) GetTimeCount () - startTime < count && ci.dir != dir_None);
}


////////////////////////////////////////////////////////////////////
//
// Draw a menu
//
////////////////////////////////////////////////////////////////////
void DrawMenu (const CP_iteminfo * item_i, const CP_itemtype * items)
{
    int i, which = item_i->curpos;


    WindowX = PrintX = item_i->x + item_i->indent;
    WindowY = PrintY = item_i->y;
    WindowW = LOGIC_WIDTH;
    WindowH = LOGIC_HEIGHT;

    for (i = 0; i < item_i->amount; i++)
    {
        SetTextColor (items + i, which == i);

        PrintY = item_i->y + i * 13;
        if ((items + i)->active)
            US_Print ((items + i)->string);
        else
        {
            SETFONTCOLOR (s_deactive, Menu::g_bkgdColor);
            US_Print ((items + i)->string);
            SETFONTCOLOR (TEXTCOLOR, Menu::g_bkgdColor);
        }

        US_Print ("\n");
    }
}


////////////////////////////////////////////////////////////////////
//
// SET TEXT COLOR (HIGHLIGHT OR NO)
//
////////////////////////////////////////////////////////////////////
static void SetTextColor (const CP_itemtype * items, int hlight)
{
    // IOANCH 20130302: unification
    if (hlight)
    {
        SETFONTCOLOR (IMPALE(color_hlite)[items->active], Menu::g_bkgdColor);
    }
    else
    {
        SETFONTCOLOR (IMPALE(color_norml)[items->active], Menu::g_bkgdColor);
    }
}


////////////////////////////////////////////////////////////////////
//
// WAIT FOR CTRLKEY-UP OR BUTTON-UP
//
////////////////////////////////////////////////////////////////////
void
WaitKeyUp ()
{
    CursorInfo ci;
    while (ReadAnyControl (&ci), ci.button0 |
           ci.button1 |
           ci.button2 | ci.button3 | myInput.keyboard(sc_Space) | myInput.keyboard(sc_Enter) | myInput.keyboard(sc_Escape))
    {
        myInput.waitAndProcessEvents();
    }
}


////////////////////////////////////////////////////////////////////
//
// READ KEYBOARD, JOYSTICK AND MOUSE FOR INPUT
//
////////////////////////////////////////////////////////////////////
void ReadAnyControl (CursorInfo * ci)
{
    int mouseactive = 0;

	bool oldTouchDown = myInput.m_fingerdown;
   *ci = myInput.readControl();

    if (mouseenabled && myInput.inputGrabbed())
    {
        int mousex, mousey, buttons;
        buttons = SDL_GetMouseState(&mousex, &mousey);
        int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
        int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));
        if(middlePressed) buttons |= 1 << 2;
        if(rightPressed) buttons |= 1 << 1;

        if(mousey - CENTERY < -SENSITIVE)
        {
            ci->dir = dir_North;
            mouseactive = 1;
        }
        else if(mousey - CENTERY > SENSITIVE)
        {
            ci->dir = dir_South;
            mouseactive = 1;
        }

        if(mousex - CENTERX < -SENSITIVE)
        {
            ci->dir = dir_West;
            mouseactive = 1;
        }
        else if(mousex - CENTERX > SENSITIVE)
        {
            ci->dir = dir_East;
            mouseactive = 1;
        }

        if(mouseactive)
            myInput.centreMouse();

        if (buttons)
        {
            ci->button0 = (buttons & 1) ? true : false;
            ci->button1 = (buttons & 2) ? true : false;
			ci->button2 = (buttons & 4) ? true : false;
            ci->button3 = false;
            mouseactive = 1;
        }
    }

    if (joystickenabled && !mouseactive)
    {
        int jx, jy, jb;

        myInput.getJoyDelta (&jx, &jy);
        if (jy < -SENSITIVE)
            ci->dir = dir_North;
        else if (jy > SENSITIVE)
            ci->dir = dir_South;

        if (jx < -SENSITIVE)
            ci->dir = dir_West;
        else if (jx > SENSITIVE)
            ci->dir = dir_East;

        jb = myInput.joyButtons ();
        if (jb)
        {
            ci->button0 = (jb & 1) ? true : false;
            ci->button1 = (jb & 2) ? true : false;
            ci->button2 = (jb & 4) ? true : false;
            ci->button3 = (jb & 8) ? true : false;
        }
    }
	
	// Touchscreen support
	ci->touch = myInput.m_fingerdown ? touch_Hold : oldTouchDown && !myInput.m_fingerdown ? touch_Release : touch_None;
	ci->x = (short)myInput.m_touchx;
	ci->y = (short)myInput.m_touchy;
}


////////////////////////////////////////////////////////////////////
//
// DRAW DIALOG AND CONFIRM YES OR NO TO QUESTION
//
////////////////////////////////////////////////////////////////////
static int Confirm (const char *string)
{
    int xit = 0, x, y, tick = 0, lastBlinkTime;
    int whichsnd[2] = { ESCPRESSEDSND, SHOOTSND };
    CursorInfo ci;

    Message (string);
    myInput.clearKeysDown ();
    WaitKeyUp ();

    //
    // BLINK CURSOR
    //
    x = PrintX;
    y = PrintY;
    lastBlinkTime = GetTimeCount();

    do
    {
        ReadAnyControl(&ci);

        if (GetTimeCount() - lastBlinkTime >= 10)
        {
            switch (tick)
            {
                case 0:
                    VL_Bar (x, y, 8, 13, TEXTCOLOR);
                    break;
                case 1:
                    PrintX = x;
                    PrintY = y;
                    US_Print ("_");
            }
            I_UpdateScreen ();
            tick ^= 1;
            lastBlinkTime = GetTimeCount();
        }
        else I_Delay(5);

#ifdef SPANISH
    }
    while (!myInput.keyboard(sc_S) && !myInput.keyboard(sc_N) && !myInput.keyboard(sc_Escape));
#else
    }
    while (!myInput.keyboard(sc_Y) && !myInput.keyboard(sc_N) && !myInput.keyboard(sc_Escape) && !ci.button0 && !ci.button1 && ci.touch != touch_Release);
#endif

#ifdef SPANISH
    if (myInput.keyboard(sc_S) || ci.button0 || ci.touch == touch_Release)
    {
        xit = 1;
        ShootSnd ();
    }
#else
    if (myInput.keyboard(sc_Y) || ci.button0 ||
		(ci.touch == touch_Release && ci.x >= static_cast<short>(g_messageX * vid_scaleFactor) &&
		 ci.x <= static_cast<short>((g_messageX + g_messageW) * vid_scaleFactor) &&
		 ci.y >= static_cast<short>(g_messageY * vid_scaleFactor) &&
		 ci.y <= static_cast<short>((g_messageY + g_messageH) * vid_scaleFactor)))
    {
        xit = 1;
        ShootSnd ();
    }
#endif

    myInput.clearKeysDown ();
    WaitKeyUp ();

    Sound::Play ((soundnames) whichsnd[xit]);

    return xit;
}

// IOANCH 20130301: unification culling


////////////////////////////////////////////////////////////////////
//
// PRINT A MESSAGE IN A WINDOW
//
////////////////////////////////////////////////////////////////////
void
Message (const char *string)
{
    int h = 0, w = 0, mw = 0, i, len = (int) strlen(string);
    fontstruct *font;


    graphSegs.cacheChunk (SPEAR::g(STARTFONT) + 1);
    fontnumber = 1;
    font = (fontstruct *) graphSegs[SPEAR::g(STARTFONT) + fontnumber];
    h = font->height;
    for (i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            if (w > mw)
                mw = w;
            w = 0;
            h += font->height;
        }
        else
            w += font->width[string[i]];
    }

    if (w + 10 > mw)
        mw = w + 10;

    PrintY = (WindowH / 2) - h / 2;
    PrintX = WindowX = 160 - mw / 2;

    DrawWindow (WindowX - 5, PrintY - 5, mw + 10, h + 10, TEXTCOLOR);
    DrawOutline (g_messageX = WindowX - 5, g_messageY = PrintY - 5,
				 g_messageW = mw + 10, g_messageH = h + 10, 0, HIGHLIGHT);
    SETFONTCOLOR (0, TEXTCOLOR);
    US_Print (string);
    I_UpdateScreen ();
}

////////////////////////////////////////////////////////////////////
//
// THIS MAY BE FIXED A LITTLE LATER...
//
////////////////////////////////////////////////////////////////////
static int lastmusic;

int StartCPMusic (int song)
{
    int lastoffs;

    lastmusic = song;
    lastoffs = Sound::MusicOff();
	// IOANCH 20130301: unification
	const unsigned int STARTMUSIC_max = IMPALE(STARTMUSIC);
   audioSegs.uncacheChunk(STARTMUSIC_max + lastmusic);

    SD_StartMusic(STARTMUSIC_max + song);
    return lastoffs;
}

void
FreeMusic ()
{
   audioSegs.uncacheChunk(IMPALE(STARTMUSIC) + lastmusic);
}


///////////////////////////////////////////////////////////////////////////
//
//      IN_GetScanName() - Returns a string containing the name of the
//              specified scan code
//
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
//
// CHECK FOR PAUSE KEY (FOR MUSIC ONLY)
//
///////////////////////////////////////////////////////////////////////////
static void CheckPause ()
{
    if (myInput.paused())
    {
        switch (SoundStatus)
        {
            case 0:
                Sound::MusicOn ();
                break;
            case 1:
				Sound::MusicOff();
                break;
        }

        SoundStatus ^= 1;
        VL_WaitVBL (3);
        myInput.clearKeysDown ();
        myInput.setPaused(false);
    }
}


///////////////////////////////////////////////////////////////////////////
//
// DRAW GUN CURSOR AT CORRECT POSITION IN MENU
//
///////////////////////////////////////////////////////////////////////////
static void DrawMenuGun (const CP_iteminfo * iteminfo)
{
    int x, y;


    x = iteminfo->x;
    y = iteminfo->y + iteminfo->curpos * 13 - 2;
    // IOANCH 20130302: unification
    VWB_DrawPic (x, y, SPEAR::g(C_CURSOR1PIC));
}


///////////////////////////////////////////////////////////////////////////
//
// DRAW SCREEN TITLE STRIPES
//
///////////////////////////////////////////////////////////////////////////
void DrawStripes (int y)
{
    if(!SPEAR::flag)
    {
        VL_Bar (0, y, LOGIC_WIDTH, 24, 0);
        VWB_Hlin (0, 319, y + 22, STRIPE);
    }
    else
    {
        VL_Bar (0, y, LOGIC_WIDTH, 22, 0);
        VWB_Hlin (0, 319, y + 23, 0);
    }
}

static void ShootSnd ()
{
    Sound::Play (SHOOTSND);
}

// IOANCH 20130726: moved this to CFG_

void Menu::SetSpearModuleValues()
{
	g_bkgdColor = SPEAR::flag ? 0x9d : 0x2d;
	g_bordColor = SPEAR::flag ? 0x99 : 0x29;
	
	s_bord2Color = SPEAR::flag ? 0x93 : 0x23;
	s_deactive = SPEAR::flag ? DEACTIVE_sod : DEACTIVE_wl6;
}


// IOANCH 20130303: unification
static CP_iteminfo MusicItems_wl6={CTL_X,CTL_Y,6,0,32};
static CP_itemtype MusicMenu_wl6[]=
{
	{1,"Get Them!",0},
	{1,"Searching",0},
	{1,"P.O.W.",0},
	{1,"Suspense",0},
	{1,"War March",0},
	{1,"Around The Corner!",0},
	
	{1,"Nazi Anthem",0},
	{1,"Lurking...",0},
	{1,"Going After Hitler",0},
	{1,"Pounding Headache",0},
	{1,"Into the Dungeons",0},
	{1,"Ultimate Conquest",0},
	
	{1,"Kill the S.O.B.",0},
	{1,"The Nazi Rap",0},
	{1,"Twelfth Hour",0},
	{1,"Zero Hour",0},
	{1,"Ultimate Conquest",0},
	{1,"Wolfpack",0}
};
static CP_iteminfo MusicItems_sod={CTL_X,CTL_Y-20,9,0,32};
static CP_itemtype MusicMenu_sod[]=
{
	{1,"Funky Colonel Bill",0},
	{1,"Death To The Nazis",0},
	{1,"Tiptoeing Around",0},
	{1,"Is This THE END?",0},
	{1,"Evil Incarnate",0},
	{1,"Jazzin' Them Nazis",0},
	{1,"Puttin' It To The Enemy",0},
	{1,"The SS Gonna Get You",0},
	{1,"Towering Above",0}
};

void Menu::DoJukebox()
// IOANCH 20130301: unification culling
{
    int which,lastsong=-1;
    unsigned start;
    // IOANCH 20130303: unification
    unsigned songs_wl6[]=
    {
        GETTHEM_MUS_wl6,
        SEARCHN_MUS_wl6,
        POW_MUS_wl6,
        SUSPENSE_MUS_wl6,
        WARMARCH_MUS_wl6,
        CORNER_MUS_wl6,
		
        NAZI_OMI_MUS_wl6,
        PREGNANT_MUS_wl6,
        GOINGAFT_MUS_wl6,
        HEADACHE_MUS_wl6,
        DUNGEON_MUS_wl6,
        ULTIMATE_MUS_wl6,
		
        INTROCW3_MUS_wl6,
        NAZI_RAP_MUS_wl6,
        TWELFTH_MUS_wl6,
        ZEROHOUR_MUS_wl6,
        ULTIMATE_MUS_wl6,
        PACMAN_MUS_wl6
    };
    unsigned songs_sod[]=
    {
        XFUNKIE_MUS_sod,             // 0
        XDEATH_MUS_sod,              // 2
        XTIPTOE_MUS_sod,             // 4
        XTHEEND_MUS_sod,             // 7
        XEVIL_MUS_sod,               // 17
        XJAZNAZI_MUS_sod,            // 18
        XPUTIT_MUS_sod,              // 21
        XGETYOU_MUS_sod,             // 22
        XTOWER2_MUS_sod              // 23
    };
    unsigned *songs = IMPALE(songs);
    // IOANCH 20130303: unification
    CP_iteminfo &MusicItems = IMPALE(MusicItems);
    CP_itemtype *MusicMenu = IMPALE(MusicMenu);
	
    myInput.clearKeysDown();
    if (!sd_adLibPresent && !sd_soundBlasterPresent)
        return;
	
    MenuFadeOut();
	
    // IOANCH 20130301: unification culling
    start = SPEAR::flag ? 0 : ((I_GetTicks()/10)%3)*6;
	
    graphSegs.cacheChunk (SPEAR::g(STARTFONT)+1);
    
    if(SPEAR::flag)
        CacheLump (SPEAR::g(BACKDROP_LUMP_START),SPEAR::g(BACKDROP_LUMP_END));
    else
        CacheLump (SPEAR::g(CONTROLS_LUMP_START),SPEAR::g(CONTROLS_LUMP_END));
	audioSegs.loadAllSounds(sd_soundMode);
    // IOANCH 20130302: unification
    fontnumber=1;
    ClearMScreen ();
    VWB_DrawPic(112,184,SPEAR::g(C_MOUSELBACKPIC));
    DrawStripes (10);
    SETFONTCOLOR (TEXTCOLOR,Menu::g_bkgdColor);
	
    if(!SPEAR::flag)
        DrawWindow (CTL_X-2,CTL_Y-6,280,13*7,Menu::g_bkgdColor);
    else
        DrawWindow (CTL_X-2,CTL_Y-26,280,13*10,Menu::g_bkgdColor);
	
    DrawMenu (&MusicItems,&MusicMenu[start]);
	
    SETFONTCOLOR (READHCOLOR,Menu::g_bkgdColor);
    PrintY=15;
    WindowX = 0;
    WindowY = LOGIC_WIDTH;
    US_CPrint ("Robert's Jukebox");
	
    SETFONTCOLOR (TEXTCOLOR,Menu::g_bkgdColor);
    I_UpdateScreen();
    MenuFadeIn();
	
    do
    {
        which = HandleMenu(&MusicItems,&MusicMenu[start],NULL);
        if (which>=0)
        {
            if (lastsong >= 0)
                MusicMenu[start+lastsong].active = 1;
			
            StartCPMusic(songs[start + which]);
            MusicMenu[start+which].active = 2;
            DrawMenu (&MusicItems,&MusicMenu[start]);
            I_UpdateScreen();
            lastsong = which;
        }
    } while(which>=0);
	
    MenuFadeOut();
    myInput.clearKeysDown();
    // IOANCH 20130303: unification
	
    if(SPEAR::flag)
        UnCacheLump (SPEAR::g(BACKDROP_LUMP_START),SPEAR::g(BACKDROP_LUMP_END));
    else
        UnCacheLump (SPEAR::g(CONTROLS_LUMP_START),SPEAR::g(CONTROLS_LUMP_END));
	
}