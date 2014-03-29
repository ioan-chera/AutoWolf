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
// Various text handling routines
//
////////////////////////////////////////////////////////////////////////////////

// WL_TEXT.C

#include "wl_def.h"
#include "i_video.h"
#include "id_ca.h"
#include "id_sd.h"
#include "id_vh.h"
#include "version.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_menu.h"
#include "Exception.h"

/*
=============================================================================

TEXT FORMATTING COMMANDS
------------------------
^C<hex digit>           Change text color
^E[enter]               End of layout (all pages)
^G<y>,<x>,<pic>[enter]  Draw a graphic and push margins
^P[enter]               start new page, must be the first chars in a layout
^L<x>,<y>[ENTER]        Locate to a specific spot, x in pixels, y in lines

=============================================================================
*/

/*
=============================================================================

                                                 LOCAL CONSTANTS

=============================================================================
*/

// IOANCH 20130303: SPEAR::flag unification
// #ifndef SPEAR::flag

#define BACKCOLOR       0x11


#define WORDLIMIT       80
#define FONTHEIGHT      10
#define TOPMARGIN       16
#define BOTTOMMARGIN    32
#define LEFTMARGIN      16
#define RIGHTMARGIN     16
#define PICMARGIN       8
#define TEXTROWS        ((200-TOPMARGIN-BOTTOMMARGIN)/FONTHEIGHT)
#define SPACEWIDTH      7
#define SCREENPIXWIDTH  320
#define SCREENMID       (SCREENPIXWIDTH/2)

/*
=============================================================================

                                LOCAL VARIABLES

=============================================================================
*/

static int pagenum;
static int s_numpages;

static unsigned leftmargin[TEXTROWS];
static unsigned rightmargin[TEXTROWS];
static const char*    text;
static unsigned rowon;

static int     picx;
static int     picy;
static int     picnum;
static int     picdelay;
static Boolean8 layoutdone;

//===========================================================================

// IOANCH 20130301: unification culling
/*
=====================
=
= RipToEOL
=
=====================
*/

void RipToEOL ()
{
    while (*text++ != '\n')         // scan to end of line
        ;
}


/*
=====================
=
= ParseNumber
=
=====================
*/

int ParseNumber ()
{
    char  ch;
    char  num[80];
    char *numptr;

    //
    // scan until a number is found
    //
    ch = *text;
    while (ch < '0' || ch >'9')
        ch = *++text;

    //
    // copy the number out
    //
    numptr = num;
    do
    {
        *numptr++ = ch;
        ch = *++text;
    } while (ch >= '0' && ch <= '9');
    *numptr = 0;

    return atoi (num);
}



/*
=====================
=
= ParsePicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void ParsePicCommand ()
{
    picy=ParseNumber();
    picx=ParseNumber();
    picnum=ParseNumber();
    RipToEOL ();
}


void ParseTimedCommand ()
{
    picy=ParseNumber();
    picx=ParseNumber();
    picnum=ParseNumber();
    picdelay=ParseNumber();
    RipToEOL ();
}


/*
=====================
=
= TimedPicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void TimedPicCommand ()
{
    ParseTimedCommand ();

    //
    // update the screen, and wait for time delay
    //
    I_UpdateScreen ();

    //
    // wait for time
    //
    Delay(picdelay);

    //
    // draw pic
    //
    VWB_DrawPic (picx&~7,picy,picnum);
}


/*
=====================
=
= HandleCommand
=
=====================
*/

void HandleCommand ()
{
    int     i,margin,top,bottom;
    int     picwidth,picheight,picmid;

    switch (toupper(*++text))
    {
        case 'B':
            picy=ParseNumber();
            picx=ParseNumber();
            picwidth=ParseNumber();
            picheight=ParseNumber();
            VL_Bar(picx,picy,picwidth,picheight,BACKCOLOR);
            RipToEOL();
            break;
        case ';':               // comment
            RipToEOL();
            break;
        case 'P':               // ^P is start of next page, ^E is end of file
        case 'E':
            layoutdone = true;
            text--;             // back up to the '^'
            break;

        case 'C':               // ^c<hex digit> changes text color
            i = toupper(*++text);
            if (i>='0' && i<='9')
                g_fontcolor = i-'0';
            else if (i>='A' && i<='F')
                g_fontcolor = i-'A'+10;

            g_fontcolor *= 16;
            i = toupper(*++text);
            if (i>='0' && i<='9')
                g_fontcolor += i-'0';
            else if (i>='A' && i<='F')
                g_fontcolor += i-'A'+10;
            text++;
            break;

        case '>':
            px = 160;
            text++;
            break;

        case 'L':
            py=ParseNumber();
            rowon = (py-TOPMARGIN)/FONTHEIGHT;
            py = TOPMARGIN+rowon*FONTHEIGHT;
            px=ParseNumber();
            while (*text++ != '\n')         // scan to end of line
                ;
            break;

        case 'T':               // ^Tyyy,xxx,ppp,ttt waits ttt tics, then draws pic
            TimedPicCommand ();
            break;

        case 'G':               // ^Gyyy,xxx,ppp draws graphic
            ParsePicCommand ();
            VWB_DrawPic (picx&~7,picy,picnum);
            picwidth = graphSegs.sizeAt(picnum-SPEAR::g(STARTPICS)).width;
            picheight = graphSegs.sizeAt(picnum-SPEAR::g(STARTPICS)).height;
            //
            // adjust margins
            //
            picmid = picx + picwidth/2;
            if (picmid > SCREENMID)
                margin = picx-PICMARGIN;                        // new right margin
            else
                margin = picx+picwidth+PICMARGIN;       // new left margin

            top = (picy-TOPMARGIN)/FONTHEIGHT;
            if (top<0)
                top = 0;
            bottom = (picy+picheight-TOPMARGIN)/FONTHEIGHT;
            if (bottom>=TEXTROWS)
                bottom = TEXTROWS-1;

            for (i=top;i<=bottom;i++)
                if (picmid > SCREENMID)
                    rightmargin[i] = margin;
                else
                    leftmargin[i] = margin;

            //
            // adjust this line if needed
            //
            if (px < (int) leftmargin[rowon])
                px = leftmargin[rowon];
            break;
    }
}


/*
=====================
=
= NewLine
=
=====================
*/

void NewLine ()
{
    char    ch;

    if (++rowon == TEXTROWS)
    {
        //
        // overflowed the page, so skip until next page break
        //
        layoutdone = true;
        do
        {
            if (*text == '^')
            {
                ch = toupper(*(text+1));
                if (ch == 'E' || ch == 'P')
                {
                    layoutdone = true;
                    return;
                }
            }
            text++;
        } while (1);
    }
    px = leftmargin[rowon];
    py+= FONTHEIGHT;
}



/*
=====================
=
= HandleCtrls
=
=====================
*/

void HandleCtrls ()
{
    char    ch;

    ch = *text++;                   // get the character and advance

    if (ch == '\n')
    {
        NewLine ();
        return;
    }
}


/*
=====================
=
= HandleWord
=
=====================
*/

void HandleWord ()
{
    char    wword[WORDLIMIT];
    int     wordindex;
    word    wwidth,wheight,newpos;


    //
    // copy the next word into [word]
    //
    wword[0] = *text++;
    wordindex = 1;
    while (*text>32)
    {
        wword[wordindex] = *text++;
        if (++wordindex == WORDLIMIT)
            throw Exception ("PageLayout: Word limit exceeded");
    }
    wword[wordindex] = 0;            // stick a null at end for C

    //
    // see if it fits on this line
    //
    VW_MeasurePropString (wword,&wwidth,&wheight);

    while (px+wwidth > (int) rightmargin[rowon])
    {
        NewLine ();
        if (layoutdone)
            return;         // overflowed page
    }

    //
    // print it
    //
    newpos = px+wwidth;
    VWB_DrawPropString (wword);
    px = newpos;

    //
    // suck up any extra spaces
    //
    while (*text == ' ')
    {
        px += SPACEWIDTH;
        text++;
    }
}

/*
=====================
=
= PageLayout
=
= Clears the screen, draws the pics on the page, and word wraps the text.
= Returns a pointer to the terminating command
=
=====================
*/

void PageLayout (Boolean8 shownumber)
{
    int     i,oldfontcolor;
    char    ch;

    oldfontcolor = g_fontcolor;

    g_fontcolor = 0;

    //
    // clear the screen
    //
    VL_Bar (0,0,320,200,BACKCOLOR);
    VWB_DrawPic (0,0,SPEAR::g(H_TOPWINDOWPIC));
    VWB_DrawPic (0,8,SPEAR::g(H_LEFTWINDOWPIC));
    VWB_DrawPic (312,8,SPEAR::g(H_RIGHTWINDOWPIC));
    VWB_DrawPic (8,176,SPEAR::g(H_BOTTOMINFOPIC));


    for (i=0; i<TEXTROWS; i++)
    {
        leftmargin[i] = LEFTMARGIN;
        rightmargin[i] = SCREENPIXWIDTH-RIGHTMARGIN;
    }

    px = LEFTMARGIN;
    py = TOPMARGIN;
    rowon = 0;
    layoutdone = false;

    //
    // make sure we are starting layout text (^P first command)
    //
    while (*text <= 32)
        text++;

    if (*text != '^' || toupper(*++text) != 'P')
        throw Exception ("PageLayout: Text not headed with ^P");

    while (*text++ != '\n')
        ;


    //
    // process text stream
    //
    do
    {
        ch = *text;

        if (ch == '^')
            HandleCommand ();
        else
            if (ch == 9)
            {
                px = (px+8)&0xf8;
                text++;
            }
            else if (ch <= 32)
                HandleCtrls ();
            else
                HandleWord ();

    } while (!layoutdone);

    pagenum++;

    if (shownumber)
    {
#ifdef SPANISH
        sprintf(str, "Hoja %d de %d", pagenum, s_numpages);
        px = 208;
#else
        sprintf(str, "pg %d of %d", pagenum, s_numpages);
        px = 213;
#endif
        py = 183;
        g_fontcolor = 0x4f;                          //12^BACKCOLOR;

        VWB_DrawPropString (str);
    }

    g_fontcolor = oldfontcolor;
}

//===========================================================================

/*
=====================
=
= BackPage
=
= Scans for a previous ^P
=
=====================
*/

void BackPage ()
{
    pagenum--;
    do
    {
        text--;
        if (*text == '^' && toupper(*(text+1)) == 'P')
            return;
    } while (1);
}


//===========================================================================


/*
=====================
=
= CacheLayoutGraphics
=
= Scans an entire layout file (until a ^E) marking all graphics used, and
= counting pages, then caches the graphics in
=
=====================
*/
static void CacheLayoutGraphics ()
{
    const char    *bombpoint, *textstart;
    char    ch;

    textstart = text;
    bombpoint = text+30000;
    s_numpages = pagenum = 0;

    do
    {
        if (*text == '^')
        {
            ch = toupper(*++text);
            if (ch == 'P')          // start of a page
                s_numpages++;
            if (ch == 'E')          // end of file, so load graphics and return
            {
                graphSegs.cacheChunk(SPEAR::g(H_TOPWINDOWPIC));
                graphSegs.cacheChunk(SPEAR::g(H_LEFTWINDOWPIC));
                graphSegs.cacheChunk(SPEAR::g(H_RIGHTWINDOWPIC));
                graphSegs.cacheChunk(SPEAR::g(H_BOTTOMINFOPIC));
                //                              CA_CacheMarks ();
                text = textstart;
                return;
            }
            if (ch == 'G')          // draw graphic command, so mark graphics
            {
                ParsePicCommand ();
                graphSegs.cacheChunk (picnum);
            }
            if (ch == 'T')          // timed draw graphic command, so mark graphics
            {
                ParseTimedCommand ();
                graphSegs.cacheChunk (picnum);
            }
        }
        else
            text++;

    } while (text<bombpoint);

    throw Exception ("CacheLayoutGraphics: No ^E to terminate file!");
}

/*
=====================
=
= ShowArticle
=
=====================
*/
// IOANCH 20130301: unification culling
static void ShowArticle (const char *article)
{
    // IOANCH 20130301: unification culling

    unsigned    oldfontnumber;
    Boolean8     newpage,firstpage;
    CursorInfo ci;

    // IOANCH 20130301: unification culling
    text = article;
    oldfontnumber = fontnumber;
    fontnumber = 0;
    graphSegs.cacheChunk(SPEAR::g(STARTFONT));
    VL_Bar (0,0,320,200,BACKCOLOR);
    CacheLayoutGraphics ();

    newpage = true;
    firstpage = true;

    do
    {
        if (newpage)
        {
            newpage = false;
            // IOANCH 20130301: unification culling
            PageLayout (true);
            I_UpdateScreen ();
            if (firstpage)
            {
                // IOANCH 20130202: unification process
                VL_FadeIn(0,255,IMPALE(vid_palette),10);
                firstpage = false;
            }
        }
        I_Delay(5);

       myInput.setLastScan(sc_None);
        ReadAnyControl(&ci);
        Direction dir = ci.dir;
        switch(dir)
        {
            case dir_North:
            case dir_South:
                break;

            default:
                if(ci.button0) dir = dir_South;
                switch(myInput.lastScan())
                {
                    case sc_UpArrow:
                    case sc_PgUp:
                    case sc_LeftArrow:
                        dir = dir_North;
                        break;

                    case sc_Enter:
                    case sc_DownArrow:
                    case sc_PgDn:
                    case sc_RightArrow:
                        dir = dir_South;
                        break;
                      default: ;
                      ;
                }
                break;
        }

        switch(dir)
        {
            case dir_North:
            case dir_West:
                if (pagenum>1)
                {
                    // IOANCH 20130301: unification culling
                    BackPage ();
                    BackPage ();
                    newpage = true;
                }
                TicDelay(20);
                break;

            case dir_South:
            case dir_East:
                if (pagenum<s_numpages)
                {
                    newpage = true;
                    // IOANCH 20130301: unification culling
                }
                TicDelay(20);
                break;
			default:
				;
        }
    } while (myInput.lastScan() != sc_Escape && !ci.button1);

    myInput.clearKeysDown ();
    fontnumber = oldfontnumber;
}


//===========================================================================
// IOANCH 20130301: unification culling
#ifdef ARTSEXTERN
int     endextern = T_ENDART1;
int     helpextern = T_HELPART;
#endif
char helpfilename[13] = "HELPART.";
std::string cfg_endfilename("ENDART1.");

/*
=================
=
= HelpScreens
=
=================
*/
void HelpScreens ()
{
    int     artnum;
    const char    *text;
#ifndef ARTSEXTERN
    memptr  layout;
#endif

// IOANCH 20130301: unification culling

#ifdef ARTSEXTERN
    // IOANCH 20130302: helpextern used to be here
    artnum = SPEAR::g(T_HELPART);
    graphSegs.cacheChunk (artnum);
    text = (const char *)graphSegs[artnum];
#else
    CA_LoadFile (helpfilename,&layout);
    text = (const char *)layout;
#endif

    ShowArticle (text);

#ifdef ARTSEXTERN
    graphSegs.uncacheChunk(artnum);
#else
    free(layout);
#endif

    VW_FadeOut();

    FreeMusic ();

}

//
// END ARTICLES
//
void EndText ()
{
    int     artnum;
    const char    *text;
#ifndef ARTSEXTERN
    memptr  layout;
#endif

    Sound::StopDigitized ();

    // IOANCH 20130301: unification culling



#ifdef ARTSEXTERN
    // IOANCH 20130302: unification: map it here. (endextern used to be SPEAR::g(T_ENDART1)
    artnum = SPEAR::g(T_ENDART1)+gamestate.episode;
    graphSegs.cacheChunk (artnum);
    text = (const char *)graphSegs[artnum];
#else
    cfg_endfilename[6] = '1'+gamestate.episode;
    CA_LoadFile (cfg_endfilename.buffer(),&layout);
    text = (char *)layout;
#endif

    ShowArticle (text);

#ifdef ARTSEXTERN
    graphSegs.uncacheChunk(artnum);
#else
    free(layout);
#endif


    VW_FadeOut();
    SETFONTCOLOR(0,15);
    myInput.clearKeysDown();
    if (myInput.mousePresent() && myInput.inputGrabbed())
        myInput.centreMouse();  // Clear accumulated mouse movement

    FreeMusic ();

}
// #endif
