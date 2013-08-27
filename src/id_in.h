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
//	ID_IN.h - Header file for Input Manager
//	v1.0d1
//	By Jason Blochowiak
//

#ifndef	__ID_IN__
#define	__ID_IN__

#include "Config.h"

#ifdef	__DEBUG__
#define	__DEBUG_InputMgr__
#endif

typedef	enum		{
   motion_Left = -1,motion_Up = -1,
   motion_None = 0,
   motion_Right = 1,motion_Down = 1
} Motion;
typedef	enum		{
   dir_North,dir_NorthEast,
   dir_East,dir_SouthEast,
   dir_South,dir_SouthWest,
   dir_West,dir_NorthWest,
   dir_None
} Direction;

struct  CursorInfo
{
   Boolean8		button0,button1,button2,button3;
   short		x,y;
   Motion		xaxis,yaxis;
   Direction	dir;
};

enum ScanCode
{
   sc_None =         0,
   sc_Bad =          0xff,
   sc_Return =       SDLK_RETURN,
   sc_Enter =        sc_Return,
   sc_Escape =       SDLK_ESCAPE,
   sc_Space =        SDLK_SPACE,
   sc_BackSpace =    SDLK_BACKSPACE,
   sc_Tab =          SDLK_TAB,
   sc_Alt =          SDLK_LALT,
   sc_Control =      SDLK_LCTRL,
   sc_CapsLock =     SDLK_CAPSLOCK,
   sc_LShift =       SDLK_LSHIFT,
   sc_RShift =       SDLK_RSHIFT,
   sc_UpArrow =      SDLK_UP,
   sc_DownArrow =    SDLK_DOWN,
   sc_LeftArrow =    SDLK_LEFT,
   sc_RightArrow =   SDLK_RIGHT,
   sc_Insert =       SDLK_INSERT,
   sc_Delete =       SDLK_DELETE,
   sc_Home =         SDLK_HOME,
   sc_End =          SDLK_END,
   sc_PgUp =         SDLK_PAGEUP,
   sc_PgDn =         SDLK_PAGEDOWN,
   sc_F1 =           SDLK_F1,
   sc_F2 =           SDLK_F2,
   sc_F3 =           SDLK_F3,
   sc_F4 =           SDLK_F4,
   sc_F5 =           SDLK_F5,
   sc_F6 =           SDLK_F6,
   sc_F7 =           SDLK_F7,
   sc_F8 =           SDLK_F8,
   sc_F9 =           SDLK_F9,
   sc_F10 =          SDLK_F10,
   sc_F11 =          SDLK_F11,
   sc_F12 =          SDLK_F12,
   
   sc_ScrollLock =   SDLK_SCROLLOCK,
   sc_PrintScreen =  SDLK_PRINT,
   
   sc_1 =            SDLK_1,
   sc_2 =            SDLK_2,
   sc_3 =            SDLK_3,
   sc_4 =            SDLK_4,
   sc_5 =            SDLK_5,
   sc_6 =            SDLK_6,
   sc_7 =            SDLK_7,
   sc_8 =            SDLK_8,
   sc_9 =            SDLK_9,
   sc_0 =            SDLK_0,
   
   sc_A =            SDLK_a,
   sc_B =            SDLK_b,
   sc_C =            SDLK_c,
   sc_D =            SDLK_d,
   sc_E =            SDLK_e,
   sc_F =            SDLK_f,
   sc_G =            SDLK_g,
   sc_H =            SDLK_h,
   sc_I =            SDLK_i,
   sc_J =            SDLK_j,
   sc_K =            SDLK_k,
   sc_L =            SDLK_l,
   sc_M =            SDLK_m,
   sc_N =            SDLK_n,
   sc_O =            SDLK_o,
   sc_P =            SDLK_p,
   sc_Q =            SDLK_q,
   sc_R =            SDLK_r,
   sc_S =            SDLK_s,
   sc_T =            SDLK_t,
   sc_U =            SDLK_u,
   sc_V =            SDLK_v,
   sc_W =            SDLK_w,
   sc_X =            SDLK_x,
   sc_Y =            SDLK_y,
   sc_Z =            SDLK_z,
   
   // IOANCH 20130802: added meta
   sc_LMeta =        SDLK_LMETA,
   sc_RMeta =        SDLK_RMETA,
   sc_KP5 =          SDLK_KP5,
};
const char key_None = 0;

//
// InputManager
//
// IOANCH: added C++ class
//
class InputManager
{
private:
   bool m_started;
   char       m_lastASCII;
   ScanCode   m_lastScan;
   Boolean8    m_keyboard[SDLK_LAST]; // IOANCH: removed volatile
   int        m_joyNumButtons;
   Boolean8    m_mousePresent;
   Boolean8    m_paused;
   
   SDL_Joystick *m_joystick;
   int m_joyNumHats;
   bool m_grabInput;
   bool m_needRestore;
   static const byte m_ASCIINames[], m_ShiftNames[];
   Boolean8	m_btnstate[NUMBUTTONS];
   
   int p_getMouseButtons() const;
   void p_processEvent(const SDL_Event *event);
   
   
public:
   InputManager() : m_started(false), m_lastASCII(key_None),
   m_lastScan(sc_None), m_joyNumButtons(0), m_mousePresent(false),
   m_paused(false), m_joystick(NULL), m_joyNumHats(0), m_grabInput(false),
   m_needRestore(false)
   {
      memset(m_keyboard, 0, sizeof(m_keyboard));
      memset(m_btnstate, 0, sizeof(m_btnstate));
   }
   void clearKeysDown();
   void initialize();
   void close();
   
   void centreMouse() const
   {
      SDL_WarpMouse(cfg_screenWidth / 2, cfg_screenHeight / 2);
   }
   
   bool inputGrabbed() const
   {
      return m_grabInput;
   }
   
   void getJoyDelta(int *dx, int *dy) const;
   void getJoyFineDelta(int *dx, int *dy) const;
   int joyButtons() const;
   bool joyPresent() const
   {
      return m_joystick != NULL;
   }
   
   void waitAndProcessEvents();
   void processEvents();
   CursorInfo readControl();
   ScanCode waitForKey();
   char waitForASCII();
   
   void startAck();
   int mouseButtons() const;
   bool checkAck();
   void ack();
   
   bool userInput(longword delay);
   
   Boolean8 keyboard(ScanCode code) const
   {
      return m_keyboard[code];
   }
   void setKeyboard(ScanCode code, Boolean8 value)
   {
      m_keyboard[code] = value;
   }
   Boolean8 mousePresent() const
   {
      return m_mousePresent;
   }
   Boolean8 paused() const
   {
      return m_paused;
   }
   void setPaused(Boolean8 value)
   {
      m_paused = value;
   }
   char lastASCII() const
   {
      return m_lastASCII;
   }
   void setLastASCII(char value)
   {
      m_lastASCII = value;
   }
   ScanCode lastScan() const
   {
      return m_lastScan;
   }
   void setLastScan(ScanCode value)
   {
      m_lastScan = value;
   }
   int joyNumButtons() const
   {
      return m_joyNumButtons;
   }
};
extern InputManager myInput;

// Function prototypes
#define	IN_KeyDown(code)	(myInput.keyboard(code))
#define	IN_ClearKey(code)	{myInput.setKeyboard((code), false);\
							if ((code) == myInput.lastScan()) myInput.setLastScan(sc_None);}

void IN_InitVerifyJoysticks();

#endif
