//
//  DSAppDelegate.m
//  RollerBalliOS
//
//  Created by Danilo Carvalho on 30/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSAppDelegate.h"

#import "DSViewController.h"
#include "components_external.h"
#include "ui_controller.h"
#include "director.h"
#include "ui_component.h"
#import <GameKit/GameKit.h>

using namespace rb;

@implementation DSAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    self.viewController = [[DSViewController alloc] initWithNibName:@"DSViewController" bundle:nil];
    self.viewController.storeProducts = [NSArray array];
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error){
        if(viewController != nil)
            [self.viewController presentViewController:viewController animated:YES completion:nil];
        else {
            //do nothing...
        }
    };
    //store
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self.viewController];
    SKProductsRequest *productsRequest = [[SKProductsRequest alloc]
                                          initWithProductIdentifiers:[NSSet setWithArray:@[@"GRL_15HEARTS", @"GRL_SET2LEVELS"]]];
    productsRequest.delegate = self;
    [productsRequest start];
    return YES;
}

-(void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response{
    self.viewController.storeProducts = response.products;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    if(ui_controller::is_intro())
        return;
    auto _scene = director::active_scene();
    auto _ui = dynamic_cast<ui_component*>(_scene->node_with_name(u"ui"));
    _ui->play_pause_clicked();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
