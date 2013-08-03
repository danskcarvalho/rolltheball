//
//  PropertyInspectorCell.m
//  CocoaTest
//
//  Created by Danilo Carvalho on 29/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "PropertyInspectorCell.h"
#include "Constants.h"
#import "CustomTextField.h"
#import "SnappingSlider.h"
#import "ImagePicker.h"
#import "TiledBackgroundImage.h"
#import "CustomImageView.h"
#import "BFPopoverColorWell.h"
#import "StackPanel.h"
#import "CustomLabel.h"
#import "CustomComboBox.h"
#import "CustomSegmentedControl.h"

@interface PropertyInspectorCell ()
{
    enum PropertyInspectorCellType pict;
    NSTextField* textField;
    NSTextField* secondTextField;
    NSSlider* slider;
    NSTextField* label;
    NSColorWell* color;
    NSButton* nullButton;
    NSButton* disclosureButton;
    NSButton* yesButton;
    NSButton* noButton;
    NSButton* button;
    NSComboBox* comboBox;
    NSSegmentedControl* segmented;
    NSView* alignmentView;
    NSImageView* image;
    NSPopUpButton* popUpButton;
    NSArray* emptyArray;
    NSColor* fBackgroundColor;
    NSFont* fFont;
    NSString* fTitle;
    NSString* fStringValue;
    NSColor* fTextColor;
    NSNumber* fNumberValue;
    NSNumber* fSecondNumberValue;
    NSColor* fColorValue;
    NSUInteger fMinimumNumericPrecision;
    NSUInteger fMaximumNumericPrecision;
    BOOL fContinuous;
    BOOL fEnabled;
    BOOL fShowDisclosure;
    BOOL fCollapsed;
    NSLayoutConstraint* fHeightConstraint;
    enum PropertyInspectorCellBorder fBorder;
    CGFloat fBorderWidth;
    NSUInteger fLevel;
    NSColor* fBorderColor;
    NSUInteger fPrecision;
    NSArray* fItems;
    NSFont * fTitleFont;
    BOOL fShowNullButton;
    NSImage* fImageValue;
    TiledBackgroundImage* tiledBackground;
    BOOL fDisclosureState;
    BOOL fDisableActionCallback;
}
-(void)resetCell;
-(void)setTextCell;
-(void)setBoolCell;
-(void)setRangedCell;
-(void)setColorCell;
-(void)setEnumCell;
-(void)setLabelCell;
-(void)setFloatPairCell;
-(void)setImageCell;
-(void)setButtonCell;
-(void)setComboboxCell;
-(void)setFlagsCell;
+(NSLayoutAttribute)layoutAttributeFromString:(NSString*)value;
+(NSLayoutRelation)relationFromString:(NSString*)value;
-(void)addFastConstraintWithView1:(NSView*)v1 attr1:(NSString*)attr1 relation:(NSString*)rel view2:(NSView*)v2 attr2:(NSString*)attr2 multiplier: (CGFloat)multiplier constant: (CGFloat)constant priority: (NSLayoutPriority)priority;
-(NSLayoutConstraint*)fastConstraintWithView1:(NSView *)v1 attr1:(NSString *)attr1 relation:(NSString *)rel view2:(NSView *)v2 attr2:(NSString *)attr2 multiplier:(CGFloat)multiplier constant:(CGFloat)constant priority:(NSLayoutPriority)priority;
-(void)updateConstraintsForTextCell;
-(void)updateConstraintsForBoolCell;
-(void)updateConstraintsForRangedCell;
-(void)updateConstraintsForColorCell;
-(void)updateConstraintsForEnumCell;
-(void)updateConstraintsForLabelCell;
-(void)updateConstraintsForFloatPairCell;
-(void)updateConstraintsForImageCell;
-(void)updateConstraintsForButtonCell;
-(void)updateConstraintsForFlagsCell;
-(void)updateConstraintsForComboboxCell;
-(void)setNullability:(id)sender;
-(void)textDidChange:(NSNotification *)notification;
-(void)textDidEndEditing:(NSNotification *)notification;
-(void)setYes:(id)sender;
-(void)setNo:(id)sender;
-(void)sliderValueChanged:(id)sender;
-(void)enumValueDidChange: (id)sender;
-(void)buttonWasClicked: (id)sender;
-(void)imageClicked: (id)sender;
-(void)setSliderValue: (float)value;
-(float)getSliderValue;
-(void)setComboboxItems:(id)sender;
-(void)flagsSegmentChanged:(id)sender;

//disclosure button clicked
-(void)disclosureButtonClicked:(id)sender;

//call action
-(void)callAction;

//property
@property (strong)NSColor* internalColorValue;
@end

float normalizeAngle(float angle){
    float _2_pi = M_PI * 2;
    while (angle >= _2_pi) {
        angle -= _2_pi;
    }
    return angle;
}

@implementation PropertyInspectorCell
-(void)setComboboxItems:(id)sender{
    NSMutableArray* mutableArray = [NSMutableArray array];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if(self.comboboxItemsTarget && self.comboboxItemsSelector)
        [self.comboboxItemsTarget performSelector:self.comboboxItemsSelector withObject:self withObject:mutableArray];
#pragma clang diagnostic pop
    [comboBox removeAllItems];
    [comboBox addItemsWithObjectValues:mutableArray];
}
-(void)flagsSegmentChanged:(id)sender{
    if(pict == pictFlags){
        long fl = 0;
        for (NSUInteger i = 0; i < segmented.segmentCount; i++) {
            if([segmented isSelectedForSegment:i])
                fl |= (1 << i);
        }
        fNumberValue = @(fl);
    }
    [self callAction];
}
-(BOOL)nullButtonState{
    return [nullButton state] == NSOnState;
}
-(void)callAction{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if(self.actionSelector && self.target && !fDisableActionCallback)
        [self.target performSelector:self.actionSelector withObject:self];
#pragma clang diagnostic pop
}
-(void)clicked{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if(self.clickSelector && self.clickTarget)
        [self.clickTarget performSelector:self.clickSelector withObject:self];
#pragma clang diagnostic pop
}
-(void)disclosureLabelClicked:(id)sender {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    
    if([disclosureButton state] == NSOnState){
        disclosureButton.state = NSOffState;
        fDisclosureState = NO;
    }
    else {
        disclosureButton.state = NSOnState;
        fDisclosureState = YES;
    }
    
    if (self.disclosureStateChangedSelector && self.disclosureStateChangedTarget)
        [self.disclosureStateChangedTarget performSelector:self.disclosureStateChangedSelector withObject:self];
#pragma clang diagnostic pop
}
-(void)disclosureButtonClicked:(id)sender{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if([disclosureButton state] == NSOnState)
        fDisclosureState = YES;
    else
        fDisclosureState = NO;
    
    if (self.disclosureStateChangedSelector && self.disclosureStateChangedTarget)
        [self.disclosureStateChangedTarget performSelector:self.disclosureStateChangedSelector withObject:self];
#pragma clang diagnostic pop
}

-(void)setSliderValue:(float)value{
    if(pict == pictRanged){
        [slider setFloatValue:value];
        return;
    }
    
    if(value != 0)
        value = 2 * M_PI - value;
    
    value += (M_PI / 2.0);
    value = normalizeAngle(value);
    [slider setFloatValue:value];
}
-(float)getSliderValue{
    if(pict == pictRanged){
        return [slider floatValue];
    }
    float value = [slider floatValue];
    if(value != 0)
        value = 2 * M_PI - value;
    
    value += M_PI / 2.0;
    value = normalizeAngle(value);
    return value;
}

-(void)setNullability:(id)sender{
    if(self.type == pictText || self.type == pictInt || self.type == pictFloat || self.type == pictFloatPair){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            if(self.type == pictText)
                [textField setStringValue:@""];
            else
                [textField setStringValue:@"0"];
            if(self.type == pictFloatPair){
                [secondTextField setStringValue:@"0"];
                [secondTextField setEnabled:YES];
            }
            
            [textField setEnabled:YES];
            fStringValue = @"";
            fNumberValue = @0;
            fSecondNumberValue = @0;
        }
        else {
            if(self.type == pictFloatPair){
                [secondTextField setStringValue:@""];
                [secondTextField setEnabled:NO];
            }
            
            [textField setEnabled:NO];
            [textField setStringValue:@""];
            fStringValue = nil;
            fNumberValue = nil;
            fSecondNumberValue = nil;
        }
    }
    else if(self.type == pictCombobox){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            [comboBox setStringValue:@""];
            [comboBox setEnabled:YES];
            fStringValue = @"";
        }
        else {
            [comboBox setStringValue:@""];
            [comboBox setEnabled:NO];
            fStringValue = nil;
        }
    }
    else if(self.type == pictFlags){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            for (NSUInteger i = 0; i < segmented.segmentCount; i++)
                [segmented setSelected:NO forSegment:i];
            [segmented setEnabled:YES];
            fNumberValue = @0;
        }
        else {
            for (NSUInteger i = 0; i < segmented.segmentCount; i++)
                [segmented setSelected:NO forSegment:i];
            [segmented setEnabled:NO];
            fNumberValue = nil;
        }
    }
    else if(self.type == pictButton){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            [button setEnabled:YES];
        }
        else {
            [button setEnabled:NO];
        }
    }
    else if(self.type == pictBool){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            [yesButton setEnabled:YES];
            [noButton setEnabled:YES];
            [yesButton setState:NSOffState];
            [noButton setState:NSOnState];
            fNumberValue = @0;
        }
        else {
            [yesButton setEnabled:NO];
            [noButton setEnabled:NO];
            [yesButton setState:NSOffState];
            [noButton setState:NSOffState];
            fNumberValue = nil;
        }
    }
    else if(self.type == pictRanged || self.type == pictAngle){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            [self setSliderValue:0];
            [slider setEnabled:YES];
            fNumberValue = @0;
        }
        else {
            [self setSliderValue:0];
            [slider setEnabled:NO];
            fNumberValue = nil;
        }
    }
    else if(self.type == pictColor){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            [color setColor:[NSColor colorWithDeviceRed:1 green:1 blue:1 alpha:1]];
            [color setEnabled:YES];
            fColorValue = [color color];
        }
        else {
            [color setColor:fBackgroundColor];
            [color setEnabled:NO];
            fColorValue = nil;
        }
    }
    else if(self.type == pictEnum){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            if([popUpButton indexOfItemWithTitle:@""] != -1)
                [popUpButton removeItemWithTitle:@""];
            [popUpButton setEnabled:YES];
            [popUpButton selectItemAtIndex:0];
            fStringValue = [popUpButton titleOfSelectedItem];
        }
        else {
            [popUpButton selectItemWithTitle:@""];
            [popUpButton setEnabled:NO];
            fStringValue = @"";
        }
    }
    else if(self.type == pictImage){
        NSUInteger s = [nullButton state];
        if(s == NSOnState){
            [image setImage:nil];
            [image setEnabled:YES];
            fImageValue = nil;
        }
        else {
            [image setImage:nil];
            [image setEnabled:NO];
            fImageValue = nil;
        }
    }
    
    [self callAction];
}
-(void)imageClicked:(id)sender{
    [ImagePicker showRelativeToRect:[sender bounds] ofView:sender preferredEdge:NSMinXEdge selectedImage:fImageValue block:^(NSImage *img) {
        fImageValue = img;
        [image setImage:fImageValue];
        if(!img){
            [nullButton setState:NSOffState];
            BOOL fPrevious = fDisableActionCallback;
            fDisableActionCallback = YES;
            [self setNullability:self];
            fDisableActionCallback = fPrevious;
        }
        else {
            [nullButton setState:NSOnState];
            [image setEnabled:YES];
        }
        [self callAction];
    }];
}
-(void)enumValueDidChange:(id)sender{
    if(pict == pictEnum){
        fStringValue = [popUpButton titleOfSelectedItem];
    }
    else { //pictLabel
        fStringValue = [popUpButton titleOfSelectedItem];
    }
    [self callAction];
}
-(void)buttonWasClicked:(id)sender{
    if(pict == pictButton){
        [self clicked];
    }
}
-(void)sliderValueChanged:(id)sender{
    fNumberValue = @([self getSliderValue]);
    [self callAction];
}
-(void)textDidChange:(id)sender{
    if(self.continuous){
        if(self.type == pictText){
            fStringValue = [textField stringValue];
        }
        else if(self.type == pictInt){
            NSNumberFormatter* savedFormatter = [textField formatter];
            textField.formatter = nil;
            fNumberValue = @([textField integerValue]);
            [textField setFormatter:savedFormatter];
        }
        else if(self.type == pictFloat){
            NSNumberFormatter* savedFormatter = [textField formatter];
            textField.formatter = nil;
            fNumberValue = @([textField floatValue]);
            [textField setFormatter:savedFormatter];
        }
        else {
            NSNumberFormatter* savedFormatter = [textField formatter];
            textField.formatter = nil;
            fNumberValue = @([textField floatValue]);
            [textField setFormatter:savedFormatter];
            
            savedFormatter = [secondTextField formatter];
            secondTextField.formatter = nil;
            fSecondNumberValue = @([secondTextField floatValue]);
            [secondTextField setFormatter:savedFormatter];
        }
        [self callAction];
    }
}
-(void)commitValues{
    if(self.type == pictText || self.type == pictInt || self.type == pictFloat || self.type == pictFloatPair)
        [self textDidEndEditing:nil];
    else if(self.type == pictCombobox){
        fStringValue = [comboBox stringValue];
        if (nullButton.state == NSOffState)
            fStringValue = nil;
        [self callAction];
    }
}
-(void)textDidEndEditing:(id)sender{
    if(self.type == pictText){
        fStringValue = [textField stringValue];
    }
    else if(self.type == pictInt){
        fNumberValue = @([textField integerValue]);
    }
    else if(self.type == pictFloat){
        fNumberValue = @([textField floatValue]);
    }
    else if(self.type == pictCombobox){
        fStringValue = [comboBox stringValue];
    }
    else {
        fNumberValue = @([textField floatValue]);
        fSecondNumberValue = @([secondTextField floatValue]);
    }
    if (nullButton.state == NSOffState){
        fStringValue = nil;
        fNumberValue = nil;
        fSecondNumberValue = nil;
    }
    [self callAction];
}
-(NSColor *)backgroundColor{
    return fBackgroundColor;
}
-(void)setBackgroundColor:(NSColor *)backgroundColor{
    fBackgroundColor = backgroundColor;
}

-(NSFont *)font{
    return fFont;
}

-(void)setFont:(NSFont *)font{
    fFont = font;
    if(label)
        label.font = font;
    if(nullButton)
        nullButton.font = font;
    if(textField)
        textField.font = font;
    if(popUpButton)
        popUpButton.font = font;
    if(yesButton)
        yesButton.font = font;
    if(noButton)
        noButton.font = font;
    if(secondTextField)
        secondTextField.font = font;
    if(button)
        button.font = font;
    if(comboBox)
        comboBox.font = font;
    if(segmented)
        segmented.font = font;
    [self setNeedsUpdateConstraints:YES];
}

-(BOOL)disclosureState{
    return fDisclosureState;
}

-(void)setDisclosureState:(BOOL)disclosureState{
    fDisclosureState = disclosureState;
    if(fDisclosureState)
        [disclosureButton setState:NSOnState];
    else if(!fDisclosureState)
        [disclosureButton setState:NSOffState];
}

-(NSFont *)titleFont{
    return fTitleFont;
}

-(void)setTitleFont:(NSFont *)titleFont{
    fTitleFont = titleFont;
    if(label && pict == pictLabel)
        label.font = fTitleFont;
    [self setNeedsUpdateConstraints:YES];
}

-(NSString *)title{
    return fTitle;
}

-(NSColor *)textColor{
    return fTextColor;
}

-(void)setTextColor:(NSColor *)textColor{
    fTextColor = textColor;
    if(textField)
        [textField setTextColor:fTextColor];
    if(label)
        [label setTextColor:fTextColor];
    if(secondTextField)
        [secondTextField setTextColor:fTextColor];
    if(comboBox)
        [comboBox setTextColor:fTextColor];
}

-(BOOL)continuous{
    return fContinuous;
}

-(void)setContinuous:(BOOL)continuous{
    fContinuous = continuous;
    if(slider)
        [slider setContinuous:fContinuous];
    if(color)
        [color setContinuous:fContinuous];
}

-(BOOL)enabled{
    return fEnabled;
}

-(void)setEnabled:(BOOL)enabled{
    fEnabled = enabled;
    if(disclosureButton){
        [disclosureButton setEnabled:fEnabled];
    }
    if(comboBox) {
        if(self.type == pictCombobox){
            if(!fEnabled || nullButton.state != NSOnState)
                [comboBox setEnabled:NO];
            else {
                [comboBox setEnabled:YES];
            }
        }
    }
    if(segmented){
        if(self.type == pictFlags){
            if(!fEnabled || nullButton.state != NSOnState)
                [segmented setEnabled:NO];
            else {
                [segmented setEnabled:YES];
            }
        }
    }
    if(textField){
        if(self.type == pictText || self.type == pictFloat || self.type == pictInt || self.type == pictFloatPair){
            if(!fEnabled || nullButton.state != NSOnState)
                [textField setEnabled:NO];
            else {
                [textField setEnabled:YES];
            }
        }
    }
    if(button){
        if(self.type == pictButton){
            if(!fEnabled || nullButton.state != NSOnState)
                [button setEnabled:NO];
            else
                [button setEnabled:YES];
        }
    }
    if(secondTextField){
        if(self.type == pictFloatPair){
            if(!fEnabled || nullButton.state != NSOnState)
                [secondTextField setEnabled:NO];
            else {
                [secondTextField setEnabled:YES];
            }
        }
    }
    if(yesButton){
        if(!fEnabled || nullButton.state != NSOnState)
            [yesButton setEnabled:NO];
        else {
            [yesButton setEnabled:YES];
        }
    }
    if(noButton){
        if(!fEnabled || nullButton.state != NSOnState)
            [noButton setEnabled:NO];
        else {
            [noButton setEnabled:YES];
        }
    }
    if(slider){
        if(!fEnabled || nullButton.state != NSOnState)
            [slider setEnabled:NO];
        else {
            [slider setEnabled:YES];
        }
    }
    if(label)
        [label setEnabled:fEnabled];
    if(color){
        if(!fEnabled || nullButton.state != NSOnState)
            [color setEnabled:NO];
        else {
            [color setEnabled:YES];
        }
    }
    if(nullButton)
        [nullButton setEnabled:fEnabled];
    if(image){
        if(!fEnabled || nullButton.state != NSOnState)
            [image setEnabled:NO];
        else {
            [image setEnabled:YES];
        }
    }
    if(popUpButton){
        if(pict == pictLabel){
            [popUpButton setEnabled:fEnabled];
        }
        else {
            if(!fEnabled || nullButton.state != NSOnState)
                [popUpButton setEnabled:NO];
            else {
                [popUpButton setEnabled:YES];
            }
        }
    }
}

-(void)setTitle:(NSString *)title{
    fTitle = title;
    if(label){
        if(title){
            [label setStringValue:title];
            [label setToolTip:title];
        }
        else{
            [label setStringValue:@""];
            [label setToolTip:@""];
        }
    }
}

-(NSImage *)imageValue{
    return fImageValue;
}

-(void)setImageValue:(NSImage *)imageValue{
    fImageValue = imageValue;
    if(image){
        [image setImage:imageValue];
        if(!fImageValue){
            [nullButton setState:NSOffState];
            BOOL fPrevious = fDisableActionCallback;
            fDisableActionCallback = YES;
            [self setNullability:self];
            fDisableActionCallback = fPrevious;
        }
        else {
            [nullButton setState:NSOnState];
            [image setEnabled:YES];
        }
    }
    [self setEnabled:fEnabled];
}

-(NSString *)stringValue{
    return fStringValue;
}

-(void)setStringValue:(NSString *)stringValue{
    fStringValue = stringValue;
    if(self.type == pictText){
        if(fStringValue) {
            if(fEnabled)
                [textField setEnabled:YES];
            [textField setStringValue:fStringValue];
            [nullButton setState:NSOnState];
        }
        else {
            if(fEnabled)
                [textField setEnabled:NO];
            [textField setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    else if(self.type == pictCombobox){
        if(fStringValue) {
            if(fEnabled)
                [comboBox setEnabled:YES];
            [comboBox setStringValue:fStringValue];
            [nullButton setState:NSOnState];
        }
        else {
            if(fEnabled)
                [comboBox setEnabled:NO];
            [comboBox setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    else if(self.type == pictButton){
        if(fStringValue){
            [button setTitle:fStringValue];
            if([fStringValue isEqualToString:@"<NULL>"]){
                if(fEnabled)
                    [button setEnabled:NO];
                [nullButton setState:NSOffState];
            }
            else {
                if(fEnabled)
                    [button setEnabled:YES];
                [nullButton setState:NSOnState];
            }
        }
        else
            [button setTitle:@""];
    }
    else if(self.type == pictEnum){
        if(fStringValue) {
            if(fEnabled)
                [popUpButton setEnabled:YES];
            if([popUpButton indexOfItemWithTitle:@""] != -1)
                [popUpButton removeItemWithTitle:@""];
            [popUpButton selectItemWithTitle:fStringValue];
            [nullButton setState:NSOnState];
        }
        else {
            if(fEnabled)
                [popUpButton setEnabled:NO];
            [popUpButton selectItemWithTitle:@""];
            [nullButton setState:NSOffState];
        }
    }
    [self setEnabled:fEnabled];
}

-(NSNumber *)integerValue{
    return fNumberValue;
}

-(void)setIntegerValue:(NSNumber *)integerValue{
    if(integerValue)
        fNumberValue = @([integerValue integerValue]);
    else
        fNumberValue = nil;
    if(self.type == pictInt){
        if(fNumberValue) {
            if(fEnabled)
                [textField setEnabled:YES];
            [textField setIntegerValue:[fNumberValue integerValue]];
            [nullButton setState:NSOnState];
        }
        else {
            if(fEnabled)
                [textField setEnabled:NO];
            [textField setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    else if(self.type == pictFlags){
        if(fNumberValue) {
            if(fEnabled)
                [segmented setEnabled:YES];
            NSInteger value = [fNumberValue integerValue];
            for (NSUInteger i = 0; i < segmented.segmentCount; i++){
                if (value & (1 << i))
                    [segmented setSelected:YES forSegment:i];
                else
                    [segmented setSelected:NO forSegment:i];
            }
            [nullButton setState:NSOnState];
        }
        else {
            if(fEnabled)
                [segmented setEnabled:NO];
            for (NSUInteger i = 0; i < segmented.segmentCount; i++)
                [segmented setSelected:NO forSegment:i];
            [nullButton setState:NSOffState];
        }
    }
    else if(self.type == pictBool){
        if(fNumberValue) {
            if(fEnabled){
                [yesButton setEnabled:YES];
                [noButton setEnabled:YES];
            }
            if([fNumberValue boolValue]){
                [yesButton setState:NSOnState];
                [noButton setState:NSOffState];
            }
            else {
                [yesButton setState:NSOffState];
                [noButton setState:NSOnState];
            }
            [nullButton setState:NSOnState];
        }
        else {
            if(fEnabled){
                [yesButton setEnabled:NO];
                [noButton setEnabled:NO];
            }
            [yesButton setState:NSOffState];
            [noButton setState:NSOffState];
            [nullButton setState:NSOffState];
        }
    }
    [self setEnabled:fEnabled];
}

-(NSNumber *)floatValue{
    return fNumberValue;
}

-(void)setFloatValue:(NSNumber *)floatValue{
    if(floatValue){
        if(self.type == pictRanged){
            if([floatValue floatValue] < 0)
                floatValue = @0;
            else if([floatValue floatValue] > 1)
                floatValue = @1;
        }
        else if(self.type == pictAngle){ //normalize angle
            float angleValue = [floatValue floatValue];
            if(angleValue > 0)
            {
                angleValue = angleValue - floorf(angleValue / (2 * M_PI)) * (2 * M_PI);
            }
            else if(angleValue < 0){
                angleValue = -angleValue;
                angleValue = angleValue - floorf(angleValue / (2 * M_PI)) * (2 * M_PI);
                angleValue = -angleValue;
                angleValue = (2 * M_PI) + angleValue;
            }
            floatValue = @(angleValue);
        }
    }
    
    if(floatValue)
        fNumberValue = @([floatValue floatValue]);
    else
        fNumberValue = nil;
    if(self.type == pictFloat || self.type == pictFloatPair){
        if(fNumberValue) {
            [textField setFloatValue:[fNumberValue floatValue]];
            [textField setEnabled:YES];
            if(self.type != pictFloatPair || fSecondNumberValue)
                [nullButton setState:NSOnState];
        }
        else {
            [textField setEnabled:NO];
            [textField setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    else if(self.type == pictRanged || self.type == pictAngle){
        if(fNumberValue){
            [self setSliderValue:[fNumberValue floatValue]];
            [slider setEnabled:YES];
            [nullButton setState:NSOnState];
        }
        else {
            [self setSliderValue:0];
            [slider setEnabled:NO];
            [nullButton setState:NSOffState];
        }
    }
    [self setEnabled:fEnabled];
}

-(NSNumber *)secondFloatValue{
    return fSecondNumberValue;
}

-(void)setSecondFloatValue:(NSNumber *)secondFloatValue{
    if(secondFloatValue)
        fSecondNumberValue = @([secondFloatValue floatValue]);
    else
        fSecondNumberValue = nil;
    if(self.type == pictFloatPair){
        if(fNumberValue) {
            [secondTextField setFloatValue:[fSecondNumberValue floatValue]];
            [secondTextField setEnabled:YES];
            if(fNumberValue)
                [nullButton setState:NSOnState];
        }
        else {
            [secondTextField setEnabled:NO];
            [secondTextField setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    [self setEnabled:fEnabled];
}

-(NSColor *)internalColorValue{
    return fColorValue;
}
-(void)setInternalColorValue:(NSColor *)internalColorValue{
    fColorValue = internalColorValue;
    [self callAction];
}

-(NSColor *)colorValue{
    return fColorValue;
}

-(void)setColorValue:(NSColor *)colorValue{
    fColorValue = [colorValue colorUsingColorSpace:[NSColorSpace deviceRGBColorSpace]];
    BOOL fPrevious = fDisableActionCallback;
    fDisableActionCallback = YES;
    self.internalColorValue = fColorValue;
    fDisableActionCallback = fPrevious;
    if(fColorValue){
        [color setEnabled:YES];
        [nullButton setState:NSOnState];
    }
    else {
        [color setColor: fBackgroundColor];
        [color setEnabled:NO];
        [nullButton setState:NSOffState];
    }
    [self setEnabled:fEnabled];
}

-(BOOL)showNullButton{
    return fShowNullButton;
}

-(void)setShowNullButton:(BOOL)showNullButton{
    fShowNullButton = showNullButton;
    if(nullButton)
        [nullButton setHidden:!fShowNullButton];
}

-(BOOL)showDisclosure{
    return fShowDisclosure;
}

-(void)setShowDisclosure:(BOOL)showDisclosure{
    fShowDisclosure = showDisclosure;
    if(disclosureButton)
        [disclosureButton setHidden:!fShowDisclosure];
    [self setNeedsUpdateConstraints:YES];
}

-(BOOL)collapsed{
    return fCollapsed;
}

-(void)setCollapsed:(BOOL)collapsed{
    if(fCollapsed == collapsed)
        return;
    fCollapsed = collapsed;
    [self setNeedsUpdateConstraints:YES];
    [self setNeedsLayout:YES];
}

-(NSUInteger)minimumNumericPrecision{
    return fMinimumNumericPrecision;
}

-(NSUInteger)maximumNumericPrecision{
    return fMaximumNumericPrecision;
}

-(void)setMinimumNumericPrecision:(NSUInteger)minimumNumericPrecision{
    fMinimumNumericPrecision = minimumNumericPrecision;
    if(pict == pictFloat){
        NSNumberFormatter* formatter = [textField formatter];
        [formatter setMinimumFractionDigits:fMinimumNumericPrecision];
    }
    else if(pict == pictFloatPair){
        NSNumberFormatter* formatter = [textField formatter];
        [formatter setMinimumFractionDigits:fMinimumNumericPrecision];
        formatter = [secondTextField formatter];
        [formatter setMinimumFractionDigits:fMinimumNumericPrecision];
    }
}

-(void)setMaximumNumericPrecision:(NSUInteger)maximumNumericPrecision{
    fMaximumNumericPrecision = maximumNumericPrecision;
    if(pict == pictFloat){
        NSNumberFormatter* formatter = [textField formatter];
        [formatter setMaximumFractionDigits:fMaximumNumericPrecision];
    }
    else if(pict == pictFloatPair){
        NSNumberFormatter* formatter = [textField formatter];
        [formatter setMaximumFractionDigits:fMaximumNumericPrecision];
        formatter = [secondTextField formatter];
        [formatter setMaximumFractionDigits:fMaximumNumericPrecision];
    }
}
-(NSUInteger)level{
    return fLevel;
}
-(void)setLevel:(NSUInteger)level{
    fLevel = level;
    [self needsUpdateConstraints];
}
-(enum PropertyInspectorCellBorder)border{
    return fBorder;
}
-(void)setBorder:(enum PropertyInspectorCellBorder)border{
    fBorder = border;
    [self setNeedsDisplay:YES];
}
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
    if(tiledBackground)
        tiledBackground.borderColor = fBorderColor;
    [self setNeedsDisplay:YES];
}

+(NSLayoutAttribute)layoutAttributeFromString:(NSString *)value{
    value = [value lowercaseString];
    if([value isEqualToString: @"left"])
        return NSLayoutAttributeLeft;
    else if([value isEqualToString: @"right"])
        return NSLayoutAttributeRight;
    else if([value isEqualToString: @"top"])
        return NSLayoutAttributeTop;
    else if([value isEqualToString: @"bottom"])
        return NSLayoutAttributeBottom;
    else if([value isEqualToString: @"leading"])
        return NSLayoutAttributeLeading;
    else if([value isEqualToString: @"trailing"])
        return NSLayoutAttributeTrailing;
    else if([value isEqualToString: @"width"])
        return NSLayoutAttributeWidth;
    else if([value isEqualToString: @"height"])
        return NSLayoutAttributeHeight;
    else if([value isEqualToString: @"centerx"])
        return NSLayoutAttributeCenterX;
    else if([value isEqualToString: @"centery"])
        return NSLayoutAttributeCenterY;
    else if([value isEqualToString: @"baseline"])
        return NSLayoutAttributeBaseline;
    else
        return NSLayoutAttributeNotAnAttribute;
}
+(NSLayoutRelation)relationFromString:(NSString *)value{
    if([value isEqualToString: @"=="])
        return NSLayoutRelationEqual;
    else if([value isEqualToString: @">="])
        return NSLayoutRelationGreaterThanOrEqual;
    else
        return NSLayoutRelationLessThanOrEqual;
}
-(void)addFastConstraintWithView1:(NSView *)v1 attr1:(NSString *)attr1 relation:(NSString *)rel view2:(NSView *)v2 attr2:(NSString *)attr2 multiplier:(CGFloat)multiplier constant:(CGFloat)constant priority:(NSLayoutPriority)priority{
    NSLayoutConstraint* constraint = [NSLayoutConstraint constraintWithItem:v1 attribute:[PropertyInspectorCell layoutAttributeFromString:attr1] relatedBy:[PropertyInspectorCell relationFromString:rel] toItem:v2 attribute:[PropertyInspectorCell layoutAttributeFromString:attr2] multiplier:multiplier constant:constant];
    constraint.priority = priority;
    [self addConstraint: constraint];
}
-(NSLayoutConstraint*)fastConstraintWithView1:(NSView *)v1 attr1:(NSString *)attr1 relation:(NSString *)rel view2:(NSView *)v2 attr2:(NSString *)attr2 multiplier:(CGFloat)multiplier constant:(CGFloat)constant priority:(NSLayoutPriority)priority{
    NSLayoutConstraint* constraint = [NSLayoutConstraint constraintWithItem:v1 attribute:[PropertyInspectorCell layoutAttributeFromString:attr1] relatedBy:[PropertyInspectorCell relationFromString:rel] toItem:v2 attribute:[PropertyInspectorCell layoutAttributeFromString:attr2] multiplier:multiplier constant:constant];
    constraint.priority = priority;
    return constraint;
}
-(void)resetCell{
    if(!emptyArray)
        emptyArray = [NSArray array];
    if (color)
        [color unbind:@"value"];
    
    [self setSubviews: emptyArray];
    
    if(pict == pictText || pict == pictFloat || pict == pictInt)
        [self setTextCell];
    else if(pict == pictBool)
        [self setBoolCell];
    else if(pict == pictAngle || pict == pictRanged)
        [self setRangedCell];
    else if(pict == pictColor)
        [self setColorCell];
    else if(pict == pictEnum)
        [self setEnumCell];
    else if(pict == pictLabel)
        [self setLabelCell];
    else if(pict == pictFloatPair)
        [self setFloatPairCell];
    else if(pict == pictImage)
        [self setImageCell];
    else if(pict == pictButton)
        [self setButtonCell];
    else if(pict == pictCombobox)
        [self setComboboxCell];
    else if(pict == pictFlags)
        [self setFlagsCell];
    
    [nullButton setHidden:!fShowNullButton];
    if(fDisclosureState)
        [disclosureButton setState:NSOnState];
    else
        [disclosureButton setState:NSOffState];
    
    [disclosureButton setAction:@selector(disclosureButtonClicked:)];
    [disclosureButton setTarget:self];
}

-(void)setTextCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    tiledBackground = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    textField = [[CustomTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [textField setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [[textField cell] setAllowsUndo:NO];
    
    [textField setContentHuggingPriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [textField setEditable:YES];
    [label setEditable:NO];
    [textField setSelectable:YES];
    [label setSelectable:NO];
    [textField setBezeled:YES];
    [label setBezeled:NO];
    [textField setBezelStyle:NSTextFieldSquareBezel];
    [textField setBordered:YES];
    [[textField cell] setBezeled:YES];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [textField setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    [[textField cell] setScrollable:YES];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:fTitle];
    }
    if(pict == pictText){
        if(fStringValue) {
            [textField setStringValue:fStringValue];
            [textField setEnabled:YES];
            [nullButton setState:NSOnState];
        }
        else {
            [textField setEnabled:NO];
            [textField setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    else if(pict == pictFloat){
        NSNumberFormatter* formatter = [NSNumberFormatter new];
        [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [formatter setNumberStyle:NSNumberFormatterDecimalStyle];
        [formatter setMaximumFractionDigits:fMaximumNumericPrecision];
        [formatter setMinimumFractionDigits:fMinimumNumericPrecision];
        [formatter setUsesGroupingSeparator:NO];
        [textField setFormatter: formatter];
        if(fNumberValue) {
            [textField setFloatValue:[fNumberValue floatValue]];
            [textField setEnabled:YES];
            [nullButton setState:NSOnState];
        }
        else {
            [textField setEnabled:NO];
            [textField setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    else if(pict == pictInt){
        NSNumberFormatter* formatter = [NSNumberFormatter new];
        [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [formatter setNumberStyle:NSNumberFormatterDecimalStyle];
        [formatter setMaximumFractionDigits:0];
        [formatter setMinimumFractionDigits:0];
        [formatter setUsesGroupingSeparator:NO];
        [textField setFormatter:formatter];
        if(fNumberValue) {
            [textField setIntegerValue:[fNumberValue integerValue]];
            [textField setEnabled:YES];
            [nullButton setState:NSOnState];
        }
        else {
            [textField setEnabled:NO];
            [textField setStringValue:@""];
            [nullButton setState:NSOffState];
        }
    }
    [label setTextColor:fTextColor];
    [textField setTextColor:fTextColor];
    
    [label setEnabled:fEnabled];
    if(textField.isEnabled)
        [textField setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [textField setTarget:self];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [self addSubview:textField];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setFloatPairCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    tiledBackground = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    alignmentView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    textField = [[CustomTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    secondTextField = [[CustomTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    alignmentView.identifier = @"alignmentView";
    textField.identifier = @"textField";
    secondTextField.identifier = @"secondTextField";
    label.identifier = @"label";
    nullButton.identifier = @"nullButton";
    disclosureButton.identifier = @"disclosureButton";
    
    [alignmentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [textField setTranslatesAutoresizingMaskIntoConstraints:NO];
    [secondTextField setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [[textField cell] setAllowsUndo:NO];
    [[secondTextField cell] setAllowsUndo:NO];
    
    [textField setContentHuggingPriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    [secondTextField setContentHuggingPriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [textField setEditable:YES];
    [secondTextField setEditable:YES];
    [label setEditable:NO];
    [textField setSelectable:YES];
    [secondTextField setSelectable:YES];
    [label setSelectable:NO];
    [textField setBezeled:YES];
    [secondTextField setBezeled:YES];
    [label setBezeled:NO];
    [textField setBezelStyle:NSTextFieldSquareBezel];
    [textField setBordered:YES];
    [[textField cell] setBezeled:YES];
    [secondTextField setBezelStyle:NSTextFieldSquareBezel];
    [secondTextField setBordered:YES];
    [[secondTextField cell] setBezeled:YES];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [textField setFont:fFont];
    [secondTextField setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    [[textField cell] setScrollable:YES];
    [[secondTextField cell] setScrollable:YES];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:@""];
    }
    
    NSNumberFormatter* formatter = [NSNumberFormatter new];
    [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [formatter setNumberStyle:NSNumberFormatterDecimalStyle];
    [formatter setMaximumFractionDigits:fMaximumNumericPrecision];
    [formatter setMinimumFractionDigits:fMinimumNumericPrecision];
    [formatter setUsesGroupingSeparator:NO];
    [textField setFormatter: formatter];
    [secondTextField setFormatter: formatter];
    if(fNumberValue) {
        [textField setFloatValue:[fNumberValue floatValue]];
        [textField setEnabled:YES];
        [nullButton setState:NSOnState];
    }
    else {
        [textField setEnabled:NO];
        [textField setStringValue:@""];
        [nullButton setState:NSOffState];
    }
    if(fSecondNumberValue) {
        [secondTextField setFloatValue:[fSecondNumberValue floatValue]];
        [secondTextField setEnabled:YES];
        if (fNumberValue)
            [nullButton setState:NSOnState];
    }
    else {
        [secondTextField setEnabled:NO];
        [secondTextField setStringValue:@""];
        [nullButton setState:NSOffState];
    }
    
    [label setTextColor:fTextColor];
    [textField setTextColor:fTextColor];
    [secondTextField setTextColor:fTextColor];
    
    [label setEnabled:fEnabled];
    if(textField.isEnabled)
        [textField setEnabled:fEnabled];
    if(secondTextField.isEnabled)
        [secondTextField setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [textField setTarget:self];
    [secondTextField setTarget:self];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [alignmentView addSubview:textField];
    [alignmentView addSubview:secondTextField];
    [self addSubview:alignmentView];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setEnumCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    tiledBackground = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    popUpButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [popUpButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [[popUpButton cell] setAllowsUndo:NO];
    
    [popUpButton setContentHuggingPriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    [popUpButton setContentCompressionResistancePriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [popUpButton setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:@""];
    }
    
    [label setTextColor:fTextColor];
    
    [label setEnabled:fEnabled];
    
    [popUpButton setPullsDown:NO];
    [popUpButton setAutoenablesItems:YES];
    [popUpButton setPreferredEdge:NSMaxYEdge];
    [[popUpButton cell] setAltersStateOfSelectedItem:YES];
    [popUpButton addItemsWithTitles:fItems];
    [self setStringValue:[self stringValue]];
    
    if(popUpButton.isEnabled)
        [popUpButton setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    [popUpButton setContinuous:fContinuous];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [popUpButton setTarget:self];
    [popUpButton setAction:@selector(enumValueDidChange:)];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [self addSubview:popUpButton];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setFlagsCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    tiledBackground = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    segmented = [[CustomSegmentedControl alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [segmented setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [segmented setSegmentStyle:NSSegmentStyleTexturedRounded];
    [[segmented cell] setTrackingMode:NSSegmentSwitchTrackingSelectAny];
    
    [segmented setContentHuggingPriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    [segmented setContentCompressionResistancePriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [segmented setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:@""];
    }
    
    [label setTextColor:fTextColor];
    
    [label setEnabled:fEnabled];
    
    [segmented setSegmentCount:fItems.count == 0 ? 1 : fItems.count];
    for (NSUInteger i = 0; fItems.count; i++) {
        [segmented setLabel:[fItems objectAtIndex:i] forSegment:i];
    }
    [self setIntegerValue:[self integerValue]];
    
    if(segmented.isEnabled)
        [segmented setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [segmented setTarget:self];
    [segmented setAction:@selector(flagsSegmentChanged:)];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [self addSubview:segmented];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setButtonCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    tiledBackground = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [button setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [button setButtonType:NSMomentaryPushButton];
    [button setBezelStyle:NSRoundedBezelStyle];
    [button setContentHuggingPriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    [button setContentCompressionResistancePriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [button setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:@""];
    }
    
    [label setTextColor:fTextColor];
    [label setEnabled:fEnabled];
    
    [self setStringValue:[self stringValue]];
    
    if(button.isEnabled)
        [button setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [button setTarget:self];
    [button setAction:@selector(buttonWasClicked:)];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [self addSubview:button];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setComboboxCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    tiledBackground = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    comboBox = [[CustomComboBox alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [comboBox setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [comboBox setContinuous:NO];
    [comboBox setCompletes:YES];
    [comboBox setButtonBordered:YES];
    NSComboBoxCell* cell = [comboBox cell];
    [cell setAlignment:NSLeftTextAlignment];
    [cell setBezelStyle:NSTextFieldSquareBezel];
    [cell setBordered:YES];
    [cell setBezeled:YES];
    [cell setSelectable:YES];
    [cell setScrollable:YES];
    [cell setEditable:YES];
    [comboBox setContentHuggingPriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    [comboBox setContentCompressionResistancePriority:1 forOrientation:NSLayoutConstraintOrientationHorizontal];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [comboBox setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:@""];
    }
    
    [label setTextColor:fTextColor];
    [label setEnabled:fEnabled];
    
    [comboBox addItemsWithObjectValues:fItems];
    [self setStringValue:[self stringValue]];
    
    if(comboBox.isEnabled)
        [comboBox setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [comboBox setTarget:self];
    [comboBox setAction:@selector(setComboboxItems:)];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [self addSubview:comboBox];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setImageCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    tiledBackground = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    image = [[CustomImageView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    tiledBackground = [[TiledBackgroundImage alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [image setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [tiledBackground setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [image setImageFrameStyle:NSImageFrameNone];
    [image setImageAlignment:NSImageAlignCenter];
    [image setImageScaling:NSImageScaleProportionallyDown];
    [image setEditable:NO];
    [image setAnimates:YES];
    [image setAllowsCutCopyPaste:NO];
    
    [tiledBackground setBorderColor:fBorderColor];
    [tiledBackground setBorderWidth:1];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:@""];
    }
    
    [label setTextColor:fTextColor];
    [label setEnabled:fEnabled];
    
    [self setImageValue:[self imageValue]];
    
    if(image.isEnabled)
        [image setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [image setTarget:self];
    [image setAction:@selector(imageClicked:)];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [tiledBackground addSubview:image];
    [self addSubview:tiledBackground];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setLabelCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    popUpButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[CustomLabel alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [popUpButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [[popUpButton cell] setAllowsUndo:NO];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setTarget:self];
    [label setAction:@selector(disclosureLabelClicked:)];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fTitleFont];
    [popUpButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSLeftTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle){
        [label setStringValue:fTitle];
        [label setToolTip:fTitle];
    }
    else{
        [label setStringValue:@""];
        [label setToolTip:@""];
    }
    
    [label setTextColor:fTextColor];
    [label setEnabled:fEnabled];
    
    [popUpButton setPullsDown:YES];
    [popUpButton setAutoenablesItems:YES];
    [popUpButton setPreferredEdge:NSMaxYEdge];
    [[popUpButton cell] setAltersStateOfSelectedItem:YES];
    if(fItems.count == 0){
        [popUpButton removeAllItems];
        [popUpButton setHidden:YES];
    }
    else {
        [popUpButton setHidden:NO];
        [popUpButton addItemsWithTitles:fItems];
        if(pict == pictLabel)
            [popUpButton insertItemWithTitle:@"" atIndex:0];
        [popUpButton setTitle:@"Actions"];
    }
    
    [popUpButton setEnabled:fEnabled];
    [popUpButton setContinuous:fContinuous];
    
    //actions
    [popUpButton setTarget:self];
    [popUpButton setAction:@selector(enumValueDidChange:)];
    
    [self addSubview:popUpButton];
    [self addSubview:label];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(NSArray *)items{
    return fItems;
}
-(void)setItems:(NSArray *)items{
    if(!items)
        items = [NSArray array];
    
    fItems = [NSArray arrayWithArray:items];
    
    if(popUpButton){
        if(pict == pictLabel && items.count == 0){
            [popUpButton removeAllItems];
            [popUpButton setHidden:YES];
        }
        else {
            [popUpButton setHidden:NO];
            [popUpButton removeAllItems];
            [popUpButton addItemsWithTitles:items];
            if(pict == pictEnum){
                if (fStringValue)
                    [popUpButton selectItemWithTitle:fStringValue];
                else
                    [popUpButton selectItemWithTitle:@""];
            }
            else {
                [popUpButton insertItemWithTitle:@"" atIndex:0];
                [popUpButton setTitle:@"Actions"];
            }
        }
    }
    
    if(comboBox){
        [comboBox removeAllItems];
        [comboBox addItemsWithObjectValues:items];
        if(fStringValue)
            [comboBox setStringValue:fStringValue];
        else
            [comboBox setStringValue:@""];
    }
    
    if(segmented){
        [segmented setSegmentCount:items.count == 0 ? 1 : items.count];
        
        for (NSUInteger i = 0; i < items.count; i++) {
            if(fNumberValue)
            {
                NSInteger value = [fNumberValue integerValue];
                if (value & (1 << i))
                    [segmented setSelected:YES forSegment:i];
                else
                    [segmented setSelected:NO forSegment:i];
            }
            else
                [segmented setSelected:NO forSegment:i];
            
            [segmented setLabel:[items objectAtIndex:i] forSegment:i];
        }
    }
}

-(NSUInteger)precision{
    return fPrecision;
}

-(void)setPrecision:(NSUInteger)precision{
    fPrecision = precision;
    if(slider)
        [slider setNumberOfTickMarks:precision];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setYes:(id)sender{
    [yesButton setState:NSOnState];
    [noButton setState:NSOffState];
    fNumberValue = @1;
    [self callAction];
}

-(void)setNo:(id)sender{
    [yesButton setState:NSOffState];
    [noButton setState:NSOnState];
    fNumberValue = @0;
    [self callAction];
}

-(void)setBoolCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    yesButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    noButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    alignmentView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [[yesButton cell] setAllowsUndo:NO];
    [[noButton cell] setAllowsUndo:NO];
    
    [alignmentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [yesButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [noButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [nullButton setFont:fFont];
    [yesButton setFont:fFont];
    [noButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    
    if(fTitle)
        [label setStringValue:fTitle];
    else
        [label setStringValue:@""];
    
    [label setTextColor:fTextColor];
    
    [self setIntegerValue:[self integerValue]];
    if (yesButton.isEnabled)
        [yesButton setEnabled:YES];
    if(noButton.isEnabled)
        [noButton setEnabled:YES];
    [label setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [yesButton setButtonType:NSRadioButton];
    [noButton setButtonType:NSRadioButton];
    [yesButton setTitle:@"Yes"];
    [noButton setTitle:@"No"];
    [yesButton setAction:@selector(setYes:)];
    [yesButton setTarget:self];
    [noButton setAction:@selector(setNo:)];
    [noButton setTarget:self];
    
    [alignmentView addSubview:yesButton];
    [alignmentView addSubview:noButton];
    [self addSubview:alignmentView];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)setRangedCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    slider = [[SnappingSlider alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    alignmentView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    slider.identifier = @"slider";
    label.identifier = @"label";
    nullButton.identifier = @"nullButton";
    disclosureButton.identifier = @"disclosureButton";
    alignmentView.identifier = @"alignmentView";
    self.identifier = @"cell";
    
    
    [alignmentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [slider setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [[slider cell] setAllowsUndo:NO];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [slider setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle)
        [label setStringValue:fTitle];
    else
        [label setStringValue:@""];
    
    //set up slider...
    if(pict == pictAngle){
        [[slider cell] setSliderType: NSCircularSlider];
        [slider setMinValue:0];
        [slider setMaxValue:M_PI * 2];
        [slider setNumberOfTickMarks: [self precision]];
    }
    else {
        [[slider cell] setSliderType: NSLinearSlider];
        [slider setMinValue:0];
        [slider setMaxValue:1];
        [slider setNumberOfTickMarks: [self precision]];
    }
    
    [self setFloatValue:[self floatValue]];
    
    [label setTextColor:fTextColor];
    if(slider.isEnabled)
        [slider setEnabled:fEnabled];
    [label setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [slider setAction:@selector(sliderValueChanged:)];
    [slider setTarget:self];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [slider setContinuous:fContinuous];
    
    [alignmentView addSubview:slider];
    [self addSubview:alignmentView];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    if ([keyPath isEqualToString:@"color"] && object == color) {
        [self changeColor:object];
    }
}

-(void)setColorCell{
    textField = nil;
    slider = nil;
    label = nil;
    color = nil;
    nullButton = nil;
    image = nil;
    popUpButton = nil;
    disclosureButton = nil;
    yesButton = nil;
    noButton = nil;
    alignmentView = nil;
    secondTextField = nil;
    button = nil;
    comboBox = nil;
    segmented = nil;
    
    //color = [[NSColorWell alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    color = [[BFPopoverColorWell alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    nullButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    disclosureButton = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    alignmentView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
    
    [alignmentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [color setTranslatesAutoresizingMaskIntoConstraints:NO];
    [label setTranslatesAutoresizingMaskIntoConstraints:NO];
    [nullButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    [disclosureButton setTranslatesAutoresizingMaskIntoConstraints:NO];
    id popoverColorWell = color;
    [popoverColorWell setPreferredEdgeForPopover:NSMinXEdge];
    [popoverColorWell setUseColorPanelIfAvailable:NO];
    
    [[color cell] setAllowsUndo:NO];
    
    [disclosureButton setButtonType:NSOnOffButton];
    [disclosureButton setBezelStyle:NSDisclosureBezelStyle];
    [disclosureButton setTitle:@""];
    [disclosureButton setHidden:!fShowDisclosure];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setBezeled:NO];
    [label setBordered:NO];
    [label setDrawsBackground:NO];
    [label setFont:fFont];
    [nullButton setFont:fFont];
    [label setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
    [label setAlignment:NSRightTextAlignment];
    [[label cell] setLineBreakMode:NSLineBreakByTruncatingMiddle];
    if(fTitle)
        [label setStringValue:fTitle];
    else
        [label setStringValue:@""];
    
    [label setTextColor:fTextColor];
    [label setEnabled:fEnabled];
    [nullButton setEnabled:fEnabled];
    
    //actions
    [nullButton setAction:@selector(setNullability:)];
    [nullButton setTarget:self];
    [color bind:@"value" toObject:self withKeyPath:@"internalColorValue" options:nil];
    [self setColorValue:[self colorValue]];
    [color setContinuous:fContinuous];
    
    if(color.isEnabled)
        [color setEnabled:fEnabled];
    
    [nullButton setButtonType:NSSwitchButton];
    [nullButton setTitle:@""];
    
    [alignmentView addSubview:color];
    [self addSubview:alignmentView];
    [self addSubview:label];
    [self addSubview:nullButton];
    [self addSubview:disclosureButton];
    [self setNeedsUpdateConstraints:YES];
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        fDisableActionCallback = NO;
        pict = pictText;
        textField = nil;
        slider = nil;
        label = nil;
        color = nil;
        nullButton = nil;
        image = nil;
        disclosureButton = nil;
        popUpButton = nil;
        yesButton = nil;
        noButton = nil;
        button = nil;
        comboBox = nil;
        segmented = nil;
        emptyArray = [NSArray array];
        fBackgroundColor = [NSColor controlColor];
        fFont = [NSFont systemFontOfSize:11];
        fTitle = nil;
        fStringValue = nil;
        fNumberValue = nil;
        fTextColor = [NSColor controlTextColor];
        fContinuous = NO;
        fEnabled = YES;
        fMaximumNumericPrecision = 6;
        fMinimumNumericPrecision = 0;
        fShowDisclosure = NO;
        fCollapsed = NO;
        fHeightConstraint = nil;
        fBorder = picbNone;
        fBorderWidth = 1;
        fLevel = 0;
        fBorderColor = [NSColor controlShadowColor];
        alignmentView = nil;
        fPrecision = 0;
        fColorValue = nil;
        fItems = [NSArray array];
        fTitleFont = [NSFont systemFontOfSize:11];
        fSecondNumberValue = nil;
        alignmentView = nil;
        fShowNullButton = YES;
        fImageValue = nil;
        tiledBackground = nil;
        self.comboboxItemsSelector = nil;
        self.comboboxItemsTarget = nil;
        [self resetCell];
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    if(pict == pictLabel){
        //purple
        NSColor* primaryColor = [NSColor colorWithDeviceRed:(229.0/255.0) green:(232.0/255.0) blue:(255.0/255.0) alpha:1];
        NSColor* secondaryColor = [NSColor colorWithDeviceRed:(199/255.0) green:(235/255.0) blue:(211/255.0) alpha:1];
        NSColor* groupColor = self.items.count != 0 ? secondaryColor : primaryColor;
        NSGradient* gradient = [[NSGradient alloc] initWithColors:@[groupColor, [groupColor shadowWithLevel:0.075]]];
        [gradient drawFromPoint:NSMakePoint(self.frame.size.width / 2, self.frame.size.height) toPoint:NSMakePoint(self.frame.size.width / 2, 0) options:NSGradientDrawsBeforeStartingLocation | NSGradientDrawsAfterEndingLocation];
        
        if (fBorder & picbTop) {
            NSSize size = [self frame].size;
            [fBorderColor set];
            NSRectFill(NSMakeRect(0, size.height - fBorderWidth, size.width, fBorderWidth));
        }
        
        if (fBorder & picbBottom) {
            NSSize size = [self frame].size;
            [fBorderColor set];
            NSRectFill(NSMakeRect(0, 0, size.width, fBorderWidth));
        }
    }
    else {
        [fBackgroundColor set];
        NSRectFill([self bounds]);
        
        if (fBorder & picbTop) {
            NSSize size = [self frame].size;
            [fBorderColor set];
            NSRectFill(NSMakeRect(0, size.height - fBorderWidth, size.width, fBorderWidth));
        }
        
        if (fBorder & picbBottom) {
            NSSize size = [self frame].size;
            [fBorderColor set];
            NSRectFill(NSMakeRect(0, 0, size.width, fBorderWidth));
        }
    }
}

-(enum PropertyInspectorCellType)type{
    return pict;
}
-(void)setType:(enum PropertyInspectorCellType)type{
    if(pict == type)
        return;
    pict = type;
    [self resetCell];
}


-(void)updateConstraints{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    if(!fCollapsed){
        if(pict == pictText || pict == pictFloat || pict == pictInt)
            [self updateConstraintsForTextCell];
        else if(pict == pictBool)
            [self updateConstraintsForBoolCell];
        else if(pict == pictAngle || pict == pictRanged)
            [self updateConstraintsForRangedCell];
        else if(pict == pictColor)
            [self updateConstraintsForColorCell];
        else if(pict == pictEnum)
            [self updateConstraintsForEnumCell];
        else if(pict == pictLabel)
            [self updateConstraintsForLabelCell];
        else if(pict == pictFloatPair)
            [self updateConstraintsForFloatPairCell];
        else if(pict == pictImage)
            [self updateConstraintsForImageCell];
        else if(pict == pictButton)
            [self updateConstraintsForButtonCell];
        else if(pict == pictCombobox)
            [self updateConstraintsForComboboxCell];
        else if(pict == pictFlags)
            [self updateConstraintsForFlagsCell];
    }
    
    [super updateConstraints];
}
-(void)layout{
    [super layout];
    BOOL _needsLayout = NO;
    if(fCollapsed){
        if(fHeightConstraint)
            [self removeConstraint:fHeightConstraint];
        fHeightConstraint = nil;
        [self addFastConstraintWithView1:self attr1:@"height" relation:@"==" view2:self attr2:@"width" multiplier:0 constant:0 priority:1000];
        _needsLayout = YES;
    }
    if(!fCollapsed && alignmentView){
        [self addFastConstraintWithView1:alignmentView attr1:@"width" relation:@">=" view2:self attr2:@"width" multiplier:0 constant:[yesButton frame].size.width + [noButton frame].size.width + HORIZONTAL_PADDING priority:NSLayoutPriorityRequired];
        _needsLayout = YES;
    }
    if(!fCollapsed && (slider || color)){
        if(slider){
            if(fHeightConstraint)
                [self removeConstraint:fHeightConstraint];
            
            if(slider.frame.size.height >= label.frame.size.height)
            {
                fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:slider attr2:@"height" multiplier:1 constant: 1.5 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
                [self addConstraint:fHeightConstraint];
            }
            else {
                fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant: 2.1 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
                [self addConstraint:fHeightConstraint];
            }
        }
        else if(color) {
            if(fHeightConstraint)
                [self removeConstraint:fHeightConstraint];
            
            //NSRect colorFrame = color.frame;
            if(color.frame.size.height >= label.frame.size.height)
            {
                fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:color attr2:@"height" multiplier:1 constant: 1.4 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
                [self addConstraint:fHeightConstraint];
            }
            else {
                fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant: 2.1 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
                [self addConstraint:fHeightConstraint];
            }
        }
        _needsLayout = YES;
    }
    if(_needsLayout)
        [super layout];
}
-(void)updateConstraintsForTextCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[textField(>=%d)]-%d-[nullButton]-%d-|", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH, HORIZONTAL_PADDING, MIN_CONTENT_WIDTH, HORIZONTAL_PADDING, HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, textField, nullButton, disclosureButton);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    [self addFastConstraintWithView1:textField attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant: 1.4 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForImageCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, image, tiledBackground, nullButton, disclosureButton);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    horizontalLayout = [NSString stringWithFormat: @"H:[nullButton]-%d-|", HORIZONTAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]-%d-[tiledBackground]-%d-|", VERTICAL_PADDING, VERTICAL_PADDING, VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    
    //more constraints regarding the image...
    [self addFastConstraintWithView1:tiledBackground attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:0 constant:120 priority:1000];
    [self addFastConstraintWithView1:tiledBackground attr1:@"width" relation:@">=" view2:self attr2:@"width" multiplier:0 constant:MIN_CONTENT_WIDTH priority:1000];
    [self addFastConstraintWithView1:tiledBackground attr1:@"height" relation:@"==" view2:tiledBackground attr2:@"width" multiplier:1 constant:0 priority:1000];
    [self addFastConstraintWithView1:tiledBackground attr1:@"left" relation:@"==" view2:label attr2:@"right" multiplier:1 constant:HORIZONTAL_PADDING priority:1000];
    [tiledBackground addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|-2-[image]-2-|" options:0 metrics:nil views:bindings]];
    [tiledBackground addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|-2-[image]-2-|" options:0 metrics:nil views:bindings]];
    
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"bottom" relation:@"==" view2:tiledBackground attr2:@"bottom" multiplier:1 constant: VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForEnumCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[popUpButton(>=%d)]-%d-[nullButton]-%d-|", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH, HORIZONTAL_PADDING, MIN_CONTENT_WIDTH, HORIZONTAL_PADDING, HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, popUpButton, nullButton, disclosureButton);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    [self addFastConstraintWithView1:popUpButton attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:popUpButton attr2:@"height" multiplier:1 constant: 1.4 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForComboboxCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[comboBox(>=%d)]-%d-[nullButton]-%d-|", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH, HORIZONTAL_PADDING, MIN_CONTENT_WIDTH, HORIZONTAL_PADDING, HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, comboBox, nullButton, disclosureButton);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    [self addFastConstraintWithView1:comboBox attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:comboBox attr2:@"height" multiplier:1 constant: 1.4 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForFlagsCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[segmented(>=%d)]-%d-[nullButton]-%d-|", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH, HORIZONTAL_PADDING, MIN_CONTENT_WIDTH, HORIZONTAL_PADDING, HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, segmented, nullButton, disclosureButton);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    [self addFastConstraintWithView1:segmented attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:segmented attr2:@"height" multiplier:1 constant: 1.4 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForButtonCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[button(>=%d)]-%d-[nullButton]-%d-|", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH, HORIZONTAL_PADDING, MIN_CONTENT_WIDTH, HORIZONTAL_PADDING, HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, button, nullButton, disclosureButton);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    [self addFastConstraintWithView1:button attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:button attr2:@"height" multiplier:1 constant: 1.4 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForLabelCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING - 5, MIN_LABEL_WIDTH];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, popUpButton, disclosureButton);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    horizontalLayout = [NSString stringWithFormat:@"H:[popUpButton(70)]-%d-|", HORIZONTAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    //[self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    NSString* verticalLayout = nil;
    if(popUpButton.itemArray.count == 0)
        verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING - 7];
    else
        verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING - 6];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    [self addFastConstraintWithView1:popUpButton attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:1 priority:NSLayoutPriorityRequired];
    
    if(popUpButton.itemArray.count == 0)
        fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant: VERTICAL_PADDING - 3 priority:NSLayoutPriorityRequired];
    else
        fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant: VERTICAL_PADDING priority:NSLayoutPriorityRequired];

    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForBoolCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[alignmentView]-%d-[nullButton]-%d-|",
                                  HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH,
                                  HORIZONTAL_PADDING,
                                  MIN_LABEL_WIDTH,
                                  HORIZONTAL_PADDING,
                                  HORIZONTAL_PADDING,
                                  HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, nullButton, disclosureButton, yesButton, noButton, alignmentView);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    horizontalLayout = [NSString stringWithFormat:@"H:|[yesButton(>=%d)]-%d-[noButton(>=%d)]", MIN_RADIO_WIDTH, HORIZONTAL_PADDING, MIN_RADIO_WIDTH];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    verticalLayout = [NSString stringWithFormat:@"V:|-%d-[alignmentView]|", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:yesButton attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:noButton attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[yesButton]" options:0 metrics:nil views:bindings]];
    
    //Height Constraint
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:yesButton attr2:@"height" multiplier:1 constant:  1.7 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForFloatPairCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[alignmentView]-%d-[nullButton]-%d-|",
                                  HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH,
                                  HORIZONTAL_PADDING,
                                  MIN_LABEL_WIDTH,
                                  HORIZONTAL_PADDING,
                                  HORIZONTAL_PADDING,
                                  HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, nullButton, disclosureButton, textField, secondTextField, alignmentView);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    horizontalLayout = [NSString stringWithFormat:@"H:|[textField(>=%d)]-%d-[secondTextField(>=%d)]|", MIN_CONTENT_WIDTH, HORIZONTAL_PADDING, MIN_CONTENT_WIDTH];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"[secondTextField(==textField)]" options:0 metrics:nil views:bindings]];
    //[self addFastConstraintWithView1:textField attr1:@"width" relation:@"==" view2:alignmentView attr2:@"width" multiplier:0.5 constant:-HORIZONTAL_PADDING priority:1000];
    
    NSString* verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    verticalLayout = [NSString stringWithFormat:@"V:|[alignmentView]|"];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:textField attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:secondTextField attr1:@"baseline" relation:@"==" view2:label attr2:@"baseline" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    
    //Height Constraint
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant:  1.4 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)updateConstraintsForRangedCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[alignmentView]-%d-[nullButton]-%d-|", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH, HORIZONTAL_PADDING, HORIZONTAL_PADDING, HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(self, label, slider, nullButton, disclosureButton, alignmentView);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];

    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];

    NSString* verticalLayout = nil;
    if(pict == pictRanged){
        verticalLayout = [NSString stringWithFormat:@"V:|-%d-[label]", VERTICAL_PADDING];
        [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    }
    else {
        [self addFastConstraintWithView1:label attr1:@"centerY" relation:@"==" view2:self attr2:@"centerY" multiplier:1 constant:0 priority:1000];
    }
    verticalLayout = [NSString stringWithFormat:@"V:|[alignmentView]|"];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    
    
    [self addFastConstraintWithView1:slider attr1:@"height" relation:@"==" view2:self attr2:@"height" multiplier:0 constant:[slider intrinsicContentSize].height priority:1000];
    
    if(pict == pictRanged) {
        if(self.precision > 0)
            [self addFastConstraintWithView1:slider attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
        else
            [self addFastConstraintWithView1:slider attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    }
    else
        [self addFastConstraintWithView1:slider attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    
    if(pict == pictRanged){
        int padding = self.precision == 0 ? 0 : 1;
        horizontalLayout = [NSString stringWithFormat:@"H:|-%d-[slider]-%d-|", padding, padding];
        [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    }
    
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant: 2.2 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
    //[[self window] visualizeConstraints:[self constraintsAffectingLayoutForOrientation:NSLayoutConstraintOrientationVertical]];
}
-(void)viewDidMoveToSuperview{
    if (color) {
        if(![self superview])
            [color unbind:@"value"];
        else{
            [color bind:@"value" toObject:self withKeyPath:@"internalColorValue" options:nil];
            [self setColorValue:[self colorValue]];
        }
    }
}
-(void)updateConstraintsForColorCell{
    [self removeConstraints:[self constraints]];
    fHeightConstraint = nil;
    NSString* horizontalLayout = [NSString stringWithFormat: @"H:|-%d-[disclosureButton]-%d-[label(>=%d)]-%d-[alignmentView]-%d-[nullButton]-%d-|", HORIZONTAL_PADDING + (int)fLevel * LEVEL_WIDTH, HORIZONTAL_PADDING, MIN_LABEL_WIDTH, HORIZONTAL_PADDING, HORIZONTAL_PADDING, HORIZONTAL_PADDING];
    NSDictionary* bindings = NSDictionaryOfVariableBindings(label, color, nullButton, disclosureButton, alignmentView);
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:horizontalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:label attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:LABEL_SHARE constant:-LABEL_SHARE * fLevel * LEVEL_WIDTH priority:1000];
    
    [self addFastConstraintWithView1:alignmentView attr1:@"width" relation:@">=" view2:self attr2:@"width" multiplier:0.70 constant:0 priority:NSLayoutPriorityDragThatCannotResizeWindow];
    
    NSString* verticalLayout = nil;
    [self addFastConstraintWithView1:label attr1:@"centerY" relation:@"==" view2:self attr2:@"centerY" multiplier:1 constant:0 priority:1000];
    
    verticalLayout = [NSString stringWithFormat:@"V:|[alignmentView]|"];
    [self addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:verticalLayout options:0 metrics:nil views:bindings]];
    
    [self addFastConstraintWithView1:color attr1:@"height" relation:@"==" view2:self attr2:@"height" multiplier:0 constant:23 priority:1000];
    [self addFastConstraintWithView1:color attr1:@"width" relation:@"==" view2:self attr2:@"width" multiplier:0 constant:44 priority:1000];
    
    [self addFastConstraintWithView1:color attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    
    [self addFastConstraintWithView1:nullButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    [self addFastConstraintWithView1:disclosureButton attr1:@"centerY" relation:@"==" view2:label attr2:@"centerY" multiplier:1 constant:0 priority:NSLayoutPriorityRequired];
    fHeightConstraint = [self fastConstraintWithView1:self attr1:@"height" relation:@"==" view2:label attr2:@"height" multiplier:1 constant: 2.2 * VERTICAL_PADDING priority:NSLayoutPriorityRequired];
    [self addConstraint:fHeightConstraint];
}
-(void)dealloc{
    if(color)
        [color unbind:@"value"];
}
-(NSView *)firstLoopableView{
    if(pict == pictAngle)
        return slider;
    else if(pict == pictBool)
        return yesButton;
    else if(pict == pictColor)
        return color;
    else if(pict == pictEnum)
        return popUpButton;
    else if(pict == pictFloat)
        return textField;
    else if(pict == pictFloatPair)
        return textField;
    else if(pict == pictImage)
        return image;
    else if(pict == pictInt)
        return textField;
    else if(pict == pictLabel)
        return label;
    else if(pict == pictRanged)
        return slider;
    else if(pict == pictCombobox)
        return comboBox;
    else //textField
        return textField;
}
-(void)setLoop:(NSView *)nextView{
    if(pict == pictAngle)
    {
        slider.nextKeyView = nextView;
    }
    else if(pict == pictBool)
    {
        yesButton.nextKeyView = noButton;
        noButton.nextKeyView = nextView;
    }
    else if(pict == pictColor)
    {
        color.nextKeyView = nextView;
    }
    else if(pict == pictEnum)
    {
        popUpButton.nextKeyView = nextView;
    }
    else if(pict == pictFloat)
    {
        textField.nextKeyView = nextView;
    }
    else if(pict == pictFloatPair)
    {
        textField.nextKeyView = secondTextField;
        secondTextField.nextKeyView = nextView;
    }
    else if(pict == pictImage)
    {
        image.nextKeyView = nextView;
    }
    else if(pict == pictInt)
    {
        textField.nextKeyView = nextView;
    }
    else if(pict == pictLabel)
    {
        label.nextKeyView = nextView;
    }
    else if(pict == pictRanged)
    {
        slider.nextKeyView = nextView;
    }
    else if(pict == pictCombobox)
    {
        comboBox.nextKeyView = nextView;
    }
    else //textField
    {
        textField.nextKeyView = nextView;
    }
}
@end







































