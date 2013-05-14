#ifndef WL_STATE_H_
#define WL_STATE_H_

#define TURNTICS        10
#define SPDPATROL       512
#define SPDDOG          1500


void    InitHitRect (objtype *ob, unsigned radius);
void    SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state);
void    NewState (objtype *ob, statetype *state);

Boolean TryWalk (objtype *ob);
void    SelectChaseDir (objtype *ob);
void    SelectDodgeDir (objtype *ob);
void    SelectRunDir (objtype *ob);
void    MoveObj (objtype *ob, int32_t move);
Boolean SightPlayer (objtype *ob);

void    KillActor (objtype *ob);
void    DamageActor (objtype *ob, unsigned damage);

Boolean CheckLine (objtype *ob, Boolean solidActors = false);
Boolean CheckSight (objtype *ob);


#endif

