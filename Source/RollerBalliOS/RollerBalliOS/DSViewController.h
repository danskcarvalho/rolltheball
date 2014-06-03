//
//  DSViewController.h
//  RollerBalliOS
//
//  Created by Danilo Carvalho on 30/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <GameKit/GameKit.h> 
#import <StoreKit/StoreKit.h>

@interface DSViewController : GLKViewController <GKGameCenterControllerDelegate, UIAlertViewDelegate, SKPaymentTransactionObserver>
@property (strong, nonatomic) NSArray *storeProducts;
@end
