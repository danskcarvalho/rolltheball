//
//  ImagePicker.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 02/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "ImagePicker.h"
#import "ImagePickerDelegate.h"

@interface ImagePicker ()
{
    NSArray* topObjects;
    ImagePickerDelegate* delegate;
}
@end

@implementation ImagePicker
static ImagePicker* singleton;
-(ImagePickerDelegate*)getDelegate{
    return delegate;
}
-(id)init{
    NSArray* tempTopObjects;
    if(self = [super init]){
        [[NSBundle mainBundle] loadNibNamed:@"ImagePicker" owner:[NSApplication sharedApplication] topLevelObjects:&tempTopObjects];
        topObjects = [NSArray arrayWithArray:tempTopObjects];
        
        for (int i = 0; i < topObjects.count; i++) {
            if ([[topObjects objectAtIndex:i] class] == [ImagePickerDelegate class]) {
                delegate = [topObjects objectAtIndex:i];
                break;
            }
        }
    }
    return self;
}
+(void)initialize{
    if ([self class] == [ImagePicker class]) {
        singleton = [[ImagePicker alloc] init];
    }
}

+(void)showRelativeToRect:(NSRect)positioningRect ofView:(NSView *)positioningView preferredEdge:(NSRectEdge)preferredEdge selectedImage: (NSImage*)img block:(ImageSelectedBlock) blck{
    [[singleton getDelegate] showRelativeToRect:positioningRect ofView:positioningView preferredEdge:preferredEdge selectedImage:img block:blck];
}
+(NSUInteger)countOfImages{
    return [[singleton getDelegate] countOfImages];
}
+(id)objectInImagesAtIndex:(NSUInteger)index{
    return [[singleton getDelegate] objectInImagesAtIndex:index];
}
+(NSArray *)imagesAtIndexes:(NSIndexSet *)indexes{
    return [[singleton getDelegate] imagesAtIndexes:indexes];
}
+(void)insertObject:(NSImage *)object inImagesAtIndex:(NSUInteger)index{
    [[singleton getDelegate] insertObject:object inImagesAtIndex:index];
}
+(void)insertImages:(NSArray *)array atIndexes:(NSIndexSet *)indexes{
    [[singleton getDelegate] insertImages:array atIndexes:indexes];
}
+(void)removeObjectFromImagesAtIndex:(NSUInteger)index{
    [[singleton getDelegate] removeObjectFromImagesAtIndex:index];
}
+(void)removeImagesAtIndexes:(NSIndexSet *)indexes{
    [[singleton getDelegate] removeImagesAtIndexes:indexes];
}
+(void)addImage:(NSImage*)img{
    [[singleton getDelegate] insertObject:img inImagesAtIndex:[[singleton getDelegate] countOfImages]];
}
+(void)removeImage:(NSImage*)img{
    NSInteger index = [[[singleton getDelegate] images] indexOfObject:img];
    if(index == -1)
        return;
    [[singleton getDelegate] removeObjectFromImagesAtIndex:index];
}
+(void)removeAllImages{
    NSIndexSet* indexSet = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [[singleton getDelegate] countOfImages])];
    [[singleton getDelegate] removeImagesAtIndexes:indexSet];
}
@end




















