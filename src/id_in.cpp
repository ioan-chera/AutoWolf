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

//
//	ID Engine
//	ID_IN.c - Input Manager
//	v1.0d1
//	By Jason Blochowiak
//

//
//	This module handles dealing with the various input devices
//
//	Depends on: Memory Mgr (for demo recording), Sound Mgr (for timing stuff),
//				User Mgr (for command line parms)
//
//	Globals:
//		in_lastScan - The keyboard scan code of the last key pressed
//		in_lastASCII - The ASCII value of the last key pressed
//	DEBUG - there are more globals
//

#include "wl_def.h"
#include "Config.h"
#include "i_system.h"
#include "i_video.h"
#include "wl_main.h"

////////////////////////////////////////////////////////////////////////////////
//
//                    GLOBAL VARIABLES
//
////////////////////////////////////////////////////////////////////////////////



//
// configuration variables
//
Boolean8 in_mousePresent;


// 	Global variables
volatile Boolean8    in_keyboard[SDLK_LAST];
volatile Boolean8	in_paused;
volatile char		in_lastASCII;
volatile ScanCode	in_lastScan;

//KeyboardDef	KbdDefs = {0x1d,0x38,0x47,0x48,0x49,0x4b,0x4d,0x4f,0x50,0x51};
static KeyboardDef KbdDefs = {
    sc_Control,             // button0
    sc_Alt,                 // button1
    sc_Home,                // upleft
    sc_UpArrow,             // up
    sc_PgUp,                // upright
    sc_LeftArrow,           // left
    sc_RightArrow,          // right
    sc_End,                 // downleft
    sc_DownArrow,           // down
    sc_PgDn                 // downright
};

static SDL_Joystick *in_joystick;
int in_joyNumButtons;
static int in_joyNumHats;

static bool in_grabInput = false;
static bool in_needRestore = false;

////////////////////////////////////////////////////////////////////////////////
//
//                    LOCAL VARIABLES
//
////////////////////////////////////////////////////////////////////////////////

byte        ASCIINames[] =		// Unshifted ASCII for scan codes       // TODO: keypad
{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,8  ,9  ,0  ,0  ,0  ,13 ,0  ,0  ,	// 0
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,27 ,0  ,0  ,0  ,	// 1
	' ',0  ,0  ,0  ,0  ,0  ,0  ,39 ,0  ,0  ,'*','+',',','-','.','/',	// 2
	'0','1','2','3','4','5','6','7','8','9',0  ,';',0  ,'=',0  ,0  ,	// 3
	'`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',	// 4
	'p','q','r','s','t','u','v','w','x','y','z','[',92 ,']',0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0		// 7
};
byte ShiftNames[] =		// Shifted ASCII for scan codes
{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,8  ,9  ,0  ,0  ,0  ,13 ,0  ,0  ,	// 0
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,27 ,0  ,0  ,0  ,	// 1
	' ',0  ,0  ,0  ,0  ,0  ,0  ,34 ,0  ,0  ,'*','+','<','_','>','?',	// 2
	')','!','@','#','$','%','^','&','*','(',0  ,':',0  ,'+',0  ,0  ,	// 3
	'~','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',	// 4
	'P','Q','R','S','T','U','V','W','X','Y','Z','{','|','}',0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0		// 7
};
byte SpecialNames[] =	// ASCII for 0xe0 prefixed codes
{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 0
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,13 ,0  ,0  ,0  ,	// 1
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 2
	0  ,0  ,0  ,0  ,0  ,'/',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 4
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
};


static	Boolean8		IN_Started;

static	Direction	in_dirTable[] =		// Quick lookup for total direction
{
    dir_NorthWest,	dir_North,	dir_NorthEast,
    dir_West,		dir_None,	dir_East,
    dir_SouthWest,	dir_South,	dir_SouthEast
};


///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseButtons() - Gets the status of the mouse buttons from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static int INL_GetMouseButtons()
{
    int buttons = SDL_GetMouseState(NULL, NULL);
    int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));
    if(middlePressed) buttons |= 1 << 2;
    if(rightPressed) buttons |= 1 << 1;

    return buttons;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyDelta() - Returns the relative movement of the specified
//		joystick (from +/-127)
//
///////////////////////////////////////////////////////////////////////////
void IN_GetJoyDelta(int *dx,int *dy)
{
    if(!in_joystick)
    {
        *dx = *dy = 0;
        return;
    }

    SDL_JoystickUpdate();
#ifdef _arch_dreamcast
    int x = 0;
    int y = 0;
#else
    int x = SDL_JoystickGetAxis(in_joystick, 0) >> 8;
    int y = SDL_JoystickGetAxis(in_joystick, 1) >> 8;
#endif

    if(cfg_joystickhat != -1)
    {
        uint8_t hatState = SDL_JoystickGetHat(in_joystick, cfg_joystickhat);
        if(hatState & SDL_HAT_RIGHT)
            x += 127;
        else if(hatState & SDL_HAT_LEFT)
            x -= 127;
        if(hatState & SDL_HAT_DOWN)
            y += 127;
        else if(hatState & SDL_HAT_UP)
            y -= 127;

        if(x < -128) x = -128;
        else if(x > 127) x = 127;

        if(y < -128) y = -128;
        else if(y > 127) y = 127;
    }

    *dx = x;
    *dy = y;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyFineDelta() - Returns the relative movement of the specified
//		joystick without dividing the results by 256 (from +/-127)
//
///////////////////////////////////////////////////////////////////////////
void IN_GetJoyFineDelta(int *dx, int *dy)
{
    if(!in_joystick)
    {
        *dx = 0;
        *dy = 0;
        return;
    }

    SDL_JoystickUpdate();
    int x = SDL_JoystickGetAxis(in_joystick, 0);
    int y = SDL_JoystickGetAxis(in_joystick, 1);

    if(x < -128) x = -128;
    else if(x > 127) x = 127;

    if(y < -128) y = -128;
    else if(y > 127) y = 127;

    *dx = x;
    *dy = y;
}

////////////////////////////////////////////////////////////////////////////////
//
// =
// = IN_JoyButtons
// =
//
////////////////////////////////////////////////////////////////////////////////

int IN_JoyButtons()
{
    if(!in_joystick) return 0;

    SDL_JoystickUpdate();

    int res = 0;
    for(int i = 0; i < in_joyNumButtons && i < 32; i++)
        res |= SDL_JoystickGetButton(in_joystick, i) << i;
    return res;
}

//
// IN_JoyPresent
//
Boolean8 IN_JoyPresent()
{
    return in_joystick != NULL;
}

//
// processEvent
//
static void INL_processEvent(SDL_Event *event)
{
    switch (event->type)
    {
        // exit if the window is closed
        case SDL_QUIT:
            Quit(NULL);

        // check for keypresses
        case SDL_KEYDOWN:
        {
            if(event->key.keysym.sym==SDLK_SCROLLOCK || event->key.keysym.sym==SDLK_F12)
            {
                in_grabInput = !in_grabInput;
                SDL_WM_GrabInput(in_grabInput ? SDL_GRAB_ON : SDL_GRAB_OFF);
                return;
            }

            in_lastScan = event->key.keysym.sym;
            SDLMod mod = SDL_GetModState();
            if(in_keyboard[sc_Alt])
            {
                if(in_lastScan==SDLK_F4)
                    Quit(NULL);
            }
// IOANCH 20130801: added meta key mapping
            if(in_lastScan == SDLK_KP_ENTER) in_lastScan = SDLK_RETURN;
            else if(in_lastScan == SDLK_RSHIFT) in_lastScan = SDLK_LSHIFT;
            else if(in_lastScan == SDLK_RALT) in_lastScan = SDLK_LALT;
            else if(in_lastScan == SDLK_RCTRL) in_lastScan = SDLK_LCTRL;
            else if(in_lastScan == SDLK_RMETA) in_lastScan = SDLK_LMETA;
            else
            {
                if((mod & KMOD_NUM) == 0)
                {
                    switch(in_lastScan)
                    {
                        case SDLK_KP2: in_lastScan = SDLK_DOWN; break;
                        case SDLK_KP4: in_lastScan = SDLK_LEFT; break;
                        case SDLK_KP6: in_lastScan = SDLK_RIGHT; break;
                        case SDLK_KP8: in_lastScan = SDLK_UP; break;
                    }
                }
            }

            int sym = in_lastScan;
            if(sym >= 'a' && sym <= 'z')
                sym -= 32;  // convert to uppercase

            if(mod & (KMOD_SHIFT | KMOD_CAPS))
            {
                if(sym < lengthof(ShiftNames) && ShiftNames[sym])
                    in_lastASCII = ShiftNames[sym];
            }
            else
            {
                if(sym < lengthof(ASCIINames) && ASCIINames[sym])
                    in_lastASCII = ASCIINames[sym];
            }
            if(in_lastScan<SDLK_LAST)
                in_keyboard[in_lastScan] = 1;
            if(in_lastScan == SDLK_PAUSE)
                in_paused = true;
            break;
		}

        case SDL_KEYUP:
        {
            int key = event->key.keysym.sym;
            if(key == SDLK_KP_ENTER) key = SDLK_RETURN;
            else if(key == SDLK_RSHIFT) key = SDLK_LSHIFT;
            else if(key == SDLK_RALT) key = SDLK_LALT;
            else if(key == SDLK_RCTRL) key = SDLK_LCTRL;
            else if(key == SDLK_RMETA) key = SDLK_LMETA;
            else
            {
                if((SDL_GetModState() & KMOD_NUM) == 0)
                {
                    switch(key)
                    {
                        case SDLK_KP2: key = SDLK_DOWN; break;
                        case SDLK_KP4: key = SDLK_LEFT; break;
                        case SDLK_KP6: key = SDLK_RIGHT; break;
                        case SDLK_KP8: key = SDLK_UP; break;
                    }
                }
            }

            if(key<SDLK_LAST)
                in_keyboard[key] = 0;
            break;
        }

        case SDL_ACTIVEEVENT:
        {
            if(cfg_fullscreen && (event->active.state & SDL_APPACTIVE) != 0)
            {
                if(event->active.gain)
                {
                    if(in_needRestore)
                    {
                        I_FreeLatchMem();
                        I_LoadLatchMem();
                    }

                    in_needRestore = false;
                }
                else in_needRestore = true;
            }
        }

#if defined(GP2X)
        case SDL_JOYBUTTONDOWN:
            GP2X_ButtonDown(event->jbutton.button);
            break;

        case SDL_JOYBUTTONUP:
            GP2X_ButtonUp(event->jbutton.button);
            break;
#endif
    }
}

//
// IN_WaitAndProcessEvents
//
void IN_WaitAndProcessEvents()
{
    SDL_Event event;
    if(!SDL_WaitEvent(&event)) return;
    do
    {
        INL_processEvent(&event);
    }
    while(SDL_PollEvent(&event));
}

//
// IN_ProcessEvents
//
void IN_ProcessEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        INL_processEvent(&event);
    }
}


///////////////////////////////////////////////////////////////////////////
//
//	IN_Startup() - Starts up the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void IN_Startup()
{
	if (IN_Started)
		return;

    IN_ClearKeysDown();

    if(cfg_joystickindex >= 0 && cfg_joystickindex < SDL_NumJoysticks())
    {
        in_joystick = SDL_JoystickOpen(cfg_joystickindex);
        if(in_joystick)
        {
            in_joyNumButtons = SDL_JoystickNumButtons(in_joystick);
            if(in_joyNumButtons > 32)
               in_joyNumButtons = 32;      // only up to 32 buttons are supported
            in_joyNumHats = SDL_JoystickNumHats(in_joystick);
            if(cfg_joystickhat < -1 || cfg_joystickhat >= in_joyNumHats)
                Quit(PString("The joystickhat param must be between 0 and ").concat(in_joyNumHats - 1).concat("!")());
        }
    }

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

    if(cfg_fullscreen || cfg_forcegrabmouse)
    {
        in_grabInput = true;
        SDL_WM_GrabInput(SDL_GRAB_ON);
    }

    // I didn't find a way to ask libSDL whether a mouse is present, yet...
#if defined(GP2X)
    in_mousePresent = false;
#elif defined(_arch_dreamcast)
    in_mousePresent = DC_MousePresent();
#else
    in_mousePresent = true;
#endif

    IN_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Shutdown() - Shuts down the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void IN_Shutdown()
{
	if (!IN_Started)
		return;

    if(in_joystick)
        SDL_JoystickClose(in_joystick);

	IN_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ClearKeysDown() - Clears the keyboard array
//
///////////////////////////////////////////////////////////////////////////
void IN_ClearKeysDown()
{
	in_lastScan = sc_None;
	in_lastASCII = key_None;
	memset ((void *) in_keyboard,0,sizeof(in_keyboard));
}


///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadControl() - Reads the device associated with the specified
//		player and fills in the control info struct
//
///////////////////////////////////////////////////////////////////////////
void IN_ReadControl(int player,ControlInfo *info)
{
	word		buttons;
	int			dx,dy;
	Motion		mx,my;

	dx = dy = 0;
	mx = my = motion_None;
	buttons = 0;

	IN_ProcessEvents();

    if (in_keyboard[KbdDefs.upleft])
        mx = motion_Left,my = motion_Up;
    else if (in_keyboard[KbdDefs.upright])
        mx = motion_Right,my = motion_Up;
    else if (in_keyboard[KbdDefs.downleft])
        mx = motion_Left,my = motion_Down;
    else if (in_keyboard[KbdDefs.downright])
        mx = motion_Right,my = motion_Down;

    if (in_keyboard[KbdDefs.up])
        my = motion_Up;
    else if (in_keyboard[KbdDefs.down])
        my = motion_Down;

    if (in_keyboard[KbdDefs.left])
        mx = motion_Left;
    else if (in_keyboard[KbdDefs.right])
        mx = motion_Right;

    if (in_keyboard[KbdDefs.button0])
        buttons += 1 << 0;
    if (in_keyboard[KbdDefs.button1])
        buttons += 1 << 1;

	dx = mx * 127;
	dy = my * 127;

	info->x = dx;
	info->xaxis = mx;
	info->y = dy;
	info->yaxis = my;
	info->button0 = (buttons & (1 << 0)) != 0;
	info->button1 = (buttons & (1 << 1)) != 0;
	info->button2 = (buttons & (1 << 2)) != 0;
	info->button3 = (buttons & (1 << 3)) != 0;
	info->dir = in_dirTable[((my + 1) * 3) + (mx + 1)];
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForKey() - Waits for a scan code, then clears in_lastScan and
//		returns the scan code
//
///////////////////////////////////////////////////////////////////////////
ScanCode IN_WaitForKey()
{
	ScanCode	result;

	while ((result = in_lastScan)==0)
		IN_WaitAndProcessEvents();
	in_lastScan = 0;
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForASCII() - Waits for an ASCII char, then clears in_lastASCII and
//		returns the ASCII value
//
///////////////////////////////////////////////////////////////////////////
char IN_WaitForASCII()
{
	char		result;

	while ((result = in_lastASCII)==0)
		IN_WaitAndProcessEvents();
	in_lastASCII = '\0';
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Ack() - waits for a button or key press.  If a button is down, upon
// calling, it must be released for it to be recognized
//
///////////////////////////////////////////////////////////////////////////

static Boolean8	in_btnstate[NUMBUTTONS];

//
// IN_StartAck
//
void IN_StartAck()
{
    IN_ProcessEvents();
//
// get initial state of everything
//
	IN_ClearKeysDown();
	memset(in_btnstate, 0, sizeof(in_btnstate));

	int buttons = IN_JoyButtons() << 4;

	if(in_mousePresent)
		buttons |= IN_MouseButtons();

	for(int i = 0; i < NUMBUTTONS; i++, buttons >>= 1)
		if(buttons & 1)
			in_btnstate[i] = true;
}

//
// IN_CheckAck
//
Boolean8 IN_CheckAck ()
{
    IN_ProcessEvents();
//
// see if something has been pressed
//
	if(in_lastScan)
		return true;

	int buttons = IN_JoyButtons() << 4;

	if(in_mousePresent)
		buttons |= IN_MouseButtons();

	for(int i = 0; i < NUMBUTTONS; i++, buttons >>= 1)
	{
		if(buttons & 1)
		{
			if(!in_btnstate[i])
            {
                // Wait until button has been released
                do
                {
                    IN_WaitAndProcessEvents();
                    buttons = IN_JoyButtons() << 4;

                    if(in_mousePresent)
                        buttons |= IN_MouseButtons();
                }
                while(buttons & (1 << i));

				return true;
            }
		}
		else
			in_btnstate[i] = false;
	}

	return false;
}

//
// IN_Ack
//
void IN_Ack ()
{
	IN_StartAck ();

    do
    {
        IN_WaitAndProcessEvents();
    }
	while(!IN_CheckAck ());
}


///////////////////////////////////////////////////////////////////////////
//
//	IN_UserInput() - Waits for the specified delay time (in ticks) or the
//		user pressing a key or a mouse button. If the clear flag is set, it
//		then either clears the key or waits for the user to let the mouse
//		button up.
//
///////////////////////////////////////////////////////////////////////////
Boolean8 IN_UserInput(longword delay)
{
	longword	lasttime;

	lasttime = GetTimeCount();
	IN_StartAck ();
	do
	{
        IN_ProcessEvents();
		if (IN_CheckAck())
			return true;
        I_Delay(5);
	} while (GetTimeCount() - lasttime < delay);
	return(false);
}

//===========================================================================

////////////////////////////////////////////////////////////////////////////////
//
// =
// = IN_MouseButtons
// =
//
////////////////////////////////////////////////////////////////////////////////

int IN_MouseButtons ()
{
	if (in_mousePresent)
		return INL_GetMouseButtons();
	else
		return 0;
}

//
// IN_IsInputGrabbed
//
bool IN_IsInputGrabbed()
{
    return in_grabInput;
}

//
// IN_CenterMouse
//
void IN_CenterMouse()
{
    SDL_WarpMouse(cfg_screenWidth / 2, cfg_screenHeight / 2);
}

//
// IN_InitVerifyJoysticks
//
// IOANCH: moved the startup joystick validity check here
//
void IN_InitVerifyJoysticks()
{
   int numJoysticks = SDL_NumJoysticks();
   if(cfg_joystickindex && (cfg_joystickindex < -1 ||
      cfg_joystickindex >= numJoysticks))
   {
      if(!numJoysticks)
         Quit("No joysticks are available to SDL!\n");
      else
         Quit((PString("The joystick index must be between -1 and %i!\n") <<
            (numJoysticks - 1))());
   }
}