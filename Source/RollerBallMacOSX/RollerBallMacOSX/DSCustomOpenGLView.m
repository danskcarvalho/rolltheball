//
//  DSCustomOpenGLView.m
//  RollerBallMacOSX
//
//  Created by Danilo Carvalho on 31/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#import "DSCustomOpenGLView.h"
#include "components_external.h"

using namespace rb;

@interface DSCustomOpenGLView () {
    apple_program_manager* _program_manager;
    basic_process* _basic_process;
    basic_process* _basic_process2;
    mesh* _mesh01;
    mesh* _mesh02;
    mesh* _mesh03;
    dynamic_mesh_batch* _d_batch;
    dynamic_mesh_batch* _d_batch2;
    texture_atlas* _atlas;
}
- (void)setupGL;
- (void)tearDownGL;
@end


@implementation DSCustomOpenGLView

- (void)tearDownGL
{
    [[self openGLContext] makeCurrentContext];
    
    delete _atlas;
    delete _d_batch;
    delete _mesh01;
    delete _mesh02;
    delete _mesh03;
    delete _basic_process;
    delete _program_manager;
}

-(void)setupGL
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DITHER);
    NSSize bounds = [self convertSizeToBacking: [self bounds].size];
    glViewport(0, 0, bounds.width, bounds.height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    //NSURL* _auto_1_url = [[NSBundle mainBundle] URLForResource:@"Auto-1" withExtension:@"png"];
    NSURL* _ground_texture_url = [[NSBundle mainBundle] URLForResource:@"Stone-Ground" withExtension:@"png"];
    NSURL* _brick_url = [[NSBundle mainBundle] URLForResource:@"Bricks-4" withExtension:@"png"];
    NSURL* _track_url = [[NSBundle mainBundle] URLForResource:@"Track" withExtension:@"png"];
    NSURL* _mountain_url = [[NSBundle mainBundle] URLForResource:@"Mountain-2" withExtension:@"png"];
    NSURL* _mountain2_url = [[NSBundle mainBundle] URLForResource:@"Mountain-6" withExtension:@"png"];
    //cg_texture_source _texture_source = cg_texture_source(_auto_1_url);
    cg_texture_source _ground_source = cg_texture_source(_ground_texture_url);
    cg_texture_source _brick_source = cg_texture_source(_brick_url);
    cg_texture_source _track_source = cg_texture_source(_track_url);
    cg_texture_source _mountain_source = cg_texture_source(_mountain_url);
    cg_texture_source _mountain2_source = cg_texture_source(_mountain2_url);
    
    _atlas = new texture_atlas(false);
    std::vector<rb_string> _groups = {{u"one_group"}};
    //_atlas->add_texture(u"Auto", _groups, _texture_source, texture_border::empty);
    _atlas->add_texture(u"StoneGround", _groups, _ground_source, texture_border::repeat);
    _atlas->add_texture(u"Brick", _groups, _brick_source, texture_border::repeat);
    _atlas->add_texture(u"Track", _groups, _track_source, texture_border::repeat);
    _atlas->add_texture(u"Mountain", _groups, _mountain_source);
    _atlas->add_texture(u"Mountain2", _groups, _mountain2_source);
    _atlas->compile();
    
    std::vector<texture*> _textures;
    _atlas->get_atlas(u"one_group", _textures);
    
    vec2 _t_size = _textures[0]->texture_size();
    
    rectangle r(0, 0, 2, 2);
    std::vector<vec2> _quad_points = {{-1, -1}, {1, -1}, {1, 1}, {0, 0}, {-1, 1}};
    _mesh01 = new mesh();
    _mesh02 = new mesh();
    polygon _polygon;
    smooth_curve _curve;
    smooth_curve::build_closed_curve(_quad_points, _curve);
    _curve.to_polygon(_polygon, 0.01f);
    //polygon::build_closed_polygon(_quad_points, _polygon);
    matrix3x3::build_scale(0.3, 0.3).transform_polygon(_polygon);
    polygon _connector;
    //_polygon.outline(_connector, 0.1, corner_type::bevel);
    transform_space _t_transform = transform_space(vec2::zero, 0.2);
    auto _mapping = _atlas->create_mapping(u"one_group", u"Track", _t_transform);
    //_polygon.to_mesh(*_mesh01, 0, *_mapping);
    //_polygon.to_outline_mesh(*_mesh01, *_mapping, 0.1, corner_type::miter);
    _polygon.textured_outline(*_mesh01, _mapping->bounds(), 10, 0.1 / 5);
    matrix3x3::build_scale(3, 3).transform_mesh(*_mesh01);
    _mesh01->set_color(color::from_rgba(1, 1, 1, 1));
    _mesh01->to_line_mesh(*_mesh02);
    //matrix3x3::build_scale(3, 3).transform_mesh(*_mesh02);
    _mesh02->set_color(color::from_rgba(0, 1, 0, 0));
    _mesh02->set_blend(0);
//    if(!_connector.is_empty())
//        _connector.to_mesh(*_mesh02, 0, *_mapping);
    
    _program_manager = new apple_program_manager();
    _basic_process = new basic_process(_program_manager);
    _basic_process->texture_sampler(_textures[0]);
//    _basic_process2 = new basic_process(_program_manager);
//    _basic_process2->texture_sampler(_textures[0]);
    
    _d_batch = new dynamic_mesh_batch();
    _d_batch->add_mesh(_mesh01);
    //if(!_connector.is_empty())
    //    _d_batch->add_mesh(_mesh02);
    _d_batch->process(_basic_process);
    
    _d_batch2 = new dynamic_mesh_batch();
    _d_batch2->add_mesh(_mesh02);
    //if(!_connector.is_empty())
    //    _d_batch->add_mesh(_mesh02);
    _d_batch2->process(_basic_process);
    _d_batch2->geometry_type(geometry_type::line);
    _d_batch2->line_width(1);
    
    
    
    assert(glGetError() == GL_NO_ERROR);
}

-(BOOL)windowShouldClose:(id)sender{
    [[NSApplication sharedApplication] terminate:self];
    return YES;
}

-(void)drawFrame{
    glClear(GL_COLOR_BUFFER_BIT);
    assert(glGetError() == GL_NO_ERROR);
    //glLineWidth(1.0);
    _d_batch->draw();
    _d_batch2->draw();
   
    [[self openGLContext] flushBuffer];
}

-(void)prepareOpenGL{
    GLint swap = 1;
    [[self openGLContext] setValues:&swap forParameter:NSOpenGLCPSwapInterval];
    _timer = [NSTimer scheduledTimerWithTimeInterval:DESIRED_FPS target:self selector:@selector(updateGameLogic) userInfo:nil repeats:YES];
    _fullScreen = NO;
    [self setupGL];
}

-(void)drawRect:(NSRect)dirtyRect{
    [super drawRect:dirtyRect];
    [self drawFrame];
}

-(void)updateGameLogic{
    //update game logic here...
    //_ua += 0.001f;
    //*_d_mesh = *_d_mesh_2;
    //matrix3x3::build_translation(_curve.sample_along_path(_ua)).transform_mesh(*_d_mesh);
    
    [self drawFrame];
}

-(void)reshape{
    [super reshape];
    NSSize bounds = [self convertSizeToBacking: [self bounds].size];
    glViewport(0, 0, bounds.width, bounds.height);
}

-(void)awakeFromNib{
    [super awakeFromNib];
    [[self window] makeFirstResponder:self];
}

-(void)keyDown:(NSEvent *)theEvent{
    [super keyDown:theEvent];
}

-(void)cancelOperation:(id)sender{
    [self setFullScreen];
}

-(BOOL)acceptsFirstResponder{
    return YES;
}

-(void)setFullScreen{
    CGDirectDisplayID mainDisplay = CGMainDisplayID();
    if(!_fullScreen){
        CGDisplayCapture(mainDisplay);
        NSRect screenSize = [[NSScreen mainScreen] frame];
        NSWindow* window = [self window];
        [window setStyleMask:NSBorderlessWindowMask];
        _previousFrame = window.frame;
        int windowLevel = CGShieldingWindowLevel();
        [window setFrame:CGRectMake(0, 0, screenSize.size.width, screenSize.size.height) display:YES];
        [window setLevel:windowLevel];
        [window setHidesOnDeactivate:YES];
        [window makeKeyAndOrderFront:self];
        [[self window] makeFirstResponder:self];
        [NSCursor hide];
        _fullScreen = YES;
        
    }
    else {
        CGDisplayRelease(mainDisplay);
        NSWindow* window = [self window];
        [window setFrame:_previousFrame display:YES];
        [window setStyleMask: NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask];
        [window setLevel:0];
        [window setHidesOnDeactivate:NO];
        [window makeKeyAndOrderFront:self];
        [[self window] makeFirstResponder:self];
        [NSCursor unhide];
        _fullScreen = NO;
    }
}

-(void)dealloc{
    [_timer invalidate];
    [self tearDownGL];
}
@end
