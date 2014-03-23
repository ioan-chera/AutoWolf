/*
 =============================================================================
 
 WL_MAIN DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_MAIN_H_
#define WL_MAIN_H_



extern  Boolean8  loadedgame;
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

extern  Boolean8  startgame;
extern  char     str[80];
class PString;
extern  std::string  cfg_configname;
extern PString global_error;


void            NewGame (int difficulty,int episode);
void            main_NewViewSize (int width);
Boolean8         SetViewSize (unsigned width, unsigned height);
Boolean8         LoadTheGame(FILE *file,int x,int y);
Boolean8         SaveTheGame(FILE *file,int x,int y);
void            ShowViewSize (int width);

void Quit(const char *message)
#ifndef _WIN32
__attribute__((noreturn))
#endif
;
#endif
