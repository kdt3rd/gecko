//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font.h"
#include <cwchar>
#include <limits>
#include <base/contract.h>

namespace script
{

////////////////////////////////////////

font::font( std::string fam, std::string sty, extent_type sz )
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
		extent_type k = kerning( static_cast<char32_t>( prev ), static_cast<char32_t>( ccode ) );

		if ( prev == L'\0' )
			retval.x_bearing = gext.x_bearing;

		retval.y_bearing = std::max( retval.y_bearing, gext.y_bearing );
		retval.x_advance -= k;
		retval.height = std::max( retval.height, gext.height );
		retval.x_advance += gext.x_advance;
		retval.y_advance += gext.y_advance;
		retval.width = retval.x_advance - gext.x_advance + gext.x_bearing + gext.width;

		prev = ccode;
	}

	return retval;
}

////////////////////////////////////////

void
font::render(
	const std::function<void(coord_type,coord_type,coord_type,coord_type)> &add_point,
	const std::function<void(size_t,size_t,size_t)> &add_tri,
	coord_type startX, coord_type startY, const std::string &utf8 )
{
	if ( utf8.empty() )
		return;

	std::wstring tmp;
	{
		std::mbstate_t s = std::mbstate_t();
		size_t curpos = 0;
		size_t nleft = utf8.size();
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
			tmp.push_back( ccode );
		}
	}

	// Preload all of the glyphs
	for ( wchar_t ccode: tmp )
		get_glyph( static_cast<char32_t>( ccode ) );

	wchar_t prev = L'\0';
	extent_type curposX = startX;
	size_t points = 0;
	for ( wchar_t ccode: tmp )
	{
		const text_extents &gext = get_glyph( static_cast<char32_t>( ccode ) );
		extent_type k = kerning( static_cast<char32_t>( prev ), static_cast<char32_t>( ccode ) );
		curposX -= k;

		auto idx_base_i = _glyph_index_offset.find( static_cast<char32_t>( ccode ) );
		if ( idx_base_i != _glyph_index_offset.end() )
		{
			size_t coordOff = idx_base_i->second;
			size_t idx_base = points;

			if ( ( idx_base / 2 + 3 ) > static_cast<size_t>( std::numeric_limits<uint16_t>::max() ) )
				throw std::runtime_error( "String too long for OpenGL ES" );

			extent_type upperY = startY - gext.y_bearing;
			extent_type lowerY = upperY + gext.height;
			extent_type leftX = curposX + gext.x_bearing;
			extent_type rightX = leftX + gext.width;

			add_point(
				static_cast<coord_type>( leftX ), static_cast<coord_type>( upperY ),
				_glyph_coords[coordOff + 0], _glyph_coords[coordOff + 1] );

			add_point(
				static_cast<coord_type>( rightX ), static_cast<coord_type>( upperY ),
				_glyph_coords[coordOff + 2], _glyph_coords[coordOff + 3] );

			add_point(
				static_cast<coord_type>( rightX ), static_cast<coord_type>( lowerY ),
				_glyph_coords[coordOff + 4], _glyph_coords[coordOff + 5] );

			add_point(
				static_cast<coord_type>( leftX ), static_cast<coord_type>( lowerY ),
				_glyph_coords[coordOff + 6], _glyph_coords[coordOff + 7] );

			points += 4;

			uint16_t idxVal = static_cast<uint16_t>( idx_base );
			add_tri( idxVal, idxVal + 1, idxVal + 2 );
			add_tri( idxVal + 2, idxVal + 3, idxVal );
		}

		curposX += gext.x_advance;
		prev = ccode;
	}
}

////////////////////////////////////////

std::pair<extent_type, extent_type>
font::align_text( const std::string &utf8, extent_type x1, extent_type y1, extent_type x2, extent_type y2, base::alignment a )
{
	base::rect<extent_type> rect;
	rect.set_x1( std::ceil( x1 ) );
	rect.set_y1( std::ceil( y1 ) );
	rect.set_x2( std::floor( x2 ) );
	rect.set_y2( std::floor( y2 ) );

	if ( utf8.empty() )
		return { rect.x(), rect.y() };

	// TODO: add multi-line support?

	font_extents fex = extents();
	text_extents tex = extents( utf8 );

	extent_type y = 0.0, x = 0.0;
	extent_type textHeight = fex.ascent - fex.descent;

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
			x = rect.x2() - tex.width;
			break;

		case base::alignment::CENTER:
		case base::alignment::TOP:
		case base::alignment::BOTTOM:
			x = rect.x1() + std::round( ( rect.width() - tex.width ) / 2.0 );
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
	extent_type texNormW = static_cast<extent_type>( bmW );
	extent_type texNormH = static_cast<extent_type>( bmH );

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

		coord_type leftX = static_cast<coord_type>( static_cast<extent_type>(gA.x) / texNormW );
		coord_type topY = static_cast<coord_type>( static_cast<extent_type>(gA.y) / texNormH );
		coord_type rightX = static_cast<coord_type>( static_cast<extent_type>(gA.x + h) / texNormW );
		coord_type bottomY = static_cast<coord_type>( static_cast<extent_type>(gA.y + w) / texNormH );

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
		coord_type leftX = static_cast<coord_type>( static_cast<extent_type>(gA.x) / texNormW );
		coord_type topY = static_cast<coord_type>( static_cast<extent_type>(gA.y) / texNormH );
		coord_type rightX = static_cast<coord_type>( static_cast<extent_type>(gA.x + w) / texNormW );
		coord_type bottomY = static_cast<coord_type>( static_cast<extent_type>(gA.y + h) / texNormH );

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
	bool didBump = false;
	if ( nPackW == 0 )
	{
		didBump = true;
		nPackW = 256;
		nPackH = 256;
	}
	else
	{
		int newW = std::min( _max_glyph_w, nPackW * 2 );
		didBump = newW != nPackW;
		nPackW = newW;
	}

	if ( nPackH == 0 )
		nPackH = 256;
	else if ( ! didBump )
	{
		int newH = std::min( _max_glyph_h, nPackH * 2 );
		didBump = newH != nPackH;
		nPackH = newH;
	}

	if ( ! didBump )
		throw_runtime( "Max font cache size reached" );

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

