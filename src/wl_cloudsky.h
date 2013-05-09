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

#if defined(USE_CLOUDSKY) && !defined(_WL_CLOUDSKY_H_)
#define _WL_CLOUDSKY_H_

typedef struct
{
    int length;
    int startAndDir;
} colormapentry_t;

typedef struct
{
    int numColors;
    colormapentry_t *entries;
} colormap_t;

typedef struct
{
    // The seed defines the look of the sky and every value (0-4294967295)
    // describes an unique sky. You can play around with these inside the game
    // when pressing <TAB>+Z in debug mode. There you'll be able to change the
    // active seed to find out a value, which is suitable for your needs.
    uint32_t seed;

    // The speed defines how fast the clouds will move (0-65535)
    uint32_t speed;

    // The angle defines the move direction (0-359)
    uint32_t angle;

    // An index selecting the color map to be used for this sky definition.
    // This value can also be chosen with <TAB>+Z
    uint32_t colorMapIndex;
} cloudsky_t;

extern cloudsky_t *curSky;
extern colormap_t colorMaps[];
extern const int numColorMaps;

void InitSky();
void DrawClouds(byte *vbuf, unsigned vbufPitch, int min_wallheight);

#ifndef USE_FEATUREFLAGS
int GetCloudSkyDefID();
#endif

#endif
