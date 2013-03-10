#ifndef WL_PLAY_H_
#define WL_PLAY_H_


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
extern  boolean     buttonstate[NUMBUTTONS];
extern  objtype     objlist[MAXACTORS];
extern  boolean     buttonheld[NUMBUTTONS];
extern  exit_t      playstate;
extern  boolean     madenoise;
extern  statobj_t   statobjlist[MAXSTATS];
extern  statobj_t   *laststatobj;
extern  objtype     *newobj,*killerobj;
extern  doorobj_t   doorobjlist[MAXDOORS];
extern  doorobj_t   *lastdoorobj;
extern  int         godmode;

extern  boolean     demorecord,demoplayback;
extern  int8_t      *demoptr, *lastdemoptr;
extern  memptr      demobuffer;

//
// control info
//
extern  boolean     mouseenabled,joystickenabled;
extern  int         dirscan[4];
extern  int         buttonscan[NUMBUTTONS];
extern  int         buttonmouse[4];
extern  int         buttonjoy[32];

void    InitActorList (void);
void    GetNewActor (void);
void    PlayLoop (void);

void    CenterWindow(word w,word h);

void    InitRedShifts (void);
void    FinishPaletteShifts (void);

void    RemoveObj (objtype *gone);
void    PollControls (void);
int     StopMusic(void);
void    StartMusic(void);
void    ContinueMusic(int offs);
void    StartDamageFlash (int damage);
void    StartBonusFlash (void);

// IOANCH 20130302: unification
extern  int32_t     funnyticount;           // FOR FUNNY BJ FACE

extern  objtype     *objfreelist;     // *obj,*player,*lastobj,

extern  boolean     noclip,ammocheat;
extern  int         singlestep, extravbls;


#endif
