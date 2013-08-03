//
//  DSResizeDefFile.h
//  Image Batch Resizer
//
//  Created by Danilo Carvalho on 26/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DSResizeDefFileDelegate.h"

@interface DSResizeDefFile : NSObject < NSXMLParserDelegate >
@property(readonly, nonatomic) CGSize defaultBlockSize;
@property(readonly, nonatomic) CGSize defaultResolution;
@property(readonly, nonatomic) int resolutionCount;
@property(readonly, nonatomic, strong) NSURL* basePath;
@property(readonly, nonatomic, strong) NSURL* outputPath;

-(CGSize)getResolutionAtIndex:(int)index;

-(id)initWithXmlPath:(NSURL*)xmlPath basePath:(NSURL*)basePath
    outputPath: (NSURL*)outputPath;
-(BOOL)writeOutputPathWithDelegate:(id<DSResizeDefFileDelegate>)delegate;
@end
