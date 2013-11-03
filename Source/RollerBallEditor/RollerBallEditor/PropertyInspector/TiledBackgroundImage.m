//
//  TiledBackgroundImage.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 01/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "TiledBackgroundImage.h"

@interface TiledBackgroundImage ()
{
    CGImageRef backgroundImage;
    CGFloat fBorderWidth;
    NSColor* fBorderColor;
}
@end

@implementation TiledBackgroundImage
-(CGFloat)borderWidth{
    return fBorderWidth;
}
-(void)setBorderWidth:(CGFloat)borderWidth{
    fBorderWidth = borderWidth;
    [self setNeedsDisplay:YES];
}
-(NSColor *)borderColor{
    return fBorderColor;
}
-(void)setBorderColor:(NSColor *)borderColor{
    fBorderColor = borderColor;
    [self setNeedsDisplay:YES];
}
- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        fBorderWidth = 0;
        fBorderColor = [NSColor controlShadowColor];
        // Initialization code here.
        NSURL* url = [[NSBundle mainBundle] URLForResource:@"Transparent-Background" withExtension:@"jpg"];
        NSImage* img = [[NSImage alloc] initWithContentsOfURL:url];
        CGImageSourceRef source = CGImageSourceCreateWithData((__bridge CFDataRef)[img TIFFRepresentation], NULL);
        backgroundImage =  CGImageSourceCreateImageAtIndex(source, 0, NULL);
        CGImageRef subimage = CGImageCreateWithImageInRect(backgroundImage, CGRectMake(0, 0, CGImageGetWidth(backgroundImage), CGImageGetHeight(backgroundImage)));
        CGImageRelease(backgroundImage);
        backgroundImage = subimage;
        CFRelease(source);
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
    NSGraphicsContext* ctx = [NSGraphicsContext currentContext];
    CGContextRef ctxRef = (CGContextRef)[ctx graphicsPort];
    CGContextDrawTiledImage(ctxRef, NSMakeRect(0, 0, CGImageGetWidth(backgroundImage), CGImageGetHeight(backgroundImage)), backgroundImage);
    if(fBorderWidth > 0)
    {
        [fBorderColor set];
        NSFrameRectWithWidthUsingOperation([self bounds], fBorderWidth, NSCompositeCopy);
    }
}

-(void)dealloc{
    CGImageRelease(backgroundImage);
}

@end
