//
//  AppDelegate.h
//  AutoWolfStarter
//
//  Created by ioan on 11.05.2013.
//  Copyright (c) 2013 ichera. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSTableView *locationTableView;
@property (assign) IBOutlet NSTextField *parametersTextField;

@property (readonly) NSMutableArray *locationArray;
@property (assign) NSTask *task;

-(IBAction)addLocation:(id)sender;
-(IBAction)removeLocation:(id)sender;
-(IBAction)startAutoWolf:(id)sender;

@end
