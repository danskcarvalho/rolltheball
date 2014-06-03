//
//  DSViewController.m
//  RollerBalliOS
//
//  Created by Danilo Carvalho on 30/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSViewController.h"
#include "components_external.h"
#include "ui_controller.h"
#include "sound_player.h"
#include "game_saver.h"

using namespace rb;

typedef enum : NSUInteger {
    kBuy,
    kRestore,
    kCancel,
} BuyEnum;


@interface DSViewController () {
    BOOL _reportingScores;
    BOOL _buying;
    size_t _scoreIndex;
    void(^_response)(BOOL);
    void(^_responseBuy)(BuyEnum);
    UIAlertView* _ask;
    UIAlertView* _askBuy;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
- (void)setupScene;
- (void)changeScene;
- (void)reportScores;
- (void)showLeaderboard:(size_t)set;
- (void)maybeShowLeaderboard;
- (void)loadHearts;
- (void)ask:(NSString*)msg title:(NSString*)title response:(void(^)(BOOL))response;
- (void)alert:(NSString*)msg title:(NSString*)title;
- (void)buySet2;
- (void)buy15Hearts;
- (void)askBuy:(NSString *)msg title:(NSString *)title response:(void (^)(BuyEnum))response;
@end

static rb_string _current_level;

@implementation DSViewController

-(void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions{
    for (SKPaymentTransaction *transaction in transactions) {
        if([transaction.payment.productIdentifier isEqualToString:@"GRL_SET2LEVELS"]){
            switch (transaction.transactionState) {
                    // Call the appropriate custom method.
                case SKPaymentTransactionStatePurchased:
                case SKPaymentTransactionStateRestored:
                    ui_controller::unlock_set2();
                    saved_data_v1 sd;
                    game_saver::load_saved(&sd);
                    sd.set2Availability = 1;
                    game_saver::save(&sd);
                    ui_controller::buy_set2(false);
                    _buying = NO;
                    if(transaction.transactionState == SKPaymentTransactionStatePurchased)
                        [self alert:@"Congratulations! Now you can play the Set 2 Levels!" title:@"Congratulations!"];
                    else
                        [self alert:@"Congratulations! You restored the Set 2 Levels!" title:@"Congratulations!"];
                    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                    break;
                case SKPaymentTransactionStateFailed:
                    [self alert:@"Failed to buy the Set 2 Levels." title:@"Failure"];
                    ui_controller::buy_set2(false);
                    _buying = NO;
                    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                    break;
                default:
                    break;
            }
        }
        else{
            switch (transaction.transactionState) {
                    // Call the appropriate custom method.
                case SKPaymentTransactionStatePurchased:
                case SKPaymentTransactionStateRestored:
                    saved_data_v1 sd;
                    game_saver::load_saved(&sd);
                    sd.hearts += 15;
                    game_saver::save(&sd);
                    ui_controller::buy_hearts(false);
                    ui_controller::hearts(ui_controller::hearts() + 15);
                    _buying = NO;
                    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                    if(transaction.transactionState == SKPaymentTransactionStatePurchased)
                        [self alert:@"Congratulations! You bought 15 Hearts!" title:@"Congratulations!"];
                    break;
                case SKPaymentTransactionStateFailed:
                    [self alert:@"Failed to buy 15 Hearts." title:@"Failure"];
                    ui_controller::buy_hearts(false);
                    _buying = NO;
                    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                    break;
                default:
                    break;
            }
        }
    }
}

-(void)buySet2{
    if(_buying)
        return;
    if(![SKPaymentQueue canMakePayments]){
        [self alert:@"Failed to buy the Set 2 Levels." title:@"Failure"];
        ui_controller::buy_set2(false);
        _buying = NO;
        return;
    }
    _buying = YES;
    [self askBuy:@"Would you like to buy the Set 2 Levels for just $0.99? You can also restore (for free) the Set 2 Levels if you bought it before." title:@"Buy Set 2 Levels?" response:^void(BuyEnum response) {
        if(response == kCancel)
        {
            ui_controller::buy_set2(false);
            _buying = NO;
            return;
        }
        if(response == kRestore){
            [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
            return;
        }
        SKProduct* set2Prod = nil;
        for(SKProduct* prod in self.storeProducts){
            if([prod.productIdentifier isEqualToString:@"GRL_SET2LEVELS"]){
                set2Prod = prod;
                break;
            }
        }
        
        
        if(set2Prod == nil){
            ui_controller::buy_set2(false);
            _buying = NO;
            return;
        }
        SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:set2Prod];
        payment.quantity = 1;
        
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }];
}

-(void)buy15Hearts{
    if(_buying)
        return;
    if(![SKPaymentQueue canMakePayments]){
        [self alert:@"Failed to buy the 15 Hearts." title:@"Failure"];
        ui_controller::buy_hearts(false);
        _buying = NO;
        return;
    }
    _buying = YES;
    [self ask:@"Would you like to buy 15 Hearts for just $0.99?" title:@"Buy 15 Hearts Levels?" response:^void(BOOL response) {
        if(!response)
        {
            ui_controller::buy_hearts(false);
            _buying = NO;
            return;
        }
        SKProduct* _15heartsProd = nil;
        for(SKProduct* prod in self.storeProducts){
            if([prod.productIdentifier isEqualToString:@"GRL_15HEARTS"]){
                _15heartsProd = prod;
                break;
            }
        }
        
        
        if(_15heartsProd == nil){
            ui_controller::buy_hearts(false);
            _buying = NO;
            return;
        }
        SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:_15heartsProd];
        payment.quantity = 1;
        
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }];
}

-(void)ask:(NSString *)msg title:(NSString *)title response:(void (^)(BOOL))response{
    UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:title
                                                        message:msg
                                                       delegate:self
                                              cancelButtonTitle:@"No"
                                              otherButtonTitles:@"Yes", nil];
    _response = response;
    _ask = alertView;
    _askBuy = nil;
    [alertView show];
}

-(void)askBuy:(NSString *)msg title:(NSString *)title response:(void (^)(BuyEnum))response{
    UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:title
                                                        message:msg
                                                       delegate:self
                                              cancelButtonTitle:@"Cancel"
                                              otherButtonTitles:@"Buy", @"Restore", nil];
    _responseBuy = response;
    _ask = nil;
    _askBuy = alertView;
    [alertView show];
}

-(void)alert:(NSString *)msg title:(NSString *)title{
    UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:title
                                                        message:msg
                                                       delegate:self
                                              cancelButtonTitle:@"Ok"
                                              otherButtonTitles:nil];
    _ask = nil;
    _askBuy = nil;
    [alertView show];
}

-(void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex{
    if(_ask == nil && _askBuy == nil)
        return;
    if(_ask != nil){
        if(buttonIndex == 1)
            _response(YES);
        else
            _response(NO);
    }
    else {
        if(buttonIndex == 1)
            _responseBuy(kBuy);
        else if(buttonIndex == 2)
            _responseBuy(kRestore);
        else
            _responseBuy(kCancel);
    }
    _ask = nil;
    _askBuy = nil;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormatNone;
    view.multipleTouchEnabled = YES;
    self.preferredFramesPerSecond = 1.0 / DESIRED_FPS;
    
    _reportingScores = NO;
    [self setupGL];
}

- (void)viewDidUnload
{    
    [super viewDidUnload];
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
	self.context = nil;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    if(UIInterfaceOrientationIsLandscape(interfaceOrientation))
        return YES;
    else
        return NO;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DITHER);
    GLsizei _s_width = [[UIScreen mainScreen] bounds].size.width * [[UIScreen mainScreen] scale];
    GLsizei _s_height = [[UIScreen mainScreen] bounds].size.height * [[UIScreen mainScreen] scale];
    
    if(_s_height > _s_width)
        std::swap(_s_height, _s_width);
    glViewport(0, 0, _s_width, _s_height);
    
    [self loadHearts];
    _buying = NO;
    _ask = nil;
    _askBuy = nil;
    [self setupScene];
}

- (void)loadHearts{
    saved_data_v1 sd;
    game_saver::load_saved(&sd);
    ui_controller::hearts(sd.hearts);
    if(sd.set2Availability == 1)
        ui_controller::unlock_set2();
}

- (void)setupScene
{
    sound_player::preload_effects();
    ui_controller::set_intro(true);
    ui_controller::set_playing(false);
    ui_controller::set_tutorial(false);
    ui_controller::set_force_load_level(false);
    director::in_editor(false);
    sound_player::play_background();
    director::active_scene(nullptr, true);
    auto _scene = scene_loader::load(u"Intro");
    director::active_scene(_scene, true);
    director::active_scene()->playing(true);
    director::active_scene()->camera(transform_space(vec2::zero, 6));
    _current_level = u"Intro";
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)changeScene{
    nullable<rb_string> _changeLevel = nullptr;
    if(ui_controller::get_level() == nullptr && _current_level != u"Intro"){ //return to intro
        _changeLevel = u"Intro";
    }
    else if(ui_controller::get_level() != nullptr && ui_controller::get_level() != _current_level){
        _changeLevel = ui_controller::get_level();
    }
    else if(ui_controller::is_force_load_level() && ui_controller::get_level() != nullptr)
        _changeLevel = ui_controller::get_level();
    else if(ui_controller::is_force_load_level() && ui_controller::get_level() == nullptr)
        _changeLevel = u"Intro";
    
    if(_changeLevel.has_value()){
        if(_changeLevel.value() == u"Intro"){
            auto _fd_color = director::active_scene()->fade_color();
            director::active_scene()->playing(false);
            [self setupScene];
            director::active_scene()->fade_color(_fd_color);
        }
        else {
            auto _fd_color = director::active_scene()->fade_color();
            director::active_scene()->playing(false);
            director::active_scene(nullptr, true);
            auto _scene = scene_loader::load(_changeLevel.value());
            director::active_scene(_scene, true);
            director::active_scene()->playing(true);
            director::active_scene()->fade_color(_fd_color);
            _current_level = ui_controller::get_level().value();
            ui_controller::set_intro(false);
            ui_controller::set_force_load_level(false);
        }
    }
}

-(void)showLeaderboard:(size_t)set{
    if(![GKLocalPlayer localPlayer].authenticated)
        return;
    GKGameCenterViewController *gcViewController = [[GKGameCenterViewController alloc] init];
    
    gcViewController.gameCenterDelegate = self;
    
    gcViewController.viewState = GKGameCenterViewControllerStateLeaderboards;
    gcViewController.leaderboardIdentifier = set == 1 ? @"GRL_SET1_LEADERBOARD" : @"GRL_SET2_LEADERBOARD";
    
    [self presentViewController:gcViewController animated:YES completion:nil];
}

-(void)maybeShowLeaderboard{
    if(ui_controller::leaderboard_to_show() != 0)
    {
        [self showLeaderboard:ui_controller::leaderboard_to_show()];
        ui_controller::leaderboard_to_show() = 0;
    }
}

-(void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController{
    [gameCenterViewController dismissViewControllerAnimated:YES completion:nil];
}

-(void)reportScores{
    if(_reportingScores)
        return;
    
    if(![GKLocalPlayer localPlayer].authenticated)
        return;
    
    auto& _scores = ui_controller::scores();
    if(_scores.size() == 0)
        return;
    
    _reportingScores = YES;
    auto _sc = _scores.back();
    _scoreIndex = _scores.size() - 1;
    GKScore* score = [[GKScore alloc] initWithLeaderboardIdentifier: _sc.set == 1 ? @"GRL_SET1_LEADERBOARD" : @"GRL_SET2_LEADERBOARD"];
    score.value = _sc.value;
    [GKScore reportScores:@[score] withCompletionHandler:^(NSError *error) {
        _reportingScores = NO;
        if (error != nil) {
            NSLog(@"%@", [error localizedDescription]);
        }
        if(error == nil){
            _scores.erase(_scores.begin() + _scoreIndex);
        }
    }];
}

- (void)update
{
    if(ui_controller::buy_hearts())
        [self buy15Hearts];
    else if(ui_controller::buy_set2())
        [self buySet2];
    [self changeScene];
    [self reportScores];
    [self maybeShowLeaderboard];
    director::active_responder()->update();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    [EAGLContext setCurrentContext:self.context];
    assert(glGetError() == GL_NO_ERROR);
    director::active_responder()->render();
}

@end
