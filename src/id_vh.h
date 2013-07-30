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

// ID_VH.H

#define WHITE			15			// graphics mode independant colors
#define BLACK			0
#define FIRSTCOLOR		1
#define SECONDCOLOR		12
#define F_WHITE			15
#define F_BLACK			0
#define F_FIRSTCOLOR	1
#define F_SECONDCOLOR	12

//===========================================================================

#define MAXSHIFTS	1

#pragma pack(push, 1)
struct pictabletype
{
	int16_t width,height;
};


struct fontstruct
{
	int16_t height;
	int16_t location[256];
	int8_t width[256];
} ;
#pragma pack(pop)

//===========================================================================


extern	pictabletype	*pictable;
extern	pictabletype	*picmtable;

extern  byte            fontcolor,backcolor;
extern	int             fontnumber;
extern	int             px,py;

#define SETFONTCOLOR(f,b) fontcolor=f;backcolor=b;

//
// mode independant routines
// coordinates in pixels, rounded to best screen res
// regions marked in double buffer
//

void VWB_DrawPropString	 (const char *string);

void VWB_DrawTile8 (int x, int y, int tile);
void VWB_DrawTile8M (int x, int y, int tile);
void VWB_DrawPic (int x, int y, int chunknum);
void VWB_DrawPicScaledCoord (int x, int y, int chunknum);
void VL_Bar (int x, int y, int width, int height, int color);
// IOANCH: removed redundant defs
void VWB_Plot (int x, int y, int color);
void VWB_Hlin (int x1, int x2, int y, int color);
void VWB_Vlin (int y1, int y2, int x, int color);

// void I_UpdateScreen();
// IOANCH 20130717: deleted I_UpdateScreen

//
// wolfenstein EGA compatability stuff
//


// IOANCH: removed redundant entries
#define VW_Hlin(x,z,y,c)	VL_Hlin(x,y,(z)-(x)+1,c)
#define VW_Vlin(y,z,x,c)	VL_Vlin(x,y,(z)-(y)+1,c)
// IOANCH 20130202: unification process
#define VW_FadeIn()		    VL_FadeIn(0,255,IMPALE(vid_palette),30);
#define VW_FadeOut()	    VL_FadeOut(0,255,0,0,0,30)
void	VW_MeasurePropString (const char *string, word *width, word *height);

#define LatchDrawChar(x,y,p) I_LatchToScreen(0,((p)&7)*8,((p)>>3)*8*64,8,8,x,y)
#define LatchDrawTile(x,y,p) I_LatchToScreen(1,(p)*64,0,16,16,x,y)

void    LatchDrawPic (unsigned x, unsigned y, unsigned picnum);
void    LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum);

void    VH_Startup();
Boolean FizzleFade (int x1, int y1,
    unsigned width, unsigned height, unsigned frames, Boolean abortable);


// IOANCH 20130726: made extern
extern SDL_Color vid_curpal[256];
