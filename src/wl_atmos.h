//
// Copyright (C) 2007-2011  Moritz Kroll
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

#ifndef _WL_ATMOS_H_
#define _WL_ATMOS_H_

#if defined(USE_STARSKY) || defined(USE_RAIN) || defined(USE_SNOW)
    void Init3DPoints();
#endif

#ifdef USE_STARSKY
    void DrawStarSky(byte *vbuf, uint32_t vbufPitch);
#endif

#ifdef USE_RAIN
    void DrawRain(byte *vbuf, uint32_t vbufPitch);
#endif

#ifdef USE_SNOW
    void DrawSnow(byte *vbuf, uint32_t vbufPitch);
#endif

#endif
