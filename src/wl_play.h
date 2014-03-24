/*
 =============================================================================
 
 WL_PLAY DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_PLAY_H_
#define WL_PLAY_H_

#include <functional>

#define BASEMOVE                35
#define RUNMOVE                 70
#define BASETURN                35
#define RUNTURN                 70

#define JOYSCALE                2

extern  byte            tilemap[MAPSIZE][MAPSIZE];      // wall values only
extern  byte            spotvis[MAPSIZE][MAPSIZE];
extern  objtype         *actorat[MAPSIZE][MAPSIZE];

extern  objtype         *player;

extern  unsigned        tics;
extern  int             viewsize;

extern  int             lastgamemusicoffset;

//
// current user input
//
extern  int         controlx,controly;              // range from -100 to 100
extern  Boolean8     buttonstate[NUMBUTTONS];
extern  objtype     objlist[MAXACTORS];
extern  Boolean8     buttonheld[NUMBUTTONS];
extern  exit_t      playstate;
extern  Boolean8     madenoise;
extern  statobj_t   statobjlist[MAXSTATS];
extern  statobj_t   *laststatobj;
extern  objtype     *newobj,*killerobj;
extern  doorobj_t   doorobjlist[MAXDOORS];
extern  doorobj_t   *lastdoorobj;
extern  int         godmode;

extern  Boolean8     demorecord,demoplayback;
extern  int8_t      *demoptr, *lastdemoptr;
extern  memptr      demobuffer;

//
// control info
//
extern  Boolean8     mouseenabled,joystickenabled;
extern  int         dirscan[4];
extern  int         buttonscan[NUMBUTTONS];
extern  int         buttonmouse[4];
extern  int         buttonjoy[32];

void    InitActorList ();
void    GetNewActor ();
void    PlayLoop ();

void    CenterWindow(word w,word h);

void    InitRedShifts ();
void    FinishPaletteShifts ();

void    PollControls ();
int     StopMusic();
void    StartMusic();
void    ContinueMusic(int offs);
void    StartDamageFlash (int damage);
void    StartBonusFlash ();

// IOANCH 20130302: unification
extern  int32_t     funnyticount;           // FOR FUNNY BJ FACE

extern  objtype     *objfreelist;     // *obj,*player,*lastobj,

extern  Boolean8     noclip,ammocheat;
extern  int         singlestep;

extern  unsigned        g_sessionNo;	// the time of the SetupGameLevel.
void StartNewSession();
void AddPostCommand(const std::function<void(void)> &cmd, unsigned inSessionNo);
void StartBackgroundWork(const std::function<void(unsigned)> &cmd, const std::function<void(void)> &postcmd);

#endif
