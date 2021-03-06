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

#if defined(USE_STARSKY) || defined(USE_RAIN) || defined(USE_SNOW)

#include "wl_def.h"

#if defined(USE_RAIN) || defined(USE_SNOW)
    uint32_t rainpos = 0;
#endif

typedef struct {
    int32_t x, y, z;
} point3d_t;

#define MAXPOINTS 400
point3d_t points[MAXPOINTS];

byte moon[100]={
     0,  0, 27, 18, 15, 16, 19, 29,  0,  0,
     0, 22, 16, 15, 15, 16, 16, 18, 24,  0,
    27, 17, 15, 17, 16, 16, 17, 17, 18, 29,
    18, 15, 15, 15, 16, 16, 17, 17, 18, 20,
    16, 15, 15, 16, 16, 17, 17, 18, 19, 21,
    16, 15, 17, 20, 18, 17, 18, 18, 20, 22,
    19, 16, 18, 19, 17, 17, 18, 19, 22, 24,
    28, 19, 17, 17, 17, 18, 19, 21, 25, 31,
     0, 23, 18, 19, 18, 20, 22, 24, 28,  0,
     0,  0, 28, 21, 20, 22, 28, 30,  0,  0 };

void Init3DPoints()
{
    int hvheight = viewheight >> 1;
    for(int i = 0; i < MAXPOINTS; i++)
    {
        point3d_t *pt = &points[i];
        pt->x = 16384 - (rand() & 32767);
        pt->z = 16384 - (rand() & 32767);
        float len = sqrt((float)pt->x * pt->x + (float)pt->z * pt->z);
        int j=50;
        do
        {
            pt->y = 1024 + (rand() & 8191);
            j--;
        }
        while(j > 0 && (float)pt->y * 256.F / len >= hvheight);
    }
}

#endif

#ifdef USE_STARSKY

void DrawStarSky(byte *vbuf, uint32_t vbufPitch)
{
    int hvheight = viewheight >> 1;
    int hvwidth = viewwidth >> 1;

    byte *ptr = vbuf;
    int i;
    for(i = 0; i < hvheight; i++, ptr += vbufPitch)
        memset(ptr, 0, viewwidth);

    for(i = 0; i < MAXPOINTS; i++)
    {
        point3d_t *pt = &points[i];
        int32_t x = pt->x * viewcos + pt->z * viewsin;
        int32_t y = pt->y << 16;
        int32_t z = (pt->z * viewcos - pt->x * viewsin) >> 8;
        if(z <= 0) continue;
        int shade = z >> 18;
        if(shade > 15) continue;
        int32_t xx = x / z + hvwidth;
        int32_t yy = hvheight - y / z;
        if(xx >= 0 && xx < viewwidth && yy >= 0 && yy < hvheight)
            vbuf[yy * vbufPitch + xx] = shade + 15;
    }

    int32_t x = 16384 * viewcos + 16384 * viewsin;
    int32_t z = (16384 * viewcos - 16384 * viewsin) >> 8;
    if(z <= 0) return;
    int32_t xx = x / z + hvwidth;
    int32_t yy = hvheight - ((hvheight - (hvheight >> 3)) << 22) / z;
    if(xx > -10 && xx < viewwidth)
    {
        int stopx = 10, starty = 0, stopy = 10;
        i = 0;
        if(xx < 0) i = -xx;
        if(xx > viewwidth - 11) stopx = viewwidth - xx;
        if(yy < 0) startj = -yy;
        if(yy > viewheight - 11) stopy = viewheight - yy;
        for(; i < stopx; i++)
            for(int j = starty; j < stopy; j++)
                vbuf[(yy + j) * vbufPitch + xx + i] = moon[j * 10 + i];
    }
}

#endif

#ifdef USE_RAIN

void DrawRain(byte *vbuf, uint32_t vbufPitch)
{
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
    fixed dist;                                // distance to row projection
    fixed tex_step;                            // global step per one screen pixel
    fixed gu, gv, floorx, floory;              // global texture coordinates
#endif

    fixed px = (player->y + FixedMul(0x7900, viewsin)) >> 6;
    fixed pz = (player->x - FixedMul(0x7900, viewcos)) >> 6;
    int32_t ax, az, x, y, z, xx, yy, height, actheight;
    int shade;
    int hvheight = viewheight >> 1;
    int hvwidth = viewwidth >> 1;

    rainpos -= tics * 900;
    for(int i = 0; i < MAXPOINTS; i++)
    {
        point3d_t *pt = &points[i];
        ax = pt->x + px;
        ax = 0x1fff - (ax & 0x3fff);
        az = pt->z + pz;
        az = 0x1fff - (az & 0x3fff);
        x = ax * viewcos + az * viewsin;
        y = -(heightnumerator << 7) + ((((pt->y << 6) + rainpos) & 0x0ffff) << 11);
        z = (az * viewcos - ax * viewsin) >> 8;
        if(z <= 0) continue;
        shade = z >> 17;
        if(shade > 13) continue;
        xx = x / z + hvwidth;
        if(xx < 0 || xx >= viewwidth) continue;
        actheight = y / z;
        yy = hvheight - actheight;
        height = (heightnumerator << 10) / z;
        if(actheight < 0) actheight = -actheight;
        if(actheight < (vid_wallheight[xx] >> 3) && height < vid_wallheight[xx]) continue;

        if(xx >= 0 && xx < viewwidth && yy > 0 && yy < viewheight)
        {
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
            // Find the rain's tile coordinate
            // NOTE: This sometimes goes over the map edges.
            dist = ((heightnumerator / ((height >> 3) + 1)) << 5);
            gu =  viewx + FixedMul(dist, viewcos);
            gv = -viewy + FixedMul(dist, viewsin);
            floorx = (  gu >> TILESHIFT     ) & 63;
            floory = (-(gv >> TILESHIFT) - 1) & 63;

            // Is there a ceiling tile?
            if(MAPSPOT(floorx, floory, 2) >> 8) continue;
#endif

            vbuf[yy * vbufPitch + xx] = shade+15;
            vbuf[(yy - 1) * vbufPitch + xx] = shade+16;
            if(yy > 2)
                vbuf[(yy - 2) * vbufPitch + xx] = shade+17;
        }
    }
}

#endif

#ifdef USE_SNOW

void DrawSnow(byte *vbuf, uint32_t vbufPitch)
{
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
    fixed dist;                                // distance to row projection
    fixed tex_step;                            // global step per one screen pixel
    fixed gu, gv, floorx, floory;              // global texture coordinates
#endif

    fixed px = (player->y + FixedMul(0x7900, viewsin)) >> 6;
    fixed pz = (player->x - FixedMul(0x7900, viewcos)) >> 6;
    int32_t ax, az, x, y, z, xx, yy, height, actheight;
    int shade;
    int hvheight = viewheight >> 1;
    int hvwidth = viewwidth >> 1;

    rainpos -= tics * 256;
    for(int i = 0; i < MAXPOINTS; i++)
    {
        point3d_t *pt = &points[i];
        ax = pt->x + px;
        ax = 0x1fff - (ax & 0x3fff);
        az = pt->z + pz;
        az = 0x1fff - (az & 0x3fff);
        x = ax * viewcos + az * viewsin;
        y = -(heightnumerator << 7) + ((((pt->y << 6) + rainpos) & 0x0ffff) << 11);
        z = (az * viewcos - ax * viewsin) >> 8;
        if(z <= 0) continue;
        shade = z >> 17;
        if(shade > 13) continue;
        xx = x / z + hvwidth;
        if(xx < 0 || xx >= viewwidth) continue;
        actheight = y / z;
        yy = hvheight - actheight;
        height = (heightnumerator << 10) / z;
        if(actheight < 0) actheight = -actheight;
        if(actheight < (vid_wallheight[xx] >> 3) && height < vid_wallheight[xx]) continue;
        if(xx > 0 && xx < viewwidth && yy > 0 && yy < viewheight)
        {
#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
            // Find the snow's tile coordinate
            // NOTE: This sometimes goes over the map edges.
            dist = ((heightnumerator / ((height >> 3) + 1)) << 5);
            gu =  viewx + FixedMul(dist, viewcos);
            gv = -viewy + FixedMul(dist, viewsin);
            floorx = (  gu >> TILESHIFT     ) & 63;
            floory = (-(gv >> TILESHIFT) - 1) & 63;

            // Is there a ceiling tile?
            if(MAPSPOT(floorx, floory, 2) >> 8) continue;
#endif

            if(shade < 10)
            {
                vbuf[yy * vbufPitch + xx] = shade+17;
                vbuf[yy * vbufPitch + xx - 1] = shade+16;
                vbuf[(yy - 1) * vbufPitch + xx] = shade+16;
                vbuf[(yy - 1) * vbufPitch + xx - 1] = shade+15;
            }
            else
                vbuf[yy * vbufPitch + xx] = shade+15;
        }
    }
}

#endif
