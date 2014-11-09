//
//  LauncherViewController.m
//  AutoWolf-iOS
//
//  Created by ioan on 09.11.2014.
//  Copyright (c) 2014 Ioan Chera. All rights reserved.
//

#include <SDL.h>
#import "LauncherViewController.h"

extern int SDL_main(int argc, char* argv[]);

@interface LauncherViewController ()

@end

@implementation LauncherViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    CGSize ss = {self.view.frame.size.width, self.view.frame.size.height};
    
//    UITextField* tf = [[[UITextField alloc] initWithFrame:CGRectMake(8, (ss.height - 44) / 2, ss.width - 16, 44)] autorelease];
//    [self.view addSubview:tf];

    UIButton* b = [UIButton buttonWithType:UIButtonTypeSystem];
    b.frame = CGRectMake(8, (ss.height - 44) / 2, ss.width - 16, 44);
    [b setTitle:@"Start!" forState:UIControlStateNormal];
    [b addTarget:self action:@selector(buttonClicked:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:b];
}

-(void)buttonClicked:(id)sender
{
    NSString* rp = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"wolftest"];
    char** argv = calloc(4, sizeof(char*));
    argv[0] = "AutoWolf";
    argv[1] = "--wolfdir";
    argv[2] = calloc(PATH_MAX, sizeof(char));
    [rp getCString:argv[2] maxLength:PATH_MAX - 1 encoding:NSUTF8StringEncoding];
    
    SDL_SetMainReady();
    SDL_main(3, argv);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
