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
// Cocoa/OSX-specific calls.
//
////////////////////////////////////////////////////////////////////////////////

#include "CocoaFun.h"
#import <Cocoa/Cocoa.h>

#define APPLICATION_UTI @"com.ichera.autowolf"

//
// Cocoa_ApplicationSupportDirectory
//
// Returns the directory where to save configs
// Based on Apple documentation example
//
const char *Cocoa_ApplicationSupportDirectory()
{
   NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
   if([paths count] <= 0)
      return ".";
   
   NSString *applicationSupportDirectory = [paths objectAtIndex:0];
   return [[applicationSupportDirectory stringByAppendingPathComponent:APPLICATION_UTI] cStringUsingEncoding:NSUTF8StringEncoding];

}

//
// Cocoa_DisplayErrorAlert
//
void Cocoa_DisplayErrorAlert(const char *msg)
{
    [[NSAlert alertWithMessageText:@"Automatic Wolfenstein quit with an error." defaultButton:@"Quit" alternateButton:nil otherButton:nil informativeTextWithFormat:@"%s", msg] runModal];
}
