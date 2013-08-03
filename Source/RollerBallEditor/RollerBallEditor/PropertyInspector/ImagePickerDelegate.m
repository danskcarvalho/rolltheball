//
//  ImagePickerDelegate.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 01/12/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "ImagePickerDelegate.h"

@interface ImagePickerDelegate ()
{
    ImageSelectedBlock fBlck;
}
@property (readwrite) NSUInteger selectedIndex;
@end

@implementation ImagePickerDelegate
-(void)awakeFromNib{
    self.images = [NSMutableArray array];
    [self.arrayController addObserver:self forKeyPath:@"selectionIndexes" options:NSKeyValueObservingOptionNew context:NULL];
    self.selectedIndex = -1;
    fBlck = nil;
}
-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    if ([keyPath isEqualToString:@"selectionIndexes"] && object == self.arrayController){
        NSIndexSet* set = [self.arrayController selectionIndexes];
        if([set class] != [NSNull class] && set.count > 0)
            self.selectedIndex = [set firstIndex];
        else
            self.selectedIndex = -1;
        
        if(self.selectedIndex == -1 && fBlck)
            fBlck(nil);
        else if(fBlck)
            fBlck([self.images objectAtIndex:self.selectedIndex]);
    }
}

-(void)showRelativeToRect:(NSRect)positioningRect ofView:(NSView *)positioningView preferredEdge:(NSRectEdge)preferredEdge selectedImage: (NSImage*)img block:(ImageSelectedBlock)blck{
    fBlck = blck;
    if (!img){
        self.selectedIndex = -1;
        self.arrayController.selectionIndex = -1;
    }
    else {
        NSInteger index = [self.images indexOfObject:img];
        self.selectedIndex = index;
        self.arrayController.selectionIndex = index;
    }
    [self.popover close];
    [self.popover showRelativeToRect:positioningRect ofView:positioningView preferredEdge:preferredEdge];
}

-(NSUInteger)countOfImages{
    return [self.images count];
}
-(id)objectInImagesAtIndex:(NSUInteger)index{
    return [self.images objectAtIndex:index];
}
-(NSArray *)imagesAtIndexes:(NSIndexSet *)indexes{
    return [self.images objectsAtIndexes:indexes];
}
-(void)insertObject:(NSImage *)object inImagesAtIndex:(NSUInteger)index{
    [self.images insertObject:object atIndex:index];
}
-(void)insertImages:(NSArray *)array atIndexes:(NSIndexSet *)indexes{
    [self.images insertObjects:array atIndexes:indexes];
}
-(void)removeObjectFromImagesAtIndex:(NSUInteger)index{
    [self.images removeObjectAtIndex:index];
}
-(void)removeImagesAtIndexes:(NSIndexSet *)indexes{
    [self.images removeObjectsAtIndexes:indexes];
}
-(void)dealloc{
    [self.arrayController removeObserver:self forKeyPath:@"selectionIndexes"];
}
@end
