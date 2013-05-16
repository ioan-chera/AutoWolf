#ifndef WL_GAME_H_
#define WL_GAME_H_

extern  gametype        gamestate;
extern  byte            bordercol;
extern  SDL_Surface     *latchpics[NUMLATCHPICS];
class PString;
extern  PString         demoname;

void    SetupGameLevel (void);
void    GameLoop (void);
void    DrawPlayBorder (void);
void    DrawStatusBorder (byte color);
void    DrawPlayScreen (void);
void    DrawPlayBorderSides (void);
void    ShowActStatus();

void    PlayDemo (int demonumber);
void    RecordDemo (void);

// IOANCH 20130302: unification

extern  Point2D<int32_t>   spearPoint;
extern  unsigned        spearangle;
extern  Boolean         spearflag;



#define ClearMemory SD_StopDigitized


// JAB
#define PlaySoundLocTile(s,tx,ty)       PlaySoundLocGlobal(s,(((int32_t)(tx) << TILESHIFT) + (1L << (TILESHIFT - 1))),(((int32_t)ty << TILESHIFT) + (1L << (TILESHIFT - 1))))
#define PlaySoundLocActor(s,ob)         PlaySoundLocGlobal(s,(ob)->x,(ob)->y)
void    PlaySoundLocGlobal(word s,fixed gx,fixed gy);
void UpdateSoundLoc(void);


#endif
