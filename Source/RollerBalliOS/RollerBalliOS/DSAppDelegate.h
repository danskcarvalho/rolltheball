//
//  DSAppDelegate.h
//  RollerBalliOS
//
//  Created by Danilo Carvalho on 30/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

@class DSViewController;

@interface DSAppDelegate : UIResponder <UIApplicationDelegate, SKProductsRequestDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) DSViewController *viewController;

@end
