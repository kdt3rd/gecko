//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "file_sequence.h"
#include <base/contract.h>
#include <base/string_util.h>
#include <base/string_variable_util.h>

namespace media
{

////////////////////////////////////////

file_sequence::file_sequence( base::uri path, marker_style ms )
	: _uri( std::move( path ) ), _marker_style( ms ),
	  _pattern_size( 0 )
{
	precondition( !_uri.path().empty(), "invalid uri path" );

	_pattern_prefix = std::move( _uri.path().back() );
	_uri.path().pop_back();

	switch ( ms )
	{
		case MARKER_POUND:
			extract_pound_pattern();
			break;
		case MARKER_DOLLARF:
			extract_dollarf_pattern();
			break;
		case MARKER_PERCENTD:
			extract_percentd_pattern();
			break;
		case MARKER_AUTO:
			if ( extract_pound_pattern() )
				_marker_style = MARKER_POUND;
			else if ( extract_dollarf_pattern() )
				_marker_style = MARKER_DOLLARF;
			else if ( extract_percentd_pattern() )
				_marker_style = MARKER_PERCENTD;
			break;
		//default:
		//	throw_logic( "Invalid marker style" );
	}
}

////////////////////////////////////////

base::uri
file_sequence::uri( void ) const
{
	base::uri result( _uri );
	std::string fname = _pattern_prefix;
	fname.append( _pattern );
	fname.append( _pattern_suffix );
	result.path().emplace_back( std::move( fname ) );
	return result;
}

////////////////////////////////////////

base::uri file_sequence::get_frame( int64_t f ) const
{
	base::uri result( _uri );

	if ( _pattern_size > 0 )
	{
		std::string num = base::to_string( f );
		if ( num.size() < _pattern_size )
			num.insert( 0, _pattern_size - num.size(), '0' );

		result.path().emplace_back( _pattern_prefix + num + _pattern_suffix );
	}
	else
	{
		precondition( _pattern_suffix.empty(), "No pattern size, but pattern suffix is not empty" );
		result.path().push_back( _pattern_prefix );
	}

	return result;
}

////////////////////////////////////////

bool file_sequence::extract_frame( const base::uri &u, int64_t &f ) const
{
	f = 0;
	if ( u.scheme() != _uri.scheme() )
		return false;
	if ( u.user() != _uri.user() )
		return false;
	if ( u.host() != _uri.host() )
		return false;
	if ( u.port() != _uri.port() )
		return false;
	if ( u.path().size() != ( _uri.path().size() + 1 ) )
		return false;

	for ( size_t p = 0, n = _uri.path().size(); p < n; ++p )
	{
		if ( u.path( p ) != _uri.path( p ) )
			return false;
	}

	const std::string &fn = u.path().back();
	if ( _pattern_size == 0 )
	{
		if ( fn != _pattern_prefix )
			return false;
	}
	else
	{
		if ( fn.size() < ( _pattern_prefix.size() + _pattern_suffix.size() + 1 ) )
			return false;

		if ( fn.compare( 0, _pattern_prefix.size(), _pattern_prefix ) != 0 )
			return false;

		int64_t fnum = 0;
		std::string::size_type fpos = _pattern_prefix.size();
		size_t digCount = 0;
		while ( digCount < _pattern_size && fpos < fn.size() && std::isdigit( fn[fpos] ) )
		{
			fnum = fnum * 10 + ( int64_t(fn[fpos]) - int64_t('0') );
			++fpos;
			++digCount;
		}
		if ( ( fn.size() - fpos ) != _pattern_suffix.size() )
			return false;
		if ( fn.compare( fpos, _pattern_suffix.size(), _pattern_suffix ) != 0 )
			return false;

		f = fnum;
	}

	return true;
}

////////////////////////////////////////

file_sequence
file_sequence::guess( base::uri u, marker_style ms )
{
	precondition( !u.path().empty(), "invalid empty uri path" );
	if ( ms == MARKER_AUTO )
		ms = MARKER_POUND;

	std::string &fn = u.path().back();

	// we'll pull the longest string of digits as the frame number
	std::string::size_type longestStart = std::string::npos;
	size_t count = 0;
	for ( size_t i = 0; i < fn.size(); ++i )
	{
		if ( std::isdigit( fn[i] ) )
		{
			size_t tmpPos = i + 1;
			size_t tmpCnt = 1;
			while ( tmpPos < fn.size() && std::isdigit( fn[tmpPos] ) )
			{
				++tmpPos;
				++tmpCnt;
			}
			if ( tmpCnt > count )
			{
				longestStart = i;
				count = tmpCnt;
			}
			i = tmpPos;
		}
	}

	if ( count > 0 )
	{
		std::string seqPref = fn.substr( 0, longestStart );
		std::string seqSuf = fn.substr( longestStart + count );

		if ( ms == MARKER_POUND )
		{
			fn = seqPref + std::string( count, '#' ) + seqSuf;
		}
		else if ( ms == MARKER_DOLLARF )
		{
			if ( fn[longestStart] == '0' )
				fn = seqPref + std::string( "$F" ) + std::to_string( count ) + seqSuf;
			else
				fn = seqPref + std::string( "$F" ) + seqSuf;
		}
		else if ( ms == MARKER_PERCENTD )
		{
			if ( fn[longestStart] == '0' )
				fn = seqPref + std::string( "%0" ) + std::to_string( count ) + std::string( 1, 'd' ) + seqSuf;
			else
				fn = seqPref + std::string( "%d" ) + seqSuf;
		}
		else
			throw_logic( "Unknown marker style" );
	}

	return file_sequence( std::move( u ), ms );
}

////////////////////////////////////////

bool
file_sequence::extract_pound_pattern( void )
{
	size_t start = _pattern_prefix.find( '#' );
	if ( start == std::string::npos )
		return false;

	_pattern_size = _pattern_prefix.rfind( '#' ) - start + 1;
	split_pattern( start, _pattern_size );
	return true;
}

////////////////////////////////////////


bool
file_sequence::extract_dollarf_pattern( void )
{
	std::vector<std::string> pieces;
	std::vector<std::string> vars;
	if ( base::find_unix_vars( pieces, vars, _pattern_prefix ) )
	{
		// had some variables
		size_t frameVar = size_t(-1);

		for ( size_t i = 0; i < vars.size(); ++i )
		{
			const std::string &v = vars[i];
			if ( ! v.empty() && ( v[0] == 'f' || v[0] == 'F' ) )
			{
				int size = 0;
				bool ok = true;
				for ( size_t x = 1; x < v.size(); ++x )
				{
					if ( ! std::isdigit( v[x] ) )
					{
						ok = false;
						break;
					}
					else
					{
						size = size * 10 + ( int(v[x]) - int('0') );
					}
				}
				if ( ok )
				{
					frameVar = i;
					_pattern = "$" + v;
					_pattern_size = ( size == 0 ) ? size_t(1) : static_cast<size_t>( size );
					break;
				}
			}
		}

		if ( frameVar != size_t(-1) )
		{
			std::string::size_type vpos = _pattern_prefix.find( _pattern );
			std::string::size_type len = _pattern.size();
			if ( vpos == std::string::npos )
			{
				std::string tmp = "${" + vars[frameVar] + "}";
				vpos = _pattern_prefix.find( tmp );
				len = tmp.size();
				if ( vpos == std::string::npos )
					throw_runtime( "Invalid assumption about variable names looking for $F variable in {0} (var name '{1}')", _pattern_prefix, vars[frameVar] );
			}
			_pattern_suffix = _pattern_prefix.substr( vpos + len );
			_pattern_prefix.erase( vpos );
			return true;
		}
	}

	return false;
}

////////////////////////////////////////

bool
file_sequence::extract_percentd_pattern( void )
{
	std::string::size_type start = _pattern_prefix.find_first_of( '%' );

	if ( start != std::string::npos )
	{
		std::string::size_type tmpPos = start + 1;
		// this will work if the user puts in either %07d or %7d
		// it doesn't match printf which would space pad the latter
		// but who wants that?
		int count = 0;
		while ( tmpPos < _pattern_prefix.size() && std::isdigit( _pattern_prefix[tmpPos] ) )
		{
			int dig = int( _pattern_prefix[tmpPos] ) - int( '0' );
			count = count * 10 + dig;
			++tmpPos;
		}

		if ( _pattern_prefix[tmpPos] != 'd' )
			throw_runtime( "Expect a sequence marker of the form '%07d' or '%d' in {0}", _pattern_prefix );
		if ( count == 0 )
			count = 1;
		_pattern_size = static_cast<size_t>( count );
		split_pattern( start, tmpPos - start + 1 );
		return true;
	}

	return false;
}

////////////////////////////////////////

void
file_sequence::split_pattern( size_t start, size_t sz )
{
	_pattern = _pattern_prefix.substr( start, sz );
	_pattern_prefix.erase( start, sz );
	if ( _pattern_prefix.size() > start )
	{
		_pattern_suffix = _pattern_prefix.substr( start );
		_pattern_prefix.erase( start );
	}
}

////////////////////////////////////////

}
