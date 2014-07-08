
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

		const text_extents &gext = get_glyph( ccode );
		double k = kerning( prev, ccode );

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
font::render( std::vector<float> &outCoords,
			  std::vector<float> &texCoords,
			  std::vector<uint16_t> &renderIndices,
			  const base::point &start, const std::string &utf8 )
{
	if ( utf8.empty() )
		return;

	// In case we have to change the texture size in the
	// middle of rendering, add this extra loop
	uint32_t fVer = _glyph_version;
	do
	{
		outCoords.clear();
		texCoords.clear();
		renderIndices.clear();

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

			const text_extents &gext = get_glyph( ccode );
			double k = kerning( prev, ccode );
			curposX -= k;

			auto idx_base_i = _glyph_index_offset.find( ccode );
			if ( idx_base_i != _glyph_index_offset.end() )
			{
				size_t coordOff = idx_base_i->second;
				size_t idx_base = texCoords.size();

				if ( ( idx_base / 2 + 3 ) > static_cast<size_t>( std::numeric_limits<uint16_t>::max() ) )
					throw std::runtime_error( "String too long for OpenGL ES" );


				for ( size_t i = 0; i < 8; ++i )
					texCoords.push_back( _glyph_coords[coordOff + i] );

				uint16_t idxVal = static_cast<uint16_t>( idx_base / 2 );
				renderIndices.push_back( idxVal );
				renderIndices.push_back( idxVal + 1 );
				renderIndices.push_back( idxVal + 2 );
				renderIndices.push_back( idxVal + 2 );
				renderIndices.push_back( idxVal + 3 );
				renderIndices.push_back( idxVal );

				double upperY = start.y() - gext.y_bearing;
				double lowerY = upperY + gext.height;
				double leftX = curposX + gext.x_bearing;
				double rightX = leftX + gext.width;

				outCoords.push_back( leftX ); outCoords.push_back( upperY );
				outCoords.push_back( rightX ); outCoords.push_back( upperY );
				outCoords.push_back( rightX ); outCoords.push_back( lowerY );
				outCoords.push_back( leftX ); outCoords.push_back( lowerY );
			}

			curposX += gext.x_advance;
			prev = ccode;
		}
	} while ( fVer != _glyph_version );
}

////////////////////////////////////////

void
font::add_glyph( char32_t char_code, const uint8_t *glData, size_t glPitch, size_t w, size_t h )
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
		for ( size_t y = 0; y <= w; ++y )
		{
			size_t destY = gA.y + y;
			if ( y == w )
			{
				for ( size_t x = 0, destX = gA.x; x <= h; ++x, ++destX )
					bmData[destY*bmW + destX] = 0;
			}
			else
			{
				int srcX = w - y - 1;
				int destX = gA.x;
				for ( size_t x = 0; x < h; ++x, ++destX )
				{
					bmData[destY*bmW + destX] = glData[x*glPitch + srcX];
				}
				bmData[destY*bmW + destX] = 0;
			}
		}

		float leftX = static_cast<double>(gA.x) / texNormW;
		float topY = static_cast<double>(gA.y) / texNormH;
		float rightX = static_cast<double>(gA.x + h) / texNormW;
		float bottomY = static_cast<double>(gA.y + w) / texNormH;

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
		for ( size_t y = 0; y <= h; ++y )
		{
			uint8_t *bmLine = _glyph_bitmap.data() + bmW * ( gA.y + y );
			if ( y == h )
			{
				for ( size_t x = 0; x <= w; ++x )
					bmLine[x] = 0;
			}
			else
			{
				bmLine += gA.x;
				const uint8_t *glLine = glData + y * glPitch;
				for ( size_t x = 0; x < w; ++x )
					bmLine[x] = glLine[x];
				bmLine[w] = 0;
			}
		}

		// things go in naturally, upper left of bitmap is at x, y
		float leftX = static_cast<double>(gA.x) / texNormW;
		float topY = static_cast<double>(gA.y) / texNormH;
		float rightX = static_cast<double>(gA.x + w) / texNormW;
		float bottomY = static_cast<double>(gA.y + h) / texNormH;

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
	size_t nPackW = _glyph_pack.width();
	size_t nPackH = _glyph_pack.height();
	if ( nPackW == 0 )
		nPackW = 256;
	else if ( nPackW <= nPackH )
		nPackW *= 2;

	if ( nPackH == 0 )
		nPackH = 256;
	else if ( nPackH <= nPackW )
		nPackH *= 2;

	std::cout << "Need to know the max texture size at some point, using 1024 for now..." << std::endl;
	if ( nPackW > 1024 || nPackH > 1024 )
		throw std::runtime_error( "Max font cache size reached" );

	_glyph_pack.reset( nPackW, nPackH, true );

	_glyph_bitmap.resize( nPackW * nPackH );
	_glyph_cache.clear();
	_glyph_coords.clear();
	_glyph_index_offset.clear();
}

////////////////////////////////////////

}

