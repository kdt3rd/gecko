//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font.h"
#include <cwchar>
#include <limits>
#include <stdexcept>

namespace script
{

////////////////////////////////////////

font::font( std::string fam, std::string sty, double sz )
	: _family( std::move(fam) ), _style( std::move(sty) ), _size( sz )
{
}

////////////////////////////////////////

font::~font( void )
{
}

////////////////////////////////////////

text_extents
font::extents( const std::string &utf8 )
{
	text_extents retval;

	if ( utf8.empty() )
		return retval;

	std::mbstate_t s = std::mbstate_t();
	size_t curpos = 0;
	size_t nleft = utf8.size();
	wchar_t prev = L'\0';
	while ( true )
	{
		wchar_t ccode = 0;
		size_t r = std::mbrtowc( &ccode, utf8.data() + curpos, nleft, &s );
		if ( r == size_t(-2) )
			throw std::runtime_error( "Unable to parse multi-byte sequence" );
		if ( r == size_t(-1) )
			throw std::runtime_error( "Invalid multi-byte sequence" );

		curpos += r;
		if ( r == 0 || ccode == '\0' )
			break;

		const text_extents &gext = get_glyph( static_cast<char32_t>( ccode ) );
		double k = kerning( static_cast<char32_t>( prev ), static_cast<char32_t>( ccode ) );

		if ( prev == L'\0' )
			retval.x_bearing = gext.x_bearing;

		retval.y_bearing = std::max( retval.y_bearing, gext.y_bearing );
		retval.width -= k;
		retval.x_advance -= k;
		retval.width += gext.x_bearing + gext.width;
		retval.height = std::max( retval.height, gext.height );
		retval.x_advance += gext.x_advance;
		retval.y_advance += gext.y_advance;

		prev = ccode;
	}

	return retval;
}

////////////////////////////////////////

void
font::render(
	const std::function<void(float,float,float,float)> &add_point,
	const std::function<void(size_t,size_t,size_t)> &add_tri,
	const std::function<void(void)> &reset,
	const base::point &start, const std::string &utf8 )
{
	if ( utf8.empty() )
		return;

	// In case we have to change the texture size in the
	// middle of rendering, add this extra loop
	uint32_t fVer;
	do
	{
		fVer = _glyph_version;
		size_t points = 0;
		std::mbstate_t s = std::mbstate_t();
		size_t curpos = 0;
		size_t nleft = utf8.size();
		wchar_t prev = L'\0';

		double curposX = start.x();
		while ( true )
		{
			wchar_t ccode = 0;
			size_t r = std::mbrtowc( &ccode, utf8.data() + curpos, nleft, &s );
			if ( r == size_t(-2) )
				throw std::runtime_error( "Invalid multi-byte sequence" );
			if ( r == size_t(-1) )
				throw std::runtime_error( "Invalid multi-byte sequence" );

			curpos += r;
			if ( r == 0 || ccode == '\0' )
				break;

			const text_extents &gext = get_glyph( static_cast<char32_t>( ccode ) );
//			std::cout << "char code: '" << char(ccode) << "' (" << ccode << ") gext.x_advance: " << gext.x_advance << std::endl;
			if ( fVer != _glyph_version )
			{
				reset();
				// kill the inner loop and start over
				break;
			}
			double k = kerning( static_cast<char32_t>( prev ), static_cast<char32_t>( ccode ) );
			curposX -= k;

			auto idx_base_i = _glyph_index_offset.find( static_cast<char32_t>( ccode ) );
			if ( idx_base_i != _glyph_index_offset.end() )
			{
				size_t coordOff = idx_base_i->second;
				size_t idx_base = points;

				if ( ( idx_base / 2 + 3 ) > static_cast<size_t>( std::numeric_limits<uint16_t>::max() ) )
					throw std::runtime_error( "String too long for OpenGL ES" );

				double upperY = start.y() - gext.y_bearing;
				double lowerY = upperY + gext.height;
				double leftX = curposX + gext.x_bearing;
				double rightX = leftX + gext.width;

//				std::cout << "char code: '" << char(ccode) << "' (" << ccode << ") coordOff: " << coordOff << " upperY: " << upperY << " lowerY: " << lowerY << " leftX: " << leftX << " rightX: " << rightX << '\n'
//						  << " coords: "
//						  << _glyph_coords[coordOff + 0] << ", " << _glyph_coords[coordOff + 1]
//						  << ", " << _glyph_coords[coordOff + 2] << ", " << _glyph_coords[coordOff + 3]
//						  << ", " << _glyph_coords[coordOff + 4] << ", " << _glyph_coords[coordOff + 5]
//						  << ", " << _glyph_coords[coordOff + 6] << ", " << _glyph_coords[coordOff + 7]
//						  << " idx_base: " << idx_base << " (" << idx_base/2 << ")"
//						  << std::endl;
				add_point(
					static_cast<float>( leftX ), static_cast<float>( upperY ),
					_glyph_coords[coordOff + 0], _glyph_coords[coordOff + 1] );

				add_point(
					static_cast<float>( rightX ), static_cast<float>( upperY ),
					_glyph_coords[coordOff + 2], _glyph_coords[coordOff + 3] );

				add_point(
					static_cast<float>( rightX ), static_cast<float>( lowerY ),
					_glyph_coords[coordOff + 4], _glyph_coords[coordOff + 5] );

				add_point(
					static_cast<float>( leftX ), static_cast<float>( lowerY ),
					_glyph_coords[coordOff + 6], _glyph_coords[coordOff + 7] );

				points += 4;

				uint16_t idxVal = static_cast<uint16_t>( idx_base );
				add_tri( idxVal, idxVal + 1, idxVal + 2 );
				add_tri( idxVal + 2, idxVal + 3, idxVal );
			}

			curposX += gext.x_advance;
			prev = ccode;
		}
	} while ( fVer != _glyph_version );
}

////////////////////////////////////////

base::point font::align_text( const std::string &utf8, const base::rect &r, base::alignment a )
{
	base::rect rect;
	rect.set_x1( std::ceil( r.x1() ) );
	rect.set_y1( std::ceil( r.y1() ) );
	rect.set_x2( std::floor( r.x2() ) );
	rect.set_y2( std::floor( r.y2() ) );

	if ( utf8.empty() )
		return { 0.0, 0.0 };

	// TODO: add multi-line support?

	font_extents fex = extents();
	text_extents tex = extents( utf8 );

	double y = 0.0, x = 0.0;
	double textHeight = fex.ascent - fex.descent;

	switch ( a )
	{
		case base::alignment::CENTER:
		case base::alignment::LEFT:
		case base::alignment::RIGHT:
			y = rect.y() + std::round( ( rect.height() + textHeight ) / 2.0 ) + fex.descent;
			break;

		case base::alignment::BOTTOM:
		case base::alignment::BOTTOM_RIGHT:
		case base::alignment::BOTTOM_LEFT:
			y = rect.y2() - fex.descent;
			break;

		case base::alignment::TOP:
		case base::alignment::TOP_RIGHT:
		case base::alignment::TOP_LEFT:
			y = rect.y1() + fex.ascent;
			break;
	}

	switch ( a )
	{
		case base::alignment::LEFT:
		case base::alignment::TOP_LEFT:
		case base::alignment::BOTTOM_LEFT:
			x = rect.x() - tex.x_bearing;
			break;

		case base::alignment::RIGHT:
		case base::alignment::TOP_RIGHT:
		case base::alignment::BOTTOM_RIGHT:
			x = rect.x2() - tex.width - tex.x_bearing;
			break;

		case base::alignment::CENTER:
		case base::alignment::TOP:
		case base::alignment::BOTTOM:
			x = rect.x1() + std::round( ( rect.width() - tex.width - tex.x_bearing ) / 2.0 );
			break;
	}

	return { x, y };
}

////////////////////////////////////////

void
font::add_glyph( char32_t char_code, const uint8_t *glData, int glPitch, int w, int h )
{
	// We want each glyph to be separated by at least one black pixel
	// (for example for shader used in demo-subpixel.c)
	base::pack::area gA = _glyph_pack.insert( w + 1, h + 1 );
	while ( gA.empty() )
	{
		bump_glyph_store_size();
		gA = _glyph_pack.insert( w + 1, h + 1 );
	}

	_glyph_index_offset[char_code] = _glyph_coords.size();
	int bmW = _glyph_pack.width();
	int bmH = _glyph_pack.height();
	double texNormW = static_cast<double>( bmW );
	double texNormH = static_cast<double>( bmH );

	if ( gA.flipped( w + 1, h + 1 ) )
	{
		// store things ... flipped so
		// upper left is at x, y + w,
		// upper right is at x, y
		// lower left is at x + h, y + w
		// lower right is at x + h, y
		uint8_t *bmData = _glyph_bitmap.data();

		for ( int y = 0; y < w; ++y )
		{
			int destY = gA.y + y;
			int srcX = w - y - 1;
			int destX = gA.x;
			for ( int x = 0; x < h; ++x, ++destX )
				bmData[destY*bmW + destX] = glData[x*glPitch + srcX];
		}

		float leftX = static_cast<float>( static_cast<double>(gA.x) / texNormW );
		float topY = static_cast<float>( static_cast<double>(gA.y) / texNormH );
		float rightX = static_cast<float>( static_cast<double>(gA.x + h) / texNormW );
		float bottomY = static_cast<float>( static_cast<double>(gA.y + w) / texNormH );

		_glyph_coords.push_back( leftX );
		_glyph_coords.push_back( bottomY );
		_glyph_coords.push_back( leftX );
		_glyph_coords.push_back( topY );
		_glyph_coords.push_back( rightX );
		_glyph_coords.push_back( topY );
		_glyph_coords.push_back( rightX );
		_glyph_coords.push_back( bottomY );
	}
	else
	{
		for ( int y = 0; y < h; ++y )
		{
			uint8_t *bmLine = _glyph_bitmap.data() + bmW * ( gA.y + y ) + gA.x;
			const uint8_t *glLine = glData + y * glPitch;
			for ( int x = 0; x < w; ++x )
				bmLine[x] = glLine[x];
		}

		// things go in naturally, upper left of bitmap is at x, y
		float leftX = static_cast<float>( static_cast<double>(gA.x) / texNormW );
		float topY = static_cast<float>( static_cast<double>(gA.y) / texNormH );
		float rightX = static_cast<float>( static_cast<double>(gA.x + w) / texNormW );
		float bottomY = static_cast<float>( static_cast<double>(gA.y + h) / texNormH );

		_glyph_coords.push_back( leftX );
		_glyph_coords.push_back( topY );
		_glyph_coords.push_back( rightX );
		_glyph_coords.push_back( topY );
		_glyph_coords.push_back( rightX );
		_glyph_coords.push_back( bottomY );
		_glyph_coords.push_back( leftX );
		_glyph_coords.push_back( bottomY );
	}
	++_glyph_version;
}

////////////////////////////////////////

void
font::bump_glyph_store_size( void )
{
	int nPackW = _glyph_pack.width();
	int nPackH = _glyph_pack.height();
	if ( nPackW == 0 )
		nPackW = 256;
	else if ( nPackW <= nPackH )
		nPackW *= 2;

	if ( nPackH == 0 )
		nPackH = 256;
	else if ( nPackH <= nPackW )
		nPackH *= 2;

	std::cout << "Need to know the max texture size at some point, using 1024 for now as max, cur size: " << nPackW << ", " << nPackH << std::endl;
	if ( nPackW > 1024 || nPackH > 1024 )
		throw std::runtime_error( "Max font cache size reached" );

	_glyph_pack.reset( nPackW, nPackH, true );

	// if we ever stop using a vector for storing the bitmap data,
	// re-add the zero initialization for the spare line / column we
	// put between glyphs
	_glyph_bitmap.resize( static_cast<size_t>( nPackW * nPackH ), uint8_t(0) );
	_glyph_cache.clear();
	_glyph_coords.clear();
	_glyph_index_offset.clear();
}

////////////////////////////////////////

}

