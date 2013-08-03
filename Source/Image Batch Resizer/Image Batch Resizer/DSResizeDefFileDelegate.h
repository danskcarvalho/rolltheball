//
//  DSResizeDefFileDelegate.h
//  Image Batch Resizer
//
//  Created by Danilo Carvalho on 26/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol DSResizeDefFileDelegate <NSObject>
    @required
    -(void)beforeProcessingFile:(NSString*)fileName withResolution:(CGSize)resolution
        withIndex:(int)index outOf:(int)total;
    -(void)afterProcessingFile:(NSString*)fileName withResolution:(CGSize)resolution
                      withIndex:(int)index outOf:(int)total;
@end
