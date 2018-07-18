
#include "font.h"
#include <sstream>
#include <utf/utf.h>
#include <base/scope_guard.h>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <script/extents.h>

namespace script { namespace cocoa
{

////////////////////////////////////////

font::font( void *font, std::string fam, std::string style, points pts )
		: script::font( std::move( fam ), std::move( style ), pts ),
		  _font( font )
{
}

////////////////////////////////////////

font::~font( void )
{
}

////////////////////////////////////////

void font::init_extents( void )
{
    // we have the dpi now, so we can convert points to dots...
    NSFont *nf = reinterpret_cast<NSFont *>( _font );
    _extents.ascent = [nf ascender] * extent_type( _dpi_v ) / extent_type( 72.0 ); // in points?
    _extents.descent = [nf descender] * extent_type( _dpi_v ) / extent_type( 72.0 );
    NSRect bbox = [nf boundingRectForFont];
    NSSize madv = [nf maximumAdvancement];
    _extents.width = NSWidth( bbox );
    _extents.height = NSHeight( bbox );
    _extents.max_x_advance = madv.width;
    _extents.max_y_advance = madv.height;
}

////////////////////////////////////////

extent_type
font::kerning( char32_t /*c1*/, char32_t /*c2*/ )
{
	return extent_type(0);
}

////////////////////////////////////////

const text_extents &
font::get_glyph( char32_t char_code )
{
    auto i = _glyph_cache.find( char_code );
	if ( i == _glyph_cache.end() )
	{
		std::stringstream str;
		utf::write( str, char_code );
        NSString *string = [NSString stringWithUTF8String: str.str().c_str()];
        NSDictionary *attribs = @{ NSFontAttributeName:static_cast<id>( _font ) };
        NSSize size = [string sizeWithAttributes:attribs];
        int width = static_cast<int>( ceil(size.width) );
        int height = static_cast<int>( ceil(size.height) );

        // Create the context and fill it with white background
        void *data = malloc( static_cast<size_t>( width * height ) );
        on_scope_exit{ free( data ); };
        memset( data, 0, static_cast<size_t>( width * height ) );

        CGColorSpaceRef space = CGColorSpaceCreateDeviceGray();
        CGBitmapInfo bitmapInfo = 0;
        CGContextRef ctx = CGBitmapContextCreate(data, static_cast<size_t>( width ), static_cast<size_t>( height ), 8, static_cast<size_t>( width ), space, bitmapInfo );
        CGColorSpaceRelease( space );
        CGContextSetRGBFillColor( ctx, 0.0, 0.0, 0.0, 0.0 ); // white background
        CGContextFillRect( ctx, CGRectMake( 0.0, 0.0, width, height ) );

        // Create an attributed string with string and font information
        NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                static_cast<id>( _font ), kCTFontAttributeName,
                                CGColorGetConstantColor( kCGColorWhite ), kCTForegroundColorAttributeName,
                                nil];
        NSAttributedString *as = [[NSAttributedString alloc] initWithString:string attributes:attributes];

        // Figure out how big an image we need
        CTLineRef line = CTLineCreateWithAttributedString( static_cast<CFAttributedStringRef>( as ) );
        CGFloat descent = 0.0, ascent = 0.0, leading = 0.0;
        /*double fWidth =*/ CTLineGetTypographicBounds( line, &ascent, &descent, &leading );

        // On iOS 4.0 and Mac OS X v10.6 you can pass null for data
//		size_t width = (size_t)ceilf(fWidth);
//		size_t height = (size_t)ceilf(ascent + descent);

        // Draw the text
        CGFloat x = 0.5;
        CGFloat y = descent;
        CGContextSetTextPosition( ctx, x, y );
        CTLineDraw( line, ctx );
        CFRelease( line );

        add_glyph( char_code, static_cast<const uint8_t *>( data ), width, width, height );
        text_extents &gle = _glyph_cache[char_code];
        gle.x_bearing = extent_type(0);
        gle.y_bearing = std::floor( height / extent_type(2) ); //ascent + descent;
        gle.width = static_cast<extent_type>( width );
        gle.height = static_cast<extent_type>( height );
        gle.x_advance = width;
        gle.y_advance = extent_type(0);

        // Save as JPEG
        /*
        CGImageRef imageRef = CGBitmapContextCreateImage(ctx);
        NSBitmapImageRep* imageRep = [[NSBitmapImageRep alloc] initWithCGImage:imageRef];
        NSData* imageData = [imageRep representationUsingType:NSJPEGFileType properties:nil];
        NSString* fileName = [NSString stringWithFormat:@"Kevin.jpg"];
        NSString* fileDirectory = NSHomeDirectory();
        NSString* filePath = [fileDirectory stringByAppendingPathComponent:fileName];
        [imageData writeToFile:filePath atomically:YES];
        [imageRep release];
        CGImageRelease(imageRef);
        */

        return gle;
    }

    return i->second;
}

////////////////////////////////////////

} // namespace cocoa
} // namespace script
