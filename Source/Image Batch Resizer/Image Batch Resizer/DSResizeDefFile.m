//
//  DSResizeDefFile.m
//  Image Batch Resizer
//
//  Created by Danilo Carvalho on 26/08/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#import "DSResizeDefFile.h"
#include <math.h>

@interface DSResizeDefFile ()
{
    @private
    NSMutableDictionary* _fileDimensions;
    NSMutableArray* _resolutions;
    NSURL* _xmlPath;
    BOOL _foundDefaultBlockSize;
    BOOL _foundDefaultResolution;
}
@property(readwrite, nonatomic) CGSize defaultBlockSize;
@property(readwrite, nonatomic) CGSize defaultResolution;
@property(readwrite, nonatomic) int resolutionCount;
@property(readwrite, nonatomic, strong) NSURL* basePath;
@property(readwrite, nonatomic, strong) NSURL* outputPath;

//Private Methods
-(void)addResolution:(CGSize)resolution;
-(BOOL)processXmlFile;
-(BOOL)resizePNGImageWithPath:(NSURL*)path toDestination:(NSURL*)destPath
            withNewDimensions:(CGSize)dimensions;
-(CGSize)getImageDimensionsWithPath:(NSURL*)url;
@end

@implementation DSResizeDefFile

-(void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict {
    if([elementName isEqualToString:@"blockSize"]){
        NSString* _width = [attributeDict objectForKey:@"width"];
        NSString* _height = [attributeDict objectForKey:@"height"];
        if(!_width || !_height)
        {
            NSLog(@"Attrib not found...");
            [parser abortParsing];
            return;
        }
        self.defaultBlockSize = CGSizeMake([_width doubleValue], [_height doubleValue]);
        if(self.defaultBlockSize.width <= 0){
            NSLog(@"Invalid width for block size...");
            [parser abortParsing];
            return;
        }
        if(self.defaultBlockSize.height <= 0){
            NSLog(@"Invalid height for block size...");
            [parser abortParsing];
            return;
        }
        if(_foundDefaultResolution){
            if(self.defaultBlockSize.width >= self.defaultResolution.width){
                NSLog(@"Block size width too big...");
                [parser abortParsing];
                return;
            }
            if(self.defaultBlockSize.height >= self.defaultResolution.height){
                NSLog(@"Block size height too big...");
                [parser abortParsing];
                return;
            }
        }
        _foundDefaultBlockSize = YES;
    }
    else if([elementName isEqualToString:@"defaultResolution"]){
        NSString* _width = [attributeDict objectForKey:@"width"];
        NSString* _height = [attributeDict objectForKey:@"height"];
        if(!_width || !_height)
        {
            NSLog(@"Attrib not found...");
            [parser abortParsing];
            return;
        }
        self.defaultResolution = CGSizeMake([_width doubleValue], [_height doubleValue]);
        if(self.defaultResolution.width <= 0){
            NSLog(@"Invalid width for resolution...");
            [parser abortParsing];
            return;
        }
        if(self.defaultResolution.height <= 0){
            NSLog(@"Invalid height for resolution...");
            [parser abortParsing];
            return;
        }
        if(_foundDefaultBlockSize){
            if(self.defaultBlockSize.width >= self.defaultResolution.width){
                NSLog(@"Block size width too big...");
                [parser abortParsing];
                return;
            }
            if(self.defaultBlockSize.height >= self.defaultResolution.height){
                NSLog(@"Block size height too big...");
                [parser abortParsing];
                return;
            }
        }
        _foundDefaultResolution = YES;
    }
    else if([elementName isEqualToString:@"resolution"]){
        NSString* _width = [attributeDict objectForKey:@"width"];
        NSString* _height = [attributeDict objectForKey:@"height"];
        if(!_width || !_height)
        {
            NSLog(@"Attrib not found...");
            [parser abortParsing];
            return;
        }
        
        CGSize res = CGSizeMake([_width doubleValue], [_height doubleValue]);
        if(res.width <= 0){
            NSLog(@"Invalid width for resolution...");
            [parser abortParsing];
            return;
        }
        if(res.height <= 0){
            NSLog(@"Invalid height for resolution...");
            [parser abortParsing];
            return;
        }
        if(!_resolutions)
            _resolutions = [[NSMutableArray alloc] initWithCapacity:1];
        [self addResolution:res];
    }
    else if([elementName isEqualToString:@"file"]){
        NSString* _path = [attributeDict objectForKey:@"path"];
        NSString* _width = [attributeDict objectForKey:@"width"];
        NSString* _height = [attributeDict objectForKey:@"height"];
        if(!_width || !_height)
        {
            NSLog(@"Attrib not found...");
            [parser abortParsing];
            return;
        }
        if(!_path)
        {
            NSLog(@"Attrib not found...");
            [parser abortParsing];
            return;
        }
        if([_path length] == 0)
        {
            NSLog(@"Empty path");
            [parser abortParsing];
            return;
        }
        CGSize dim = CGSizeMake([_width doubleValue], [_height doubleValue]);
        if(dim.width <= 0){
            NSLog(@"Invalid width for dimension...");
            [parser abortParsing];
            return;
        }
        if(dim.height <= 0){
            NSLog(@"Invalid height for dimension...");
            [parser abortParsing];
            return;
        }
        if(!_fileDimensions)
            _fileDimensions = [NSMutableDictionary dictionaryWithCapacity:1];
        
        [_fileDimensions setObject:[NSValue valueWithSize:dim] forKey:_path];
    }
    else if([elementName isEqualToString:@"images"]){
        //Do nothing
    }
    else {
        NSLog(@"Unknown element: %@", elementName);
        [parser abortParsing];
    }
}

-(void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError{
    NSLog(@"Erro processando o arquivo XML...");
}

-(BOOL)processXmlFile {
    NSXMLParser* parser = [[NSXMLParser alloc] initWithContentsOfURL: _xmlPath];
    parser.shouldProcessNamespaces = NO;
    parser.shouldReportNamespacePrefixes = NO;
    parser.shouldResolveExternalEntities = NO;
    parser.delegate = self;
    BOOL success = [parser parse];
    if(success){
        if (!_resolutions || !_fileDimensions || !_foundDefaultBlockSize || !_foundDefaultResolution){
            NSLog(@"Element missing...");
            return NO;
        }
    }
    return success;
}

-(CGSize)getResolutionAtIndex:(int)index{
    CGSize size;
    NSValue* value = [_resolutions objectAtIndex:index];
    [value getValue:&size];
    return size;
}

-(void)addResolution:(CGSize)resolution{
    NSValue* value = [NSValue valueWithSize:resolution];
    [_resolutions addObject:value];
}

-(id)initWithXmlPath:(NSURL *)xmlPath basePath:(NSURL *)basePath outputPath:(NSURL *)outputPath {
    self = [super init];
    if(self){
        _xmlPath = xmlPath;
        self.basePath = basePath;
        self.outputPath = outputPath;
        if(![self processXmlFile]){
            return nil;
        }
        self.resolutionCount = _resolutions.count;
    }
    
    return self;
}

-(BOOL)writeOutputPathWithDelegate:(id<DSResizeDefFileDelegate>)delegate{
    NSURL *urlBasePath = self.basePath;
    NSURL *urlOutputPath = self.outputPath;
    int i = 0;
    int total = (int)(_resolutions.count * _fileDimensions.count);
    BOOL finalResult = YES;
    for (NSString* path in [_fileDimensions allKeys]) {
        CGSize outputSize = [((NSValue*)[_fileDimensions objectForKey:path]) sizeValue];
        for (NSValue* resValue in _resolutions) {
            CGSize resolution = [resValue sizeValue];
            //Pega o nome da pasta...
            NSString* widthStr = [[NSNumber numberWithInt:(int)resolution.width] stringValue];
            NSString* heightStr = [[NSNumber numberWithInt:(int)resolution.height] stringValue];
            NSString* resolutionName = [widthStr stringByAppendingString:@"x"];
            resolutionName = [resolutionName stringByAppendingString:heightStr];
            
            //Pega o tamanho em blocos do arquivo
            NSURL* inputFile = [urlBasePath URLByAppendingPathComponent:path];
            NSURL* outputFile = [urlOutputPath URLByAppendingPathComponent:resolutionName isDirectory:YES];
            outputFile = [outputFile URLByAppendingPathComponent: [path lastPathComponent]];
            
            //1. Devemos descobrir o fator de transformação dessa resolução.
            //We changed to get the maximum factor.
            double wFactor = (double)resolution.width / (double)self.defaultResolution.width;
            double hFactor = (double)resolution.height / (double)self.defaultResolution.height;
            double factor = MAX(wFactor, hFactor);
            
            //2. Pegamos o tamanho da imagem original
            CGSize origSize = [self getImageDimensionsWithPath:inputFile];
            
            //3. Computamos o fator de transformação do tamanho original para o tamanho da resolução padrão.
            wFactor = (double)(outputSize.width * self.defaultBlockSize.width) / origSize.width;
            hFactor = (double)(outputSize.height * self.defaultBlockSize.height) / origSize.height;
            double origFactor = MAX(wFactor, hFactor);
            
            //4. Concatenamos os dois fatores...
            factor *= origFactor;
            
            //5. Finalmente encontramos o tamanho final...
            CGSize finalSize = CGSizeMake(floorf(factor * origSize.width), floorf(factor * origSize.height));
            
            if(delegate)
                [delegate beforeProcessingFile:[inputFile absoluteString] withResolution:resolution withIndex:i outOf:total];
            
            //6. Copiamos o arquivo
            if(!
               [self resizePNGImageWithPath:inputFile toDestination:outputFile withNewDimensions:finalSize]){
                finalResult = NO;
                NSLog(@"[Res (%u, %u)]Failed to resize input file %@", (unsigned int)resolution.width, (unsigned int)resolution.height, inputFile);
            }
            
            
            if(delegate)
                [delegate afterProcessingFile:[inputFile absoluteString] withResolution:resolution withIndex:i outOf:total];
            
            i++;
        }
        
    }
    
    return finalResult;
}

-(CGSize)getImageDimensionsWithPath:(NSURL*)url {
    NSImageRep* image = [NSImageRep imageRepWithContentsOfURL:url];
    return  CGSizeMake(image.pixelsWide, image.pixelsHigh);
}

void releaseCGObjects(CGImageRef finalImage, CGContextRef bitmapRef, CGImageRef image, CGDataProviderRef fileNameProvider, CGColorSpaceRef colorSpace, CGImageDestinationRef dest)
{
    if(finalImage)
        CGImageRelease(finalImage);
    if(bitmapRef)
        CGContextRelease(bitmapRef);
    if(image)
        CGImageRelease(image);
    if(fileNameProvider)
        CGDataProviderRelease(fileNameProvider);
    if(colorSpace)
        CGColorSpaceRelease(colorSpace);
    if(dest)
        CFRelease(dest);
}

-(BOOL)resizePNGImageWithPath:(NSURL*)path toDestination:(NSURL*)destPath
    withNewDimensions:(CGSize)dimensions {
    BOOL result = NO;
    //cria o espaço de cor
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    //cria o provedor de dados...
    const char* fileName = [[path path] cStringUsingEncoding:NSASCIIStringEncoding];
    CGDataProviderRef fileNameProvider = CGDataProviderCreateWithFilename(fileName);
    if(!fileNameProvider){
        releaseCGObjects(NULL, NULL, NULL, fileNameProvider, colorSpace, NULL);
        return NO;
    }
    //cria a imagem...
    CGImageRef image = CGImageCreateWithPNGDataProvider(fileNameProvider, NULL, YES, kCGRenderingIntentDefault);
    CGContextRef bitmapRef = CGBitmapContextCreate(NULL, dimensions.width, dimensions.height, 8, 0, colorSpace, kCGImageAlphaPremultipliedLast);
    if(!bitmapRef){
        releaseCGObjects(NULL, bitmapRef, image, fileNameProvider, colorSpace, NULL);
        return NO;
    }
    //inicialmente vou confiar na qualidade do Quartz... se for necessário eu mudo esse método para ter mais qualidade
    //configuramos o contexto..
    CGContextSetBlendMode(bitmapRef, kCGBlendModeNormal);
    CGContextSetAlpha(bitmapRef, 1.0);
    CGContextSetAllowsAntialiasing(bitmapRef, YES);
    CGContextSetInterpolationQuality(bitmapRef, kCGInterpolationHigh);
    CGContextSetShouldAntialias(bitmapRef, YES);
    CGContextDrawImage(bitmapRef, CGRectMake(0, 0, dimensions.width, dimensions.height), image);
    CGContextFlush(bitmapRef);
    CGImageRef finalImage = CGBitmapContextCreateImage(bitmapRef);
    NSFileManager * fm = [[NSFileManager alloc] init];
    if(![fm createDirectoryAtURL:[destPath URLByDeletingLastPathComponent] withIntermediateDirectories:YES attributes:nil error:nil])
    {
        releaseCGObjects(finalImage, bitmapRef, image, fileNameProvider, colorSpace, NULL);
        return NO;
    }
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL((__bridge CFURLRef)(destPath), kUTTypePNG, 1, NULL);
    if(!destination)
    {
        releaseCGObjects(finalImage, bitmapRef, image, fileNameProvider, colorSpace, NULL);
        return NO;
    }
    
    CGImageDestinationAddImage(destination, finalImage, NULL);
    //Liberamos os recursos alocados...
    result = CGImageDestinationFinalize(destination);
    releaseCGObjects(finalImage, bitmapRef, image, fileNameProvider, colorSpace, destination);
    return result;
}

@end
