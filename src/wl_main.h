#ifndef WL_MAIN_H_
#define WL_MAIN_H_

#include <stdio.h>


extern  boolean  loadedgame;
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

extern  boolean  startgame;
extern  char     str[80];
class PString;
extern  PString  configdir;            // IOANCH 20130509: use PString for paths
//extern  char     configdir[MAX_PATH_LENGTH];    // IOANCH 20130307
extern  char     configname[13];

//
// Command line parameter variables
//
extern  boolean  param_debugmode;
extern  boolean  param_nowait;
extern  int      param_difficulty;
extern  int      param_tedlevel;
extern  int      param_joystickindex;
extern  int      param_joystickhat;
extern  int      param_samplerate;
extern  int      param_audiobuffer;
extern  int      param_mission;
extern  boolean  param_goodtimes;
extern  boolean  param_ignorenumchunks;


void            NewGame (int difficulty,int episode);
void            CalcProjection (int32_t focal);
void            NewViewSize (int width);
boolean         SetViewSize (unsigned width, unsigned height);
boolean         LoadTheGame(FILE *file,int x,int y);
boolean         SaveTheGame(FILE *file,int x,int y);
void            ShowViewSize (int width);
void            ShutdownId (void);

#endif
