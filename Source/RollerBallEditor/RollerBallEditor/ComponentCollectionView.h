//
//  ComponentCollectionView.h
//  RollerBallEditor
//
//  Created by Danilo Santos de Carvalho on 23/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ComponentCollectionView : NSCollectionView
@property(weak) IBOutlet id deleteTarget;
@property SEL deleteAction;
@end
