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

#include "Config.h"
#include "Exception.h"
#include "i_system.h"
#include "i_video.h"
#include "id_in.h"
#include "id_sd.h"
#include "id_us.h"
#include "ioan_bot.h"
#include "Logger.h"
#include "PString.h"
#include "wl_def.h"
#include "wl_main.h"
#include "wl_play.h"

InputManager myInput;

// IOANCH: added C++ class

const byte InputManager::m_ASCIINames[] =		// Unshifted ASCII for scan codes       // TODO: keypad
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

const byte InputManager::m_ShiftNames[] =		// Shifted ASCII for scan codes
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

//
// InputManager::p_getMouseButtons
//
// Gets mouse click info
//
int InputManager::p_getMouseButtons() const
{
   int buttons = SDL_GetMouseState(NULL, NULL);
   int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
   int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
   buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));
   if(middlePressed) buttons |= 1 << 2;
   if(rightPressed) buttons |= 1 << 1;
   
   return buttons;
}

//
// InputManager::p_processEvent
//
// Reads SDL events and puts them in its variables
//
void InputManager::p_processEvent(const SDL_Event *event)
{
//	static int lastOffset;
	
   switch (event->type)
   {
         // exit if the window is closed
      case SDL_QUIT:
		  Logger::Write("SDL_QUIT processed");
		   if(!g_appForcedToQuit)
		   {
			   Logger::Write("...not forced");
			   DestroySavedInstance();
			  if (ingame)
				  bot.SaveData();
			 Quit();
		   }
		   else
		   {
			   Logger::Write("...forced");

			   exit(0);
		   }
		   
         
         // check for keypresses
      case SDL_KEYDOWN:
      {
         if(event->key.keysym.sym == SDLK_SCROLLLOCK ||
            event->key.keysym.sym == SDLK_F12)
         {
            m_grabInput = !m_grabInput;
			 SDL_SetRelativeMouseMode(m_grabInput ? SDL_TRUE : SDL_FALSE);
//            SDL_WM_GrabInput(m_grabInput ? SDL_GRAB_ON : SDL_GRAB_OFF);
            return;
         }
         
         m_lastScan = (ScanCode)event->key.keysym.sym;
         SDL_Keymod mod = SDL_GetModState();
         if(m_keyboard.count(sc_Alt))
         {
            if(static_cast<int>(m_lastScan) == static_cast<int>(SDLK_F4))
			{
				DestroySavedInstance();
				if (ingame)
					bot.SaveData();
               Quit();
			}
         }
         // IOANCH 20130801: added meta key mapping
         if(static_cast<int>(m_lastScan) == static_cast<int>(SDLK_KP_ENTER)) m_lastScan = (ScanCode)SDLK_RETURN;
         else if(m_lastScan == static_cast<ScanCode>(SDLK_RSHIFT)) m_lastScan = (ScanCode)SDLK_LSHIFT;
         else if(m_lastScan == static_cast<ScanCode>(SDLK_RALT)) m_lastScan = (ScanCode)SDLK_LALT;
         else if(m_lastScan == static_cast<ScanCode>(SDLK_RCTRL)) m_lastScan = (ScanCode)SDLK_LCTRL;
         else if(m_lastScan == static_cast<ScanCode>(SDLK_RGUI)) m_lastScan = (ScanCode)SDLK_LGUI;
		 else if(m_lastScan == static_cast<ScanCode>(SDLK_AC_BACK)) m_lastScan = (ScanCode)SDLK_ESCAPE;
         else
         {
            if((mod & KMOD_NUM) == 0)
            {
               switch((int)m_lastScan)
               {
                  case SDLK_KP_2: m_lastScan = (ScanCode)SDLK_DOWN; break;
                  case SDLK_KP_4: m_lastScan = (ScanCode)SDLK_LEFT; break;
                  case SDLK_KP_6: m_lastScan = (ScanCode)SDLK_RIGHT; break;
                  case SDLK_KP_8: m_lastScan = (ScanCode)SDLK_UP; break;
               }
            }
         }
         
         int sym = m_lastScan;
		
         if(sym >= 'a' && sym <= 'z')
            sym -= 32;  // convert to uppercase
		  
         if(mod & (KMOD_SHIFT | KMOD_CAPS))
         {
            if(sym < lengthof(m_ShiftNames) && m_ShiftNames[sym])
               m_lastASCII = m_ShiftNames[sym];
         }
         else
         {
            if(sym < lengthof(m_ASCIINames) && m_ASCIINames[sym])
               m_lastASCII = m_ASCIINames[sym];
         }
		m_keyboard.insert(m_lastScan);
         if(m_lastScan == static_cast<ScanCode>(SDLK_PAUSE))
            m_paused = true;
         break;
		}
         
      case SDL_KEYUP:
      {
         int key = event->key.keysym.sym;
         if(key == SDLK_KP_ENTER) key = SDLK_RETURN;
         else if(key == SDLK_RSHIFT) key = SDLK_LSHIFT;
         else if(key == SDLK_RALT) key = SDLK_LALT;
         else if(key == SDLK_RCTRL) key = SDLK_LCTRL;
         else if(key == SDLK_RGUI) key = SDLK_LGUI;
		 else if(key == SDLK_AC_BACK) key = SDLK_ESCAPE;
         else
         {
            if((SDL_GetModState() & KMOD_NUM) == 0)
            {
               switch(key)
               {
                  case SDLK_KP_2: key = SDLK_DOWN; break;
                  case SDLK_KP_4: key = SDLK_LEFT; break;
                  case SDLK_KP_6: key = SDLK_RIGHT; break;
                  case SDLK_KP_8: key = SDLK_UP; break;
               }
            }
         }
         
         m_keyboard.erase(key);
         break;
      }
         
		   //
		   // For Android and possibly iOS
		   //
	   case SDL_APP_WILLENTERBACKGROUND:
		   Sound::Stop();
		   Sound::MusicOff();
		   break;
	   case SDL_APP_DIDENTERFOREGROUND:
		   Sound::MusicOn();
		   break;
	   case SDL_FINGERDOWN:
	   case SDL_FINGERMOTION:
	   case SDL_FINGERUP:
	   {
		   m_fingerdown = event->type != SDL_FINGERUP;
		   int windowWidth, windowHeight;
		   SDL_GetWindowSize(vid_window, &windowWidth, &windowHeight);
		   // WARNING: it's assumed that the window is in the middle. Who's to
		   // know for sure?
		   
		   if(128*windowWidth / windowHeight >
			  static_cast<int>(128*cfg_screenWidth / cfg_screenHeight))
		   {
			   // Greater aspect ratio of window: pillar box. Height is correct
			   
			   // 0... a... 1 - a... 1
			   // w / W
			   // w = H / sh * sw
			   // a = 1 / 2 - ca
			   // ca = 1 / 2 * w / W
			   // a = 1 / 2 - 1 / 2 * w / W
			   // a = 1 / 2 * (1 - w / W)
			   // a = 1 / 2 * (1 - H / sh * sw / W)
			   float a = 0.5f * (1 - (float)windowHeight * cfg_aspectRatio / windowWidth);
			   m_touchx = static_cast<int>((event->tfinger.x - a) / (1 - 2 * a) * cfg_screenWidth);
			   m_touchy = static_cast<int>(event->tfinger.y * cfg_screenHeight);
			   
		   }
		   else
		   {
			   float a = 0.5f * (1 - (float)windowWidth / cfg_aspectRatio / windowHeight);
			   m_touchx = static_cast<int>(event->tfinger.x * cfg_screenWidth);
			   m_touchy = static_cast<int>((event->tfinger.y - a) / (1 - 2 * a) * cfg_screenHeight);
		   }
	   }
		   break;
	   case SDL_TEXTINPUT:
		   if(event->text.text[0] && !event->text.text[1])
			   m_lastASCII = event->text.text[0];
		   break;
//      case SDL_ACTIVEEVENT:
//      {
//         if(cfg_fullscreen && (event->active.state & SDL_APPACTIVE) != 0)
//         {
//            if(event->active.gain)
//            {
//               if(m_needRestore)
//               {
//                  I_FreeLatchMem();
//                  I_LoadLatchMem();
//               }
//               
//               m_needRestore = false;
//            }
//            else m_needRestore = true;
//         }
//      }
         
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
// InputManager::clearKeysDown
//
// Removes input information so key presses aren't responded to
//
void InputManager::clearKeysDown()
{
   m_lastScan = sc_None;
	m_lastASCII = key_None;
	m_keyboard.clear();
}

//
// InputManager::initialize
//
// Sets up the parameters
//
void InputManager::initialize()
{
   if (m_started)
		return;
   
   clearKeysDown();
   
   if(cfg_joystickindex >= 0 && cfg_joystickindex < SDL_NumJoysticks())
   {
      m_joystick = SDL_JoystickOpen(cfg_joystickindex);
      if(m_joystick)
      {
         m_joyNumButtons = SDL_JoystickNumButtons(m_joystick);
         if(m_joyNumButtons > 32)
            m_joyNumButtons = 32;      // only up to 32 buttons are supported
         m_joyNumHats = SDL_JoystickNumHats(m_joystick);
         if(cfg_joystickhat < -1 || cfg_joystickhat >= m_joyNumHats)
            throw Exception(PString("The joystickhat param must be between 0 and ").concat(m_joyNumHats - 1).concat("!")());
      }
   }
   
   SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
   
   if(cfg_fullscreen || cfg_forcegrabmouse)
   {
      m_grabInput = true;
	   SDL_SetRelativeMouseMode(SDL_TRUE);

	   //      SDL_WM_GrabInput(SDL_GRAB_ON);
   }
   
   // I didn't find a way to ask libSDL whether a mouse is present, yet...
#if defined(GP2X)
   m_mousePresent = false;
#elif defined(_arch_dreamcast)
   m_mousePresent = DC_MousePresent();
#else
   m_mousePresent = true;
#endif
   
   m_started = true;
}

//
// InputManager::close
//
// Cleans up and disables the started flag
//
void InputManager::close()
{
   if (!m_started)
		return;
   
   if(m_joystick)
      SDL_JoystickClose(m_joystick);
   
	m_started = false;
}

//
// InputManager::getJoyDelta
//
// Outputs the joystick handling
//
void InputManager::getJoyDelta(int *dx, int *dy) const
{
   if(!m_joystick)
   {
      *dx = *dy = 0;
      return;
   }
   
   SDL_JoystickUpdate();
#ifdef _arch_dreamcast
   int x = 0;
   int y = 0;
#else
   int x = SDL_JoystickGetAxis(m_joystick, 0) >> 8;
   int y = SDL_JoystickGetAxis(m_joystick, 1) >> 8;
#endif
   
   if(cfg_joystickhat != -1)
   {
      uint8_t hatState = SDL_JoystickGetHat(m_joystick, cfg_joystickhat);
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

//
// InputManager::getJoyFineDelta
//
// Outputs for dreamcast
//
void InputManager::getJoyFineDelta(int *dx, int *dy) const
{
   if(!m_joystick)
   {
      *dx = 0;
      *dy = 0;
      return;
   }
   
   SDL_JoystickUpdate();
   int x = SDL_JoystickGetAxis(m_joystick, 0);
   int y = SDL_JoystickGetAxis(m_joystick, 1);
   
   if(x < -128) x = -128;
   else if(x > 127) x = 127;
   
   if(y < -128) y = -128;
   else if(y > 127) y = 127;
   
   *dx = x;
   *dy = y;
}

//
// InputManager::joyButtons
//
// Returns the status of joystick buttons
//
int InputManager::joyButtons() const
{
   if(!m_joystick) return 0;
   
   SDL_JoystickUpdate();
   
   int res = 0;
   for(int i = 0; i < m_joyNumButtons && i < 32; i++)
      res |= SDL_JoystickGetButton(m_joystick, i) << i;
   return res;
}

//
// InputManager::waitAndProcessEvents
//
// Process events after waiting for input
//
void InputManager::waitAndProcessEvents()
{
   SDL_Event event;
   if(!SDL_WaitEvent(&event)) return;
   do
   {
      p_processEvent(&event);
   }
   while(SDL_PollEvent(&event));
}

//
// InputManager::processEvents
//
// Gets instantaneous keyboard signal
//
void InputManager::processEvents()
{
   SDL_Event event;
   
   while (SDL_PollEvent(&event))
   {
      p_processEvent(&event);
   }
}

//
// InputManager::readControl
//
// Returns cursor movement data
//

struct  KeyboardDef
{
   ScanCode	button0,button1,
   upleft,		up,		upright,
   left,				right,
   downleft,	down,	downright;
};

//KeyboardDef	KbdDefs = {0x1d,0x38,0x47,0x48,0x49,0x4b,0x4d,0x4f,0x50,0x51};
static const KeyboardDef KbdDefs =
{
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

static	Direction	dirTable[] =		// Quick lookup for total direction
{
   dir_NorthWest,	dir_North,	dir_NorthEast,
   dir_West,		dir_None,	dir_East,
   dir_SouthWest,	dir_South,	dir_SouthEast
};

CursorInfo InputManager::readControl()
{
   word		buttons;
	int			dx,dy;
	Motion		mx,my;
   
   CursorInfo ret;
   
	mx = my = motion_None;
	buttons = 0;
   
	processEvents();
   
   if (m_keyboard.count(KbdDefs.upleft))
      mx = motion_Left,my = motion_Up;
   else if (m_keyboard.count(KbdDefs.upright))
      mx = motion_Right,my = motion_Up;
   else if (m_keyboard.count(KbdDefs.downleft))
      mx = motion_Left,my = motion_Down;
   else if (m_keyboard.count(KbdDefs.downright))
      mx = motion_Right,my = motion_Down;
   
   if (m_keyboard.count(KbdDefs.up))
      my = motion_Up;
   else if (m_keyboard.count(KbdDefs.down))
      my = motion_Down;
   
   if (m_keyboard.count(KbdDefs.left))
      mx = motion_Left;
   else if (m_keyboard.count(KbdDefs.right))
      mx = motion_Right;
   
   if (m_keyboard.count(KbdDefs.button0))
      buttons += 1 << 0;
   if (m_keyboard.count(KbdDefs.button1))
      buttons += 1 << 1;
   
	dx = mx * 127;
	dy = my * 127;
   
	ret.x = dx;
	ret.xaxis = mx;
	ret.y = dy;
	ret.yaxis = my;
	ret.touch = touch_None;
	ret.button0 = (buttons & (1 << 0)) != 0;
	ret.button1 = (buttons & (1 << 1)) != 0;
	ret.button2 = (buttons & (1 << 2)) != 0;
	ret.button3 = (buttons & (1 << 3)) != 0;
	ret.dir = dirTable[((my + 1) * 3) + (mx + 1)];
   return ret;
}

//
// InputManager::waitForKey
//
// Returns a key that should be pressed
//
ScanCode InputManager::waitForKey()
{
   ScanCode	result;
   
	while ((result = m_lastScan)==0)
		waitAndProcessEvents();
	m_lastScan = sc_None;
	return(result);
}

char InputManager::waitForASCII()
{
	char		result;
   
	while ((result = m_lastASCII)==0)
		waitAndProcessEvents();
	m_lastASCII = '\0';
	return(result);
}

void InputManager::startAck()
{
   processEvents();
   //
   // get initial state of everything
   //
	clearKeysDown();
	memset(m_btnstate, 0, sizeof(m_btnstate));
   
	int buttons = joyButtons() << 4;
   
	if(m_mousePresent)
		buttons |= mouseButtons();
   
	for(int i = 0; i < NUMBUTTONS; i++, buttons >>= 1)
		if(buttons & 1)
			m_btnstate[i] = true;
}

int InputManager::mouseButtons () const
{
	if (m_mousePresent)
		return p_getMouseButtons();
	else
		return 0;
}

bool InputManager::checkAck ()
{
   processEvents();
   //
   // see if something has been pressed
   //
	if(m_lastScan)
		return true;
   
	int buttons = joyButtons() << 4;
   
	if(m_mousePresent)
		buttons |= mouseButtons();
   
	for(int i = 0; i < NUMBUTTONS; i++, buttons >>= 1)
	{
		if(buttons & 1)
		{
			if(!m_btnstate[i])
         {
            // Wait until button has been released
            do
            {
               waitAndProcessEvents();
               buttons = joyButtons() << 4;
               
               if(m_mousePresent)
                  buttons |= mouseButtons();
            }
            while(buttons & (1 << i));
            
				return true;
         }
		}
		else
			m_btnstate[i] = false;
	}
	return false;
}

void InputManager::ack ()
{
	startAck ();
   
   do
   {
      waitAndProcessEvents();
   }
	while(!checkAck ());
}

bool InputManager::userInput(longword delay)
{
	longword	lasttime;
   
	lasttime = GetTimeCount();
	startAck ();
	do
	{
      processEvents();
		if (checkAck())
			return true;
      I_Delay(5);
	} while (GetTimeCount() - lasttime < delay);
	return(false);
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
         throw Exception("No joysticks are available to SDL!\n");
      else
         throw Exception((PString("The joystick index must be between -1 and %i!\n") <<
            (numJoysticks - 1))());
   }
}