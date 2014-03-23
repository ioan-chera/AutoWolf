/*
 =============================================================================
 
 WL_GAME DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_GAME_H_
#define WL_GAME_H_

extern  gametype        gamestate;
extern  byte            bordercol;
extern  std::string         cfg_demoname;

void    SetupGameLevel ();
void    GameLoop ();
void    DrawPlayBorder ();
void    DrawStatusBorder (byte color);
void    DrawPlayScreen ();
void    DrawPlayBorderSides ();
void    ShowActStatus();

void    PlayDemo (int demonumber);
void    RecordDemo ();

// IOANCH 20130302: unification

extern  int32_t            spearx,speary;
extern  unsigned        spearangle;
extern  Boolean8         spearflag;

// IOANCH: cleared ClearMemory
// IOANCH 20130621: added constant #defines for player starts
#define PLAYER_START_NORTH 19
#define PLAYER_START_EAST  20
#define PLAYER_START_SOUTH 21
#define PLAYER_START_WEST  22

// JAB
// IOANCH 20130802: made these as functions
#define PlaySoundLocTile(s,tx,ty)       PlaySoundLocGlobal(s,(((int32_t)(tx) << TILESHIFT) + (1L << (TILESHIFT - 1))),(((int32_t)ty << TILESHIFT) + (1L << (TILESHIFT - 1))))
//#define PlaySoundLocActor(s,ob)         PlaySoundLocGlobal(s,(ob)->x,(ob)->y)
void    PlaySoundLocActor(word s, const objtype *ob,  objtype *source = NULL);
void    PlaySoundLocGlobal(word s,fixed gx,fixed gy);
void UpdateSoundLoc();


#endif
