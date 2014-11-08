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

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#include "CocoaFun.h"

#define APPLICATION_UTI @"com.ichera.autowolf"

//
// Cocoa_CreateApplicationSupportPathString
//
// Returns the directory where to save configs
// Based on Apple documentation example
//
// IMPORTANT: you must free the return string when done with it!
//
char *Cocoa_CreateApplicationSupportPathString()
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
		if([paths count] <= 0)
		{
			char *ret = new char[2];
			ret[0] = '.';
			ret[1] = 0;
            
            [pool release];
			return ret;
		}
		
		NSString *applicationSupportDirectory = [paths objectAtIndex:0];
		const char* content = [[applicationSupportDirectory stringByAppendingPathComponent:APPLICATION_UTI] cStringUsingEncoding:NSUTF8StringEncoding];
		
		size_t len = strlen(content);
		char *ret = (char*)calloc(len, sizeof(char));
		memcpy(ret, content, len);
        
        [pool release];
		return ret;
	}
//    [pool release];
}

//
// Cocoa_DisplayErrorAlert
//
void Cocoa_DisplayErrorAlert(const char *msg, const char* title)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	{
#if TARGET_OS_IPHONE
        UIAlertView* av = [[UIAlertView alloc] initWithTitle:[NSString stringWithCString:title encoding:NSUTF8StringEncoding] message:[NSString stringWithCString:msg encoding:NSUTF8StringEncoding] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [av show];
#elif TARGET_OS_MAC
        [[NSAlert alertWithMessageText:[NSString stringWithFormat:@"%s", title] defaultButton:@"Quit" alternateButton:nil otherButton:nil informativeTextWithFormat:@"%s", msg] runModal];
#endif
	}
    [pool release];
}

//
// Cocoa_Notify
//
// Posts a notification, either to the OS X 10.8 notification centre, or by
// other means
//
void Cocoa_Notify(const char *title, const char *msg)
{
#if !TARGET_OS_IPHONE
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	{
        Class UserNotificationCenter = NSClassFromString(@"NSUserNotificationCenter");
		
		if(UserNotificationCenter)
		{
            Class UserNotification = NSClassFromString(@"NSUserNotification");
			id notification = [[[UserNotification alloc] init] autorelease];
			NSString *infoText = [[[NSString alloc] initWithUTF8String:msg] autorelease],
			*notifTitle = [[[NSString alloc] initWithUTF8String:title] autorelease];
			
			[notification setTitle:notifTitle];
			[notification setInformativeText:infoText];
			id notifCentre = [UserNotificationCenter defaultUserNotificationCenter];
			
			[notifCentre deliverNotification:notification];
		}
	}
    [pool release];
#endif
}