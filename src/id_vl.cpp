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

// ID_VL.C

#include "wl_def.h"
#include "wl_draw.h"
#include "Config.h"
#include "i_system.h"
#include "i_video.h"
// Uncomment the following line, if you get destination out of bounds
// assertion errors and want to ignore them during debugging
//#define IGNORE_BAD_DEST

#ifdef IGNORE_BAD_DEST
#undef assert
#define assert(x) if(!(x)) return
#define assert_ret(x) if(!(x)) return 0
#else
#define assert_ret(x) assert(x)
#endif

unsigned vid_scaleFactor;
bool	 vid_screenfaded;

SDL_Color palette1[256], palette2[256];
SDL_Color vid_curpal[256];


#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define PAL_RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 255}

// IOANCH 20130202: unification process
// SDL_Color gamepal[]={
SDL_Color vid_palette_sod[]={
// #ifdef SPEAR::flag
    #include "sodpal.inc"
};
// #else
SDL_Color vid_palette_wl6[]={
    #include "wolfpal.inc"
// #endif
};

// IOANCH 20130202: unification process
CASSERT(lengthof(vid_palette_sod) == 256 && lengthof(vid_palette_wl6) == 256)

//===========================================================================


////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_Shutdown
// =
//
////////////////////////////////////////////////////////////////////////////////


void	VL_Shutdown ()
{
	//VL_SetTextMode ();
}


// IOANCH 20130726: Moved SetVGAPlaneMode to i_system

////////////////////////////////////////////////////////////////////////////////
//
//                        PALETTE OPS
//
//        To avoid snow, do a WaitVBL BEFORE calling these
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_ConvertPalette
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_ConvertPalette(const byte *srcpal, SDL_Color *destpal, int numColors)
{
    for(int i=0; i<numColors; i++)
    {
        destpal[i].r = *srcpal++ * 255 / 63;
        destpal[i].g = *srcpal++ * 255 / 63;
        destpal[i].b = *srcpal++ * 255 / 63;
    }
}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_GetPalette
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_GetPalette (SDL_Color *palette)
{
    memcpy(palette, vid_curpal, sizeof(SDL_Color) * 256);
}


//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_FadeOut
// =
// = Fades the current palette to the given color in the given number of steps
// =
//
////////////////////////////////////////////////////////////////////////////////

SDL_Color g_lastColor;

void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
	int		    i;

    g_lastColor.r = red = red * 255 / 63;
    g_lastColor.g = green = green * 255 / 63;
    g_lastColor.b = blue = blue * 255 / 63;
    g_lastColor.a = 0;

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
        I_RedrawFrame();
        I_FillColour(red, green, blue, 255 / steps * i);
        I_RenderPresent();
//        SDL_Delay(100);
	}

//
// final color
//
    I_FillColour(red, green, blue, 255);
    I_RenderPresent();

	vid_screenfaded = true;
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_FadeIn
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_FadeIn (int start, int end, SDL_Color *palette, int steps)
{
	int i;

	VL_WaitVBL(1);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
        I_RedrawFrame();
        I_FillColour(g_lastColor.r, g_lastColor.g, g_lastColor.b,
                     255 - 255 * i / steps);
        I_RenderPresent();
//        SDL_Delay(100);
	}

    I_RedrawFrame();
    I_RenderPresent();

//
// final color
//
	vid_screenfaded = false;
}
// IOANCH 20130726: moved to i_system

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_Plot
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_Plot (int x, int y, int color)
{
    byte *ptr;

    assert(x >= 0 && (unsigned) x < cfg_logicalWidth
            && y >= 0 && (unsigned) y < cfg_logicalHeight
            && "VL_Plot: Pixel out of bounds!");

    ptr = I_LockBuffer();
    if(ptr == NULL) return;

    ptr[y * vid_bufferPitch + x] = color;

    I_UnlockBuffer();
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_GetPixel
// =
//
////////////////////////////////////////////////////////////////////////////////


byte VL_GetPixel (int x, int y)
{
    byte *ptr;
    byte col;

    assert_ret(x >= 0 && (unsigned) x < cfg_logicalWidth
            && y >= 0 && (unsigned) y < cfg_logicalHeight
            && "VL_GetPixel: Pixel out of bounds!");

    ptr = I_LockBuffer();
    if(ptr == NULL) return 0;

    col = ptr[y * vid_bufferPitch + x];   // IOANCH: just use ptr

    I_UnlockBuffer();

    return col;
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_Hlin
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    byte *ptr;

    assert(x >= 0 && x + width <= cfg_logicalWidth
            && y >= 0 && y < cfg_logicalHeight
            && "VL_Hlin: Destination rectangle out of bounds!");

    ptr = I_LockBuffer();
    if(ptr == NULL) return;

    memset(ptr + y * vid_bufferPitch + x, color, width);

    I_UnlockBuffer();
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_Vlin
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_Vlin (int x, int y, int height, int color)
{
	byte *ptr;

	assert(x >= 0 && (unsigned) x < cfg_logicalWidth
			&& y >= 0 && (unsigned) y + height <= cfg_logicalHeight
			&& "VL_Vlin: Destination rectangle out of bounds!");

	ptr = I_LockBuffer();
	if(ptr == NULL) return;

	ptr += y * vid_bufferPitch + x;

	while (height--)
	{
		*ptr = color;
		ptr += vid_bufferPitch;
	}

	I_UnlockBuffer();
}


////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_Bar
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
	byte *ptr;

	assert(scx >= 0 && (unsigned) scx + scwidth <= cfg_logicalWidth
			&& scy >= 0 && (unsigned) scy + scheight <= cfg_logicalHeight
			&& "VL_BarScaledCoord: Destination rectangle out of bounds!");

	ptr = I_LockBuffer();
	if(ptr == NULL) return;

	ptr += scy * vid_bufferPitch + scx;

	while (scheight--)
	{
		memset(ptr, color, scwidth);
		ptr += vid_bufferPitch;
	}
	I_UnlockBuffer();
}

////////////////////////////////////////////////////////////////////////////////
//
//                            MEMORY OPS
//
////////////////////////////////////////////////////////////////////////////////



//===========================================================================


////////////////////////////////////////////////////////////////////////////////
//
// VL_MemToScreenScaledCoord
//
// Draws a block of data to the screen with scaling according to vid_scaleFactor.
//
////////////////////////////////////////////////////////////////////////////////


void VL_MemToScreenScaledCoord (const byte *source, int width, int height,
                                int destx, int desty)
{
    byte *ptr;
    int i, j, sci, scj;
    unsigned m, n;

    assert(destx >= 0 && destx + width * vid_scaleFactor <= cfg_logicalWidth
            && desty >= 0 && desty + height * vid_scaleFactor <= cfg_logicalHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    ptr = I_LockBuffer();
    if(ptr == NULL)
       return;

    for(j = 0, scj = 0; j < height; j++, scj += vid_scaleFactor)
    {
        for(i = 0, sci = 0; i < width; i++, sci += vid_scaleFactor)
        {
            byte col = source[(j * (width >> 2) + (i >> 2)) + (i & 3) *
                              (width >> 2) * height];
            for(m = 0; m < vid_scaleFactor; m++)
            {
                for(n = 0; n < vid_scaleFactor; n++)
                {
                    ptr[(scj + m + desty) * vid_bufferPitch + sci + n + destx] = col;
                }
            }
        }
    }
    I_UnlockBuffer();
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_MemToScreenScaledCoord
// =
// = Draws a part of a block of data to the screen.
// = The block has the size origwidth*origheight.
// = The part at (srcx, srcy) has the size width*height
// = and will be painted to (destx, desty) with scaling according to 
// vid_scaleFactor.
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_MemToScreenScaledCoord (const byte *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    byte *ptr;
    int i, j, sci, scj;
    unsigned m, n;

    assert(destx >= 0 && destx + width * vid_scaleFactor <= cfg_logicalWidth
            && desty >= 0 && desty + height * vid_scaleFactor <= cfg_logicalHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    ptr = I_LockBuffer();
    if(ptr == NULL) return;

    for(j = 0, scj = 0; j < height; j++, scj += vid_scaleFactor)
    {
        for(i = 0, sci = 0; i < width; i++, sci += vid_scaleFactor)
        {
            byte col = source[((j + srcy) * (origwidth >> 2) + ((i + srcx) >>2 ))
                + ((i + srcx) & 3) * (origwidth >> 2) * origheight];

            for(m = 0; m < vid_scaleFactor; m++)
            {
                for(n = 0; n < vid_scaleFactor; n++)
                {
                    ptr[(scj + m + desty) * vid_bufferPitch + sci + n + destx] = col;
                }
            }
        }
    }
    I_UnlockBuffer();
}

//
// VL_SetViewportValues
//
// IOANCH: moved from I_ here, because not low-level enough
//
void VL_SetViewportValues()
{
   vid_scaleFactor = cfg_logicalWidth/LOGIC_WIDTH;
   if(cfg_logicalHeight/LOGIC_HEIGHT < vid_scaleFactor)
      vid_scaleFactor = cfg_logicalHeight/LOGIC_HEIGHT;
   
   vid_pixelangle = (short *) I_CheckedMalloc(cfg_logicalWidth * sizeof(short));

   vid_wallheight = (int *) I_CheckedMalloc(cfg_logicalWidth * sizeof(int));

}
