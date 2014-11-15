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
// Video operations. Library-specific ops isolated here
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __Wolf4SDL__i_video__
#define __Wolf4SDL__i_video__

#include "id_vl.h"

extern  unsigned vid_bufferPitch;

#ifdef USE_SDL1_2
//extern SDL_Surface* vid_screen;
#else
extern SDL_Window* vid_window;
extern SDL_Renderer* vid_renderer;
extern int vid_screenWidth, vid_screenHeight;
#endif

void I_InitEngine();
void I_InitAfterSignon();
byte *I_LockBuffer();
SDL_Color *I_LockDirect();
void I_UnlockBuffer();
void I_UnlockDirect();
void I_UpdateScreen();
void I_UpdateDirect();

void I_ClearScreen(int color);
void I_SetColor    (int color, int red, int green, int blue);
void I_GetColor    (int color, int *red, int *green, int *blue);
void I_SetPalette  (SDL_Color *palette, bool forceupdate);

void I_LatchToScreenScaledCoord (int surf_index, int xsrc, int ysrc,
                                 int width, int height, int scxdest, int scydest);

static void inline I_LatchToScreen (int surf_index, int xsrc, int ysrc,
                                    int width, int height, int xdest, int ydest)
{
   I_LatchToScreenScaledCoord(surf_index,xsrc,ysrc,width,height,
                               vid_scaleFactor*xdest,vid_scaleFactor*ydest);
}
void I_LatchToScreen (int surf_index, int x, int y);

void I_LatchToScreenScaledCoord (int surf_index, int scx, int scy);

void    I_FreeLatchMem();
void    I_LoadLatchMem ();
void I_MemToLatch              (const byte *source, int width, int height,
                                SDL_Surface *destSurface, int x, int y);

void I_SaveBufferBMP(const char *fname);

//void VL_ScreenToScreen          (SDL_Surface *source, SDL_Surface *dest);


#endif /* defined(__Wolf4SDL__i_video__) */
