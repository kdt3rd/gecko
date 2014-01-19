
#include "gradient.h"
#include <gl/context.h>
#include <gl/check.h>
#include <cmath>

////////////////////////////////////////

namespace
{
	inline uint8_t clampColorByte( double c )
	{
		c = std::max( 0.0, std::min( 255.0, std::floor( c * 256.0 ) ) );
		return uint8_t( c );
	}
}

////////////////////////////////////////

namespace draw
{

////////////////////////////////////////

core::color gradient::sample( double v ) const
{
	size_t i = 0;
	while ( i < _stops.size() )
	{
		if ( _stops[i].first > v )
			break;
		++i;
	}

	if ( i < _stops.size() )
	{
		if ( i == 0 )
			return _stops[0].second;

		double range = _stops[i].first - _stops[i-1].first;
		double mix = ( _stops[i].first - v ) / range;

		return core::color::mix( _stops[i].second, _stops[i-1].second, mix );
	}

	return _stops.back().second;
}

////////////////////////////////////////

std::shared_ptr<gl::texture> gradient::generate( gl::context &ctxt )
{
	std::vector<uint8_t> bytes;
	bytes.resize( 128 * 4 );

	for ( size_t i = 0; i < 128; ++i )
	{
		double stop = double(i)/128.0;
		core::color c = sample( stop );
		bytes[i*4+0] = clampColorByte( c.red() );
		bytes[i*4+1] = clampColorByte( c.green() );
		bytes[i*4+2] = clampColorByte( c.blue() );
		bytes[i*4+3] = clampColorByte( c.alpha() );
//		std::cout << (int)bytes[i*4+0] << ' ' << (int)bytes[i*4+1] << ' ' << (int)bytes[i*4+2] << ' ' << (int)bytes[i*4+3] << std::endl;
	}

	auto ret = ctxt.new_texture();
	checkgl();
	auto txt = ret->bind( gl::texture::target::TEXTURE_RECTANGLE );
	checkgl();
	txt.image_2d( gl::format::RGBA, bytes.size(), 1, gl::image_type::UNSIGNED_BYTE, bytes.data() );
	checkgl();
	return ret;
}

////////////////////////////////////////

}
