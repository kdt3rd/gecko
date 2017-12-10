
#include "font.h"
#include <sstream>
#include <utf/utf.h>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <script/extents.h>

namespace script { namespace cocoa
{

////////////////////////////////////////

font::font( void *font, std::string fam, std::string style, double pixsize )
		: script::font( std::move( fam ), std::move( style ), pixsize ),
		  _font( font )
{
}

////////////////////////////////////////

font::~font( void )
{
//	FT_Done_Face( _face );
}

////////////////////////////////////////

extent_type
font::kerning( char32_t /*c1*/, char32_t /*c2*/ )
{
	/*
	FT_Vector kerning;
	FT_UInt prev_index = FT_Get_Char_Index( _face, c1 );
	FT_UInt next_index = FT_Get_Char_Index( _face, c2 );

	FT_Get_Kerning( _face, prev_index, next_index, FT_KERNING_UNFITTED, &kerning );
	*/

	return 0.0;
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
		void *data = malloc( static_cast<size_t>( width*height ) );
		memset( data, 0, static_cast<size_t>( width * height ) );
		CGColorSpaceRef space = CGColorSpaceCreateDeviceGray();
		CGBitmapInfo bitmapInfo = 0;
		CGContextRef ctx = CGBitmapContextCreate(data, static_cast<size_t>( width ), static_cast<size_t>( height ), 8, static_cast<size_t>( width ), space, bitmapInfo );
		CGColorSpaceRelease( space );
		CGContextSetRGBFillColor( ctx, 0.0, 0.0, 0.0, 0.0 ); // white background
		CGContextFillRect( ctx, CGRectMake( 0.0, 0.0, width, height ) );

//		CGGlyph glyph;
//		CTFontGetGlyphsForCharacters( font, &char_code, &glyph, 1 );

		// Create an attributed string with string and font information
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
									static_cast<id>( _font ), kCTFontAttributeName,
//									(id)_font, NSFontAttributeName, 
//									NSColor.whiteColor, NSForegroundColorAttributeName,
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
//		CFRelease(font);
//		std::cout << "Glyph: " << height << ' ' << ascent << ' ' << descent << std::endl;

		add_glyph( char_code, static_cast<const uint8_t *>( data ), width, width, height );
		text_extents &gle = _glyph_cache[char_code];
		gle.x_bearing = 0.0;
		gle.y_bearing = std::floor( height/2 ); //ascent + descent;
		gle.width = static_cast<double>( width );
		gle.height = static_cast<double>( height );
		gle.x_advance = width;
		gle.y_advance = 0.0;

		// Save as JPEG
		/*
		CGImageRef imageRef = CGBitmapContextCreateImage(ctx);
		NSBitmapImageRep* imageRep = [[NSBitmapImageRep alloc] initWithCGImage:imageRef];
//		NSAssert(imageRep, @"imageRep must not be nil");
		NSData* imageData = [imageRep representationUsingType:NSJPEGFileType properties:nil];
		NSString* fileName = [NSString stringWithFormat:@"Kevin.jpg"];
		NSString* fileDirectory = NSHomeDirectory();
		NSString* filePath = [fileDirectory stringByAppendingPathComponent:fileName];
		[imageData writeToFile:filePath atomically:YES];
		*/

		// Clean up
		//[imageRep release];
		//CGImageRelease(imageRef);
		free(data);

		return gle;
	}

	return i->second;
}

////////////////////////////////////////

} }

