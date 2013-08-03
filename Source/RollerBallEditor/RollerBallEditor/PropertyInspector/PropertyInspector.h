//
//  PropertyInspector.h
//  CocoaTest
//
//  Created by Danilo Carvalho on 15/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "ScrollablePropertyInspectorGroup.h"
#import "ObservableObject.h"

@class NavigablePropertyInspector;
@interface PropertyInspector : ScrollablePropertyInspectorGroup
@property (strong, nonatomic) id<ObservableObject> object;
@property (strong, nonatomic) NSString* propertyName;
@property (weak, nonatomic) NavigablePropertyInspector* navigableInspector;
@end
