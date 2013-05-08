//
//  CocoaFun.m
//  Wolf4SDL
//
//  Created by ioan on 03.03.2013.
//
//

// Geared towards OS X 10.6 and later

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
    NSFileManager* sharedFM = [NSFileManager defaultManager];
    NSArray* possibleURLs = [sharedFM URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];
    NSURL* appSupportDir = nil;
    NSURL* appDirectory = nil;
    
    if ([possibleURLs count] >= 1)
    {
        // Use the first directory (if multiple are returned)
        appSupportDir = [possibleURLs objectAtIndex:0];
    }
    
    // If a valid app support directory exists, add the
    // app's bundle ID to it to specify the final directory.
    if (appSupportDir)
    {
        NSString* appBundleID = APPLICATION_UTI;
        appDirectory = [appSupportDir URLByAppendingPathComponent:appBundleID];
    }
    
    const char *res = [[appDirectory path] cStringUsingEncoding:NSUTF8StringEncoding];
    
    return res;
}