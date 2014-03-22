/*
 =============================================================================
 
 WL_STATE DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_STATE_H_
#define WL_STATE_H_

#define TURNTICS        10
#define SPDPATROL       512
#define SPDDOG          1500


void    SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state);

void    SelectChaseDir (objtype *ob);
void    SelectDodgeDir (objtype *ob);
void    SelectRunDir (objtype *ob);
void    MoveObj (objtype *ob, int32_t move);


#endif

