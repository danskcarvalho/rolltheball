//
//  PropertyInspector.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 15/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "PropertyInspector.h"
#import "UIObjectSynchronizator.h"

@interface PropertyInspector ()
{
    UIObjectSynchronizator* fSync;
    id fObject;
}
@end

@implementation PropertyInspector

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        fObject = nil;
        fSync = nil;
        self.propertyName = nil;
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
}

-(id<ObservableObject>)object{
    return fObject;
}

-(void)setObject:(id<ObservableObject>)object{
    if(!fSync){
        fSync = [[UIObjectSynchronizator alloc] init];
    }
    else {
        if(object)
            [UIObjectSynchronizator setAvoidLayout:YES];
        [fSync clear];
        [[self group] clearChildren];
        fSync = [[UIObjectSynchronizator alloc] init];
        if(object)
            [UIObjectSynchronizator setAvoidLayout:NO];
    }
    
    fObject = object;
    if(fObject){
        fSync.inspector = self.navigableInspector;
        [fSync startWithObject:object group:[self group] forProperty:self.propertyName];
    }
    

    [[self group] redraw];
    [self setNeedsDisplay:YES];
}

@end
