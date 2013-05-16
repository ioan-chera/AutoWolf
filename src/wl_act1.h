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

#ifndef WL_ACT1_H_
#define WL_ACT1_H_

void InitStaticList ();
void SpawnStatic (Point2D<int> tilePoint, int type);
void PlaceItemType (int itemtype, Point2D<int> tilePoint);
void InitAreas ();
void InitDoorList ();
void SpawnDoor (Point2D<int> tilePoint, Boolean vertical, int lock);
void OpenDoor (int door);
void OperateDoor (int door);
void MoveDoors ();
void PushWall (Point2D<int> checkPoint, int dir);
void MovePWalls ();


extern  short     doornum;
extern  word      doorposition[MAXDOORS];
extern  byte      areaconnect[NUMAREAS][NUMAREAS];
extern  Boolean   areabyplayer[NUMAREAS];
extern word     pwallstate;
extern word     pwallpos;        // amount a pushable wall has been moved (0-63)
extern word     pwallx,pwally;
extern byte     pwalldir,pwalltile;

#endif
