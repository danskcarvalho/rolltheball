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

using namespace rb;


@interface DSViewController () {
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
- (void)setupScene;
@end

static rb_string _current_level;

@implementation DSViewController

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
    
    [self setupScene];
}

- (void)setupScene
{
    ui_controller::set_intro(true);
    ui_controller::set_playing(false);
    ui_controller::set_tutorial(false);
    ui_controller::set_force_load_level(false);
    director::in_editor(false);
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

- (void)update
{
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
    director::active_responder()->update();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    [EAGLContext setCurrentContext:self.context];
    assert(glGetError() == GL_NO_ERROR);
    director::active_responder()->render();
}

@end
