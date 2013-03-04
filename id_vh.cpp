#include "wl_def.h"


pictabletype	*pictable;
SDL_Surface     *latchpics[NUMLATCHPICS];

int	    px,py;
byte	fontcolor,backcolor;
int	    fontnumber;

//==========================================================================

//
// VWB_DrawPropString
//
void VWB_DrawPropString(const char* string)
{
	fontstruct  *font;
	int		    width, step, height;
	byte	    *source, *dest;
	byte	    ch;
	int i;
	unsigned sx, sy;

	byte *vbuf = VL_LockSurface(curSurface);
	if(vbuf == NULL) return;

	font = (fontstruct *) grsegs[gfxvmap[STARTFONT][SPEAR]+fontnumber];
	height = font->height;
	dest = vbuf + scaleFactor * (py * curPitch + px);

	while ((ch = (byte)*string++)!=0)
	{
		width = step = font->width[ch];
		source = ((byte *)font)+font->location[ch];
		while (width--)
		{
			for(i=0; i<height; i++)
			{
				if(source[i*step])
				{
					for(sy=0; sy<scaleFactor; sy++)
						for(sx=0; sx<scaleFactor; sx++)
							dest[(scaleFactor*i+sy)*curPitch+sx]=fontcolor;
				}
			}

			source++;
			px++;
			dest+=scaleFactor;
		}
	}

	VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_MungePic
=
=================
*/
void VL_MungePic (byte *source, unsigned width, unsigned height)
{
	unsigned x,y,plane,size,pwidth;
	byte *temp, *dest, *srcline;

	size = width*height;

	if (width&3)
		Quit ("VL_MungePic: Not divisable by 4!");

//
// copy the pic to a temp buffer
//
	temp=(byte *) malloc(size);
    CHECKMALLOCRESULT(temp);
	memcpy (temp,source,size);

//
// munge it back into the original buffer
//
	dest = source;
	pwidth = width/4;

	for (plane=0;plane<4;plane++)
	{
		srcline = temp;
		for (y=0;y<height;y++)
		{
			for (x=0;x<pwidth;x++)
				*dest++ = *(srcline+x*4+plane);
			srcline+=width;
		}
	}

	free(temp);
}

//
// VWL_MeasureString
//
void VWL_MeasureString (const char *string, word *width, word *height, fontstruct *font)
{
	*height = font->height;
	for (*width = 0;*string;string++)
		*width += font->width[*((byte *)string)];	// proportional width
}

//
// VW_MeasurePropString
//
void VW_MeasurePropString (const char *string, word *width, word *height)
{
	VWL_MeasureString(string,width,height,(fontstruct *)grsegs[gfxvmap[STARTFONT][SPEAR]+fontnumber]);
}

/*
=============================================================================

				Double buffer management routines

=============================================================================
*/

//
// VH_UpdateScreen
//
void VH_UpdateScreen()
{
	SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
	SDL_Flip(screen);
}

//
// VWB_DrawTile8
//
void VWB_DrawTile8 (int x, int y, int tile)
{
	LatchDrawChar(x,y,tile);
}

//
// VWB_DrawTile8M
//
void VWB_DrawTile8M (int x, int y, int tile)
{
	VL_MemToScreen (((byte *)grsegs[gfxvmap[STARTTILE8M][SPEAR]])+tile*64,8,8,x,y);
}

//
// VWB_DrawPic
//
void VWB_DrawPic (int x, int y, int chunknum)
{
	int	picnum = chunknum - gfxvmap[STARTPICS][SPEAR];
	unsigned width,height;

	x &= ~7;

	width = pictable[picnum].width;
	height = pictable[picnum].height;

	VL_MemToScreen (grsegs[chunknum],width,height,x,y);
}

//
// VWB_DrawPicScaledCoord
//
void VWB_DrawPicScaledCoord (int scx, int scy, int chunknum)
{
	int	picnum = chunknum - gfxvmap[STARTPICS][SPEAR];
	unsigned width,height;

	width = pictable[picnum].width;
	height = pictable[picnum].height;

    VL_MemToScreenScaledCoord (grsegs[chunknum],width,height,scx,scy);
}

//
// VWB_Bar
//
void VWB_Bar (int x, int y, int width, int height, int color)
{
	VW_Bar (x,y,width,height,color);
}

//
// VWB_Plot
//
void VWB_Plot (int x, int y, int color)
{
    if(scaleFactor == 1)
        VW_Plot(x,y,color);
    else
        VW_Bar(x, y, 1, 1, color);
}

//
// VWB_Hlin
//
void VWB_Hlin (int x1, int x2, int y, int color)
{
    if(scaleFactor == 1)
    	VW_Hlin(x1,x2,y,color);
    else
        VW_Bar(x1, y, x2-x1+1, 1, color);
}

//
// VWB_Vlin
//
void VWB_Vlin (int y1, int y2, int x, int color)
{
    if(scaleFactor == 1)
		VW_Vlin(y1,y2,x,color);
    else
        VW_Bar(x, y1, 1, y2-y1+1, color);
}


/*
=============================================================================

						WOLFENSTEIN STUFF

=============================================================================
*/

/*
=====================
=
= LatchDrawPic
=
=====================
*/

void LatchDrawPic (unsigned x, unsigned y, unsigned picnum)
{
	VL_LatchToScreen (latchpics[2+picnum-gfxvmap[LATCHPICS_LUMP_START][SPEAR]], x*8, y);
}

//
// LatchDrawPicScaledCoord
//
void LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum)
{
	VL_LatchToScreenScaledCoord (latchpics[2+picnum-gfxvmap[LATCHPICS_LUMP_START][SPEAR]], scx*8, scy);
}


//==========================================================================

//
// FreeLatchMem
// 
void FreeLatchMem()
{
    int i;
    for(i = 0; i < (2 + (signed int)gfxvmap[LATCHPICS_LUMP_END][SPEAR] - (signed int)gfxvmap[LATCHPICS_LUMP_START][SPEAR]); i++)
    {
        SDL_FreeSurface(latchpics[i]);
        latchpics[i] = NULL;
    }
}

/*
===================
=
= LoadLatchMem
=
===================
*/

void LoadLatchMem (void)
{
	int	i,width,height,start,end;
	byte *src;
	SDL_Surface *surf;

//
// tile 8s
//
    surf = SDL_CreateRGBSurface(SDL_HWSURFACE, 8*8,
        ((gfxvmap[NUMTILE8][SPEAR] + 7) / 8) * 8, 8, 0, 0, 0, 0);
    if(surf == NULL)
    {
        Quit("Unable to create surface for tiles!");
    }
    // IOANCH 20130202: unification process
    SDL_SetColors(surf, IMPALE(palette), 0, 256);

	latchpics[0] = surf;
	CA_CacheGrChunk (gfxvmap[STARTTILE8][SPEAR]);
	src = grsegs[gfxvmap[STARTTILE8][SPEAR]];

	for (i=0;i<(signed int)gfxvmap[NUMTILE8][SPEAR];i++)
	{
		VL_MemToLatch (src, 8, 8, surf, (i & 7) * 8, (i >> 3) * 8);
		src += 64;
	}
	UNCACHEGRCHUNK (gfxvmap[STARTTILE8][SPEAR]);

	latchpics[1] = NULL;  // not used

//
// pics
//
	start = gfxvmap[LATCHPICS_LUMP_START][SPEAR];
	end = gfxvmap[LATCHPICS_LUMP_END][SPEAR];

	for (i=start;i<=end;i++)
	{
		width = pictable[i-gfxvmap[STARTPICS][SPEAR]].width;
		height = pictable[i-gfxvmap[STARTPICS][SPEAR]].height;
		surf = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 8, 0, 0, 0, 0);
        if(surf == NULL)
        {
            Quit("Unable to create surface for picture!");
        }
        // IOANCH 20130202: unification process
        SDL_SetColors(surf, IMPALE(palette), 0, 256);

		latchpics[2+i-start] = surf;
		CA_CacheGrChunk (i);
		VL_MemToLatch (grsegs[i], width, height, surf, 0, 0);
		UNCACHEGRCHUNK(i);
	}
}

//==========================================================================

/*
===================
=
= FizzleFade
=
= returns true if aborted
=
= It uses maximum-length Linear Feedback Shift Registers (LFSR) counters.
= You can find a list of them with lengths from 3 to 168 at:
= http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
= Many thanks to Xilinx for this list!!!
=
===================
*/

// XOR masks for the pseudo-random number sequence starting with n=17 bits
static const uint32_t rndmasks[] = {
                    // n    XNOR from (starting at 1, not 0 as usual)
    0x00012000,     // 17   17,14
    0x00020400,     // 18   18,11
    0x00040023,     // 19   19,6,2,1
    0x00090000,     // 20   20,17
    0x00140000,     // 21   21,19
    0x00300000,     // 22   22,21
    0x00420000,     // 23   23,18
    0x00e10000,     // 24   24,23,22,17
    0x01200000,     // 25   25,22      (this is enough for 8191x4095)
};

static unsigned int rndbits_y;
static unsigned int rndmask;

extern SDL_Color curpal[256];

//
// log2_ceil
//
// Returns the number of bits needed to represent the given value
//
static int log2_ceil(uint32_t x)
{
    int n = 0;
    uint32_t v = 1;
    while(v < x)
    {
        n++;
        v <<= 1;
    }
    return n;
}

//
// VH_Startup
//
void VH_Startup()
{
    int rndbits_x = log2_ceil(screenWidth);
    rndbits_y = log2_ceil(screenHeight);

    int rndbits = rndbits_x + rndbits_y;
    if(rndbits < 17)
        rndbits = 17;       // no problem, just a bit slower
    else if(rndbits > 25)
        rndbits = 25;       // fizzle fade will not fill whole screen

    rndmask = rndmasks[rndbits - 17];
}

//
// FizzleFade
//
boolean FizzleFade (SDL_Surface *source, int x1, int y1, unsigned width, unsigned height, unsigned frames, boolean abortable)
{
    unsigned x, y, frame, pixperframe;
    int32_t  rndval, lastrndval;
    int      first = 1;

    lastrndval = 0;
    pixperframe = width * height / frames;

    IN_StartAck ();

    frame = GetTimeCount();
    byte *srcptr = VL_LockSurface(source);
    if(srcptr == NULL) return false;

    do
    {
        IN_ProcessEvents();

        if(abortable && IN_CheckAck ())
        {
            VL_UnlockSurface(source);
            SDL_BlitSurface(source, NULL, screen, NULL);
            SDL_Flip(screen);
            return true;
        }

        byte *destptr = VL_LockSurface(screen);

        if(destptr != NULL)
        {
            rndval = lastrndval;

            // When using double buffering, we have to copy the pixels of the last AND the current frame.
            // Only for the first frame, there is no "last frame"
            for(int i = first; i < 2; i++)
            {
                for(unsigned p = 0; p < pixperframe; p++)
                {
                    //
                    // seperate random value into x/y pair
                    //

                    x = rndval >> rndbits_y;
                    y = rndval & ((1 << rndbits_y) - 1);

                    //
                    // advance to next random element
                    //

                    rndval = (rndval >> 1) ^ (rndval & 1 ? 0 : rndmask);

                    if(x >= width || y >= height)
                    {
                        if(rndval == 0)     // entire sequence has been completed
                            goto finished;
                        p--;
                        continue;
                    }

                    //
                    // copy one pixel
                    //

                    if(screenBits == 8)
                    {
                        *(destptr + (y1 + y) * screen->pitch + x1 + x)
                            = *(srcptr + (y1 + y) * source->pitch + x1 + x);
                    }
                    else
                    {
                        byte col = *(srcptr + (y1 + y) * source->pitch + x1 + x);
                        uint32_t fullcol = SDL_MapRGB(screen->format, curpal[col].r, curpal[col].g, curpal[col].b);
                        memcpy(destptr + (y1 + y) * screen->pitch + (x1 + x) * screen->format->BytesPerPixel,
                            &fullcol, screen->format->BytesPerPixel);
                    }

                    if(rndval == 0)		// entire sequence has been completed
                        goto finished;
                }

                if(!i || first) lastrndval = rndval;
            }

            // If there is no double buffering, we always use the "first frame" case
            if(usedoublebuffering) first = 0;

            VL_UnlockSurface(screen);
            SDL_Flip(screen);
        }
        else
        {
            // No surface, so only enhance rndval
            for(int i = first; i < 2; i++)
            {
                for(unsigned p = 0; p < pixperframe; p++)
                {
                    rndval = (rndval >> 1) ^ (rndval & 1 ? 0 : rndmask);
                    if(rndval == 0)
                        goto finished;
                }
            }
        }

        frame++;
        Delay(frame - GetTimeCount());        // don't go too fast
    } while (1);

finished:
    VL_UnlockSurface(source);
    VL_UnlockSurface(screen);
    SDL_BlitSurface(source, NULL, screen, NULL);
    SDL_Flip(screen);
    return false;
}
