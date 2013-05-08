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

#include "version.h"

#ifdef USE_PARALLAX
#include <assert.h>
#include "wl_def.h"

#ifdef USE_FEATUREFLAGS

// The lower left tile of every map determines the start texture of the parallax sky.
static int GetParallaxStartTexture()
{
    int startTex = ffDataBottomLeft;
    assert(startTex >= 0 && startTex < PMSpriteStart);
    return startTex;
}

#else

static int GetParallaxStartTexture()
{
    int startTex;
    switch(gamestate.episode * 10 + mapon)
    {
        case  0: startTex = 20; break;
        default: startTex =  0; break;
    }
    assert(startTex >= 0 && startTex < PMSpriteStart);
    return startTex;
}

#endif

void DrawParallax(byte *vbuf, unsigned vbufPitch)
{
    int startpage = GetParallaxStartTexture();
    int midangle = player->angle * (FINEANGLES / ANGLES);
    int skyheight = viewheight >> 1;
    int curtex = -1;
    byte *skytex;

    startpage += USE_PARALLAX - 1;

    for(int x = 0; x < viewwidth; x++)
    {
        int curang = pixelangle[x] + midangle;
        if(curang < 0) curang += FINEANGLES;
        else if(curang >= FINEANGLES) curang -= FINEANGLES;
        int xtex = curang * USE_PARALLAX * TEXTURESIZE / FINEANGLES;
        int newtex = xtex >> TEXTURESHIFT;
        if(newtex != curtex)
        {
            curtex = newtex;
            skytex = PM_GetTexture(startpage - curtex);
        }
        int texoffs = TEXTUREMASK - ((xtex & (TEXTURESIZE - 1)) << TEXTURESHIFT);
        int yend = skyheight - (wallheight[x] >> 3);
        if(yend <= 0) continue;

        for(int y = 0, offs = x; y < yend; y++, offs += vbufPitch)
            vbuf[offs] = skytex[texoffs + (y * TEXTURESIZE) / skyheight];
    }
}

#endif
