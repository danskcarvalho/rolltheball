//
//  DSEditorDelegate.h
//  RollerBallEditor
//
//  Created by Danilo Carvalho on 26/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "DSCustomOpenGLViewDelegate.h"
#include "PropertyInspector.h"
#import "GCJumpBar.h"
#import "ComponentCollectionView.h"
#import "NavigablePropertyInspector.h"

@class DSCustomOpenGLView;
@class ComponentInfo;

typedef void (^ConfirmationBlock)(BOOL);

@interface DSEditorDelegate : NSObject <DSCustomOpenGLViewDelegate, GCJumpBarDelegate>
- (IBAction)playStop:(id)sender;
- (IBAction)textureAtlas:(id)sender;
@property (weak) IBOutlet DSCustomOpenGLView *openGLView;
- (IBAction)showHideSettings:(id)sender;
@property (weak) IBOutlet NSSegmentedControl *navigationSegment;
@property (weak) IBOutlet NSSegmentedControl *alignmentSegment;
@property (weak) IBOutlet NSSegmentedControl *orderSegment;
@property (weak) IBOutlet NSSegmentedControl *hierarchySegment;
@property (weak) IBOutlet NSToolbarItem *playPauseButton;
@property (weak) IBOutlet NSPopUpButton *creatableSelector;
@property (weak) IBOutlet NSButtonCell *oneClassRadio;
@property (weak) IBOutlet NSButtonCell *allClassesRadio;
@property (weak) IBOutlet NSButtonCell *namedRadio;
@property (weak) IBOutlet NSTextField *classesField;
@property (weak) IBOutlet NSSegmentedControl *nModeSegment;
- (IBAction)toggleNewMode:(id)sender;
- (IBAction)centerCameraOnSelection:(id)sender;
- (IBAction)setMoveView:(id)sender;
- (IBAction)deselectAllNodes:(id)sender;
- (IBAction)setRotateView:(id)sender;
- (IBAction)setZoomView:(id)sender;
- (IBAction)resetZoom:(id)sender;
- (IBAction)resetCamera:(id)sender;
@property (unsafe_unretained) IBOutlet NSWindow *window;
- (IBAction)resetRotation:(id)sender;
@property (strong) IBOutlet NSWindow *selectNodeWithClassWindow;
- (IBAction)cancelSelectionNodesWithClass:(id)sender;
- (IBAction)select:(id)sender;
- (IBAction)removeFromSelection:(id)sender;
@property (weak) IBOutlet NSMenuItem *recentFilesMenu;
@property (weak) IBOutlet NSMenu *transformSubmenu;
@property (weak) IBOutlet GCJumpBar *jumpBar;
@property (weak) IBOutlet NSView *mainView;
@property (weak) IBOutlet NSLayoutConstraint *componentsHeight;
@property (weak) IBOutlet NSLayoutConstraint *propertyInspectorWidth;
@property (weak) IBOutlet NSLayoutConstraint *spaceToComponentsTab;
@property (weak) IBOutlet NSScrollView *componentScrollView;
@property (weak) IBOutlet NSView *propertyCustomView;
@property (weak) IBOutlet NavigablePropertyInspector *propertyInspector;
@property (weak) IBOutlet ComponentCollectionView *componentsCollectionView;
@property (weak) IBOutlet NSArrayController *componentsArrayController;
@property (strong) NSMutableArray* components;
-(NSUInteger)countOfComponents;
-(id)objectInComponentsAtIndex:(NSUInteger)index;
-(void)insertObject:(ComponentInfo *)object inComponentsAtIndex:(NSUInteger)index;
-(void)removeObjectFromComponentsAtIndex:(NSUInteger)index;
- (IBAction)removeDegenerated:(id)sender;
-(void)removeComponentsAtIndexes:(NSIndexSet *)indexes;
- (IBAction)selectNodeWithClass:(id)sender;
-(void)delegateSelectionChanged:(void*)container;
-(void)delegateHierarchyChanged:(void*)container;
-(void)delegateCurrentChanged;
-(void)delegateNameChanged:(void*)n;
-(void)delegateCreatableTypeWasRegistered:(void*)td;
-(void)delegatePushObjectInPropertyInspector:(void*)obj withTitle: (NSString*)title;
- (IBAction)gotoLayer:(id)sender;
-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context;
-(void)alert:(NSString*)message;
- (IBAction)groupObjects:(id)sender;
- (IBAction)ungroupObjects:(id)sender;
- (IBAction)moveUpHierarchy:(id)sender;
- (IBAction)moveDownHierarchy:(id)sender;
- (IBAction)newScene:(id)sender;
- (IBAction)saveScene:(id)sender;
- (IBAction)saveSceneAs:(id)sender;
- (IBAction)openScene:(id)sender;
- (IBAction)showSceneInFinder:(id)sender;
- (IBAction)showAtlasInFinder:(id)sender;
- (IBAction)bringToFront:(id)sender;
- (IBAction)bringFrontward:(id)sender;
- (IBAction)sendBackward:(id)sender;
- (IBAction)sendToBack:(id)sender;
- (IBAction)matchToCurrent:(id)sender;
- (IBAction)quitApplication:(id)sender;
- (IBAction)blockNode:(id)sender;
- (IBAction)unblockAllNodes:(id)sender;
- (IBAction)unblockAllNodesCurrent:(id)sender;
- (IBAction)unblockAllNodesCurrentLayer:(id)sender;
- (IBAction)hideNode:(id)sender;
- (IBAction)hideCurrentNode:(id)sender;
- (IBAction)unhideAllNodes:(id)sender;
- (IBAction)unhideNodesOnThisLayer:(id)sender;
- (IBAction)unhideNodesOnCurrent:(id)sender;
- (IBAction)unhideAllLayers:(id)sender;
- (IBAction)alignHorCenter:(id)sender;
- (IBAction)alignHorLeft:(id)sender;
- (IBAction)alignHorRight:(id)sender;
- (IBAction)alignVerCenter:(id)sender;
- (IBAction)alignVerTop:(id)sender;
- (IBAction)alignVerBottom:(id)sender;
- (IBAction)bringLayerFront:(id)sender;
- (IBAction)bringLayerForward:(id)sender;
- (IBAction)sendLayerBackward:(id)sender;
- (IBAction)sendLayerBack:(id)sender;
- (IBAction)addToSelection:(id)sender;
-(void)confirm:(NSString*)message confirmationBlock:(ConfirmationBlock)block;
- (IBAction)lockSelection:(id)sender;
- (IBAction)unlockSelection:(id)sender;
@end
