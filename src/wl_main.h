/*
 =============================================================================
 
 WL_MAIN DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_MAIN_H_
#define WL_MAIN_H_



extern  Boolean  loadedgame;
extern  fixed    focallength;
extern  int      viewscreenx, viewscreeny;
extern  int      viewwidth;
extern  int      viewheight;
extern  short    centerx;
extern  int32_t  heightnumerator;
extern  fixed    scale;

extern  int      dirangle[9];

extern  int      mouseadjustment;
extern  int      shootdelta;
extern  unsigned screenofs;

extern  Boolean  startgame;
extern  char     str[80];
class PString;
extern  PString  cfg_configname;


void            NewGame (int difficulty,int episode);
void            CalcProjection (int32_t focal);
void            NewViewSize (int width);
Boolean         SetViewSize (unsigned width, unsigned height);
Boolean         LoadTheGame(FILE *file,int x,int y);
Boolean         SaveTheGame(FILE *file,int x,int y);
void            ShowViewSize (int width);
void            ShutdownId ();

#endif
