//
//  DSEditorDelegate.m
//  RollerBallEditor
//
//  Created by Danilo Carvalho on 26/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSEditorDelegate.h"
#define MACOSX_TARGET
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#define _OBJC_BRIDGE_
#include "components_external.h"
#import "Resolutions.h"
#import "DSCustomOpenGLView.h"
#import "CppBridge.h"
#import "ComponentInfo.h"
#include "EditorDelegateImplementation.h"
#import "GCJumpBar.h"

#define MAX_RECENT_FILES 10

using namespace rb;

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

inline NSString* from_name_to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    if(_platform_str.length == 0)
        _platform_str = @"<Unnamed>";
    return _platform_str;
}

inline rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}

@interface ConfirmationInfo : NSObject
@property(strong)NSString* message;
@property(strong)ConfirmationBlock block;
@end

@implementation ConfirmationInfo

@end

@interface DSEditorDelegate ()
{
    NSOpenPanel* fTextureAtlasOpenPanel;
    NSSavePanel* fSceneSavePanel;
    NSOpenPanel* fSceneOpenPanel;
    NSURL* fLastSavedScene;
    NSImage* fPausedImage;
    NSImage* fPlayingImage;
    BOOL fComponentsTabHidden;
    BOOL fPropertyTabHidden;
    NSLayoutConstraint* fComponentsHiddenConstraint;
    NSLayoutConstraint* fSavedComponentsHiddenConstraint;
    NSLayoutConstraint* fSpaceToComponentsTabConstraint;
    NSLayoutConstraint* fSavedSpaceToComponentsTabConstraint;
    NSLayoutConstraint* fPropertyHiddenConstraint;
    NSArray* fSavedPropertyHiddenConstraint;
    NavigablePropertyInspector* fSavedPropertyInspector;
    std::vector<void*> fPreviousSelection;
    multiple_nodes* fCurrentMultipleNodes;
    EditorDelegateImplementation* fEditorDelegate;
    BOOL fChangingComponentsSelection;
    BOOL fComponentsTabNeedsUpdate;
    std::vector<rb::node_container*> fLastHierarchy;
    BOOL fSettingHierarchyTab;
    BOOL fSaveAndClose;
    BOOL fInitialized;
    
    //Hierarchy Images
    NSImage* fMultiNodesIcon;
    NSImage* fLayerIcon;
    NSImage* fSceneIcon;
    NSImage* fNodeIcon;
    NSImage* fComponentIcon;
    NSMutableDictionary* fCreatableDictionary;
    
    //Alerts and Confirms
    BOOL fInAlertOrConfirmation;
    NSMutableArray* fStackedAlertConfirmations;
    
    //Current Scene
    rb::scene* fCurrentScene;
}
-(void)updateComponentsTab:(BOOL)onlySelection;
-(void)updateHierarchyTab;
-(NSMenuItem*)createMenuItemForNodes:(std::vector<rb::node_container*>&)n index:(size_t)index;
-(void)updateTransformationMenu;
- (void) openSceneFromURL:(NSURL*)url;
-(void)reorderNodeWithSelector: (NSUInteger)s;
- (IBAction)saveScene:(id)sender;
-(void)alignNodeWithSelector: (NSUInteger)s;
@end

@implementation DSEditorDelegate
-(void)startTransformation:(id)sender{
    NSInteger tag = [((NSMenuItem*)sender) tag];
    node_container* _target = nullptr;
    
    if(director::active_scene()->current()) {
        std::vector<node*> _selected;
        director::active_scene()->current()->fill_with_selection(_selected);
        if(_selected.size() == 1)
            _target = _selected[0];
        else if(_selected.size() == 0)
            _target = director::active_scene()->current();
    }
    
    if(_target){
        _target->start_transformation(tag);
    }
}
-(void)updateTransformationMenu{
    if(!director::active_scene())
        return;
    
    [self.transformSubmenu removeAllItems];
    node_container* _target = nullptr;
    
    if(director::active_scene()->current()) {
        std::vector<node*> _selected;
        director::active_scene()->current()->fill_with_selection(_selected);
        if(_selected.size() == 1)
            _target = _selected[0];
        else if(_selected.size() == 0)
            _target = director::active_scene()->current();
    }
    
    if(!_target){
        NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:@"Nothing To Transform" action:nil keyEquivalent:@""];
        [self.transformSubmenu addItem:item];
    }
    else {
        auto _transformables = _target->transformables();
        
        if(_transformables.size() == 0)
        {
            NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:@"Nothing To Transform" action:nil keyEquivalent:@""];
            [self.transformSubmenu addItem:item];
        }
        else {
            size_t i = 0;
            for (auto& _t : _transformables){
                NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:to_platform_string(_t) action:@selector(startTransformation:) keyEquivalent:@""];
                [item setTarget:self];
                [item setTag:i];
                [self.transformSubmenu addItem:item];
                i++;
            }
        }
    }
}

-(void)updateComponentsTab:(BOOL)onlySelection{
    if(fChangingComponentsSelection || !director::active_scene())
        return;
    
    BOOL fPreviousChangingComponentsSelection = fChangingComponentsSelection;
    fChangingComponentsSelection = YES;
    if(!onlySelection){
        if(director::active_scene()->current() == nullptr)
        {
            [self removeComponentsAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, self.components.count)]];
            fChangingComponentsSelection = fPreviousChangingComponentsSelection;
            return;
        }
        else {
            std::vector<rb::node*> _nodes;
            director::active_scene()->current()->copy_nodes_to_vector(_nodes, node_filter::non_renderable);
            std::unordered_set<rb::node*> _s_nodes;
            for (auto _n : _nodes)
                _s_nodes.insert(_n);
            
            //remove deleted components
            std::unordered_set<rb::node*> _added_nodes;
            NSMutableIndexSet* indexesToRemoves = [NSMutableIndexSet indexSet];
            for (NSUInteger i = 0; i < self.components.count; i++) {
                ComponentInfo* ci = (ComponentInfo*)[self.components objectAtIndex:i];
                if(!_s_nodes.count((rb::node*)ci.tag))
                    [indexesToRemoves addIndex:i];
                else {
                    _added_nodes.insert(reinterpret_cast<rb::node*>(ci.tag));
                }
            }
            [self removeComponentsAtIndexes:indexesToRemoves];
            
            //add new components
            for (auto _n : _s_nodes){
                if(!_added_nodes.count(_n)){
                    [self insertObject:[[ComponentInfo alloc] initWithName:from_name_to_platform_string(_n->name()) subname:to_platform_string(_n->displayable_type_name()) tag:_n] inComponentsAtIndex:[self countOfComponents]];
                }
            }
        }
    }
    //update selection
    if (director::active_scene()->current()){
        NSMutableIndexSet* selectedIndexes = [NSMutableIndexSet indexSet];
        std::vector<rb::node*> _selected_nodes;
        director::active_scene()->current()->fill_with_selection(_selected_nodes, node_filter::non_renderable);
        std::unordered_set<rb::node*> _selected_set;
        for (auto _n : _selected_nodes)
            _selected_set.insert(_n);
        for (NSUInteger i = 0; i < self.components.count; i++) {
            ComponentInfo* ci = (ComponentInfo*)[self.components objectAtIndex:i];
            if(_selected_set.count((rb::node*)ci.tag))
                [selectedIndexes addIndex:i];
        }
        [self.componentsArrayController setSelectionIndexes:selectedIndexes];
    }
    
    fChangingComponentsSelection = fPreviousChangingComponentsSelection;
}
-(void)updateHierarchyTab{
    if(!director::active_scene())
        return;
    
    fSettingHierarchyTab = YES;
    if(!director::active_scene()->current())
    {
        NSMenu* rootMenu = [[NSMenu alloc] initWithTitle:@""];
        NSMenuItem* sceneMenuItem = [[NSMenuItem alloc] initWithTitle:@"Scene" action:nil keyEquivalent:@""];
        [sceneMenuItem setImage:fSceneIcon];
        [sceneMenuItem setTag:(NSInteger)director::active_scene()];
        [rootMenu addItem:sceneMenuItem];
        [self.jumpBar setMenu:rootMenu];
        return;
    }
    
    std::vector<rb::node_container*> nodes;
    
    rb::node_container* _current = director::active_scene()->current();
    
    while (_current != nullptr) {
        nodes.push_back(_current);
        auto _n = dynamic_cast<rb::node*>(_current);
        if(_n)
            _current = _n->parent_node() ? (node_container*)_n->parent_node() : _n->parent_layer();
        else
            _current = nullptr;
    }
    std::reverse(nodes.begin(), nodes.end());
    
//    std::vector<rb::node*> _selected;
//    director::active_scene()->current()->fill_with_selection(_selected);
//    
//    if(_selected.size() == 1)
//        nodes.push_back(_selected[0]);
//    else if(_selected.size() > 1)
//        nodes.push_back(nullptr);
    
    NSMenu* rootMenu = [[NSMenu alloc] initWithTitle:@""];
    NSMenuItem* sceneMenuItem = [[NSMenuItem alloc] initWithTitle:@"Scene" action:nil keyEquivalent:@""];
    [sceneMenuItem setImage:fSceneIcon];
    [sceneMenuItem setTag:(NSInteger)director::active_scene()];
    [rootMenu addItem:sceneMenuItem];
    
    NSMenu* menu = [[NSMenu alloc] initWithTitle:@""];
    [menu addItem:[self createMenuItemForNodes:nodes index:0]];
    [sceneMenuItem setSubmenu:menu];
    
    [self.jumpBar setMenu:rootMenu];
    NSUInteger* path = (NSUInteger*)malloc(sizeof(NSUInteger) * (1 + nodes.size()));
    for (size_t i = 0; i < (1 + nodes.size()); i++) {
        path[i] = 0;
    }
    fLastHierarchy = nodes;
    [self.jumpBar setSelectedIndexPath:[NSIndexPath indexPathWithIndexes:path length:1 + nodes.size()]];
    free(path);
    fSettingHierarchyTab = NO;
}
-(NSMenuItem *)createMenuItemForNodes:(std::vector<rb::node_container*>&)n index:(size_t)index{
    auto _n = dynamic_cast<rb::node*>(n[index]);
    NSMenuItem* mi = nil;
    if(!n[index]){
        mi = [[NSMenuItem alloc] initWithTitle:@"Multiple Nodes" action:nil keyEquivalent:@""];
        [mi setImage:fMultiNodesIcon];
        [mi setTag:(NSInteger)nullptr];
    }
    else if(_n){
        mi = [[NSMenuItem alloc] initWithTitle:from_name_to_platform_string(_n->name()) action:nil keyEquivalent:@""];
        [mi setTag:(NSInteger)_n];
        if(!_n->renderable())
            [mi setImage:fComponentIcon];
        else if(dynamic_cast<group_component*>(_n))
            [mi setImage:fMultiNodesIcon];
        else
            [mi setImage:fNodeIcon];
    }
    else
    {
        auto _l = dynamic_cast<rb::layer*>(n[index]);
        [mi setTag:(NSInteger)_l];
        for (uint32_t i = 0; i < (MAX_LAYERS + 1); i++) {
            if (_l == director::active_scene()->layer(i)){
                if(_l != director::active_scene()->hidden_layer())
                    mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"Layer %d", i] action:nil keyEquivalent:@""];
                else
                    mi = [[NSMenuItem alloc] initWithTitle:@"Hidden Layer" action:nil keyEquivalent:@""];
                [mi setImage:fLayerIcon];
                break;
            }
        }
        assert(mi);
    }
    if(index < (n.size() - 1)){
        NSMenu* submenu = [[NSMenu alloc] initWithTitle:@""];
        [submenu addItem:[self createMenuItemForNodes:n index:index + 1]];
        [mi setSubmenu:submenu];
    }
    return mi;
}
//Actually, play or pause
- (IBAction)playStop:(id)sender {
    director::active_scene()->playing(!director::active_scene()->playing());
}
- (IBAction)stop:(id)sender {
    
}
-(void)showHideComponentsBar{
    NSLayoutConstraint* cnt = self.componentsHeight;
    
    if(fComponentsTabHidden || director::active_scene()->playing()){
        if(!fSavedComponentsHiddenConstraint){
            fComponentsHiddenConstraint = [NSLayoutConstraint constraintWithItem:cnt.firstItem attribute:cnt.firstAttribute relatedBy:cnt.relation toItem:cnt.secondItem attribute:cnt.secondAttribute multiplier:cnt.multiplier constant:0];
            fSavedComponentsHiddenConstraint = [NSLayoutConstraint constraintWithItem:cnt.firstItem attribute:cnt.firstAttribute relatedBy:cnt.relation toItem:cnt.secondItem attribute:cnt.secondAttribute multiplier:cnt.multiplier constant:cnt.constant];
            [self.componentScrollView removeConstraint:cnt];
            [self.componentScrollView addConstraint: fComponentsHiddenConstraint];
            
            
            
            fSpaceToComponentsTabConstraint = [NSLayoutConstraint constraintWithItem:self.spaceToComponentsTab.firstItem attribute:self.spaceToComponentsTab.firstAttribute relatedBy:self.spaceToComponentsTab.relation toItem:self.spaceToComponentsTab.secondItem attribute:self.spaceToComponentsTab.secondAttribute multiplier:self.spaceToComponentsTab.multiplier constant:-2];
            
            
            fSavedSpaceToComponentsTabConstraint = [NSLayoutConstraint constraintWithItem:self.spaceToComponentsTab.firstItem attribute:self.spaceToComponentsTab.firstAttribute relatedBy:self.spaceToComponentsTab.relation toItem:self.spaceToComponentsTab.secondItem attribute:self.spaceToComponentsTab.secondAttribute multiplier:self.spaceToComponentsTab.multiplier constant:self.spaceToComponentsTab.constant];
            
            [self.mainView removeConstraint:self.spaceToComponentsTab];
            [self.mainView addConstraint:fSpaceToComponentsTabConstraint];
        }
    }
    else {
        if(fSavedComponentsHiddenConstraint){
            [self.componentScrollView removeConstraint:fComponentsHiddenConstraint];
            [self.componentScrollView addConstraint:fSavedComponentsHiddenConstraint];
            [self.mainView removeConstraint:fSpaceToComponentsTabConstraint];
            [self.mainView addConstraint:fSavedSpaceToComponentsTabConstraint];
            self.componentsHeight = fSavedComponentsHiddenConstraint;
            self.spaceToComponentsTab = fSavedSpaceToComponentsTabConstraint;
            fComponentsHiddenConstraint = nil;
            fSavedComponentsHiddenConstraint = nil;
            fSpaceToComponentsTabConstraint = nil;
            fSavedSpaceToComponentsTabConstraint = nil;
        }
    }
}
- (IBAction)showHideComponentsBar:(id)sender {
    fComponentsTabHidden = !fComponentsTabHidden;
    [self showHideComponentsBar];
}

- (IBAction)textureAtlas:(id)sender {
    [NSOpenGLContext clearCurrentContext];
    [self.openGLView setRenderingEnabled:NO];
    [fTextureAtlasOpenPanel beginSheetModalForWindow:[[NSApplication sharedApplication] mainWindow] completionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelCancelButton){
            [[self.openGLView openGLContext] makeCurrentContext];
            [self.openGLView setRenderingEnabled:YES];
            return;
        }
        NSURL* fileNameURL = [[fTextureAtlasOpenPanel URLs] lastObject];
        [[self.openGLView openGLContext] makeCurrentContext];
        [self.openGLView setRenderingEnabled:YES];
        director::active_scene()->load_atlas(from_platform_string([fileNameURL absoluteString]));
    }];
}
-(void)setToolbarImages{
    //Navigation
    NSImage* translationImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845874_64909" ofType:@"icns"]];
    [translationImage setSize:NSMakeSize(16, 16)];
    [self.navigationSegment setImage:translationImage forSegment:0];
    
    NSImage* rotationImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845637_35691" ofType:@"icns"]];
    [rotationImage setSize:NSMakeSize(16, 16)];
    [self.navigationSegment setImage:rotationImage forSegment:1];
    
    NSImage* zoomImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845665_36178" ofType:@"icns"]];
    [zoomImage setSize:NSMakeSize(16, 16)];
    [self.navigationSegment setImage:zoomImage forSegment:2];
    
    //Alignment
    NSImage* alignCenter = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845350_36426" ofType:@"icns"]];
    [alignCenter setSize:NSMakeSize(16, 16)];
    [self.alignmentSegment setImage:alignCenter forSegment:0];
    
    NSImage* alignLeft = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845367_36427" ofType:@"icns"]];
    [alignLeft setSize:NSMakeSize(16, 16)];
    [self.alignmentSegment setImage:alignLeft forSegment:1];
    
    NSImage* alignRight = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845379_36429" ofType:@"icns"]];
    [alignRight setSize:NSMakeSize(16, 16)];
    [self.alignmentSegment setImage:alignRight forSegment:2];
    
    NSImage* alignVCenter = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845401_36428" ofType:@"icns"]];
    [alignVCenter setSize:NSMakeSize(16, 16)];
    [self.alignmentSegment setImage:alignVCenter forSegment:3];
    
    NSImage* alignTop = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845411_36430" ofType:@"icns"]];
    [alignTop setSize:NSMakeSize(16, 16)];
    [self.alignmentSegment setImage:alignTop forSegment:4];
    
    NSImage* alignBottom = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845392_36425" ofType:@"icns"]];
    [alignBottom setSize:NSMakeSize(16, 16)];
    [self.alignmentSegment setImage:alignBottom forSegment:5];
    
    //Order
    NSImage* sendToFront = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845836_64641" ofType:@"icns"]];
    [sendToFront setSize:NSMakeSize(16, 16)];
    [self.orderSegment setImage:sendToFront forSegment:0];
    
    NSImage* moveFront = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845838_64640" ofType:@"icns"]];
    [moveFront setSize:NSMakeSize(16, 16)];
    [self.orderSegment setImage:moveFront forSegment:1];
    
    NSImage* moveBack = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845833_36436" ofType:@"icns"]];
    [moveBack setSize:NSMakeSize(16, 16)];
    [self.orderSegment setImage:moveBack forSegment:2];
    
    NSImage* sendToBack = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361845825_36435" ofType:@"icns"]];
    [sendToBack setSize:NSMakeSize(16, 16)];
    [self.orderSegment setImage:sendToBack forSegment:3];
    
    //Hierarchy
    NSImage* upHr = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361851234_35696" ofType:@"icns"]];
    [upHr setSize:NSMakeSize(16, 16)];
    [self.hierarchySegment setImage:upHr forSegment:0];
    
    NSImage* downHr = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361851219_35679" ofType:@"icns"]];
    [downHr setSize:NSMakeSize(16, 16)];
    [self.hierarchySegment setImage:downHr forSegment:1];
    
}
-(void)navigationChanged:(id)sender {
    NSInteger selected = [self.navigationSegment selectedSegment];
    if(selected == 0)
        director::active_scene()->navigation_mode(navigation_mode::scroll);
    else if(selected == 1)
        director::active_scene()->navigation_mode(navigation_mode::rotate);
    else
        director::active_scene()->navigation_mode(navigation_mode::zoom);
}
-(void)nModeChanged:(id)sender{
    NSString* componentName = [fCreatableDictionary objectForKey: @(self.creatableSelector.indexOfSelectedItem)];
    NSInteger selected = [self.nModeSegment selectedSegment];
    if(selected == 0 && !director::active_scene()->template_for_new()){
        if(!director::active_scene()->current()){
            [self alert:@"Can't add nodes directly to scene."];
            return;
        }
        director::active_scene()->enter_new_mode(type_descriptor::with_name(from_platform_string(componentName)));
    }
    else
        director::active_scene()->exit_new_mode();
}
-(void)componentsDelete:(id)sender event:(NSEvent*)theEvent {
    keyboard_modifier _mod = (keyboard_modifier)([theEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask);
    auto _responder = director::active_responder();
    if(_responder)
        _responder->keydown([theEvent keyCode], _mod);
}
-(void)moveUpDownhierarchy:(id)sender {
    NSInteger selected = [self.hierarchySegment selectedSegment];
    if(selected == 0)
        [self moveUpHierarchy:sender];
    else
        [self moveDownHierarchy:sender];
}
-(void)createAppDir {
    NSArray* directories = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSURL* dir = [NSURL fileURLWithPath: [directories objectAtIndex:0] isDirectory:YES];
    dir = [dir URLByAppendingPathComponent:@"RB-Editor-1.0"];
    if(![[NSFileManager defaultManager] fileExistsAtPath:[dir path]]){
        [[NSFileManager defaultManager] createDirectoryAtURL:dir withIntermediateDirectories:NO attributes:nil error:nil];
    }
}
-(void)createRecentFilesList {
    [self createAppDir];
    NSArray* directories = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSURL* file = [NSURL fileURLWithPath: [directories objectAtIndex:0] isDirectory:YES];
    file = [file URLByAppendingPathComponent:@"RB-Editor-1.0"];
    file = [file URLByAppendingPathComponent:@"RecentFiles"];
    if(![[NSFileManager defaultManager] fileExistsAtPath:[file path]]){
        NSArray* files = [NSArray array];
        [files writeToURL:file atomically:YES];
    }
}
-(NSArray*)recentFilesList {
    NSArray* directories = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSURL* file = [NSURL fileURLWithPath: [directories objectAtIndex:0] isDirectory:YES];
    file = [file URLByAppendingPathComponent:@"RB-Editor-1.0"];
    file = [file URLByAppendingPathComponent:@"RecentFiles"];
    NSArray* files = [NSArray arrayWithContentsOfURL:file];
    return files;
}
-(void)setRecentFilesList: (NSArray*)files {
    NSArray* directories = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSURL* file = [NSURL fileURLWithPath: [directories objectAtIndex:0] isDirectory:YES];
    file = [file URLByAppendingPathComponent:@"RB-Editor-1.0"];
    file = [file URLByAppendingPathComponent:@"RecentFiles"];
    [files writeToURL:file atomically:YES];
}
-(void)updateRecentFilesList: (NSString*)path{
    path = [path stringByExpandingTildeInPath];
    NSArray* rf = [self recentFilesList];
    NSMutableArray* newArray = [NSMutableArray array];
    BOOL containsPath = NO;
    for (NSUInteger i = 0; i < rf.count; i++) {
        NSString* p = [rf objectAtIndex:i];
        p = [p stringByExpandingTildeInPath];
        if([[NSFileManager defaultManager] fileExistsAtPath:p])
            [newArray addObject:[p stringByAbbreviatingWithTildeInPath]];
        
        if([p isEqualToString:path])
            containsPath = YES;
    }
    if(path && [[NSFileManager defaultManager] fileExistsAtPath:path] && !containsPath){
        path = [path stringByAbbreviatingWithTildeInPath];
        [newArray addObject:path];
    }
    while(newArray.count > MAX_RECENT_FILES)
        [newArray removeObjectAtIndex:0];
    
    [self setRecentFilesList:newArray];
}
-(void)openRecentFile:(id)sender {
    NSString* file = [sender title];
    file = [file stringByExpandingTildeInPath];
    if([[NSFileManager defaultManager] fileExistsAtPath:file])
        [self openSceneFromURL:[NSURL fileURLWithPath:file]];
    [self updateRecentFilesListMenu:nil];
}
-(void)clearRecentFiles:(id)sender {
    [self setRecentFilesList:@[]];
    [self updateRecentFilesListMenu:nil];
}
-(void)updateRecentFilesListMenu: (NSString*)path{
    [self updateRecentFilesList:path];
    NSArray* rf = [self recentFilesList];
    NSMenu* menu = [[NSMenu alloc] initWithTitle:@"Some title"];
    for (NSUInteger i = 0; i < rf.count; i++) {
        NSString* file = [rf objectAtIndex:i];
        NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:file action:@selector(openRecentFile:) keyEquivalent:@""];
        [item setTarget:self];
        [menu addItem:item];
    }
    if(rf.count != 0){
        [menu addItem:[NSMenuItem separatorItem]];
        NSMenuItem* clearItem = [[NSMenuItem alloc] initWithTitle:@"Clear Recent Files List" action:@selector(clearRecentFiles:) keyEquivalent:@""];
        [clearItem setTarget:self];
        [menu addItem:clearItem];
    }
    [self.recentFilesMenu setSubmenu:menu];
}
-(void)reorderNodeWithSelector: (NSUInteger)s {
    std::vector<node*> _selected;
    if(director::active_scene()->current()){
        director::active_scene()->current()->fill_with_selection(_selected, node_filter::renderable, true);
        if(_selected.size() == 0)
            return;
        std::vector<node*> _unreversed(_selected);
        if(s == 3 || s == 1)
            std::reverse(_selected.begin(), _selected.end());
        
        if(s == 2 || s == 3){
            auto _index = director::active_scene()->current()->search_node(_unreversed[0]);
            assert(_index.has_value());
            
            if(_index.value() == 0)
                return;
        }
        else {
            auto _index = director::active_scene()->current()->search_node(_unreversed[_unreversed.size() - 1]);
            assert(_index.has_value());
            
            if(_index.value() == (director::active_scene()->current()->node_count() - 1))
                return;
        }
        
        for (auto _s : _selected){
            if(s == 0) {
                director::active_scene()->current()->send_to_back(_s);
            }
            else if(s == 1) {
                director::active_scene()->current()->send_backward(_s);
            }
            else if(s == 2) {
                director::active_scene()->current()->bring_forward(_s);
            }
            else {
                director::active_scene()->current()->bring_to_front(_s);
            }
        }
    }
}
-(void)reorderNode: (id)sender{
    NSUInteger s = [self.orderSegment selectedSegment];
    [self reorderNodeWithSelector: s];
}
-(void)alignNodeWithSelector: (NSUInteger)s{
    std::vector<node*> _selected;
    if(director::active_scene()->current()){
        director::active_scene()->current()->fill_with_selection(_selected, node_filter::renderable, true);
        if(_selected.size() <= 1)
            return;
        
        if (s == 0)
            director::active_scene()->current()->hor_align_by_center(_selected);
        else if (s == 1)
            director::active_scene()->current()->hor_align_by_left_edge(_selected);
        else if (s == 2)
            director::active_scene()->current()->hor_align_by_right_edge(_selected);
        else if (s == 3)
            director::active_scene()->current()->ver_align_by_center(_selected);
        else if (s == 4)
            director::active_scene()->current()->ver_align_by_top_edge(_selected);
        else
            director::active_scene()->current()->ver_align_by_bottom_edge(_selected);
    }
}
-(void)alignNode: (id)sender {
    NSUInteger s = [self.alignmentSegment selectedSegment];
    [self alignNodeWithSelector:s];
}
-(void)awakeFromNib{
    if(fInitialized)
        return;
    
    [self createRecentFilesList];
    [self updateRecentFilesListMenu:nil];
    fTextureAtlasOpenPanel = [NSOpenPanel openPanel];
    [fTextureAtlasOpenPanel setExtensionHidden:YES];
    [fTextureAtlasOpenPanel setAllowedFileTypes:@[@"bundle"]];
    [fTextureAtlasOpenPanel setAllowsOtherFileTypes:NO];
    [fTextureAtlasOpenPanel setAllowsMultipleSelection:NO];
    
    fSceneOpenPanel = [NSOpenPanel openPanel];
    [fSceneOpenPanel setExtensionHidden:YES];
    [fSceneOpenPanel setAllowedFileTypes:@[@"scene"]];
    [fSceneOpenPanel setAllowsOtherFileTypes:NO];
    [fSceneOpenPanel setAllowsMultipleSelection:NO];
    
    fSceneSavePanel = [NSSavePanel savePanel];
    [fSceneSavePanel setExtensionHidden:NO];
    [fSceneSavePanel setAllowedFileTypes:@[@"scene"]];
    [fSceneSavePanel setAllowsOtherFileTypes:NO];
    fLastSavedScene = nil;
    
    fPausedImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361846083_35929" ofType:@"icns"]];
    fPlayingImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361846072_35921" ofType:@"icns"]];
    
    [self setToolbarImages];
    [self.openGLView setDelegate:self];
    
    //toolbar buttons
    [self.navigationSegment setAction:@selector(navigationChanged:)];
    [self.navigationSegment setTarget:self];
    
    [self.nModeSegment setAction:@selector(nModeChanged:)];
    [self.nModeSegment setTarget:self];
    
    [self.hierarchySegment setAction:@selector(moveUpDownhierarchy:)];
    [self.hierarchySegment setTarget:self];
    
    [self.orderSegment setAction:@selector(reorderNode:)];
    [self.orderSegment setTarget:self];
    
    [self.alignmentSegment setAction:@selector(alignNode:)];
    [self.alignmentSegment setTarget:self];
    
    fSaveAndClose = NO;
    fStackedAlertConfirmations = [NSMutableArray array];
    fCurrentScene = nullptr;
    fInAlertOrConfirmation = NO;
    fComponentsTabHidden = NO;
    fPropertyTabHidden = NO;
    fComponentsHiddenConstraint = nil;
    fSavedComponentsHiddenConstraint = nil;
    fPropertyHiddenConstraint = nil;
    fSavedPropertyHiddenConstraint = nil;
    fCurrentMultipleNodes = nullptr;
    fSpaceToComponentsTabConstraint = nil;
    fSavedSpaceToComponentsTabConstraint = nil;
    self.components = [NSMutableArray array];
    fEditorDelegate = new EditorDelegateImplementation(self);
    rb::director::editor_delegate(fEditorDelegate);
    fChangingComponentsSelection = NO;
    fComponentsTabNeedsUpdate = YES;
    [self.componentsArrayController addObserver:self forKeyPath:@"selectionIndexes" options:NSKeyValueObservingOptionNew context:NULL];
    fCreatableDictionary = [NSMutableDictionary dictionary];
    
    fMultiNodesIcon = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361981129_check-all" ofType:@"png"]];
    fLayerIcon = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361901969_layer" ofType:@"png"]];
    fSceneIcon = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361902076_database" ofType:@"png"]];
    fNodeIcon = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361902168_node-tree" ofType:@"png"]];
    fComponentIcon = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mainBundle]  pathForResource:@"1361902203_server_components" ofType:@"png"]];
    fSettingHierarchyTab = NO;
    self.componentsCollectionView.deleteTarget = self;
    self.componentsCollectionView.deleteAction = @selector(componentsDelete:event:);
    [self.componentScrollView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [[self.componentScrollView contentView] setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self.componentsCollectionView setTranslatesAutoresizingMaskIntoConstraints:NO];
    fInitialized = YES;
}
-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    if ([keyPath isEqualToString:@"selectionIndexes"] && object == self.componentsArrayController){
        if(fChangingComponentsSelection)
            return;
        NSIndexSet* set = [self.componentsArrayController selectionIndexes];
        if([set class] != [NSNull class] && set.count > 0) //lots of selected objects
        {
            BOOL fPreviousChangingComponentsSelection = fChangingComponentsSelection;
            fChangingComponentsSelection = YES;
            if(director::active_scene()->current()){
                auto _km = responder::modifiers();
                auto _add = rb::has_flag(_km, keyboard_modifier::shift);
                
                if(!_add)
                    director::active_scene()->current()->clear_selection();
                
                [set enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
                    ComponentInfo* ci = (ComponentInfo*)[self.components objectAtIndex:idx];
                    director::active_scene()->current()->add_to_selection((rb::node*)ci.tag);
                }];
            }
            fChangingComponentsSelection = fPreviousChangingComponentsSelection;
        }
        else { //no selected object
            BOOL fPreviousChangingComponentsSelection = fChangingComponentsSelection;
            fChangingComponentsSelection = YES;
            if(director::active_scene()->current()){
                director::active_scene()->current()->clear_selection();
            }
            fChangingComponentsSelection = fPreviousChangingComponentsSelection;
        }
    }
}
-(void)setUpPropertyInspector{
    auto _current = director::active_scene()->current();
    if(_current){
        std::vector<node *> _nselection;
        _current->fill_with_selection(_nselection);
        
        std::vector<typed_object*> _selection;
        for (auto _n : _nselection)
            _selection.push_back(_n);
        
        if(director::active_scene()->locked_selection()){
            _selection.clear();
            _selection.push_back(director::active_scene()->locked_selection());
        }
        
        rb::typed_object* _template_for_new;
        if((_template_for_new = director::active_scene()->template_for_new())){
            _selection.clear();
            _selection.push_back(_template_for_new);
        }
        
        if(_selection.size() == 0){ //no selected object
            if (fPreviousSelection.size() == 1 && fPreviousSelection[0] == _current)
                return;
            
            id<CppBridgeProtocol> _bridge = _current->get_bridge();
            if(!_bridge){
                _bridge = [[CppBridge alloc] initWithCppObject: _current];
                _current->set_bridge(_bridge);
            }
            [self updateHierarchyTab];
            [self updateTransformationMenu];
            [self.propertyInspector setObject:(id)_current->get_bridge()];
            
            fPreviousSelection.clear();
            fPreviousSelection.push_back(_current);
        }
        else {
            if (_selection.size() == fPreviousSelection.size() && std::equal(_selection.begin(), _selection.end(), fPreviousSelection.begin()))
                return; //selection didn't have changed
            
            for (auto _s : _selection){
                id<CppBridgeProtocol> _bridge = _s->get_bridge();
                if(!_bridge){
                    _bridge = [[CppBridge alloc] initWithCppObject: _s];
                    _s->set_bridge(_bridge);
                }
            }
            
            if(fCurrentMultipleNodes){
                delete fCurrentMultipleNodes;
                fCurrentMultipleNodes = nullptr;
            }
            
            if(_selection.size() == 1){
                [self updateHierarchyTab];
                [self updateTransformationMenu];
                [self.propertyInspector setObject:(id)_selection[0]->get_bridge()];
            }
            else {
                std::vector<typed_object*> _tobjs;
                for(auto _s : _selection)
                    _tobjs.push_back(_s);
                fCurrentMultipleNodes = new multiple_nodes(_tobjs);
                fCurrentMultipleNodes->set_bridge([[CppBridge alloc] initWithCppObject: fCurrentMultipleNodes]);
                [self updateHierarchyTab];
                [self updateTransformationMenu];
                [self.propertyInspector setObject:(id)fCurrentMultipleNodes->get_bridge()];
            }
            
            fPreviousSelection.clear();
            for (int i = 0; i < _selection.size(); i++)
                fPreviousSelection.push_back(_selection[i]);
        }
        
    }
    else //show scene properties
    {
        if(fPreviousSelection.size() == 1 && fPreviousSelection[0] == director::active_scene())
            return;
        
        id<CppBridgeProtocol> _bridge = director::active_scene()->get_bridge();
        if(!_bridge){
            _bridge = [[CppBridge alloc] initWithCppObject: director::active_scene()];
            director::active_scene()->set_bridge(_bridge);
        }
        [self updateHierarchyTab];
        [self updateTransformationMenu];
        [self.propertyInspector setObject:(id)director::active_scene()->get_bridge()];
        fPreviousSelection.clear();
        fPreviousSelection.push_back(director::active_scene());
    }
}
-(void)onUpdate{
    if(director::active_scene())
    {
        if(fCurrentScene == nullptr){
            fCurrentScene = director::active_scene();
            [self clearFlagsForNewScene];
        }
        if(fCurrentScene != director::active_scene()){
            fCurrentScene = director::active_scene();
            [self clearFlagsForNewScene];
        }
        if(director::active_scene()->playing())
            [self.playPauseButton setImage:fPausedImage];
        else
            [self.playPauseButton setImage:fPlayingImage];
        
        if(director::active_scene()->navigation_mode() == navigation_mode::scroll)
            [self.navigationSegment setSelected:YES forSegment:0];
        else if(director::active_scene()->navigation_mode() == navigation_mode::rotate)
            [self.navigationSegment setSelected:YES forSegment:1];
        else
            [self.navigationSegment setSelected:YES forSegment:2];
        
        if(director::active_scene()->template_for_new())
            [self.nModeSegment setSelected:YES forSegment:0];
        else
            [self.nModeSegment setSelected:NO forSegment:0];
        
        //Property Inspector
        if(!fPropertyTabHidden && !director::active_scene()->in_live_selection())
            [self setUpPropertyInspector];
        
        [self showHideComponentsBar];
        [self showHidePropertyBar];
        
        if(fComponentsTabNeedsUpdate){
            [self updateTransformationMenu];
            [self updateComponentsTab:NO];
            [self updateHierarchyTab];
            fComponentsTabNeedsUpdate = NO;
        }
        
        [self showAlertConfirmationFromStack];
    }
    else {
        [self updateTransformationMenu];
        [self updateHierarchyTab];
        [self.propertyInspector setObject: nil];
    }
}
-(void)showHidePropertyBar{
    NSLayoutConstraint* cnt = self.propertyInspectorWidth;
    if(fPropertyTabHidden || director::active_scene()->playing()){
        if(!fSavedPropertyHiddenConstraint){
            fSavedPropertyInspector = self.propertyInspector;
            fSavedPropertyHiddenConstraint = self.propertyCustomView.constraints;
            [self.propertyInspector removeFromSuperview];
            [self.propertyCustomView removeConstraint:cnt];
            fPropertyHiddenConstraint = [NSLayoutConstraint constraintWithItem:cnt.firstItem attribute:cnt.firstAttribute relatedBy:cnt.relation toItem:cnt.secondItem attribute:cnt.secondAttribute multiplier:cnt.multiplier constant:0];
//            fSavedPropertyHiddenConstraint = [NSLayoutConstraint constraintWithItem:cnt.firstItem attribute:cnt.firstAttribute relatedBy:cnt.relation toItem:cnt.secondItem attribute:cnt.secondAttribute multiplier:cnt.multiplier constant:cnt.constant];
            [self.propertyCustomView addConstraint: fPropertyHiddenConstraint];
        }
    }
    else {
        if(fSavedPropertyHiddenConstraint){
            [self.propertyCustomView removeConstraint:fPropertyHiddenConstraint];
            [self.propertyCustomView addSubview:fSavedPropertyInspector];
            [self.propertyCustomView addConstraints:fSavedPropertyHiddenConstraint];
//            self.propertyInspectorWidth = fSavedPropertyHiddenConstraint;
            fPropertyTabHidden = NO;
            fSavedPropertyHiddenConstraint = nil;
            fPropertyHiddenConstraint = nil;
            self.propertyInspector = fSavedPropertyInspector;
        }
    }
}
- (IBAction)showHideSettings:(id)sender {
    fPropertyTabHidden = !fPropertyTabHidden;
    //[self showHidePropertyBar];
}
- (IBAction)toggleNewMode:(id)sender {
    NSString* componentName = [fCreatableDictionary objectForKey: @(self.creatableSelector.indexOfSelectedItem)];
    if(!director::active_scene()->template_for_new()){
        if(!director::active_scene()->current()){
            [self alert:@"Can't add nodes directly to scene."];
            return;
        }
        director::active_scene()->enter_new_mode(type_descriptor::with_name(from_platform_string(componentName)));
    }
    else
        director::active_scene()->exit_new_mode();
}

- (IBAction)centerCameraOnSelection:(id)sender {
    director::active_scene()->center_camera_on_selection();
}

- (IBAction)setMoveView:(id)sender {
    director::active_scene()->navigation_mode(navigation_mode::scroll);
}

- (IBAction)deselectAllNodes:(id)sender {
    if(director::active_scene()->current()){
        director::active_scene()->current()->clear_selection();
    }
}

- (IBAction)setRotateView:(id)sender {
    director::active_scene()->navigation_mode(navigation_mode::rotate);
}

- (IBAction)setZoomView:(id)sender {
    director::active_scene()->navigation_mode(navigation_mode::zoom);
}

- (IBAction)resetZoom:(id)sender {
    transform_space _camera = director::active_scene()->camera();
    _camera.scale(vec2(1, 1));
    director::active_scene()->camera(_camera);
}

- (IBAction)resetCamera:(id)sender {
    transform_space _camera = transform_space(vec2::zero, vec2(1, 1), 0);
    director::active_scene()->camera(_camera);
}

- (IBAction)resetRotation:(id)sender {
    transform_space _camera = director::active_scene()->camera();
    _camera.rotation(0);
    director::active_scene()->camera(_camera);
}

- (IBAction)cancelSelectionNodesWithClass:(id)sender {
    [[NSApplication sharedApplication] endSheet:self.selectNodeWithClassWindow];
}

- (IBAction)select:(id)sender {
    BOOL allClasses = self.allClassesRadio.state == NSOnState;
    BOOL named = self.namedRadio.state == NSOnState;
    NSString* classes = self.classesField.stringValue;
    if(director::active_scene()->current()){
        if(!named){
            auto _nodes = allClasses ? director::active_scene()->node_with_all_classes(from_platform_string(classes)) :
                director::active_scene()->node_with_one_class(from_platform_string(classes));
            std::remove_if(_nodes.begin(), _nodes.end(), [](node* n){ return n->parent() != director::active_scene()->current(); });
            director::active_scene()->current()->clear_selection();
            for (auto _n : _nodes)
                director::active_scene()->current()->add_to_selection(_n);
        }
        else {
            auto _n = director::active_scene()->node_with_name(from_platform_string(classes));
            if (_n && _n->parent() == director::active_scene()->current()){
                director::active_scene()->current()->clear_selection();
                director::active_scene()->current()->add_to_selection(_n);
            }
        }
    }
    [[NSApplication sharedApplication] endSheet:self.selectNodeWithClassWindow];
}

- (IBAction)removeFromSelection:(id)sender {
    BOOL allClasses = self.allClassesRadio.state == NSOnState;
    BOOL named = self.namedRadio.state == NSOnState;
    NSString* classes = self.classesField.stringValue;
    if(director::active_scene()->current()){
        if(!named){
            auto _nodes = allClasses ? director::active_scene()->node_with_all_classes(from_platform_string(classes)) :
            director::active_scene()->node_with_one_class(from_platform_string(classes));
            std::remove_if(_nodes.begin(), _nodes.end(), [](node* n){ return n->parent() != director::active_scene()->current(); });
            for (auto _n : _nodes)
                director::active_scene()->current()->remove_from_selection(_n);
        }
        else {
            auto _n = director::active_scene()->node_with_name(from_platform_string(classes));
            if (_n && _n->parent() == director::active_scene()->current()){
                director::active_scene()->current()->remove_from_selection(_n);
            }
        }
    }
    [[NSApplication sharedApplication] endSheet:self.selectNodeWithClassWindow];
}

-(NSUInteger)countOfComponents{
    return [self.components count];
}
-(void)removeComponentsAtIndexes:(NSIndexSet *)indexes{
    [self.components removeObjectsAtIndexes:indexes];
}

-(id)objectInComponentsAtIndex:(NSUInteger)index{
    return [self.components objectAtIndex:index];
}

-(void)insertObject:(ComponentInfo *)object inComponentsAtIndex:(NSUInteger)index{
    [self.components insertObject:object atIndex:index];
}

-(void)removeObjectFromComponentsAtIndex:(NSUInteger)index{
    [self.components removeObjectAtIndex:index];
}

- (IBAction)removeDegenerated:(id)sender {
    auto _r = director::active_scene()->remove_degenerated();
    [self alert:[NSString stringWithFormat:@"%d Nodes Removed.", _r]];
}

-(void)delegateSelectionChanged:(void *)container {
    [self updateTransformationMenu];
    [self updateComponentsTab:YES];
}

-(void)delegateNameChanged:(void *)n{
    for (NSUInteger i = 0; i < self.components.count; i++) {
        ComponentInfo* ci = (ComponentInfo*)[self.components objectAtIndex:i];
        if(ci.tag == n){
            rb::node* _n = (rb::node*)n;
            [ci setName:from_name_to_platform_string(_n->name())];
            break;
        }
    }
    [self updateTransformationMenu];
    [self updateHierarchyTab];
}
-(void)delegateCreatableTypeWasRegistered:(void*)td{
    type_descriptor* _td = (type_descriptor*)td;
    [fCreatableDictionary setObject:to_platform_string(_td->name()) forKey:@(self.creatableSelector.itemArray.count)];
    [self.creatableSelector insertItemWithTitle:to_platform_string(_td->display_name()) atIndex:self.creatableSelector.itemArray.count];
}

- (IBAction)gotoLayer:(id)sender {
    NSInteger layerIdx = [(NSView*)sender tag];
    auto _layer = director::active_scene()->layer((uint32_t)layerIdx);
    if(_layer->in_editor_hidden()){
        [self alert:[NSString stringWithFormat:@"Layer %d is hidden. Unhide the layer first.", (uint32_t)layerIdx]];
    }
    else
        director::active_scene()->current(_layer);
}

-(void)delegateHierarchyChanged:(void *)container {
    [self updateTransformationMenu];
    [self updateComponentsTab:NO];
    [self updateHierarchyTab];
}
-(void)delegateCurrentChanged{
    [self updateTransformationMenu];
    [self updateComponentsTab:NO];
    [self updateHierarchyTab];
}
- (void) jumpBar:(GCJumpBar*) jumpBar didSelectItemAtIndexPath:(NSIndexPath*) indexPath{
    if(!director::active_scene())
        return;
    
    if(fSettingHierarchyTab)
        return;
    
    if([indexPath length] == 1) //scene selected
    {
        director::active_scene()->current(nullptr);
    }
    else if([indexPath length] == (fLastHierarchy.size() + 1)){ //selected last node...
        //do nothing...
    }
//    else if([indexPath length] == fLastHierarchy.size()){
//        director::active_scene()->current()->clear_selection();
//    }
    else {
        director::active_scene()->current(fLastHierarchy[indexPath.length - 2]);
    }
}
-(void)alert:(NSString*)message{
    if(fInAlertOrConfirmation){
        [fStackedAlertConfirmations addObject:message];
    }
    else {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        [alert setMessageText:message];
        [alert setInformativeText:@""];
        [alert setAlertStyle:NSCriticalAlertStyle];
        [NSOpenGLContext clearCurrentContext];
        [self.openGLView setRenderingEnabled:NO];
        [alert beginSheetModalForWindow:[[NSApplication sharedApplication] mainWindow] modalDelegate:self didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) contextInfo:nil];
        fInAlertOrConfirmation = YES;
    }
}

- (void)didEndSelectNode:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut:self];
}

- (IBAction)selectNodeWithClass:(id)sender {
    NSArray* topLevelObjects;
    [[NSBundle mainBundle] loadNibNamed: @"SelectNodeWithClass" owner: self topLevelObjects:&topLevelObjects];
    NSWindow* sheet;
    for (int i = 0; i < topLevelObjects.count; i++) {
        id obj = [topLevelObjects objectAtIndex:i];
        if([obj isKindOfClass:[NSWindow class]]){
            sheet = obj;
            break;
        }
    }
    [[NSApplication sharedApplication] beginSheet:sheet modalForWindow:[self window] modalDelegate:self didEndSelector:@selector(didEndSelectNode:returnCode:contextInfo:) contextInfo:NULL];
}

- (IBAction)groupObjects:(id)sender {
    if(!director::active_scene()->current()){
        [self alert:@"No node is current at this time."];
        return;
    }
    
    std::vector<node*> _selection;
    director::active_scene()->current()->fill_with_selection(_selection);
    
    if(_selection.size() <= 1){
        [self alert:@"You need to select at least two objects."];
        return;
    }
    
//    for (auto _n : _selection){
//        if(!_n->renderable()){
//            [self alert:[NSString stringWithFormat:@"Object %@ is not renderable.", from_name_to_platform_string(_n->name())]];
//            return;
//        }
//    }
    
    director::active_scene()->current()->group_selected();
}

- (IBAction)ungroupObjects:(id)sender {
    if(!director::active_scene()->current()){
        [self alert:@"No node is current at this time."];
        return;
    }
    
    std::vector<node*> _selection;
    director::active_scene()->current()->fill_with_selection(_selection);
    
    if(_selection.size() == 0){
        [self alert:@"No selected node."];
        return;
    }
    
    for (auto _n : _selection){
        if(!dynamic_cast<group_component*>(_n)){
            [self alert:[NSString stringWithFormat:@"Object %@ is not a group.", from_name_to_platform_string(_n->name())]];
            return;
        }
    }
    
    for (auto _n : _selection){
        dynamic_cast<group_component*>(_n)->ungroup(true);
    }
}

- (IBAction)moveUpHierarchy:(id)sender {
    if(!director::active_scene()->current()){
        [self alert:@"Can't move up in the node hierarchy."];
        return;
    }
    
    auto _current = director::active_scene()->current();
    if(dynamic_cast<node*>(_current)){
        auto _node = dynamic_cast<node*>(_current);
        if(_node->parent_node())
            director::active_scene()->current(_node->parent_node());
        else
            director::active_scene()->current(_node->parent_layer());
    }
    else {
        director::active_scene()->current(nullptr);
    }
}

- (IBAction)moveDownHierarchy:(id)sender {
    if(!director::active_scene()->current()){
        [self alert:@"Can't move down in the node hierarchy because the editor can't decide what layer to make current."];
        return;
    }
    
    std::vector<node*> _selection;
    director::active_scene()->current()->fill_with_selection(_selection);
    
    if(_selection.size() == 0){
        [self alert:@"No selected node."];
        return;
    }
    
    if(_selection.size() > 1){
        [self alert:@"More than one node selected."];
        return;
    }
    
    if(!_selection[0]->enabled(rb::node_capability::can_become_current) || !_selection[0]->renderable()){
        [self alert:[NSString stringWithFormat:@"Object %@ of type %@ cannot become current.", from_name_to_platform_string(_selection[0]->name()), to_platform_string(_selection[0]->displayable_type_name())]];
        return;
    }
    
    director::active_scene()->current(_selection[0]);
}

-(void)clearFlagsForNewScene {
    fPreviousSelection.clear();
    if(fCurrentMultipleNodes)
        delete fCurrentMultipleNodes;
    fCurrentMultipleNodes = nullptr;
    fChangingComponentsSelection = NO;
    fComponentsTabNeedsUpdate = YES;
    fSettingHierarchyTab = NO;
}

- (IBAction)newScene:(id)sender {
    [self confirm:@"Are you sure you want to create a new scene? The current scene will be lost if it wasn't saved." confirmationBlock:^(BOOL c) {
        if(c){
            director::active_scene(nullptr, true);
            director::active_scene(new scene(), true);
            [self clearFlagsForNewScene];
            fLastSavedScene = nil;
        }
    }];
}

- (IBAction)saveScene:(id)sender {
    if(fLastSavedScene){
        scene_loader::serialize_to_url(director::active_scene(), from_platform_string([fLastSavedScene path]));
        if(fSaveAndClose){
            fSaveAndClose = NO;
            [[NSApplication sharedApplication] terminate:self];
        }
    }
    else
        [self saveSceneAs:self];
}

- (IBAction)saveSceneAs:(id)sender {
    [NSOpenGLContext clearCurrentContext];
    [self.openGLView setRenderingEnabled:NO];
    [fSceneSavePanel beginSheetModalForWindow:[[NSApplication sharedApplication] mainWindow] completionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelCancelButton){
            [[self.openGLView openGLContext] makeCurrentContext];
            [self.openGLView setRenderingEnabled:YES];
            fSaveAndClose = NO;
            return;
        }
        fLastSavedScene = [fSceneSavePanel URL];
        [[self.openGLView openGLContext] makeCurrentContext];
        [self.openGLView setRenderingEnabled:YES];
        scene_loader::serialize_to_url(director::active_scene(), from_platform_string([fLastSavedScene path]));
        [self updateRecentFilesListMenu:[fLastSavedScene path]];
        if(fSaveAndClose){
            fSaveAndClose = NO;
            [[NSApplication sharedApplication] terminate:self];
        }
    }];
}

- (void) openSceneFromURL:(NSURL*)url{
    director::active_scene(nullptr, true);
    [self clearFlagsForNewScene];
    auto _loaded_scene = (scene*)scene_loader::deserialize_from_url(from_platform_string([url path]));
    director::active_scene(_loaded_scene, true);
    fLastSavedScene = url;
}

- (IBAction)openScene:(id)sender {
    [NSOpenGLContext clearCurrentContext];
    [self.openGLView setRenderingEnabled:NO];
    [fSceneOpenPanel beginSheetModalForWindow:[[NSApplication sharedApplication] mainWindow] completionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelCancelButton){
            [[self.openGLView openGLContext] makeCurrentContext];
            [self.openGLView setRenderingEnabled:YES];
            return;
        }
        NSURL* fileNameURL = [[fSceneOpenPanel URLs] lastObject];
        [[self.openGLView openGLContext] makeCurrentContext];
        [self.openGLView setRenderingEnabled:YES];
        
        [self openSceneFromURL:fileNameURL];
        [self updateRecentFilesListMenu:[fileNameURL path]];
    }];
}

- (IBAction)showSceneInFinder:(id)sender {
    if(fLastSavedScene){
        NSArray *fileURLs = [NSArray arrayWithObjects:fLastSavedScene, /* ... */ nil];
        [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];
    }
}

- (IBAction)showAtlasInFinder:(id)sender {
    if(director::active_scene()->atlas()){
        NSString* atlasPath = [to_platform_string(director::active_scene()->atlas_path()) stringByExpandingTildeInPath];
        NSArray *fileURLs = [NSArray arrayWithObjects:[NSURL fileURLWithPath:atlasPath isDirectory:YES], /* ... */ nil];
        [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];
    }
}

- (IBAction)bringToFront:(id)sender {
    [self reorderNodeWithSelector: 0];
}

- (IBAction)bringFrontward:(id)sender {
    [self reorderNodeWithSelector: 1];
}

- (IBAction)sendBackward:(id)sender {
    [self reorderNodeWithSelector: 2];
}

- (IBAction)sendToBack:(id)sender {
    [self reorderNodeWithSelector: 3];
}

- (IBAction)matchToCurrent:(id)sender {
    director::active_scene()->camera_to_match_current_rotation();
}

- (void)saveAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode
        contextInfo:(void *)contextInfo{
    [[alert window] close];
    if (returnCode == NSAlertFirstButtonReturn) {
        fSaveAndClose = YES;
        [self saveScene:self];
    }
    else if(returnCode == NSAlertSecondButtonReturn){
        [[NSApplication sharedApplication] terminate:self];
    }
    else {
        //Do nothing
    }
}

- (IBAction)quitApplication:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Save"];
    [alert addButtonWithTitle:@"Don't Save"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText:@"Save the current scene before closing?"];
    [alert setInformativeText:@"If not saved, some changes may be lost."];
    [alert setAlertStyle:NSWarningAlertStyle];
    [alert beginSheetModalForWindow:[[NSApplication sharedApplication] mainWindow] modalDelegate:self didEndSelector:@selector(saveAlertDidEnd:returnCode:contextInfo:) contextInfo:nil];
}

- (IBAction)blockNode:(id)sender {
    std::vector<node*> _selected;
    if(director::active_scene()->current()){
        director::active_scene()->current()->fill_with_selection(_selected);
        for (auto _n : _selected) {
            if(!_n->renderable())
                continue;
            _n->blocked(true);
        }
    }
}

- (IBAction)unblockAllNodes:(id)sender {
    director::active_scene()->unblock_all();
}

- (IBAction)unblockAllNodesCurrent:(id)sender {
    if(director::active_scene()->current()){
        director::active_scene()->current()->unblock_all_children();
    }
}

- (IBAction)unblockAllNodesCurrentLayer:(id)sender {
    if(director::active_scene()->current()){
        if(dynamic_cast<layer*>(director::active_scene()->current())){
            director::active_scene()->current()->unblock_all_children();
            return;
        }
        auto _n = dynamic_cast<node*>(director::active_scene()->current());
        _n->parent_layer()->unblock_all_children();
    }
}

- (IBAction)hideNode:(id)sender {
    std::vector<node*> _selected;
    if(director::active_scene()->current()){
        director::active_scene()->current()->fill_with_selection(_selected);
        for (auto _n : _selected) {
            if(!_n->renderable())
                continue;
            _n->in_editor_hidden(true);
        }
    }
}

- (IBAction)hideCurrentNode:(id)sender {
    if(director::active_scene()->current()){
        director::active_scene()->current()->in_editor_hidden(true);
    }
}

- (IBAction)unhideAllNodes:(id)sender {
    director::active_scene()->unhide_all();
}

- (IBAction)unhideNodesOnThisLayer:(id)sender {
    if(director::active_scene()->current()){
        if(dynamic_cast<layer*>(director::active_scene()->current())){
            director::active_scene()->current()->in_editor_hidden(false);
            director::active_scene()->current()->unhide_all_children();
            return;
        }
        auto _n = dynamic_cast<node*>(director::active_scene()->current());
        _n->parent_layer()->in_editor_hidden(false);
        _n->parent_layer()->unhide_all_children();
    }
}

- (IBAction)unhideNodesOnCurrent:(id)sender {
    if (director::active_scene()->current()){
        director::active_scene()->current()->in_editor_hidden(false);
        director::active_scene()->current()->unhide_all_children();
    }
}

- (IBAction)unhideAllLayers:(id)sender {
    for (uint32_t i = 0; i < MAX_LAYERS; i++) {
        director::active_scene()->layer(i)->in_editor_hidden(false);
    }
}

- (IBAction)alignHorCenter:(id)sender {
    [self alignNodeWithSelector:0];
}

- (IBAction)alignHorLeft:(id)sender {
    [self alignNodeWithSelector:1];
}

- (IBAction)alignHorRight:(id)sender {
    [self alignNodeWithSelector:2];
}

- (IBAction)alignVerCenter:(id)sender {
    [self alignNodeWithSelector:3];
}

- (IBAction)alignVerTop:(id)sender {
    [self alignNodeWithSelector:4];
}

- (IBAction)alignVerBottom:(id)sender {
    [self alignNodeWithSelector:5];
}

- (IBAction)bringLayerFront:(id)sender {
    uint32_t l = (uint32_t)[(NSView*)sender tag];
    director::active_scene()->send_to_back(l);
}

- (IBAction)bringLayerForward:(id)sender {
    uint32_t l = (uint32_t)[(NSView*)sender tag];
    director::active_scene()->send_backward(l);
}

- (IBAction)sendLayerBackward:(id)sender {
    uint32_t l = (uint32_t)[(NSView*)sender tag];
    director::active_scene()->bring_forward(l);
}

- (IBAction)sendLayerBack:(id)sender {
    uint32_t l = (uint32_t)[(NSView*)sender tag];
    director::active_scene()->bring_to_front(l);
}

- (IBAction)addToSelection:(id)sender {
    BOOL allClasses = self.allClassesRadio.state == NSOnState;
    BOOL named = self.namedRadio.state == NSOnState;
    NSString* classes = self.classesField.stringValue;
    if(director::active_scene()->current()){
        if(!named){
            auto _nodes = allClasses ? director::active_scene()->node_with_all_classes(from_platform_string(classes)) :
            director::active_scene()->node_with_one_class(from_platform_string(classes));
            std::remove_if(_nodes.begin(), _nodes.end(), [](node* n){ return n->parent() != director::active_scene()->current(); });
            for (auto _n : _nodes)
                director::active_scene()->current()->add_to_selection(_n);
        }
        else {
            auto _n = director::active_scene()->node_with_name(from_platform_string(classes));
            if (_n && _n->parent() == director::active_scene()->current()){
                director::active_scene()->current()->add_to_selection(_n);
            }
        }
    }
    [[NSApplication sharedApplication] endSheet:self.selectNodeWithClassWindow];
}

-(void)showAlertConfirmationFromStack {
    if(!fInAlertOrConfirmation && fStackedAlertConfirmations.count != 0){
        id obj = [fStackedAlertConfirmations objectAtIndex:0];
        if([obj isKindOfClass:[NSString class]])
        {
            [fStackedAlertConfirmations removeObjectAtIndex:0];
            [self alert:obj];
        }
        else {
            ConfirmationInfo* info = obj;
            [fStackedAlertConfirmations removeObjectAtIndex:0];
            [self confirm:info.message confirmationBlock:info.block];
        }
    }
}

- (void)alertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo{
    [[alert window] close];
    [[self.openGLView openGLContext] makeCurrentContext];
    [self.openGLView setRenderingEnabled:YES];
    fInAlertOrConfirmation = NO;
}

-(void)confirm:(NSString*)message confirmationBlock:(ConfirmationBlock)block{
    if (fInAlertOrConfirmation){
        ConfirmationInfo* info = [[ConfirmationInfo alloc] init];
        info.message = message;
        info.block = block;
        [fStackedAlertConfirmations addObject: info];
    }
    else {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"Yes"];
        [alert addButtonWithTitle:@"No"];
        [alert setMessageText:message];
        [alert setInformativeText:@""];
        [alert setAlertStyle:NSCriticalAlertStyle];
        [NSOpenGLContext clearCurrentContext];
        [self.openGLView setRenderingEnabled:NO];
        [alert beginSheetModalForWindow:[[NSApplication sharedApplication] mainWindow] modalDelegate:self didEndSelector:@selector(confirmDidEnd:returnCode:contextInfo:) contextInfo:(__bridge_retained void*)block];
        fInAlertOrConfirmation = YES;
    }
}

- (IBAction)lockSelection:(id)sender {
    director::active_scene()->lock_selection();
}

- (IBAction)unlockSelection:(id)sender {
    director::active_scene()->unlock_selection();
}
- (void)confirmDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo{
    ConfirmationBlock block = (__bridge_transfer ConfirmationBlock)contextInfo;
    [[self.openGLView openGLContext] makeCurrentContext];
    [self.openGLView setRenderingEnabled:YES];
    block(returnCode == NSAlertFirstButtonReturn);
    fInAlertOrConfirmation = NO;
}
-(void)delegatePushObjectInPropertyInspector:(void *)obj withTitle:(NSString *)title{
    typed_object* _obj = (typed_object*)obj;
    id<CppBridgeProtocol> _bridge = _obj->get_bridge();
    if(!_bridge){
        _bridge = [[CppBridge alloc] initWithCppObject: _obj];
        _obj->set_bridge(_bridge);
    }
    [self.propertyInspector pushObject:_bridge withTitle:title];
}
@end















