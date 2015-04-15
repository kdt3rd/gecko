
#include "file_sequence.h"
#include <base/contract.h>
#include <base/string_util.h>

namespace media
{

////////////////////////////////////////

file_sequence::file_sequence( base::uri path )
	: _uri( std::move( path ) )
{
	precondition( !_uri.path().empty(), "invali uri path" );

	const std::string &fname = _uri.path().back();
	_pattern_start = fname.find( '#' );
	if ( _pattern_start != std::string::npos )
	{
		_pattern_size = fname.rfind( '#' ) - _pattern_start + 1;
		_pattern_rest = fname.size() - _pattern_start - _pattern_size;
		precondition( _pattern_start < fname.size(), "invalid file sequence pattern" );
	}
	else
	{
		_pattern_start = fname.size();
		_pattern_size = 0;
		_pattern_rest = 0;

	}
}

////////////////////////////////////////

base::uri file_sequence::get_frame( int64_t f )
{
	base::uri result( _uri );

	if ( _pattern_size > 0 )
	{
		std::string num = base::to_string( f );
		if ( num.size() < _pattern_size )
			num.insert( 0, _pattern_size - num.size(), '0' );

		result.path().back().replace( _pattern_start, _pattern_size, num );
	}

	return result;
}

////////////////////////////////////////

bool file_sequence::extract_frame( const base::uri &u, int64_t &f )
{
	if ( u.scheme() != _uri.scheme() )
		return false;
	if ( u.user() != _uri.user() )
		return false;
	if ( u.host() != _uri.host() )
		return false;
	if ( u.port() != _uri.port() )
		return false;
	if ( u.path().size() != _uri.path().size() )
		return false;

	for ( size_t p = 0, n = _uri.path().size() - 1; p < n; ++p )
	{
		if ( u.path( p ) != _uri.path( p ) )
			return false;
	}

	const std::string &fn = u.path().back();
	if ( _pattern_size == 0 )
	{
		if ( fn != _uri.path().back() )
			return false;

		f = 0;
	}
	else
	{
		if ( fn.size() < _pattern_start + _pattern_rest + 1 )
			return false;
		if ( _uri.path().back().compare( 0, _pattern_start, fn, 0, _pattern_start ) != 0 )
			return false;
		if ( _uri.path().back().compare( _pattern_start + _pattern_size, _pattern_rest, fn, fn.size() - _pattern_rest, fn.size() ) != 0 )
			return false;

		std::string frame = fn.substr( _pattern_start, fn.size() - _pattern_start - _pattern_rest );
		size_t pos = 0;
		f = std::stol( frame, &pos );
		if ( frame[pos] != '\0' )
			return false;
	}

	return true;
}

////////////////////////////////////////

}

