//
//  XNodeParser.m
//  RollerBallCore
//
//  Created by Danilo Carvalho on 23/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "XNodeParser.h"
#include "xnode.h"

using namespace rb;

@interface XNodeParser () {
    std::vector<xnode*> node_stack;
}
@end

inline NSString* to_platform_string(const rb::rb_string& str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    auto _u8Str = _str_converter.to_bytes(str);
    NSString* _platform_str = [NSString stringWithUTF8String:_u8Str.c_str()];
    return _platform_str;
}

inline rb_string from_platform_string(NSString* str){
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> _str_converter;
    return _str_converter.from_bytes([str UTF8String]);
}

@implementation XNodeParser
-(id)initWithNode:(void *)node{
    if(self = [super init]){
        node_stack.clear();
        xnode = node;
    }
    
    return self;
}
@synthesize xnode;
-(void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict{
    if(node_stack.size() == 0){
        rb::xnode* n = (rb::xnode*)self.xnode;
        n->name(from_platform_string(elementName));
        NSArray* keys = [attributeDict allKeys];
        for (int i = 0; i < [keys count]; i++) {
            NSString* key = [keys objectAtIndex:i];
            NSString* value = [attributeDict objectForKey:key];
            n->set_attribute_value(from_platform_string(key), from_platform_string(value));
        }
        node_stack.push_back(n);
    }
    else {
        rb::xnode* n = new rb::xnode();
        n->name(from_platform_string(elementName));
        NSArray* keys = [attributeDict allKeys];
        for (int i = 0; i < [keys count]; i++) {
            NSString* key = [keys objectAtIndex:i];
            NSString* value = [attributeDict objectForKey:key];
            n->set_attribute_value(from_platform_string(key), from_platform_string(value));
        }
        node_stack.push_back(n);
    }
}
-(void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName{
    if(node_stack.size() == 1) //we're in root
    {
        node_stack.pop_back();
    }
    else if(node_stack.size() > 1) {
        node_stack[node_stack.size() - 2]->children().push_back(*node_stack.back());
        delete node_stack.back();
        node_stack.pop_back();
    }
}
-(void)parserDidStartDocument:(NSXMLParser *)parser{
    rb::xnode* n = (rb::xnode*)self.xnode;
    n->clear_attributes();
    n->children().clear();
    node_stack.clear();
}
-(void)parserDidEndDocument:(NSXMLParser *)parser{
    
}
-(BOOL)parseXml:(NSURL *)url{
    if(url.scheme == nil)
        url = [NSURL fileURLWithPath:url.path];
    
    NSXMLParser* parser = [[NSXMLParser alloc] initWithContentsOfURL:url];
    [parser setDelegate:self];
    return [parser parse];
}
-(BOOL)parseXmlWithString: (NSString*)str {
    NSXMLParser* parser = [[NSXMLParser alloc] initWithData:[str dataUsingEncoding:NSUTF8StringEncoding]];
    [parser setDelegate:self];
    return [parser parse];
}
@end
