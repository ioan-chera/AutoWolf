#ifndef WL_STATE_H_
#define WL_STATE_H_

#define TURNTICS        10
#define SPDPATROL       512
#define SPDDOG          1500


void    InitHitRect (objtype *ob, unsigned radius);
void    SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state);
void    NewState (objtype *ob, statetype *state);

boolean TryWalk (objtype *ob);
void    SelectChaseDir (objtype *ob);
void    SelectDodgeDir (objtype *ob);
void    SelectRunDir (objtype *ob);
void    MoveObj (objtype *ob, int32_t move);
boolean SightPlayer (objtype *ob);

void    KillActor (objtype *ob);
void    DamageActor (objtype *ob, unsigned damage);

boolean CheckLine (objtype *ob);
boolean CheckSight (objtype *ob);


#endif

