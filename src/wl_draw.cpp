//
// Copyright (C) 1991-1992  id Software
// Copyright (C) 2007-2011  Moritz Kroll
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
// Wolfenstein graphics routines
//
////////////////////////////////////////////////////////////////////////////////

// WL_DRAW.C

#include "wl_def.h"
#include "wl_act1.h"
#include "wl_act2.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_play.h"
#include "ActorStates.h"
#include "Config.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_in.h"
#include "id_pm.h"
#include "id_us.h"
#include "id_vh.h"
#include "wl_atmos.h"
#include "wl_cloudsky.h"
#include "wl_shade.h"

/*
=============================================================================

                               LOCAL CONSTANTS

=============================================================================
*/

// the door is the last picture before the sprites
#define DOORWALL        (vSwapData.spriteStart()-8)

#define ACTORSIZE       0x4000

/*
=============================================================================

                              GLOBAL VARIABLES

=============================================================================
*/

static byte *vbuf = NULL;
static unsigned vbufPitch = 0;

int32_t    lasttimecount;
Boolean8 fpscounter;

static int fps_frames = 0;
static int fps_time = 0;
static int fps = 0;

int *vid_wallheight;
static int min_wallheight;

//
// math tables
//
short *vid_pixelangle;
int32_t finetangent[FINEANGLES / 4];
fixed sintable[ANGLES + ANGLES / 4];
fixed *costable = sintable + (ANGLES / 4);

//
// refresh variables
//
fixed   viewx, viewy;                   // the focal point
static short   viewangle;
fixed   viewsin, viewcos;

static void    TransformActor (objtype *ob);
static int     CalcRotate (const objtype *ob);
static void    DrawScaleds ();
void    CalcTics ();
void    ThreeDRefresh ();



//
// wall optimization variables
//
static int     lastside;               // true for vertical
static int32_t    lastintercept;
static int     lasttilehit;
static int     lasttexture;

//
// ray tracing variables
//
static short    focaltx, focalty, viewtx, viewty;
static longword xpartialup, xpartialdown, ypartialup, ypartialdown;

static short   midangle;

static word    tilehit;
static int     pixx;

static short   xtile, ytile;
static short   xtilestep, ytilestep;
static int32_t    xintercept, yintercept;
static word    xspot, yspot;
static int     texdelta;

word horizwall[MAXWALLTILES], vertwall[MAXWALLTILES];


/*
============================================================================

                           3 - D  DEFINITIONS

============================================================================
*/

/*
========================
=
= TransformActor
=
= Takes paramaters:
=   gx,gy               : globalx/globaly of point
=
= globals:
=   viewx,viewy         : point of view
=   viewcos,viewsin     : sin/cos of viewangle
=   scale               : conversion from global value to screen value
=
= sets:
=   screenx,transx,transy,screenheight: projected edge location and size
=
========================
*/


//
// transform actor
//
static void TransformActor (objtype *ob)
{
   fixed gx, gy, gxt, gyt, nx, ny;

//
// translate point to view centered coordinates
//
   gx = ob->x - viewx;
   gy = ob->y - viewy;

//
// calculate newx
//
   gxt = FixedMul(gx, viewcos);
   gyt = FixedMul(gy, viewsin);
   nx = gxt - gyt - ACTORSIZE;     // fudge the shape forward a bit, because
   // the midpoint could put parts of the shape
   // into an adjacent wall

//
// calculate newy
//
   gxt = FixedMul(gx, viewsin);
   gyt = FixedMul(gy, viewcos);
   ny = gyt + gxt;

//
// calculate perspective ratio
//
   ob->transx = nx;
   ob->transy = ny;

   if (nx < MINDIST)               // too close, don't overflow the divide
   {
      ob->viewheight = 0;
      return;
   }

   ob->viewx = (word)(centerx + ny * scale / nx);

//
// calculate height (heightnumerator/(nx>>8))
//
   ob->viewheight = (word)(heightnumerator / (nx >> 8));
}

//==========================================================================

//
//
// TransformTile
//
// Takes paramaters:
//   tx,ty               : tile the object is centered in
//
// globals:
//   viewx,viewy         : point of view
//   viewcos,viewsin     : sin/cos of viewangle
//   scale               : conversion from global value to screen value
//
// sets:
//   screenx,transx,transy,screenheight: projected edge location and size
//
// Returns true if the tile is withing getting distance
//
//

static bool TransformTile (int tx, int ty, short *dispx, short *dispheight)
{
   fixed gx, gy, gxt, gyt, nx, ny;

//
// translate point to view centered coordinates
//
   gx = ((int32_t)tx << TILESHIFT) + 0x8000 - viewx;
   gy = ((int32_t)ty << TILESHIFT) + 0x8000 - viewy;

//
// calculate newx
//
   gxt = FixedMul(gx, viewcos);
   gyt = FixedMul(gy, viewsin);
   nx = gxt - gyt - 0x2000;        // 0x2000 is size of object

//
// calculate newy
//
   gxt = FixedMul(gx, viewsin);
   gyt = FixedMul(gy, viewcos);
   ny = gyt + gxt;


//
// calculate height / perspective ratio
//
   if (nx < MINDIST)               // too close, don't overflow the divide
      *dispheight = 0;
   else
   {
      *dispx = (short)(centerx + ny * scale / nx);
      *dispheight = (short)(heightnumerator / (nx >> 8));
   }

//
// see if it should be grabbed
//
   if (nx < TILEGLOBAL && ny > -TILEGLOBAL / 2 && ny < TILEGLOBAL / 2)
      return true;
   else
      return false;
}

//==========================================================================

/*
====================
=
= CalcHeight
=
= Calculates the height of xintercept,yintercept from viewx,viewy
=
====================
*/

static int CalcHeight()
{
   fixed z = FixedMul(xintercept - viewx, viewcos)
             - FixedMul(yintercept - viewy, viewsin);
   if(z < MINDIST) z = MINDIST;
   int height = heightnumerator / (z >> 8);
   if(height < min_wallheight) min_wallheight = height;
   return height;
}

//==========================================================================

/*
===================
=
= ScalePost
=
===================
*/

static const byte *postsource;
static int postx;
static int postwidth;

static void ScalePost()
{
   int ywcount, yoffs, yw, yd, yendoffs;
   byte col;

#ifdef USE_SHADING
   byte *curshades = shadetable[GetShade(vid_wallheight[postx])];
#endif

   ywcount = yd = vid_wallheight[postx] >> 3;
   if(yd <= 0) yd = 100;

   yoffs = (viewheight / 2 - ywcount) * vbufPitch;
   if(yoffs < 0) yoffs = 0;
   yoffs += postx;

   yendoffs = viewheight / 2 + ywcount - 1;
   yw = TEXTURESIZE - 1;

   while(yendoffs >= viewheight)
   {
      ywcount -= TEXTURESIZE / 2;
      while(ywcount <= 0)
      {
         ywcount += yd;
         yw--;
      }
      yendoffs--;
   }
   if(yw < 0) return;

#ifdef USE_SHADING
   col = curshades[postsource[yw]];
#else
   col = postsource[yw];
#endif
   yendoffs = yendoffs * vbufPitch + postx;
   while(yoffs <= yendoffs)
   {
      vbuf[yendoffs] = col;
      ywcount -= TEXTURESIZE / 2;
      if(ywcount <= 0)
      {
         do
         {
            ywcount += yd;
            yw--;
         }
         while(ywcount <= 0);
         if(yw < 0) break;
#ifdef USE_SHADING
         col = curshades[postsource[yw]];
#else
         col = postsource[yw];
#endif
      }
      yendoffs -= vbufPitch;
   }
}

/*
====================
=
= IOANCH 20121204
= HitWall
=
= Merged HitVertWall and HitHorizWall into one
=
= Original heading:
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

enum hitwall_t
{
   hw_vert,
   hw_horiz
};

static void HitWall (hitwall_t orient)
{
   int32_t interceptvar;
   int lastsidevar;
   int32_t lastinterceptvar;
   short *plastinterceptother;
   short tilemapxvar, tilemapyvar;
   const word *wallarray;
   int doorwallplus;

   int wallpic;
   int texture;

   switch(orient)
   {
   case hw_vert:

      interceptvar = yintercept;
      texture = ((yintercept + texdelta) >> TEXTUREFROMFIXEDSHIFT)&TEXTUREMASK;
      if (xtilestep == -1)
      {
         texture = TEXTUREMASK - texture;
         xintercept += TILEGLOBAL;
      }

      lastsidevar = 1;
      lastinterceptvar = xtile;
      plastinterceptother = &ytile;
      tilemapxvar = xtile - xtilestep;
      tilemapyvar = ytile;
      wallarray = vertwall;
      doorwallplus = 3;

      break;
   default:

      interceptvar = xintercept;
      texture = ((xintercept + texdelta) >> TEXTUREFROMFIXEDSHIFT)&TEXTUREMASK;
      if (ytilestep == -1)
         yintercept += TILEGLOBAL;
      else
         texture = TEXTUREMASK - texture;

      lastsidevar = 0;
      lastinterceptvar = ytile;
      plastinterceptother = &xtile;
      tilemapxvar = xtile;
      tilemapyvar = ytile - ytilestep;
      wallarray = horizwall;
      doorwallplus = 2;

      break;
   }

   if(lastside == lastsidevar && lastintercept == lastinterceptvar && lasttilehit == tilehit && !(lasttilehit & 0x40))
   {
      if((pixx & 3) && texture == lasttexture)
      {
         ScalePost();
         postx = pixx;
         vid_wallheight[pixx] = vid_wallheight[pixx - 1];
         return;
      }
      ScalePost();
      vid_wallheight[pixx] = CalcHeight();
      postsource += texture - lasttexture;
      postwidth = 1;
      postx = pixx;
      lasttexture = texture;
      return;
   }

   if(lastside != -1) ScalePost();

   lastside = lastsidevar;
   lastintercept = lastinterceptvar;
   lasttilehit = tilehit;
   lasttexture = texture;
   vid_wallheight[pixx] = CalcHeight();
   postx = pixx;
   postwidth = 1;

   if (tilehit & 0x40)
   {
      // check for adjacent doors
      *plastinterceptother = (short)(interceptvar >> TILESHIFT);
      if ( tilemap[tilemapxvar][tilemapyvar] & 0x80 )
         wallpic = DOORWALL + doorwallplus;
      else
         wallpic = wallarray[tilehit & ~0x40];
   }
   else
      wallpic = wallarray[tilehit];

   postsource = vSwapData.getTexture(wallpic) + texture;
}

//==========================================================================

/*
 ====================
 =
 = IOANCH 20121204
 = HitDoor
 =
 = Merged HitVertDoor and HitHorizDoor into one
 =
 ====================
 */

static void HitDoor (hitwall_t orient)
{
   int doorpage = 0;
   int doornum;
   int texture;

   // Added code
   int32_t interceptvar;
   if(orient == hw_horiz)
      interceptvar = xintercept;
   else
      interceptvar = yintercept;
   // end added code

   doornum = tilehit & 0x7f;
   texture = ((interceptvar - doorposition[doornum]) >> TEXTUREFROMFIXEDSHIFT)&TEXTUREMASK;

   if(lasttilehit == tilehit)
   {
      if((pixx & 3) && texture == lasttexture)
      {
         ScalePost();
         postx = pixx;
         vid_wallheight[pixx] = vid_wallheight[pixx - 1];
         return;
      }
      ScalePost();
      vid_wallheight[pixx] = CalcHeight();
      postsource += texture - lasttexture;
      postwidth = 1;
      postx = pixx;
      lasttexture = texture;
      return;
   }

   if(lastside != -1) ScalePost();

   lastside = 2;
   lasttilehit = tilehit;
   lasttexture = texture;
   vid_wallheight[pixx] = CalcHeight();
   postx = pixx;
   postwidth = 1;

   switch(doorobjlist[doornum].lock)
   {
   case dr_normal:
      doorpage = DOORWALL + (int)(orient == hw_vert);
      break;
   case dr_lock1:
   case dr_lock2:
   case dr_lock3:
   case dr_lock4:
      doorpage = DOORWALL + 6 + (int)(orient == hw_vert);
      break;
   case dr_elevator:
      doorpage = DOORWALL + 4 + (int)(orient == hw_vert);
      break;
   }

   postsource = vSwapData.getTexture(doorpage) + texture;
}

//==========================================================================

// IOANCH 20121204: deleted useless defines

//==========================================================================
// IOANCH 20130302: unification
static const byte vgaCeiling_wl6[] =
{
   0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0xbf,
   0x4e, 0x4e, 0x4e, 0x1d, 0x8d, 0x4e, 0x1d, 0x2d, 0x1d, 0x8d,
   0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x2d, 0xdd, 0x1d, 0x1d, 0x98,

   0x1d, 0x9d, 0x2d, 0xdd, 0xdd, 0x9d, 0x2d, 0x4d, 0x1d, 0xdd,
   0x7d, 0x1d, 0x2d, 0x2d, 0xdd, 0xd7, 0x1d, 0x1d, 0x1d, 0x2d,
   0x1d, 0x1d, 0x1d, 0x1d, 0xdd, 0xdd, 0x7d, 0xdd, 0xdd, 0xdd
};
static const byte vgaCeiling_sod[] =
{
   0x6f, 0x4f, 0x1d, 0xde, 0xdf, 0x2e, 0x7f, 0x9e, 0xae, 0x7f,
   0x1d, 0xde, 0xdf, 0xde, 0xdf, 0xde, 0xe1, 0xdc, 0x2e, 0x1d, 0xdc
};


/*
=====================
=
= VGAClearScreen
=
=====================
*/

static void VGAClearScreen ()
{
   // IOANCH 20130302: unification
   byte ceiling = IMPALE(vgaCeiling)[gamestate.episode * 10 + mapSegs.map()];

   int y;
   byte *ptr = vbuf;
#ifdef USE_SHADING
   for(y = 0; y < viewheight / 2; y++, ptr += vbufPitch)
      memset(ptr, shadetable[GetShade((viewheight / 2 - y) << 3)][ceiling], viewwidth);
   for(; y < viewheight; y++, ptr += vbufPitch)
      memset(ptr, shadetable[GetShade((y - viewheight / 2) << 3)][FLOOR_COLOUR],
             viewwidth);
#else
   for(y = 0; y < viewheight / 2; y++, ptr += vbufPitch)
      memset(ptr, ceiling, viewwidth);
   for(; y < viewheight; y++, ptr += vbufPitch)
      memset(ptr, FLOOR_COLOUR, viewwidth);
#endif
}

//==========================================================================

/*
=====================
=
= CalcRotate
=
=====================
*/

static int CalcRotate (const objtype *ob)
{
   int angle, viewangle;

   // this isn't exactly correct, as it should vary by a trig value,
   // but it is close enough with only eight rotations

   viewangle = player->angle + (centerx - ob->viewx) / 8;

   if (ob->obclass == rocketobj || ob->obclass == hrocketobj)
      angle = (viewangle - 180) - ob->angle;
   else
      angle = (viewangle - 180) - dirangle[ob->dir];

   angle += ANGLES / 16;
   while (angle >= ANGLES)
      angle -= ANGLES;
   while (angle < 0)
      angle += ANGLES;

   if (ob->state->rotate == 2)             // 2 rotation pain frame
      return 0;               // pain with shooting frame bugfix

   return angle / (ANGLES / 8);
}

static void ScaleShape (int xcenter, int shapenum, unsigned height, uint32_t flags)
{
   const t_compshape *shape;
   unsigned scale, pixheight;
   unsigned starty, endy;
   const word *cmdptr;
   byte *cline;
   byte *line;
   byte *vmem;
   int actx, i, upperedge;
   short newstart;
   int scrstarty, screndy, lpix, rpix, pixcnt, ycnt;
   unsigned j;
   byte col;

#ifdef USE_SHADING
   byte *curshades;
   if(flags & FL_FULLBRIGHT)
      curshades = shadetable[0];
   else
      curshades = shadetable[GetShade(height)];
#endif

   shape = vSwapData.getSprite(shapenum);

   scale = height >> 3;             // low three bits are fractional
   if(!scale) return;   // too close or far away

   pixheight = scale * SPRITESCALEFACTOR;
   actx = xcenter - scale;
   upperedge = viewheight / 2 - scale;

   cmdptr = shape->dataofs;

   for(i = shape->leftpix, pixcnt = i * pixheight, rpix = (pixcnt >> 6) + actx; i <= shape->rightpix; i++, cmdptr++)
   {
      lpix = rpix;
      if(lpix >= viewwidth) break;
      pixcnt += pixheight;
      rpix = (pixcnt >> 6) + actx;
      if(lpix != rpix && rpix > 0)
      {
         if(lpix < 0) lpix = 0;
         if(rpix > viewwidth)
         {
             rpix = viewwidth;
             i = shape->rightpix + 1;
         }
         cline = (byte *)shape + *cmdptr;
         while(lpix < rpix)
         {
            if(vid_wallheight[lpix] <= (int)height)
            {
               line = cline;
               while((endy = READWORD(line)) != 0)
               {
                  endy >>= 1;
                  newstart = READWORD(line);
                  starty = READWORD(line) >> 1;
                  j = starty;
                  ycnt = j * pixheight;
                  screndy = (ycnt >> 6) + upperedge;
                  if(screndy < 0) vmem = vbuf + lpix;
                  else vmem = vbuf + screndy * vbufPitch + lpix;
                  for(; j < endy; j++)
                  {
                     scrstarty = screndy;
                     ycnt += pixheight;
                     screndy = (ycnt >> 6) + upperedge;
                     if(scrstarty != screndy && screndy > 0)
                     {
#ifdef USE_SHADING
                        col = curshades[((byte *)shape)[newstart + j]];
#else
                        col = ((byte *)shape)[newstart + j];
#endif
                        if(scrstarty < 0) scrstarty = 0;
                        if(screndy > viewheight)
                        {
                            screndy = viewheight;
                            j = endy;
                        }

                        while(scrstarty < screndy)
                        {
                           *vmem = col;
                           vmem += vbufPitch;
                           scrstarty++;
                        }
                     }
                  }
               }
            }
            lpix++;
         }
      }
   }
}

static void SimpleScaleShape (int xcenter, int shapenum, unsigned height)
{
   const t_compshape   *shape;
   unsigned scale, pixheight;
   unsigned starty, endy;
   const word *cmdptr;
   byte *cline;
   byte *line;
   int actx, i, upperedge;
   short newstart;
   int scrstarty, screndy, lpix, rpix, pixcnt, ycnt;
   unsigned j;
   byte col;
   byte *vmem;

   // IOANCH 20130303: don't remap
   shape = vSwapData.getSprite(shapenum, false);

   scale = height >> 1;
   pixheight = scale * SPRITESCALEFACTOR;
   actx = xcenter - scale;
   upperedge = viewheight / 2 - scale;

   cmdptr = shape->dataofs;

   for(i = shape->leftpix, pixcnt = i * pixheight, rpix = (pixcnt >> 6) + actx; i <= shape->rightpix; i++, cmdptr++)
   {
      lpix = rpix;
      if(lpix >= viewwidth) break;
      pixcnt += pixheight;
      rpix = (pixcnt >> 6) + actx;
      if(lpix != rpix && rpix > 0)
      {
         if(lpix < 0) lpix = 0;
         if(rpix > viewwidth)
         {
             rpix = viewwidth;
             i = shape->rightpix + 1;
         }
         cline = (byte *)shape + *cmdptr;
         while(lpix < rpix)
         {
            line = cline;
            while((endy = READWORD(line)) != 0)
            {
               endy >>= 1;
               newstart = READWORD(line);
               starty = READWORD(line) >> 1;
               j = starty;
               ycnt = j * pixheight;
               screndy = (ycnt >> 6) + upperedge;
               if(screndy < 0) vmem = vbuf + lpix;
               else vmem = vbuf + screndy * vbufPitch + lpix;
               for(; j < endy; j++)
               {
                  scrstarty = screndy;
                  ycnt += pixheight;
                  screndy = (ycnt >> 6) + upperedge;
                  if(scrstarty != screndy && screndy > 0)
                  {
                     col = ((byte *)shape)[newstart + j];
                     if(scrstarty < 0) scrstarty = 0;
                     if(screndy > viewheight)
                     {
                         screndy = viewheight;
                         j = endy;
                     }

                     while(scrstarty < screndy)
                     {
                        *vmem = col;
                        vmem += vbufPitch;
                        scrstarty++;
                     }
                  }
               }
            }
            lpix++;
         }
      }
   }
}

/*
=====================
=
= DrawScaleds
=
= Draws all objects that are visable
=
=====================
*/

#define MAXVISABLE 250

struct visobj_t
{
   short      viewx,
              viewheight,
              shapenum;
   short      flags;          // this must be changed to uint32_t, when you
   // you need more than 16-flags for drawing
#ifdef USE_DIR3DSPR
   statobj_t *transsprite;
#endif
};

static visobj_t vislist[MAXVISABLE];
static visobj_t *visptr, *visstep, *farthest;

static void DrawScaleds ()
{
   int      i, least, numvisable, height;
   const byte     *tilespot, *visspot;
   unsigned spotloc;

   statobj_t *statptr;
   objtype   *obj;

   visptr = &vislist[0];

//
// place static objects
//
   for (statptr = &statobjlist[0] ; statptr != laststatobj ; statptr++)
   {
      if ((visptr->shapenum = statptr->shapenum) == -1)
         continue;                                               // object has been deleted

      if (!*statptr->visspot)
         continue;                                               // not visable

      if (TransformTile (statptr->tilex, statptr->tiley,
                         &visptr->viewx, &visptr->viewheight) && statptr->flags & FL_BONUS)
      {
          Agent::GetBonus (statptr);
         if(statptr->shapenum == -1)
            continue;                                           // object has been taken
      }

      if (!visptr->viewheight)
         continue;                                               // to close to the object

#ifdef USE_DIR3DSPR
      if(statptr->flags & FL_DIR_MASK)
         visptr->transsprite = statptr;
      else
         visptr->transsprite = NULL;
#endif

      if (visptr < &vislist[MAXVISABLE - 1])  // don't let it overflow
      {
         visptr->flags = (short) statptr->flags;
         visptr++;
      }
   }

//
// place active objects
//
   for (obj = player->next; obj; obj = obj->next)
   {
      if ((visptr->shapenum = obj->state->shapenum) == 0)
         continue;                                               // no shape

      spotloc = (obj->tilex << mapshift) + obj->tiley; // optimize: keep in struct?
      visspot = &spotvis[0][0] + spotloc;
      tilespot = &tilemap[0][0] + spotloc;

      //
      // could be in any of the nine surrounding tiles
      //
      if (*visspot
            || ( *(visspot - 1) && !*(tilespot - 1) )
            || ( *(visspot + 1) && !*(tilespot + 1) )
            || ( *(visspot - 65) && !*(tilespot - 65) )
            || ( *(visspot - 64) && !*(tilespot - 64) )
            || ( *(visspot - 63) && !*(tilespot - 63) )
            || ( *(visspot + 65) && !*(tilespot + 65) )
            || ( *(visspot + 64) && !*(tilespot + 64) )
            || ( *(visspot + 63) && !*(tilespot + 63) ) )
      {
         obj->active = ac_yes;
         TransformActor (obj);
         if (!obj->viewheight)
            continue;                                               // too close or far away

         visptr->viewx = obj->viewx;
         visptr->viewheight = obj->viewheight;
         if (visptr->shapenum == -1)
            visptr->shapenum = obj->temp1;  // special shape

         if (obj->state->rotate)
            visptr->shapenum += CalcRotate (obj);

         if (visptr < &vislist[MAXVISABLE - 1])  // don't let it overflow
         {
            visptr->flags = (short) obj->flags;
#ifdef USE_DIR3DSPR
            visptr->transsprite = NULL;
#endif
            visptr++;
         }
         obj->flags |= FL_VISABLE;
      }
      else
         obj->flags &= ~FL_VISABLE;
   }

//
// draw from back to front
//
   numvisable = (int) (visptr - &vislist[0]);

   if (!numvisable)
      return;                                                                 // no visable objects

   for (i = 0; i < numvisable; i++)
   {
      least = 32000;
      for (visstep = &vislist[0] ; visstep < visptr ; visstep++)
      {
         height = visstep->viewheight;
         if (height < least)
         {
            least = height;
            farthest = visstep;
         }
      }
      //
      // draw farthest
      //
#ifdef USE_DIR3DSPR
      if(farthest->transsprite)
         Scale3DShape(vbuf, vbufPitch, farthest->transsprite);
      else
#endif
         ScaleShape(farthest->viewx, farthest->shapenum, farthest->viewheight, farthest->flags);

      farthest->viewheight = 32000;
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = DrawPlayerWeapon
// =
// = Draw the player's hands
// =
//
////////////////////////////////////////////////////////////////////////////////


static const int weaponscale[NUMWEAPONS] = {SPR_KNIFEREADY, SPR_PISTOLREADY,
                               SPR_MACHINEGUNREADY, SPR_CHAINREADY
                              };

static void DrawPlayerWeapon ()
{
   int shapenum;

   // IOANCH 20130302: unification
   // IOANCH 20130303: do sprite remapping here
   if (!SPEAR::flag && gamestate.victoryflag)
   {
      // IOANCH 20130301: unification culling

      if (player->state == &s_deathcam && (GetTimeCount() & 32) )
         SimpleScaleShape(viewwidth / 2, SPEAR::sp(SPR_DEATHCAM), viewheight + 1);

      return;
   }

   if ((int)gamestate.weapon != -1)
   {
      shapenum = SPEAR::sp(weaponscale[gamestate.weapon]) + gamestate.weaponframe;
      SimpleScaleShape(viewwidth / 2, shapenum, viewheight + 1);
   }

   if (demorecord || demoplayback)
      SimpleScaleShape(viewwidth / 2, SPEAR::sp(SPR_DEMO), viewheight + 1);
}


//==========================================================================


/*
=====================
=
= CalcTics
=
=====================
*/

void CalcTics ()
{
//
// calculate tics since last refresh for adaptive timing
//
   if (lasttimecount > (int32_t) GetTimeCount())
      lasttimecount = GetTimeCount();    // if the game was paused a LONG time

   uint32_t curtime = I_GetTicks();
   tics = (curtime * 7) / 100 - lasttimecount;
   if(!tics)
   {
      // wait until end of current tic
      I_Delay(((lasttimecount + 1) * 100) / 7 - curtime);
      tics = 1;
   }

   lasttimecount += tics;

   if (tics > MAXTICS)
      tics = MAXTICS;
}


//==========================================================================

static void AsmRefresh()
{
   int32_t xstep = 0, ystep = 0;
   longword xpartial = 0, ypartial = 0;
   Boolean8 playerInPushwallBackTile = tilemap[focaltx][focalty] == 64;

   for(pixx = 0; pixx < viewwidth; pixx++)
   {
      short angl = midangle + vid_pixelangle[pixx];
      if(angl < 0) angl += FINEANGLES;
      if(angl >= 3600) angl -= FINEANGLES;
      if(angl < 900)
      {
         xtilestep = 1;
         ytilestep = -1;
         xstep = finetangent[900 - 1 - angl];
         ystep = -finetangent[angl];
         xpartial = xpartialup;
         ypartial = ypartialdown;
      }
      else if(angl < 1800)
      {
         xtilestep = -1;
         ytilestep = -1;
         xstep = -finetangent[angl - 900];
         ystep = -finetangent[1800 - 1 - angl];
         xpartial = xpartialdown;
         ypartial = ypartialdown;
      }
      else if(angl < 2700)
      {
         xtilestep = -1;
         ytilestep = 1;
         xstep = -finetangent[2700 - 1 - angl];
         ystep = finetangent[angl - 1800];
         xpartial = xpartialdown;
         ypartial = ypartialup;
      }
      else if(angl < 3600)
      {
         xtilestep = 1;
         ytilestep = 1;
         xstep = finetangent[angl - 2700];
         ystep = finetangent[3600 - 1 - angl];
         xpartial = xpartialup;
         ypartial = ypartialup;
      }
      yintercept = FixedMul(ystep, xpartial) + viewy;
      xtile = focaltx + xtilestep;
      xspot = (word)((xtile << mapshift) + ((uint32_t)yintercept >> 16));
      xintercept = FixedMul(xstep, ypartial) + viewx;
      ytile = focalty + ytilestep;
      yspot = (word)((((uint32_t)xintercept >> 16) << mapshift) + ytile);
      texdelta = 0;

      // Special treatment when player is in back tile of pushwall
      if(playerInPushwallBackTile)
      {
         if(    (pwalldir == di_east && xtilestep ==  1)
                || (pwalldir == di_west && xtilestep == -1))
         {
            int32_t yintbuf = yintercept - ((ystep * (64 - pwallpos)) >> 6);
            if((yintbuf >> 16) == focalty)   // ray hits pushwall back?
            {
               if(pwalldir == di_east)
                  xintercept = (focaltx << TILESHIFT) + (pwallpos << 10);
               else
                  xintercept = (focaltx << TILESHIFT) - TILEGLOBAL + ((64 - pwallpos) << 10);
               yintercept = yintbuf;
               ytile = (short) (yintercept >> TILESHIFT);
               tilehit = pwalltile;
               HitWall(hw_vert);
               continue;
            }
         }
         else if((pwalldir == di_south && ytilestep ==  1)
                 ||  (pwalldir == di_north && ytilestep == -1))
         {
            int32_t xintbuf = xintercept - ((xstep * (64 - pwallpos)) >> 6);
            if((xintbuf >> 16) == focaltx)   // ray hits pushwall back?
            {
               xintercept = xintbuf;
               if(pwalldir == di_south)
                  yintercept = (focalty << TILESHIFT) + (pwallpos << 10);
               else
                  yintercept = (focalty << TILESHIFT) - TILEGLOBAL + ((64 - pwallpos) << 10);
               xtile = (short) (xintercept >> TILESHIFT);
               tilehit = pwalltile;
               HitWall(hw_horiz);
               continue;
            }
         }
      }

      do
      {
         if(ytilestep == -1 && (yintercept >> 16) <= ytile) goto horizentry;
         if(ytilestep == 1 && (yintercept >> 16) >= ytile) goto horizentry;
vertentry:
         if((uint32_t)yintercept > mapheight * 65536 - 1 || (word)xtile >= mapwidth)
         {
            if(xtile < 0)
            {
                xintercept = 0;
                xtile = 0;
            }
            else if(xtile >= mapwidth)
            {
                xintercept = mapwidth << TILESHIFT;
                xtile = mapwidth - 1;
            }
            else
                xtile = (short) (xintercept >> TILESHIFT);
            if(yintercept < 0)
            {
                yintercept = 0;
                ytile = 0;
            }
            else if(yintercept >= (mapheight << TILESHIFT))
            {
                yintercept = mapheight << TILESHIFT;
                ytile = mapheight - 1;
            }
            yspot = 0xffff;
            tilehit = 0;
            HitWall(hw_horiz);
            break;
         }
         if(xspot >= maparea) break;
         tilehit = ((byte *)tilemap)[xspot];
         if(tilehit)
         {
            if(tilehit & 0x80)
            {
               int32_t yintbuf = yintercept + (ystep >> 1);
               if((yintbuf >> 16) != (yintercept >> 16))
                  goto passvert;
               if((word)yintbuf < doorposition[tilehit & 0x7f])
                  goto passvert;
               yintercept = yintbuf;
               xintercept = (xtile << TILESHIFT) | 0x8000;
               ytile = (short) (yintercept >> TILESHIFT);
               HitDoor(hw_vert);
            }
            else
            {
               if(tilehit == 64)
               {
                  if(pwalldir == di_west || pwalldir == di_east)
                  {
                     int32_t yintbuf;
                     int pwallposnorm;
                     int pwallposinv;
                     if(pwalldir == di_west)
                     {
                        pwallposnorm = 64 - pwallpos;
                        pwallposinv = pwallpos;
                     }
                     else
                     {
                        pwallposnorm = pwallpos;
                        pwallposinv = 64 - pwallpos;
                     }
                     if((pwalldir == di_east && xtile == pwallx && ((uint32_t)yintercept >> 16) == pwally)
                           || (pwalldir == di_west && !(xtile == pwallx && ((uint32_t)yintercept >> 16) == pwally)))
                     {
                        yintbuf = yintercept + ((ystep * pwallposnorm) >> 6);
                        if((yintbuf >> 16) != (yintercept >> 16))
                           goto passvert;

                        xintercept = (xtile << TILESHIFT) + TILEGLOBAL - (pwallposinv << 10);
                        yintercept = yintbuf;
                        ytile = (short) (yintercept >> TILESHIFT);
                        tilehit = pwalltile;
                        HitWall(hw_vert);
                     }
                     else
                     {
                        yintbuf = yintercept + ((ystep * pwallposinv) >> 6);
                        if((yintbuf >> 16) != (yintercept >> 16))
                           goto passvert;

                        xintercept = (xtile << TILESHIFT) - (pwallposinv << 10);
                        yintercept = yintbuf;
                        ytile = (short) (yintercept >> TILESHIFT);
                        tilehit = pwalltile;
                        HitWall(hw_vert);
                     }
                  }
                  else
                  {
                     int pwallposi = pwallpos;
                     if(pwalldir == di_north) pwallposi = 64 - pwallpos;
                     if((pwalldir == di_south && (word)yintercept < (pwallposi << 10))
                           || (pwalldir == di_north && (word)yintercept > (pwallposi << 10)))
                     {
                        if(((uint32_t)yintercept >> 16) == pwally && xtile == pwallx)
                        {
                           if((pwalldir == di_south && (int32_t)((word)yintercept) + ystep < (pwallposi << 10))
                                 || (pwalldir == di_north && (int32_t)((word)yintercept) + ystep > (pwallposi << 10)))
                              goto passvert;

                           if(pwalldir == di_south)
                              yintercept = (yintercept & 0xffff0000) + (pwallposi << 10);
                           else
                              yintercept = static_cast<int32_t>((yintercept & 0xffff0000) - TILEGLOBAL + (pwallposi << 10));
                           xintercept = xintercept - ((xstep * (64 - pwallpos)) >> 6);
                           xtile = (short) (xintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_horiz);
                        }
                        else
                        {
                           texdelta = -(pwallposi << 10);
                           xintercept = xtile << TILESHIFT;
                           ytile = (short) (yintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_vert);
                        }
                     }
                     else
                     {
                        if(((uint32_t)yintercept >> 16) == pwally && xtile == pwallx)
                        {
                           texdelta = -(pwallposi << 10);
                           xintercept = xtile << TILESHIFT;
                           ytile = (short) (yintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_vert);
                        }
                        else
                        {
                           if((pwalldir == di_south && (int32_t)((word)yintercept) + ystep > (pwallposi << 10))
                                 || (pwalldir == di_north && (int32_t)((word)yintercept) + ystep < (pwallposi << 10)))
                              goto passvert;

                           if(pwalldir == di_south)
                              yintercept = (yintercept & 0xffff0000) - ((64 - pwallpos) << 10);
                           else
                              yintercept = (yintercept & 0xffff0000) + ((64 - pwallpos) << 10);
                           xintercept = xintercept - ((xstep * pwallpos) >> 6);
                           xtile = (short) (xintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_horiz);
                        }
                     }
                  }
               }
               else
               {
                  xintercept = xtile << TILESHIFT;
                  ytile = (short) (yintercept >> TILESHIFT);
                  HitWall(hw_vert);
               }
            }
            break;
         }
passvert:
         *((byte *)spotvis + xspot) = 1;
         xtile += xtilestep;
         yintercept += ystep;
         xspot = (word)((xtile << mapshift) + ((uint32_t)yintercept >> 16));
      }
      while(1);
      continue;

      do
      {
         if(xtilestep == -1 && (xintercept >> 16) <= xtile) goto vertentry;
         if(xtilestep == 1 && (xintercept >> 16) >= xtile) goto vertentry;
horizentry:
         if((uint32_t)xintercept > mapwidth * 65536 - 1 || (word)ytile >= mapheight)
         {
            if(ytile < 0)
            {
                yintercept = 0;
                ytile = 0;
            }
            else if(ytile >= mapheight)
            {
                yintercept = mapheight << TILESHIFT;
                ytile = mapheight - 1;
            }
            else
                ytile = (short) (yintercept >> TILESHIFT);
            if(xintercept < 0)
            {
                xintercept = 0;
                xtile = 0;
            }
            else if(xintercept >= (mapwidth << TILESHIFT))
            {
                xintercept = mapwidth << TILESHIFT;
                xtile = mapwidth - 1;
            }
            xspot = 0xffff;
            tilehit = 0;
            HitWall(hw_vert);
            break;
         }
         if(yspot >= maparea) break;
         tilehit = ((byte *)tilemap)[yspot];
         if(tilehit)
         {
            if(tilehit & 0x80)
            {
               int32_t xintbuf = xintercept + (xstep >> 1);
               if((xintbuf >> 16) != (xintercept >> 16))
                  goto passhoriz;
               if((word)xintbuf < doorposition[tilehit & 0x7f])
                  goto passhoriz;
               xintercept = xintbuf;
               yintercept = (ytile << TILESHIFT) + 0x8000;
               xtile = (short) (xintercept >> TILESHIFT);
               HitDoor(hw_horiz);
            }
            else
            {
               if(tilehit == 64)
               {
                  if(pwalldir == di_north || pwalldir == di_south)
                  {
                     int32_t xintbuf;
                     int pwallposnorm;
                     int pwallposinv;
                     if(pwalldir == di_north)
                     {
                        pwallposnorm = 64 - pwallpos;
                        pwallposinv = pwallpos;
                     }
                     else
                     {
                        pwallposnorm = pwallpos;
                        pwallposinv = 64 - pwallpos;
                     }
                     if((pwalldir == di_south && ytile == pwally && ((uint32_t)xintercept >> 16) == pwallx)
                           || (pwalldir == di_north && !(ytile == pwally && ((uint32_t)xintercept >> 16) == pwallx)))
                     {
                        xintbuf = xintercept + ((xstep * pwallposnorm) >> 6);
                        if((xintbuf >> 16) != (xintercept >> 16))
                           goto passhoriz;

                        yintercept = (ytile << TILESHIFT) + TILEGLOBAL - (pwallposinv << 10);
                        xintercept = xintbuf;
                        xtile = (short) (xintercept >> TILESHIFT);
                        tilehit = pwalltile;
                        HitWall(hw_horiz);
                     }
                     else
                     {
                        xintbuf = xintercept + ((xstep * pwallposinv) >> 6);
                        if((xintbuf >> 16) != (xintercept >> 16))
                           goto passhoriz;

                        yintercept = (ytile << TILESHIFT) - (pwallposinv << 10);
                        xintercept = xintbuf;
                        xtile = (short) (xintercept >> TILESHIFT);
                        tilehit = pwalltile;
                        HitWall(hw_horiz);
                     }
                  }
                  else
                  {
                     int pwallposi = pwallpos;
                     if(pwalldir == di_west) pwallposi = 64 - pwallpos;
                     if((pwalldir == di_east && (word)xintercept < (pwallposi << 10))
                           || (pwalldir == di_west && (word)xintercept > (pwallposi << 10)))
                     {
                        if(((uint32_t)xintercept >> 16) == pwallx && ytile == pwally)
                        {
                           if((pwalldir == di_east && (int32_t)((word)xintercept) + xstep < (pwallposi << 10))
                                 || (pwalldir == di_west && (int32_t)((word)xintercept) + xstep > (pwallposi << 10)))
                              goto passhoriz;

                           if(pwalldir == di_east)
                              xintercept = (xintercept & 0xffff0000) + (pwallposi << 10);
                           else
                              xintercept = static_cast<int32_t>((xintercept & 0xffff0000) - TILEGLOBAL + (pwallposi << 10));
                           yintercept = yintercept - ((ystep * (64 - pwallpos)) >> 6);
                           ytile = (short) (yintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_vert);
                        }
                        else
                        {
                           texdelta = -(pwallposi << 10);
                           yintercept = ytile << TILESHIFT;
                           xtile = (short) (xintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_horiz);
                        }
                     }
                     else
                     {
                        if(((uint32_t)xintercept >> 16) == pwallx && ytile == pwally)
                        {
                           texdelta = -(pwallposi << 10);
                           yintercept = ytile << TILESHIFT;
                           xtile = (short) (xintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_horiz);
                        }
                        else
                        {
                           if((pwalldir == di_east && (int32_t)((word)xintercept) + xstep > (pwallposi << 10))
                                 || (pwalldir == di_west && (int32_t)((word)xintercept) + xstep < (pwallposi << 10)))
                              goto passhoriz;

                           if(pwalldir == di_east)
                              xintercept = (xintercept & 0xffff0000) - ((64 - pwallpos) << 10);
                           else
                              xintercept = (xintercept & 0xffff0000) + ((64 - pwallpos) << 10);
                           yintercept = yintercept - ((ystep * pwallpos) >> 6);
                           ytile = (short) (yintercept >> TILESHIFT);
                           tilehit = pwalltile;
                           HitWall(hw_vert);
                        }
                     }
                  }
               }
               else
               {
                  yintercept = ytile << TILESHIFT;
                  xtile = (short) (xintercept >> TILESHIFT);
                  HitWall(hw_horiz);
               }
            }
            break;
         }
passhoriz:
         *((byte *)spotvis + yspot) = 1;
         ytile += ytilestep;
         xintercept += xstep;
         yspot = (word)((((uint32_t)xintercept >> 16) << mapshift) + ytile);
      }
      while(1);
   }
}

/*
====================
=
= WallRefresh
=
====================
*/

static void WallRefresh ()
{
   xpartialdown = viewx & (TILEGLOBAL - 1);
   xpartialup = TILEGLOBAL - xpartialdown;
   ypartialdown = viewy & (TILEGLOBAL - 1);
   ypartialup = TILEGLOBAL - ypartialdown;

   min_wallheight = viewheight;
   lastside = -1;                  // the first pixel is on a new wall
   AsmRefresh ();
   ScalePost ();                   // no more optimization on last post
}

static void CalcViewVariables()
{
   viewangle = player->angle;
   midangle = viewangle * (FINEANGLES / ANGLES);
   viewsin = sintable[viewangle];
   viewcos = costable[viewangle];
   viewx = player->x - FixedMul(focallength, viewcos);
   viewy = player->y + FixedMul(focallength, viewsin);

   focaltx = (short)(viewx >> TILESHIFT);
   focalty = (short)(viewy >> TILESHIFT);

   viewtx = (short)(player->x >> TILESHIFT);
   viewty = (short)(player->y >> TILESHIFT);
}

//==========================================================================

/*
========================
=
= ThreeDRefresh
=
========================
*/

void    ThreeDRefresh ()
{
//
// clear out the traced array
//
   memset(spotvis, 0, maparea);
   spotvis[player->tilex][player->tiley] = 1;
   // Detect all sprites over player fix

   vbuf = I_LockBuffer();
   if(vbuf == NULL) return;

   vbuf += screenofs;
   vbufPitch = vid_bufferPitch;

   CalcViewVariables();

//
// follow the walls from there to the right, drawing as we go
//
   VGAClearScreen ();
#if defined(USE_FEATUREFLAGS) && defined(USE_STARSKY)
   if(GetFeatureFlags() & FF_STARSKY)
      DrawStarSky(vbuf, vbufPitch);
#endif

   WallRefresh ();

#if defined(USE_FEATUREFLAGS) && defined(USE_PARALLAX)
   if(GetFeatureFlags() & FF_PARALLAXSKY)
      DrawParallax(vbuf, vbufPitch);
#endif
#if defined(USE_FEATUREFLAGS) && defined(USE_CLOUDSKY)
   if(GetFeatureFlags() & FF_CLOUDSKY)
      DrawClouds(vbuf, vbufPitch, min_wallheight);
#endif
#ifdef USE_FLOORCEILINGTEX
   DrawFloorAndCeiling(vbuf, vbufPitch, min_wallheight);
#endif

//
// draw all the scaled images
//
   DrawScaleds();                  // draw scaled stuff

#if defined(USE_FEATUREFLAGS) && defined(USE_RAIN)
   if(GetFeatureFlags() & FF_RAIN)
      DrawRain(vbuf, vbufPitch);
#endif
#if defined(USE_FEATUREFLAGS) && defined(USE_SNOW)
   if(GetFeatureFlags() & FF_SNOW)
      DrawSnow(vbuf, vbufPitch);
#endif

   DrawPlayerWeapon ();    // draw player's hands

   if(myInput.keyboard(sc_Tab) && viewsize == 21 && (int)gamestate.weapon != -1)
      ShowActStatus();

   I_UnlockBuffer();
   vbuf = NULL;

//
// show screen and time last cycle
//

   if (fizzlein)
   {
      FizzleFade(0, 0, cfg_logicalWidth, cfg_logicalHeight, 20, false);
      fizzlein = false;

      lasttimecount = GetTimeCount();          // don't make a big tic count
   }
   else
   {
#ifndef REMDEBUG
      if (fpscounter)
      {
         fontnumber = 0;
         SETFONTCOLOR(7, 127);
         PrintX = 4;
         PrintY = 1;
         VL_Bar(0, 0, 50, 10, bordercol);
         US_PrintSigned(fps);
         US_Print(" fps");
      }
#endif
      // IOANCH: use special call
      I_UpdateScreen();
   }

#ifndef REMDEBUG
   if (fpscounter)
   {
      fps_frames++;
      fps_time += tics;

      if(fps_time > 35)
      {
         fps_time -= 35;
         fps = fps_frames << 1;
         fps_frames = 0;
      }
   }
#endif
}
