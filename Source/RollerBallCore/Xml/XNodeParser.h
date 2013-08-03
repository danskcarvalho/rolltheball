//
//  XNodeParser.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 23/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface XNodeParser : NSObject < NSXMLParserDelegate>
@property void* xnode;
-(BOOL)parseXml:(NSURL*)url;
-(BOOL)parseXmlWithString: (NSString*)str;
-(id)initWithNode:(void*)node;
@end
