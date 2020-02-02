
#include "font.h"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#include <base/scope_guard.h>
#include <script/extents.h>
#include <sstream>
#include <utf/utf.h>

namespace script
{
namespace cocoa
{
////////////////////////////////////////

font::font( void *font, std::string fam, std::string style, points pts )
    : script::font( std::move( fam ), std::move( style ), pts ), _font( font )
{}

////////////////////////////////////////

font::~font( void ) {}

////////////////////////////////////////

void font::init_font( void )
{
    _scalePixToPointsHoriz = 72.f / _dpi_h;
    _scalePixToPointsVert  = 72.f / _dpi_v;
    // we have the dpi now, so we can convert points to dots...
    NSFont *nf       = reinterpret_cast<NSFont *>( _font );
    _extents.ascent  = static_cast<float>( [nf ascender] ) * _scalePixToPointsVert;
    _extents.descent = static_cast<float>( [nf descender] ) * _scalePixToPointsHoriz;
    NSRect bbox      = [nf boundingRectForFont];
    NSSize madv      = [nf maximumAdvancement];
    _extents.width   = static_cast<float>( NSWidth( bbox ) ) * _scalePixToPointsHoriz;
    _extents.height  = static_cast<float>( NSHeight( bbox ) ) * _scalePixToPointsVert;
    _extents.max_x_advance = static_cast<float>( madv.width ) * _scalePixToPointsHoriz;
    _extents.max_y_advance = static_cast<float>( madv.height ) * _scalePixToPointsVert;
    //std::cout << "font height: " << _extents.height << " asc desc " << _extents.ascent << ' ' << _extents.descent << " origin " << bbox.origin.x << ", " << bbox.origin.y << " size " << bbox.size.width << " x " << bbox.size.height << std::endl;
}

////////////////////////////////////////

points font::kerning( char32_t /*c1*/, char32_t /*c2*/ ) { return points( 0 ); }

////////////////////////////////////////

const text_extents &font::get_glyph( char32_t char_code )
{
    auto i = _glyph_cache.find( char_code );
    if ( i == _glyph_cache.end() )
    {
        std::stringstream str;
        utf::write( str, char_code );
        NSString *    string  = [NSString stringWithUTF8String:str.str().c_str()];
        NSDictionary *attribs = @{ NSFontAttributeName : static_cast<id>( _font ) };
        NSSize        size    = [string sizeWithAttributes:attribs];
        int           width   = static_cast<int>( ceil( size.width ) );
        int           height  = static_cast<int>( ceil( size.height ) );

        // Create the context and fill it with white background
        void *data = malloc( static_cast<size_t>( width * height ) );
        on_scope_exit { free( data ); };
        memset( data, 0, static_cast<size_t>( width * height ) );

        CGColorSpaceRef space = CGColorSpaceCreateDeviceGray();
        on_scope_exit { CGColorSpaceRelease( space ); };
        CGBitmapInfo bitmapInfo = 0;
        CGContextRef ctx        = CGBitmapContextCreate(
            data,
            static_cast<size_t>( width ),
            static_cast<size_t>( height ),
            8,
            static_cast<size_t>( width ),
            space,
            bitmapInfo );
        on_scope_exit { CGContextRelease( ctx ); };

        CGContextSetRGBFillColor( ctx, 0.0, 0.0, 0.0, 0.0 ); // white background
        CGContextFillRect( ctx, CGRectMake( 0.0, 0.0, width, height ) );

        // Create an attributed string with string and font information
        NSDictionary *      attributes = [NSDictionary
            dictionaryWithObjectsAndKeys:static_cast<id>( _font ),
                                         kCTFontAttributeName,
                                         CGColorGetConstantColor( kCGColorWhite ),
                                         kCTForegroundColorAttributeName,
                                         nil];
        NSAttributedString *as         = [[NSAttributedString alloc] initWithString:string
                                                                 attributes:attributes];

        // Figure out how big an image we need
        CTLineRef line =
            CTLineCreateWithAttributedString( static_cast<CFAttributedStringRef>( as ) );
        CGFloat descent = 0.0, ascent = 0.0, leading = 0.0;
        /*double fWidth =*/CTLineGetTypographicBounds(
            line, &ascent, &descent, &leading );

        // On iOS 4.0 and Mac OS X v10.6 you can pass null for data
        //		size_t width = (size_t)ceilf(fWidth);
        //		size_t height = (size_t)ceilf(ascent + descent);

        // Draw the text
        CGFloat x = 0.5;
        CGFloat y = descent;
        CGContextSetTextPosition( ctx, x, y );
        CTLineDraw( line, ctx );
        CGFloat ws = CTLineGetTrailingWhitespaceWidth( line );
        CFRelease( line );

        //std::cout << "add_glyph " << char_code << " w " << width << " h " << height << " descent " << descent << " ascent " << ascent << " leading " << leading << std::endl;
        add_glyph(
            char_code, static_cast<const uint8_t *>( data ), width, width, height );
        text_extents &gle = _glyph_cache[char_code];
        // inch / dot * points / inch
        extent_type ptw = extent_type( width ) * _scalePixToPointsHoriz;
        extent_type pth = extent_type( height ) * _scalePixToPointsVert;
        gle.x_bearing   = points( 0 );
        gle.y_bearing =
            pth -
            extent_type( descent ) *
                _scalePixToPointsVert; //std::floor( extent_type(pth) / extent_type(2) ); //ascent + descent;
        gle.width     = static_cast<points>( ptw );
        gle.height    = static_cast<points>( pth );
        gle.x_advance = ptw + static_cast<float>( ws ) * _scalePixToPointsVert;
        gle.y_advance = points( 0 );

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
