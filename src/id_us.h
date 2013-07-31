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
//	ID Engine
//	ID_US.h - Header file for the User Manager
//	v1.0d1
//	By Jason Blochowiak
//

#ifndef	__ID_US__
#define	__ID_US__

#ifdef	__DEBUG__
#define	__DEBUG_UserMgr__
#endif

//#define	HELPTEXTLINKED

#define	MaxX	320
#define	MaxY	200

#define	MaxHelpLines	500

#define	MaxHighName	57
#define	MaxScores	7

struct HighScore
{
    char	name[MaxHighName + 1];
    int32_t	score;
    word	completed,episode;
} ;
#pragma pack(push,1)

#define	MaxGameName		32
#define	MaxSaveGames	6
	struct SaveGame
{
    char	signature[4];
    word	*oldtest;
    Boolean	present;
    char	name[MaxGameName + 1];
} ;

#define	MaxString	128	// Maximum input string size

struct WindowRec
{
    int	x,y,
        w,h,
        px,py;
} ;	// Record used to save & restore screen windows
#pragma pack(pop)
extern	Boolean		ingame,		// Set by game code if a game is in progress
					loadedgame;	// Set if the current game was loaded
extern	word		PrintX,PrintY;	// Current printing location in the window
extern	word		WindowX,WindowY,// Current location of window
					WindowW,WindowH;// Current size of window

extern	void		(*USL_MeasureString)(const char *,word *,word *);
extern void			(*USL_DrawString)(const char *);

// IOANCH 27.05.2012: add rndindex
extern int rndindex;

extern	Boolean		(*USL_SaveGame)(int),(*USL_LoadGame)(int);
extern	void		(*USL_ResetGame)();
extern	SaveGame	Games[MaxSaveGames];
extern	HighScore	Scores[];

#define	US_HomeWindow()	{PrintX = WindowX; PrintY = WindowY;}

void            US_Startup();
void            US_Shutdown();
void			US_TextScreen(),
				US_UpdateTextScreen(),
				US_FinishTextScreen();
void			US_DrawWindow(word x,word y,word w,word h);
void			US_CenterWindow(word,word);
void			US_SaveWindow(WindowRec *win),
				US_RestoreWindow(WindowRec *win);
void 			US_ClearWindow();
void			US_SetPrintRoutines(void (*measure)(const char *,word *,word *),
									void (*print)(const char *));
void			US_PrintCentered(const char *s),
				US_CPrint(const char *s),
				US_CPrintLine(const char *s),
				US_Print(const char *s);
void			US_Printf(const char *formatStr, ...);
void			US_CPrintf(const char *formatStr, ...);

void			US_PrintUnsigned(longword n);
void			US_PrintSigned(int32_t n);
void			US_StartCursor(),
				US_ShutCursor();
void			US_CheckHighScore(int32_t score,word other);
void			US_DisplayHighScores(int which);
extern	Boolean	US_UpdateCursor();
Boolean         US_LineInput(int x,int y,char *buf,const char *def,Boolean escok,
                             int maxchars,int maxwidth);


void	        USL_PrintInCenter(const char *s,Rect r);
char 	        *USL_GiveSaveName(word game);

void            US_InitRndT(int randomize);
int             US_RndT();
int             US_RndTBot();

#endif
