//
//  AppDelegate.m
//  AutoWolfStarter
//
//  Created by ioan on 11.05.2013.
//  Copyright (c) 2013 ichera. All rights reserved.
//

#import "AppDelegate.h"
#define GET_IDENTIFIER(CTL) \
[CTL respondsToSelector:@selector(identifier)] ? [CTL performSelector:@selector(identifier)] : @#CTL
#define UPDATE_DEFAULTS_FOR(GENERIC, CLASS) \
if ((GENERIC) == (CLASS)) \
    [[NSUserDefaults standardUserDefaults] setObject:[GENERIC stringValue] forKey:GET_IDENTIFIER(CLASS)]


@implementation AppDelegate
@synthesize window = _window;
@synthesize locationTableView = _locationTableView;
@synthesize parametersTextField = _parametersTextField;
@synthesize locationArray = _locationArray;
@synthesize task = _task;

//
// init
//
- (id)init
{
    self = super.init;
    if(self)
    {
        _locationArray = [[NSMutableArray alloc] init];
    }
    return self;
}

//
// dealloc
//
- (void)dealloc
{
    [_locationArray release];
    [_task release];
    [super dealloc];
}

//
// saveConfigs
//
- (void)saveConfigs
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    [defaults setObject:[_parametersTextField stringValue] forKey:GET_IDENTIFIER(_parametersTextField)];
    [defaults setObject:_locationArray forKey:GET_IDENTIFIER(_locationArray)];
}

//
// controlTextDidChange:
//
- (void)controlTextDidChange:(NSNotification *)aNotification;
{
    id obj = [aNotification object];
    
    UPDATE_DEFAULTS_FOR(obj, _parametersTextField);
}

//
// loadConfigs
//
- (BOOL)loadConfigs
{
    NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
    @try
    {
        [_parametersTextField setStringValue:[defaults stringForKey:GET_IDENTIFIER(_parametersTextField)]];
        [_locationArray setArray:[defaults arrayForKey:GET_IDENTIFIER(_locationTableView)]];
        return YES;
    }
    @catch (...)
    {
        // an error occurred, most likely the data is corrupted so write empty it
        [self saveConfigs];
    }
    return NO;
}

//
// awakeFromNib
//
- (void)awakeFromNib
{
    if([self loadConfigs])
        [_locationTableView reloadData];
}

//
// windowWillClose:
//
- (void)windowWillClose:(NSNotification *)notification
{
    if ([notification object] == [self window])
    {
        [self saveConfigs];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [NSApp terminate:[notification object]];
    }
}

//
// addLocation
//
-(IBAction)addLocation:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    [panel setCanChooseFiles:NO];
    [panel setCanChooseDirectories:YES];
    [panel setResolvesAliases:YES];
    [panel setAllowsMultipleSelection:YES];
    
    [panel beginSheetModalForWindow:[self window] completionHandler:^(NSInteger result)
     {
         if(result == NSFileHandlingPanelOKButton)
         {
             for (NSURL *url in panel.URLs)
                 [_locationArray addObject:url.path];

             [_locationTableView reloadData];
             [[NSUserDefaults standardUserDefaults] setObject:_locationArray forKey:GET_IDENTIFIER(_locationTableView)];
         }
     }];
}

//
// removeLocation:
//
-(IBAction)removeLocation:(id)sender
{
    if ([_locationArray count] == 0 || [_locationTableView selectedRow] == -1)
    {
        NSBeep();
    }
    else
    {
        @try
        {

            [_locationArray removeObjectAtIndex:_locationTableView.selectedRow];
            [_locationTableView reloadData];
            [[NSUserDefaults standardUserDefaults] setObject:_locationArray forKey:GET_IDENTIFIER(_locationTableView)];
        }
        @catch(...)
        {
            NSBeep();
        }
    }
}

//
// numberOfRowsInTableView:
//
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    return [_locationArray count];
}

//
// tableView:objectValueForTableColumn:row:
//
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    return [_locationArray objectAtIndex:rowIndex];
}

//
// startAutoWolf:
//
-(IBAction)startAutoWolf:(id)sender
{
    [self saveConfigs];
    [[NSUserDefaults standardUserDefaults] synchronize];
    _task = [[NSTask alloc] init];
    NSBundle *engineBundle = [NSBundle bundleWithPath:[[NSBundle mainBundle] pathForResource:@"AutoWolf.app" ofType:nil]];
    
    if([_locationArray count] > 0 && [_locationTableView selectedRow] != -1)
        [_task setEnvironment:[NSDictionary dictionaryWithObject:[_locationArray objectAtIndex:[_locationTableView selectedRow]] forKey:@"AUTOWOLFDIR"]];
    
    [_task setLaunchPath:[engineBundle executablePath]];
    if ([[_parametersTextField stringValue] length] > 0)
    {
        [_task setArguments:[[_parametersTextField stringValue] componentsSeparatedByString:@" "]];
    }
    [_task launch];
}

@end
