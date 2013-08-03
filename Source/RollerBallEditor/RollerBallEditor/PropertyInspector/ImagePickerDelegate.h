//
//  ImagePickerDelegate.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 01/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifndef IMAGESELECTEDBLOCK_TYPE
#define IMAGESELECTEDBLOCK_TYPE
typedef void (^ImageSelectedBlock)(NSImage* image);
#endif

@interface ImagePickerDelegate : NSObject
@property (weak) IBOutlet NSBox *host;
@property (weak) IBOutlet NSPopover *popover;
@property (strong) NSMutableArray* images;
@property (readonly) NSUInteger selectedIndex;
- (void)showRelativeToRect:(NSRect)positioningRect ofView:(NSView *)positioningView preferredEdge:(NSRectEdge)preferredEdge selectedImage: (NSImage*)img block:(ImageSelectedBlock) blck;
-(NSUInteger)countOfImages;
-(id)objectInImagesAtIndex:(NSUInteger)index;
-(NSArray *)imagesAtIndexes:(NSIndexSet *)indexes;
-(void)insertObject:(NSImage *)object inImagesAtIndex:(NSUInteger)index;
-(void)insertImages:(NSArray *)array atIndexes:(NSIndexSet *)indexes;
-(void)removeObjectFromImagesAtIndex:(NSUInteger)index;
-(void)removeImagesAtIndexes:(NSIndexSet *)indexes;
@property (weak) IBOutlet NSArrayController *arrayController;
@end
