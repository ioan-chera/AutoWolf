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

// ID_VL.H

// wolf compatability

//===========================================================================

#define CHARWIDTH		2
#define TILEWIDTH		4

//===========================================================================

extern  unsigned vid_scaleFactor;

extern	bool  vid_screenfaded;

// IOANCH 20130202: unification process
extern SDL_Color vid_palette_sod[256];
extern SDL_Color vid_palette_wl6[256];

//===========================================================================

//
// VGA hardware routines
//

#define VL_WaitVBL(a) I_Delay((a)*8)

void VL_SetVGAPlaneMode ();
void VL_SetTextMode ();
void VL_Shutdown ();

void VL_ConvertPalette(const byte *srcpal, SDL_Color *destpal, int numColors);
void VL_FillPalette (int red, int green, int blue);
void VL_GetPalette  (SDL_Color *palette);
void VL_FadeOut     (int start, int end, int red, int green, int blue, int steps);
void VL_FadeIn      (int start, int end, SDL_Color *palette, int steps);

byte VL_GetPixel        (int x, int y);
void VL_Plot            (int x, int y, int color);
void VL_Hlin            (unsigned x, unsigned y, unsigned width, int color);
void VL_Vlin            (int x, int y, int height, int color);
void VL_BarScaledCoord  (int scx, int scy, int scwidth, int scheight, int color);
void inline VL_Bar      (int x, int y, int width, int height, int color)
{
    VL_BarScaledCoord(vid_scaleFactor*x, vid_scaleFactor*y,
        vid_scaleFactor*width, vid_scaleFactor*height, color);
}

void VL_MungePic                (byte *source, unsigned width, unsigned height);
void VL_DrawPicBare             (int x, int y, byte *pic, int width, int height);
void VL_MemToScreenScaledCoord  (const byte *source, int width, int height, int scx, int scy);
void VL_MemToScreenScaledCoord  (const byte *source, int origwidth, int origheight, int srcx, int srcy,
                                    int destx, int desty, int width, int height);

void inline VL_MemToScreen (const byte *source, int width, int height, int x, int y)
{
    VL_MemToScreenScaledCoord(source, width, height,
        vid_scaleFactor*x, vid_scaleFactor*y);
}

void VL_MaskedToScreen (byte *source, int width, int height, int x, int y);

// IOANCH: from setvgaplanemode
void VL_SetViewportValues();