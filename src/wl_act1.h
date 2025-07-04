//
// Copyright (C) 2013  Ioan Chera
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

/*
 =============================================================================
 
 WL_ACT1 DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_ACT1_H_
#define WL_ACT1_H_

namespace Act1
{
enum
{
	STATIC_ID_OFFSET = 23,
};

	void OpenDoor (int door);
	void SpawnDoor (int tilex, int tiley, Boolean8 vertical, int lock);
	void SetSpearModuleValues();
	wl_stat_t GetStaticType(int actorid);
}


void InitStaticList ();
void SpawnStatic (int tilex, int tiley, int type);
void PlaceItemType (int itemtype, int tilex, int tiley);
void InitAreas ();
void InitDoorList ();
void OperateDoor (int door);
void MoveDoors ();
void PushWall (int checkx, int checky, int dir);
void MovePWalls ();


extern  short     doornum;
extern  word      doorposition[MAXDOORS];
extern  byte      areaconnect[NUMAREAS][NUMAREAS];
extern  Boolean8   areabyplayer[NUMAREAS];
extern word     pwallstate;
extern word     pwallpos;        // amount a pushable wall has been moved (0-63)
extern word     pwallx,pwally;
extern byte     pwalldir,pwalltile;

#endif
