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

#include "../version.h"

#ifdef OSX
#import <AppKit/AppKit.h>
#elif defined(IOS)
#import <UIKit/UIKit.h>
#include "wl_def.h"
#include "i_video.h"
#endif

#import <Foundation/Foundation.h>
#include "CocoaFun.h"

// Unfortunately frozen to this value. Normally I should name it "org.i-chera.autowolf", but I can't
// change the name now because it would stop reading old version preference states
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
        NSString* settingsDir = [applicationSupportDirectory stringByAppendingPathComponent:APPLICATION_UTI];
        [[NSFileManager defaultManager] createDirectoryAtPath:settingsDir withIntermediateDirectories:YES attributes:nil error:NULL];
		const char* content = [settingsDir cStringUsingEncoding:NSUTF8StringEncoding];
		
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
#ifdef IOS
        UIAlertView* av = [[UIAlertView alloc] initWithTitle:[NSString stringWithCString:title encoding:NSUTF8StringEncoding] message:[NSString stringWithCString:msg encoding:NSUTF8StringEncoding] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [av show];
#else
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:[NSString stringWithFormat:@"%s", title]];
        [alert setInformativeText:[NSString stringWithFormat:@"%s", msg]];
        [alert addButtonWithTitle:@"Quit"];
        [alert runModal];
        [alert release];
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
#ifdef OSX
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

#ifdef IOS
double Cocoa_StatusBarHeight()
{
    @autoreleasepool {
        CGSize sz = [UIApplication sharedApplication].statusBarFrame.size;
        return MIN(sz.width, sz.height);
    }
}
void Cocoa_HideStatusBar()
{
    @autoreleasepool {
        [UIApplication sharedApplication].statusBarHidden = YES;
    }
}
double Cocoa_PixelsPerDot()
{
    @autoreleasepool {
        return [UIScreen mainScreen].scale;
    }
}
void Cocoa_DisableIdleTimer()
{
    @autoreleasepool {
        [UIApplication sharedApplication].idleTimerDisabled = YES;
    }
}

// https://forums.libsdl.org/viewtopic.php?t=8682&sid=baac6ce2135c88065fa22f6d777b0ae3
void Cocoa_PutBackButton()
{
    @autoreleasepool {
        SDL_SysWMinfo systemWindowInfo;
        SDL_VERSION(&systemWindowInfo.version);
        if(!SDL_GetWindowWMInfo(vid_window, &systemWindowInfo))
        {
            return;
        }
        UIWindow* iosWindow = systemWindowInfo.info.uikit.window;
        UIViewController* viewController = iosWindow.rootViewController;
        if(!viewController)
        {
            NSLog(@"ERROR: no view controller");
            return;
        }
        UIButton* button = [UIButton buttonWithType:UIButtonTypeSystem];
        [button setTitle:@"Back" forState:UIControlStateNormal];
        [button sizeToFit];
        button.frame = CGRectMake(0, 0, 88, 44);
        button.backgroundColor = [UIColor colorWithWhite:1 alpha:0.5];
        [viewController.view addSubview:button];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [viewController.view bringSubviewToFront:button];
        });
    }
}
#endif