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

#include <string.h>
#include "wl_def.h"
#include "wl_draw.h"
#pragma hdrstop
#include "Config.h"
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



#if defined(_arch_dreamcast)

#elif defined(GP2X)


#if defined(GP2X_940)
#else
#endif
#else


#endif

SDL_Surface *vid_screen = NULL;
unsigned vid_screenPitch;

SDL_Surface *vid_screenBuffer = NULL;
unsigned vid_bufferPitch;

unsigned vid_scaleFactor;

Boolean	 screenfaded;
unsigned bordercolor;

SDL_Color palette1[256], palette2[256];
SDL_Color vid_curpal[256];


#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 0}

// IOANCH 20130202: unification process
// SDL_Color gamepal[]={
SDL_Colour vid_palette_sod[]={
// #ifdef SPEAR()
    #include "sodpal.inc"
};
// #else
SDL_Colour vid_palette_wl6[]={
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


void VL_ConvertPalette(byte *srcpal, SDL_Color *destpal, int numColors)
{
    for(int i=0; i<numColors; i++)
    {
        destpal[i].r = *srcpal++ * 255 / 63;
        destpal[i].g = *srcpal++ * 255 / 63;
        destpal[i].b = *srcpal++ * 255 / 63;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_FillPalette
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_FillPalette (int red, int green, int blue)
{
    int i;
    SDL_Color pal[256];

    for(i=0; i<256; i++)
    {
        pal[i].r = red;
        pal[i].g = green;
        pal[i].b = blue;
    }

    VL_SetPalette(pal, true);
}
// IOANCH: abstracted away

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_SetPalette
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_SetPalette (SDL_Color *palette, bool forceupdate)
{
    memcpy(vid_curpal, palette, sizeof(SDL_Color) * 256);

    if(cfg_screenBits == 8)
        SDL_SetPalette(vid_screen, SDL_PHYSPAL, palette, 0, 256);
    else
    {
        SDL_SetPalette(vid_screenBuffer, SDL_LOGPAL, palette, 0, 256);
        if(forceupdate)
           I_UpdateScreen();
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


void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
	int		    i,j,orig,delta;
	SDL_Color   *origptr, *newptr;

    red = red * 255 / 63;
    green = green * 255 / 63;
    blue = blue * 255 / 63;

	VL_WaitVBL(1);
	VL_GetPalette(palette1);
	memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
		origptr = &palette1[start];
		newptr = &palette2[start];
		for (j=start;j<=end;j++)
		{
			orig = origptr->r;
			delta = red-orig;
			newptr->r = orig + delta * i / steps;
			orig = origptr->g;
			delta = green-orig;
			newptr->g = orig + delta * i / steps;
			orig = origptr->b;
			delta = blue-orig;
			newptr->b = orig + delta * i / steps;
			origptr++;
			newptr++;
		}

		if(!cfg_usedoublebuffering || cfg_screenBits == 8) VL_WaitVBL(1);
		VL_SetPalette (palette2, true);
	}

//
// final color
//
	VL_FillPalette (red,green,blue);

	screenfaded = true;
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
	int i,j,delta;

	VL_WaitVBL(1);
	VL_GetPalette(palette1);
	memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
		for (j=start;j<=end;j++)
		{
			delta = palette[j].r-palette1[j].r;
			palette2[j].r = palette1[j].r + delta * i / steps;
			delta = palette[j].g-palette1[j].g;
			palette2[j].g = palette1[j].g + delta * i / steps;
			delta = palette[j].b-palette1[j].b;
			palette2[j].b = palette1[j].b + delta * i / steps;
		}

		if(!cfg_usedoublebuffering || cfg_screenBits == 8) VL_WaitVBL(1);
		VL_SetPalette(palette2, true);
	}

//
// final color
//
	VL_SetPalette (palette, true);
	screenfaded = false;
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

    assert(x >= 0 && (unsigned) x < cfg_screenWidth
            && y >= 0 && (unsigned) y < cfg_screenHeight
            && "VL_Plot: Pixel out of bounds!");

    ptr = I_LockSurface(vid_screenBuffer);
    if(ptr == NULL) return;

    ptr[y * vid_bufferPitch + x] = color;

    I_UnlockSurface(vid_screenBuffer);
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

    assert_ret(x >= 0 && (unsigned) x < cfg_screenWidth
            && y >= 0 && (unsigned) y < cfg_screenHeight
            && "VL_GetPixel: Pixel out of bounds!");

    ptr = I_LockSurface(vid_screenBuffer);
    if(ptr == NULL) return 0;

    col = ((byte *) vid_screenBuffer->pixels)[y * vid_bufferPitch + x];

    I_UnlockSurface(vid_screenBuffer);

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

    assert(x >= 0 && x + width <= cfg_screenWidth
            && y >= 0 && y < cfg_screenHeight
            && "VL_Hlin: Destination rectangle out of bounds!");

    ptr = I_LockSurface(vid_screenBuffer);
    if(ptr == NULL) return;

    memset(ptr + y * vid_bufferPitch + x, color, width);

    I_UnlockSurface(vid_screenBuffer);
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

	assert(x >= 0 && (unsigned) x < cfg_screenWidth
			&& y >= 0 && (unsigned) y + height <= cfg_screenHeight
			&& "VL_Vlin: Destination rectangle out of bounds!");

	ptr = I_LockSurface(vid_screenBuffer);
	if(ptr == NULL) return;

	ptr += y * vid_bufferPitch + x;

	while (height--)
	{
		*ptr = color;
		ptr += vid_bufferPitch;
	}

	I_UnlockSurface(vid_screenBuffer);
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

	assert(scx >= 0 && (unsigned) scx + scwidth <= cfg_screenWidth
			&& scy >= 0 && (unsigned) scy + scheight <= cfg_screenHeight
			&& "VL_BarScaledCoord: Destination rectangle out of bounds!");

	ptr = I_LockSurface(vid_screenBuffer);
	if(ptr == NULL) return;

	ptr += scy * vid_bufferPitch + scx;

	while (scheight--)
	{
		memset(ptr, color, scwidth);
		ptr += vid_bufferPitch;
	}
	I_UnlockSurface(vid_screenBuffer);
}

////////////////////////////////////////////////////////////////////////////////
//
//                            MEMORY OPS
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_MemToLatch
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_MemToLatch(byte *source, int width, int height, SDL_Surface *destSurface,
                   int x, int y)
{
    byte *ptr;
    int xsrc, ysrc, pitch;

    assert(x >= 0 && (unsigned) x + width <= cfg_screenWidth
            && y >= 0 && (unsigned) y + height <= cfg_screenHeight
            && "VL_MemToLatch: Destination rectangle out of bounds!");

    ptr = I_LockSurface(destSurface);
    if(ptr == NULL) return;

    pitch = destSurface->pitch;
    ptr += y * pitch + x;
    for(ysrc = 0; ysrc < height; ysrc++)
    {
        for(xsrc = 0; xsrc < width; xsrc++)
        {
            ptr[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2))
                + (xsrc & 3) * (width >> 2) * height];
        }
    }
    I_UnlockSurface(destSurface);
}

//===========================================================================


////////////////////////////////////////////////////////////////////////////////
//
// VL_MemToScreenScaledCoord
//
// Draws a block of data to the screen with scaling according to vid_scaleFactor.
//
////////////////////////////////////////////////////////////////////////////////


void VL_MemToScreenScaledCoord (byte *source, int width, int height,
                                int destx, int desty)
{
    byte *ptr;
    int i, j, sci, scj;
    unsigned m, n;

    assert(destx >= 0 && destx + width * vid_scaleFactor <= cfg_screenWidth
            && desty >= 0 && desty + height * vid_scaleFactor <= cfg_screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    ptr = I_LockSurface(vid_screenBuffer);
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
    I_UnlockSurface(vid_screenBuffer);
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


void VL_MemToScreenScaledCoord (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    byte *ptr;
    int i, j, sci, scj;
    unsigned m, n;

    assert(destx >= 0 && destx + width * vid_scaleFactor <= cfg_screenWidth
            && desty >= 0 && desty + height * vid_scaleFactor <= cfg_screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    ptr = I_LockSurface(vid_screenBuffer);
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
    I_UnlockSurface(vid_screenBuffer);
}

//==========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_LatchToScreen
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_LatchToScreenScaledCoord(SDL_Surface *source, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
	assert(scxdest >= 0 && scxdest + width * vid_scaleFactor <= cfg_screenWidth
			&& scydest >= 0 && scydest + height * vid_scaleFactor <= cfg_screenHeight
			&& "VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");

	if(vid_scaleFactor == 1)
    {
        // HACK: If screenBits is not 8 and the screen is faded out, the
        //       result will be black when using SDL_BlitSurface. The reason
        //       is that the logical palette needed for the transformation
        //       to the screen color depth is not equal to the logical
        //       palette of the latch (the latch is not faded). Therefore,
        //       SDL tries to map the colors...
        //       The result: All colors are mapped to black.
        //       So, we do the blit on our own...
        if(cfg_screenBits != 8)
        {
            byte *src, *dest;
            unsigned srcPitch;
            int i, j;

            src = I_LockSurface(source);
            if(src == NULL) return;

            srcPitch = source->pitch;

            dest = I_LockSurface(vid_screenBuffer);
            if(dest == NULL) return;

            for(j = 0; j < height; j++)
            {
                for(i = 0; i < width; i++)
                {
                    byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                    dest[(scydest + j) * vid_bufferPitch + scxdest + i] = col;
                }
            }
            I_UnlockSurface(vid_screenBuffer);
            I_UnlockSurface(source);
        }
        else
        {
            SDL_Rect srcrect = { static_cast<Sint16>(xsrc), static_cast<Sint16>(ysrc), static_cast<Uint16>(width), static_cast<Uint16>(height) };
            SDL_Rect destrect = { static_cast<Sint16>(scxdest), static_cast<Sint16>(scydest), 0, 0 }; // width and height are ignored
            SDL_BlitSurface(source, &srcrect, vid_screenBuffer, &destrect);
        }
    }
    else
    {
        byte *src, *dest;
        unsigned srcPitch;
        int i, j, sci, scj;
        unsigned m, n;

        src = I_LockSurface(source);
        if(src == NULL) return;

        srcPitch = source->pitch;

        dest = I_LockSurface(vid_screenBuffer);
        if(dest == NULL) return;

        for(j = 0, scj = 0; j < height; j++, scj += vid_scaleFactor)
        {
            for(i = 0, sci = 0; i < width; i++, sci += vid_scaleFactor)
            {
                byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                for(m = 0; m < vid_scaleFactor; m++)
                {
                    for(n = 0; n < vid_scaleFactor; n++)
                    {
                        dest[(scydest + scj + m) * vid_bufferPitch + scxdest + sci + n] = col;
                    }
                }
            }
        }
        I_UnlockSurface(vid_screenBuffer);
        I_UnlockSurface(source);
    }
}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = VL_ScreenToScreen
// =
//
////////////////////////////////////////////////////////////////////////////////


void VL_ScreenToScreen (SDL_Surface *source, SDL_Surface *dest)
{
    SDL_BlitSurface(source, NULL, dest, NULL);
}
