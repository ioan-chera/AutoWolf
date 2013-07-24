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
////////////////////////////////////////////////////////////////////////////////
//
// Miscellaneous system operations. Inspired from Doom/Eternity's
//
////////////////////////////////////////////////////////////////////////////////

#include "wl_def.h"
#include "CocoaFun.h"
#include "i_system.h"

//
// I_Delay
//
// Delays the system for the given milliseconds
//
void I_Delay(unsigned ms)
{
   SDL_Delay((Uint32)ms);
}

//
// I_GetTicks
//
// Gets the number of ticks since start
//
uint32_t I_GetTicks(void)
{
   return (uint32_t)SDL_GetTicks();
}

//
// I_Notify
//
// Posts a notification, depending on platform
//
void I_Notify(const char *msg)
{
#ifdef __APPLE__
   Cocoa_PostNotification(SPEAR.FullTitle(), msg);
#endif
}