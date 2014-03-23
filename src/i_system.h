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

#ifndef __Wolf4SDL__i_system__
#define __Wolf4SDL__i_system__

#include <stdint.h>

class PString;

void I_Delay(unsigned ms);
uint32_t I_GetTicks(void);

void I_Notify(const char *msg);
void I_MakeDir(const std::string& dirname);
void I_ChangeDir(const std::string& dirname);
std::string I_GetSettingsDir();
std::string I_ResolveCaseInsensitivePath(const std::string& dirname, const std::string& basename);

void *I_CheckedMalloc(size_t sz);
void *I_CheckedRealloc(void *ptr, size_t sz);

void I_DisplayAlertOnError();

#endif /* defined(__Wolf4SDL__i_system__) */
