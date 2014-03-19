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

//
// WL_MENU.H
//
// IOANCH 20130302: unification

#ifndef WL_MENU_H_
#define WL_MENU_H_

#include "id_in.h"

namespace Menu
{
	extern int g_bkgdColor;
	extern int g_bordColor;
	
	void DoJukebox();
	void SetSpearModuleValues();
}

#define MenuFadeOut() VL_FadeOut(0, 255, SPEAR() ? 0 : 43, 0, SPEAR() ? 51 : 0, 10)

#define READCOLOR       0x4a
#define READHCOLOR      0x47
#define VIEWCOLOR       0x7f
#define TEXTCOLOR       0x17
#define HIGHLIGHT       0x13
#define MenuFadeIn()    VL_FadeIn(0,255,IMPALE(vid_palette),10)

            // IOANCH 20130301: unification music
#define MENUSONG        IMPALE((int)WONDERIN_MUS)

#define INTROSONG       (SPEAR() ? (int)XTOWER2_MUS_sod : (int)NAZI_NOR_MUS_wl6)

#define SENSITIVE       60
#define CENTERX         ((int) cfg_screenWidth / 2)
#define CENTERY         ((int) cfg_screenHeight / 2)

#define MENU_X  76
#define MENU_Y  55
#define MENU_W  178
// IOANCH 20130301: unification culling
#define MENU_H  13*9+6

#define SM_X    48
#define SM_W    250

#define SM_Y1   20
#define SM_H1   4*13-7
#define SM_Y2   SM_Y1+5*13
#define SM_H2   4*13-7
#define SM_Y3   SM_Y2+5*13
#define SM_H3   3*13-7

#define CTL_X   24
// IOANCH 20130301: unification culling
#define CTL_Y   86
#define CTL_W   284
#define CTL_H   60

#define LSM_X   85
#define LSM_Y   55
#define LSM_W   175
#define LSM_H   10*13+10

#define NM_X    50
#define NM_Y    100
#define NM_W    225
#define NM_H    13*4+15

#define NE_X    10
#define NE_Y    23
#define NE_W    320-NE_X*2
#define NE_H    200-NE_Y*2

#define CST_X           20
#define CST_Y           48
#define CST_START       60
#define CST_SPC 60

void US_SetScanNames();

//
// TYPEDEFS
//
struct CP_iteminfo
{
   short x,y,amount,curpos,indent;
} ;

struct CP_itemtype
{
   short active;
   char string[36];
   int (* routine)(int temp1);
} ;

extern CP_itemtype MainMenu[];
extern CP_iteminfo MainItems;

//
// FUNCTION PROTOTYPES
//

void US_ControlPanel(ScanCode);

void EnableEndGameMenuItem();

void menu_SetupSaveGames();

void DrawMenu(const CP_iteminfo *item_i,const CP_itemtype *items);
int  HandleMenu(CP_iteminfo *item_i, const CP_itemtype *items, void (*routine)(int w));
void ClearMScreen();
void ReadAnyControl(CursorInfo *ci);
void TicDelay(int count);
void CacheLump(int lumpstart,int lumpend);
void UnCacheLump(int lumpstart,int lumpend);
int StartCPMusic(int song);

void Message(const char *string);

void DrawStripes(int y);

int CP_ViewScores(int);

void CFG_CheckForEpisodes();

void FreeMusic();

enum menuitems
{
        newgame,
        soundmenu,
        control,
        loadgame,
        savegame,
        changeview,
// IOANCH 20130301: unification culling

        viewscores,
        backtodemo,
        quit
};

//
// WL_INTER
//
struct  LRstruct
{
 int32_t kill,secret,treasure;   // IOANCH: since it's serialized, it is int32_t
 int32_t time;
};

extern LRstruct LevelRatios[];

void Write (int x,int y,const char *string);

// IOANCH 20130726: made extern
extern CP_itemtype menu_newep[];
extern int menu_epselect[6];
extern int menu_missingep;
extern PString cfg_savename;

#endif
