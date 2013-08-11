//
//  components_constants.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 20/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef RollerBallCore_components_constants_h
#define RollerBallCore_components_constants_h

//Layer Effects
#define EFFECT_TEXTURED_COUNT                    2
#define EFFECT_TEXTURED                          0
#define EFFECT_TEXTURED_WITH_WRAPPING            1

//Scene Constants
#define MAX_LAYERS 10
#define HIDDEN_LAYER MAX_LAYERS
#define DESIRED_FPS (1.0 / 30.0)

//keys
#define KEY_ESCAPE 53
#define KEY_DOWN 125
#define KEY_UP 126
#define KEY_LEFT 123
#define KEY_RIGHT 124
#define KEY_DELETE 51

//multipliers
#define ROTATION_MULTIPLIER 1
#define SCALE_MULTIPLIER 1
#define SCROLL_MULTIPLIER 1
#define MOVE_MULTIPLIER 1

//camera zoom
#define MIN_CAMERA_ZOOM 0.01
#define ROTATION_SCROLL_MULTIPLIER (1.0f / 900.0f)
#define ZOOM_SCROLL_MULTIPLIER (1.0f / 900.0f)

//selection constants
#define NODE_SELECTION_SIZE 2
#define SCENE_SELECTION_SIZE 4
#define HANDLE_SIZE 30
#define POINT_SIZE 10
#define HANDLE_COUNT 3
#define HANDLE_STEM_SIZE 120

#endif
