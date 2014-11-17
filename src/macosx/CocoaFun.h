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


#ifndef Wolf4SDL_CocoaFun_h
#define Wolf4SDL_CocoaFun_h

char		*Cocoa_CreateApplicationSupportPathString();
void        Cocoa_DisplayErrorAlert(const char *msg, const char* title);

void        Cocoa_Notify(const char *title, const char *msg);

#ifdef IOS
double      Cocoa_StatusBarHeight();
void        Cocoa_HideStatusBar();
double      Cocoa_PixelsPerDot();
void        Cocoa_DisableIdleTimer();
#endif

#endif

