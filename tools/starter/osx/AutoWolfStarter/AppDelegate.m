//
//  AppDelegate.m
//  AutoWolfStarter
//
//  Created by ioan on 11.05.2013.
//  Copyright (c) 2013 ichera. All rights reserved.
//

#include <wordexp.h>
#import "AppDelegate.h"

@implementation AppDelegate

- (id)init
{
    self = super.init;
    if(self)
    {
        _locationArray = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)dealloc
{
    [_locationArray release];
    [_task release];
    [super dealloc];
}

- (void)saveConfigs
{
    NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
    
    [defaults setObject:_parametersTextField.stringValue forKey:_parametersTextField.identifier];
    [defaults setObject:_locationArray forKey:_locationTableView.identifier];
}

- (void)controlTextDidChange:(NSNotification *)aNotification;
{
    [NSUserDefaults.standardUserDefaults setObject:[aNotification.object stringValue] forKey:[aNotification.object identifier]];
}

- (BOOL)loadConfigs
{
    NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
    @try
    {
        _parametersTextField.stringValue = [defaults stringForKey:_parametersTextField.identifier];
        [_locationArray setArray:[defaults arrayForKey:_locationTableView.identifier]];
        return YES;
    }
    @catch (...)
    {
        [self saveConfigs];
    }
    return NO;
}

- (void)awakeFromNib
{
    if([self loadConfigs])
        [_locationTableView reloadData];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (void)windowWillClose:(NSNotification *)notification
{
    if (notification.object == self.window)
    {
        [self saveConfigs];
        [NSUserDefaults.standardUserDefaults synchronize];
        [NSApp terminate:notification.object];
    }
}

-(IBAction)addLocation:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    panel.canChooseFiles = NO;
    panel.canChooseDirectories = YES;
    panel.resolvesAliases = YES;
    panel.allowsMultipleSelection = YES;
    
    [panel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result)
     {
         if(result == NSFileHandlingPanelOKButton)
         {
             for (NSURL *url in panel.URLs)
                 [_locationArray addObject:url.path];

             [_locationTableView reloadData];
             [NSUserDefaults.standardUserDefaults setObject:_locationArray forKey:_locationTableView.identifier];
#if 0
             for(NSURL *url in panel.URLs)
             {
                 [_locationSet addObject:url];
                 [NSDocumentController.sharedDocumentController noteNewRecentDocumentURL:url];
                 [_locationTableView reloadData];
             }
#endif
         }
     }];
}

-(IBAction)removeLocation:(id)sender
{
    @try
    {
        if (_locationArray.count == 0 || _locationTableView.selectedRow == -1)
        {
            NSBeep();
        }
        else
        {
            [_locationArray removeObjectAtIndex:_locationTableView.selectedRow];
            [_locationTableView reloadData];
            [NSUserDefaults.standardUserDefaults setObject:_locationArray forKey:_locationTableView.identifier];
        }
    }
    @catch(...)
    {
        NSBeep();
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    return _locationArray.count;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    return [_locationArray objectAtIndex:rowIndex];
}

-(IBAction)startAutoWolf:(id)sender
{
//    if(_task.isRunning)
//    {
//        NSBeep();
//        return;
//    }
//    [_task release];
    [self saveConfigs];
    [NSUserDefaults.standardUserDefaults synchronize];
    _task = NSTask.alloc.init;
    NSBundle *engineBundle = [NSBundle bundleWithPath:[NSBundle.mainBundle pathForResource:@"AutoWolf.app" ofType:nil]];
    
    if(_locationArray.count > 0 && _locationTableView.selectedRow != -1)
        [_task setEnvironment:@{@"AUTOWOLFDIR":[_locationArray objectAtIndex:_locationTableView.selectedRow]}];

    _task.launchPath = engineBundle.executablePath;
    if (_parametersTextField.stringValue.length > 0)
    {
        wordexp_t we;
        memset(&we, 0, sizeof(we));
        wordexp([_parametersTextField.stringValue cStringUsingEncoding:NSUTF8StringEncoding], &we, 0);
        NSMutableArray *argumentArray = [NSMutableArray.alloc initWithCapacity:we.we_wordc];
        for (NSUInteger i = 0; i < we.we_wordc; ++i)
            [argumentArray addObject:[NSString stringWithCString:we.we_wordv[i] encoding:NSUTF8StringEncoding]];
            
        wordfree(&we);
        
        _task.arguments = argumentArray;
        [argumentArray release];
    }
    [_task launch];
}

@end
