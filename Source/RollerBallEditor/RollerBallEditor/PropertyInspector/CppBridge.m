//
//  CppBridge.m
//  RollerBallEditor
//
//  Created by Danilo Carvalho on 28/02/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#import "CppBridge.h"
#import "ImagePicker.h"
#import "ConstructibleType.h"
#import "PropertyDescriptor.h"
#include <vector>

@interface CppBridge ()
{
    NSMutableArray* observers;
    rb::typed_object* cppObject;
    TypeDescriptor* cachedTypeDescriptor;
    BOOL fDeleted;
}
@end

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

inline rb::rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}


@implementation CppBridge
static NSMutableDictionary* fImageMap;
static NSMutableDictionary* fCreatedTypes;

+(void)initialize{
    if ([self class] == [CppBridge class]) {
        fImageMap = [NSMutableDictionary dictionary];
        fCreatedTypes = [NSMutableDictionary dictionary];
    }
}

+(void)textureAtlasChanged:(void *)_newAtlas{
    rb::texture_atlas* newAtlas = (rb::texture_atlas*)_newAtlas;
    [fImageMap removeAllObjects];
    [ImagePicker removeAllImages];
    std::vector<rb::rb_string> _texture_names;
    newAtlas->get_texture_names(_texture_names);
    for(auto& _txn : _texture_names){
        std::vector<rb::rb_string> _group_names;
        newAtlas->get_groups(_txn, _group_names);
        assert(_group_names.size() != 0);
        auto _t = newAtlas->get_atlas(_group_names[0], _txn);
        auto _b = newAtlas->get_bounds_in_pixels(_group_names[0], _txn);
        CGImageRef _img = (CGImageRef)_t->to_cg_image(_b);
        float _reductionFactor =  256.0f / _b.size().x();
        NSSize _thumbnailSize = NSMakeSize(256.0f, _b.size().y() * _reductionFactor);
        NSImage* _finalImg = [[NSImage alloc] initWithCGImage:_img size: _thumbnailSize];
        CGImageRelease(_img);
        [fImageMap setObject:_finalImg forKey:to_platform_string(_txn)];
        [ImagePicker addImage:_finalImg];
    }
}

-(id)initWithCppObject:(rb::typed_object *)object{
    if(self = [self init]){
        cppObject = object;
        observers = [NSMutableArray array];
        cachedTypeDescriptor = nil;
        fDeleted = NO;
    }
    return self;
}
-(void)callAction:(NSString *)property actionName:(NSString *)actionName{
    if(fDeleted)
        return;
    cppObject->type_descriptor()->call_action(cppObject, from_platform_string(property), from_platform_string(actionName));
}
-(NSArray *)getAutoCompleteSuggestions:(NSString *)property{
    if(fDeleted)
        return nil;
    NSMutableArray* array = [NSMutableArray array];
    auto _suggestions = cppObject->type_descriptor()->get_autocomplete_suggestions(cppObject, from_platform_string(property));
    for (auto& _s : _suggestions){
        [array addObject:to_platform_string(_s)];
    }
    return array;
}
-(void)getString:(NSString *__autoreleasing *)value property:(NSString *)property{
    if(fDeleted)
        return;
    auto _r = cppObject->type_descriptor()->get_nullable_string_property(cppObject, from_platform_string(property));
    if(!_r.has_value())
        *value = nil;
    else
        *value = to_platform_string(_r.value());
}
-(void)setString:(NSString *)value property:(NSString *)property{
    if(fDeleted)
        return;
    if(!value)
        cppObject->type_descriptor()->set_nullable_string_property(cppObject, from_platform_string(property), nullptr);
    else
        cppObject->type_descriptor()->set_nullable_string_property(cppObject, from_platform_string(property), from_platform_string(value));
}
-(void)getObject:(__autoreleasing id *)value property:(NSString *)property{
    if(fDeleted)
        return;
    if(cppObject->type_descriptor()->property_type(from_platform_string(property))->gtd() == rb::general_type_descriptor::vector){
        [self getVector:value property:property];
    }
    else {
        auto _r = cppObject->type_descriptor()->get_object_property<rb::typed_object>(cppObject, from_platform_string(property));
        if(_r){
            if(!_r->get_bridge()){
                CppBridge* bridge = [[CppBridge alloc] initWithCppObject:_r];
                _r->set_bridge(bridge);
            }
            *value = _r->get_bridge();
        }
        else
            *value = nil;
    }
}
-(void)setObject:(id)value property:(NSString *)property{
    if(fDeleted)
        return;
    if(value){
        CppBridge* bridge = value;
        cppObject->type_descriptor()->set_object_property(cppObject, from_platform_string(property), bridge->cppObject);
    }
    else
        cppObject->type_descriptor()->set_object_property(cppObject, from_platform_string(property), (rb::typed_object*)nullptr);
}
-(void)getVector:(NSArray *__autoreleasing *)value property:(NSString *)property{
    if(fDeleted)
        return;
    auto _r = cppObject->type_descriptor()->get_vector_property<rb::typed_object>(cppObject, from_platform_string(property));
    
    NSMutableArray* mutArray = [NSMutableArray array];
    for (auto _i : _r){
        if(!_i->get_bridge()){
            CppBridge* bridge = [[CppBridge alloc] initWithCppObject:_i];
            _i->set_bridge(bridge);
            [mutArray addObject:_i->get_bridge()];
        }
        else
            [mutArray addObject:_i->get_bridge()];
    }
    *value = mutArray;
}
-(void)setVector:(NSArray *)value property:(NSString *)property{
    if(fDeleted)
        return;
    std::vector<rb::typed_object*> _objs;
    for (CppBridge* _i in value){
        _objs.push_back(_i->cppObject);
    }
    cppObject->type_descriptor()->set_vector_property(cppObject, from_platform_string(property), _objs);
}
-(void)getBool:(NSNumber *__autoreleasing *)value property:(NSString *)property{
    if(fDeleted)
        return;
    auto _p = cppObject->type_descriptor()->get_nullable_boolean_property(cppObject, from_platform_string(property));
    if(_p.has_value())
        *value = @(_p.value());
    else
        *value = nil;
}
-(void)setBool:(BOOL)value property:(NSString *)property{
    if(fDeleted)
        return;
    cppObject->type_descriptor()->set_boolean_property(cppObject, from_platform_string(property), value);
}
-(void)getFloat:(NSNumber *__autoreleasing *)value property:(NSString *)property{
    if(fDeleted)
        return;
    auto _p = cppObject->type_descriptor()->get_nullable_single_property(cppObject, from_platform_string(property));
    if(_p.has_value())
        *value = @(_p.value());
    else
        *value = nil;
}
-(void)setFloat:(float)value property:(NSString *)property{
    if(fDeleted)
        return;
    cppObject->type_descriptor()->set_single_property(cppObject, from_platform_string(property), value);
}
-(void)getInt:(NSNumber *__autoreleasing *)value property:(NSString *)property{
    if(fDeleted)
        return;
    auto _p = cppObject->type_descriptor()->get_nullable_integer_property(cppObject, from_platform_string(property));
    if(_p.has_value())
        *value = @(_p.value());
    else
        *value = nil;
}
-(void)setInt:(NSInteger)value property:(NSString *)property{
    if(fDeleted)
        return;
    cppObject->type_descriptor()->set_integer_property(cppObject, from_platform_string(property), value);
}
-(void)getFloatPair:(NSNumber *__autoreleasing *)value1 second:(NSNumber *__autoreleasing *)value2 property:(NSString *)property{
    if(fDeleted)
        return;
    auto _p = cppObject->type_descriptor()->get_nullable_vec2_property(cppObject, from_platform_string(property));
    if(_p.has_value()){
        *value1 = @(_p.value().x());
        *value2 = @(_p.value().y());
    }
    else{
        *value1 = nil;
        *value2 = nil;
    }
}
-(void)setFloatPair:(float)value1 second:(float)value2 property:(NSString *)property{
    if(fDeleted)
        return;
    cppObject->type_descriptor()->set_vec2_property(cppObject, from_platform_string(property), rb::vec2(value1, value2));
}
-(void)getColor:(NSColor *__autoreleasing *)value property:(NSString *)property{
    if(fDeleted)
        return;
    auto _p = cppObject->type_descriptor()->get_nullable_color_property(cppObject, from_platform_string(property));
    if(_p.has_value()){
        *value = [NSColor colorWithDeviceRed:_p.value().r() green:_p.value().g() blue:_p.value().b() alpha:_p.value().a()];
    }
    else{
        *value = nil;
    }
}
-(void)setColor:(NSColor *)value property:(NSString *)property{
    if(fDeleted)
        return;
    if(value)
    {
        if(!([[value colorSpaceName] isEqualToString: NSDeviceRGBColorSpace] || [[value colorSpaceName] isEqualToString: NSCalibratedRGBColorSpace])){
            value = [value colorUsingColorSpaceName:NSDeviceRGBColorSpace];
        }
        rb::color c = rb::color::from_rgba([value redComponent], [value greenComponent], [value blueComponent], [value alphaComponent]);
        cppObject->type_descriptor()->set_color_property(cppObject, from_platform_string(property), c);
    }
    else
        cppObject->type_descriptor()->set_nullable_color_property(cppObject, from_platform_string(property), nullptr);
}

-(void)getUnionTag:(NSString *__autoreleasing *)tag{
    if(fDeleted)
        return;
    *tag = to_platform_string(cppObject->branch_tag());
}

-(void)setUnionTag:(NSString *)tag{
    if(fDeleted)
        return;
    cppObject->branch_tag(from_platform_string(tag));
}

-(void)getImage:(NSImage *__autoreleasing *)value property:(NSString *)property{
    if(fDeleted)
        return;
    auto _p = cppObject->type_descriptor()->get_nullable_string_property(cppObject, from_platform_string(property));
    if(!_p.has_value()){
        *value = nil;
    }
    else {
        *value = [fImageMap objectForKey:to_platform_string(_p.value())];
    }
}

-(void)setImage:(NSImage *)value property:(NSString *)property{
    if(fDeleted)
        return;
    if(!value){
        cppObject->type_descriptor()->set_nullable_string_property(cppObject, from_platform_string(property), nullptr);
    }
    else {
        NSArray* _keys = [fImageMap allKeysForObject:value];
        if(_keys.count == 0)
            return;
        cppObject->type_descriptor()->set_string_property(cppObject, from_platform_string(property), from_platform_string([_keys objectAtIndex:0]));
    }
}

TypeDescriptor* createFlagsType(const rb::type_descriptor* td);
TypeDescriptor* createActionType(const rb::type_descriptor* td);
TypeDescriptor* createUnionType(const rb::type_descriptor* td);
TypeDescriptor* createEnumerationType(const rb::type_descriptor* td);
TypeDescriptor* createObjectType(const rb::type_descriptor* td);
TypeDescriptor* createVectorType(const rb::type_descriptor* td);
TypeDescriptor* createPrimitiveType(const rb::type_descriptor* td);

TypeDescriptor* createTypeDescriptor(const rb::type_descriptor* td){
    if ([fCreatedTypes objectForKey:to_platform_string(td->name())])
        return [fCreatedTypes objectForKey:to_platform_string(td->name())];
    
    TypeDescriptor* created = nil;
    
    if(td->gtd() == rb::general_type_descriptor::branch)
        created = createUnionType(td);
    else if(td->gtd() == rb::general_type_descriptor::enumeration)
        created = createEnumerationType(td);
    else if(td->gtd() == rb::general_type_descriptor::object)
        created = createObjectType(td);
    else if(td->gtd() == rb::general_type_descriptor::vector)
        created = createVectorType(td);
    else if(td->gtd() == rb::general_type_descriptor::flags)
        created = createFlagsType(td);
    else if(td->gtd() == rb::general_type_descriptor::action)
        created = createActionType(td);
    else
        created = createPrimitiveType(td);
    
    if(td->gtd() == rb::general_type_descriptor::object || td->gtd() == rb::general_type_descriptor::branch){
        ConstructibleType *ct = [ConstructibleType constructibleTypeFromTypeDescriptor:created];
        [ct setCreatorBlock:^(){
            auto _obj = td->new_object<rb::typed_object>();
            _obj->set_bridge([[CppBridge alloc] initWithCppObject:_obj]);
            return _obj->get_bridge();
        }];
        created = ct;
    }
    
    if(created){
        if(td->gtd() == rb::general_type_descriptor::vector){
            if(!td->element_type()->dynamic_type())
                [fCreatedTypes setValue:created forKey:to_platform_string(td->name())];
        }
        else {
            if(!td->dynamic_type())
                [fCreatedTypes setValue:created forKey:to_platform_string(td->name())];
        }
    }
    
    return created;
}

TypeDescriptor* createUnionType(const rb::type_descriptor* td){
    NSMutableDictionary* _union_map = [NSMutableDictionary dictionary];
    for (auto& _p : td->branches()){
        [_union_map setValue:createTypeDescriptor(_p.second) forKey:to_platform_string(_p.first)];
    }
    TypeDescriptor* uType = [TypeDescriptor unionTypeDescriptorFromName:to_platform_string(td->name()) displayName:to_platform_string(td->display_name()) selectorDisplayName:to_platform_string(td->selector_name()) unionMap:_union_map];
    if(td->nullable())
        return [TypeDescriptor nullableTypeFromTypeDescriptor:to_platform_string(td->name()) type:uType];
    else
        return uType;
}
TypeDescriptor* createEnumerationType(const rb::type_descriptor* td){
    NSMutableDictionary* _enum_values = [NSMutableDictionary dictionary];
    for (auto& _p : td->enumeration_values()){
        [_enum_values setValue:@(_p.second) forKey:to_platform_string(_p.first)];
    }
    TypeDescriptor* eType = [TypeDescriptor enumTypeDescriptorFromName:to_platform_string(td->name()) displayName:to_platform_string(td->display_name()) enumValues:_enum_values];
    
    if(td->nullable())
        return [TypeDescriptor nullableTypeFromTypeDescriptor:to_platform_string(td->name()) type:eType];
    else
        return eType;
}
TypeDescriptor* createFlagsType(const rb::type_descriptor* td){
    NSMutableArray* _names = [NSMutableArray array];
    for (auto& _p : td->flag_names()){
        [_names addObject:to_platform_string(_p)];
    }
    TypeDescriptor* type = [TypeDescriptor flagTypeDescriptorFromName:to_platform_string(td->name()) displayName:to_platform_string(td->display_name()) flagNames:_names];
    
    if(td->nullable())
        return [TypeDescriptor nullableTypeFromTypeDescriptor:to_platform_string(td->name()) type:type];
    else
        return type;
}
TypeDescriptor* createActionType(const rb::type_descriptor* td){
    NSMutableArray* _names = [NSMutableArray array];
    for (auto& _p : td->action_names()){
        [_names addObject:to_platform_string(_p)];
    }
    TypeDescriptor* type = [TypeDescriptor actionTypeDescriptorFromName:to_platform_string(td->name()) displayName:to_platform_string(td->display_name()) actionNames:_names];
    
    if(td->nullable())
        return [TypeDescriptor nullableTypeFromTypeDescriptor:to_platform_string(td->name()) type:type];
    else
        return type;
}
TypeDescriptor* createObjectType(const rb::type_descriptor* td){
    NSMutableArray* properties = [NSMutableArray array];
    std::vector<rb::rb_string> _properties;
    td->get_all_properties(_properties);
    
    for (auto& _p : _properties){
        if(td->property_private(_p))
            continue;
        [properties addObject:[PropertyDescriptor propertyDescriptorWithName:to_platform_string(_p) displayName:to_platform_string(td->property_display_name(_p)) type:createTypeDescriptor(td->property_type(_p)) editable:td->property_editable(_p) continous:td->property_continuous(_p) inlined:td->property_inlined(_p) precision:td->property_precision(_p)]];
    }
    TypeDescriptor* oType = [TypeDescriptor objectTypeDescriptorFromName:to_platform_string(td->name()) displayName:to_platform_string(td->display_name()) properties:properties];
    if(td->nullable())
        return [TypeDescriptor nullableTypeFromTypeDescriptor:to_platform_string(td->name()) type:oType];
    else
        return oType;
}
TypeDescriptor* createVectorType(const rb::type_descriptor* td){
    return [TypeDescriptor vectorTypeDescriptorFromName:to_platform_string(td->name()) displayName:to_platform_string(td->display_name()) elementType:createTypeDescriptor(td->element_type())];
}
TypeDescriptor* createPrimitiveType(const rb::type_descriptor* td){
    if(td->nullable()){
        if(td->gtd() == rb::general_type_descriptor::angle)
            return [TypeDescriptor nullableAngleType];
        else if(td->gtd() == rb::general_type_descriptor::boolean)
            return [TypeDescriptor nullableBoolType];
        else if(td->gtd() == rb::general_type_descriptor::color)
            return [TypeDescriptor nullableColorType];
        else if(td->gtd() == rb::general_type_descriptor::image)
            return [TypeDescriptor nullableImageType];
        else if(td->gtd() == rb::general_type_descriptor::integer)
            return [TypeDescriptor nullableIntType];
        else if(td->gtd() == rb::general_type_descriptor::ranged)
            return [TypeDescriptor nullableRangedType];
        else if(td->gtd() == rb::general_type_descriptor::single)
            return [TypeDescriptor nullableFloatType];
        else if(td->gtd() == rb::general_type_descriptor::string)
            return [TypeDescriptor nullableTextType];
        else if(td->gtd() == rb::general_type_descriptor::string_with_autocompletion)
            return [TypeDescriptor nullableStringWithAutoCompletionType];
        else //vec2
            return [TypeDescriptor nullableFloatPairType];
    }
    else {
        if(td->gtd() == rb::general_type_descriptor::angle)
            return [TypeDescriptor angleType];
        else if(td->gtd() == rb::general_type_descriptor::boolean)
            return [TypeDescriptor boolType];
        else if(td->gtd() == rb::general_type_descriptor::color)
            return [TypeDescriptor colorType];
        else if(td->gtd() == rb::general_type_descriptor::image)
            return [TypeDescriptor imageType];
        else if(td->gtd() == rb::general_type_descriptor::integer)
            return [TypeDescriptor intType];
        else if(td->gtd() == rb::general_type_descriptor::ranged)
            return [TypeDescriptor rangedType];
        else if(td->gtd() == rb::general_type_descriptor::single)
            return [TypeDescriptor floatType];
        else if(td->gtd() == rb::general_type_descriptor::string)
            return [TypeDescriptor textType];
        else if(td->gtd() == rb::general_type_descriptor::string_with_autocompletion)
            return [TypeDescriptor stringWithAutoCompletionType];
        else //vec2
            return [TypeDescriptor floatPairType];
    }
}

-(TypeDescriptor *)typeDescriptor{
    if(cachedTypeDescriptor)
        return cachedTypeDescriptor;
    
    auto _td = cppObject->type_descriptor();
    cachedTypeDescriptor = createTypeDescriptor(_td);
    return cachedTypeDescriptor;
}

-(void)setNil:(NSString *)property{
    if(fDeleted)
        return;
    
    auto _td = cppObject->type_descriptor()->property_type(from_platform_string(property));
    
    if (_td->gtd() == rb::general_type_descriptor::angle)
        cppObject->type_descriptor()->set_nullable_single_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::boolean)
        cppObject->type_descriptor()->set_nullable_boolean_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::branch)
        cppObject->type_descriptor()->set_object_property<rb::typed_object>(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::color)
        cppObject->type_descriptor()->set_nullable_color_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::enumeration)
        cppObject->type_descriptor()->set_nullable_enumeration_property<long>(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::image)
        cppObject->type_descriptor()->set_nullable_string_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::integer)
        cppObject->type_descriptor()->set_nullable_integer_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::object)
        cppObject->type_descriptor()->set_object_property<rb::typed_object>(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::ranged)
        cppObject->type_descriptor()->set_nullable_single_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::single)
        cppObject->type_descriptor()->set_nullable_single_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::string)
        cppObject->type_descriptor()->set_nullable_string_property(cppObject, from_platform_string(property), nullptr);
    else if (_td->gtd() == rb::general_type_descriptor::vec2)
        cppObject->type_descriptor()->set_nullable_vec2_property(cppObject, from_platform_string(property), nullptr);
    else if(_td->gtd() == rb::general_type_descriptor::string_with_autocompletion)
        cppObject->type_descriptor()->set_nullable_string_property(cppObject, from_platform_string(property), nullptr);
    else if(_td->gtd() == rb::general_type_descriptor::flags)
        cppObject->type_descriptor()->set_nullable_integer_property(cppObject, from_platform_string(property), nullptr);
    else //vector
        [NSException raise:@"Invalid Call" format:@"Can't set vector or action to null..."];
}

-(void)notifyObservers:(NSString*)property{
    if(property){
        for (id<Observer> ob in observers) {
            [ob propertyChanged:self property:property];
        }
    }
    else{
        for (id<Observer> ob in observers) {
            [ob unionTagChanged:self];
        }
    }
}

-(void)notifyObserversOfDeletedObject {
    fDeleted = YES;
    for (id<Observer> ob in observers) {
        [ob objectDeleted:self];
    }
}

-(void)addObserver:(id<Observer>)observer{
    [self removeObserver:observer];
    [observers addObject:observer];
}
-(void)removeObserver:(id)observer{
    NSUInteger index = [observers indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
        return obj == observer;
    }];
    if(index == NSNotFound)
        return;
    [observers removeObjectAtIndex:index];
}
-(void)deleteInternalObject{
    if(fDeleted)
        return;
    [observers removeAllObjects];
    delete cppObject;
    fDeleted = YES;
}
@end






























