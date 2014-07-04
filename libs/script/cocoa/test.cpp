
void test( void )
{
	NSString* string = @"Kevin";
    CGFloat fontSize = 12.0f;

    // Create an attributed string with string and font information
    CTFontRef font = CTFontCreateWithName(CFSTR("Helvetica Light"), fontSize, nil);
    NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                (id)font, kCTFontAttributeName, 
                                nil];
    NSAttributedString* as = [[NSAttributedString alloc] initWithString:string attributes:attributes];
    CFRelease(font);

    // Figure out how big an image we need 
    CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)as);
    CGFloat ascent, descent, leading;
    double fWidth = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);

    // On iOS 4.0 and Mac OS X v10.6 you can pass null for data 
    size_t width = (size_t)ceilf(fWidth);
    size_t height = (size_t)ceilf(ascent + descent);
    void* data = malloc(width*height*4);

    // Create the context and fill it with white background
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast;
    CGContextRef ctx = CGBitmapContextCreate(data, width, height, 8, width*4, space, bitmapInfo);
    CGColorSpaceRelease(space);
    CGContextSetRGBFillColor(ctx, 1.0, 1.0, 1.0, 1.0); // white background
    CGContextFillRect(ctx, CGRectMake(0.0, 0.0, width, height));

    // Draw the text 
    CGFloat x = 0.0;
    CGFloat y = descent;
    CGContextSetTextPosition(ctx, x, y);
    CTLineDraw(line, ctx);
    CFRelease(line);

    // Save as JPEG
    CGImageRef imageRef = CGBitmapContextCreateImage(ctx);
    NSBitmapImageRep* imageRep = [[NSBitmapImageRep alloc] initWithCGImage:imageRef];
    NSAssert(imageRep, @"imageRep must not be nil");
    NSData* imageData = [imageRep representationUsingType:NSJPEGFileType properties:nil];
    NSString* fileName = [NSString stringWithFormat:@"Kevin.jpg"];
    NSString* fileDirectory = NSHomeDirectory();
    NSString* filePath = [fileDirectory stringByAppendingPathComponent:fileName];
    [imageData writeToFile:filePath atomically:YES];

    // Clean up
    [imageRep release];
    CGImageRelease(imageRef);
    free(data);
}
