//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "variable_set.h"
#include "string_variable_util.h"
#include "contract.h"


////////////////////////////////////////


namespace base
{


////////////////////////////////////////


void
variable_set::set( const std::string &v, std::string val )
{
	unset_array( v );
	unset_hash( v );
	_simple_vars[v] = std::move( val );
}


////////////////////////////////////////


void
variable_set::set( const std::string &v, std::vector<std::string> val )
{
	unset_simple( v );
	unset_hash( v );
	_array_vars[v] = std::move( val );
}


////////////////////////////////////////


void
variable_set::set( const std::string &v, std::map<std::string, std::string> val )
{
	unset_simple( v );
	unset_array( v );
	_hash_vars[v] = std::move( val );
}


////////////////////////////////////////


void
variable_set::set( const std::string &v, size_t i, const std::string &val )
{
	auto s = _simple_vars.find( v );
	if ( s != _simple_vars.end() )
	{
		if ( s->second.size() <= i )
		{
			while ( s->second.size() < i )
				s->second.push_back( ' ' );
			s->second.append( val );
		}
		else
			s->second.replace( i, 1, val );

		return;
	}

	auto a = _array_vars.find( v );
	if ( a != _array_vars.end() )
	{
		if ( a->second.size() <= i )
			a->second.resize( i + 1 );
		a->second[i] = val;
		return;
	}

	auto h = _hash_vars.find( v );
	if ( h != _hash_vars.end() )
	{
		h->second[std::to_string(i)] = val;
		return;
	}

	auto &av = _array_vars[v];
	av.resize( i + 1 );
	av[i] = val;
}


////////////////////////////////////////


void
variable_set::set( const std::string &v, const std::string &key, const std::string &val )
{
	auto s = _simple_vars.find( v );
	if ( s != _simple_vars.end() )
		throw_runtime( "Variable {0} not a hash variable", v );

	auto a = _array_vars.find( v );
	if ( a != _array_vars.end() )
		throw_runtime( "Variable {0} not a hash variable", v );

	_hash_vars[v][key] = val;
}


////////////////////////////////////////


void
variable_set::append( const std::string &v, const std::string &val )
{
	auto s = _simple_vars.find( v );
	if ( s != _simple_vars.end() )
	{
		s->second.append( val );
		return;
	}

	auto a = _array_vars.find( v );
	if ( a != _array_vars.end() )
	{
		a->second.push_back( val );
		return;
	}

	auto h = _hash_vars.find( v );
	if ( h != _hash_vars.end() )
	{
		// what does this append mean, add an empty entry for now
		h->second[val] = std::string();
		return;
	}

	_simple_vars[v] = val;
}


////////////////////////////////////////


std::string
variable_set::get( const std::string &v ) const
{
	auto s = _simple_vars.find( v );
	if ( s != _simple_vars.end() )
		return s->second;
	auto a = _array_vars.find( v );
	if ( a != _array_vars.end() )
	{
		std::string retval;
		for ( const auto &av: a->second )
		{
			if ( ! retval.empty() )
				retval.push_back( ' ' );
			retval.append( av );
		}
		return retval;
	}

	auto h = _hash_vars.find( v );
	if ( h != _hash_vars.end() )
	{
		std::string retval;
		for ( const auto &hv: h->second )
		{
			if ( ! retval.empty() )
				retval.push_back( ' ' );
			retval.append( hv.second );
		}
		return retval;
	}
	return std::string();
}


////////////////////////////////////////


std::string
variable_set::get( const std::string &v, size_t i ) const
{
	auto s = _simple_vars.find( v );
	if ( s != _simple_vars.end() )
	{
		if ( s->second.size() > i )
			return std::string( 1, s->second[i] );
		return std::string();
	}

	auto a = _array_vars.find( v );
	if ( a != _array_vars.end() )
	{
		if ( a->second.size() > i )
			return a->second[i];
		return std::string();
	}

	auto h = _hash_vars.find( v );
	if ( h != _hash_vars.end() )
	{
		std::string t = std::to_string( i );
		auto x = h->second.find( t );
		if ( x != h->second.end() )
			return x->second;

		return std::string();
	}
	return std::string();
}


////////////////////////////////////////


std::string
variable_set::get( const std::string &v, const std::string &hkey ) const
{
	auto s = _simple_vars.find( v );
	if ( s != _simple_vars.end() )
	{
		size_t pos = 0;
		size_t idx = std::stoull( hkey, &pos );
		if ( pos != hkey.size() )
			throw_runtime( "Variable {0} is a simple variable, not a hash variable, expect integer index, got {1}", v, hkey );
		if ( idx == 0 )
			throw_runtime( "Invalid array index 0 dereferencing variable {0}", v );
		--idx;
		if ( idx < s->second.size() )
			return std::string( 1, s->second[idx] );
		return std::string();
	}

	auto a = _array_vars.find( v );
	if ( a != _array_vars.end() )
	{
		size_t pos = 0;
		size_t idx = std::stoull( hkey, &pos );
		if ( pos != hkey.size() )
			throw_runtime( "Variable {0} is an array, not a hash variable, expect integer index, got {1}", v, hkey );
		if ( idx == 0 )
			throw_runtime( "Invalid array index 0 dereferencing variable {0}", v );
		--idx;
		if ( idx < a->second.size() )
			return a->second[idx];
		return std::string();
	}

	auto h = _hash_vars.find( v );
	if ( h != _hash_vars.end() )
	{
		auto x = h->second.find( hkey );
		if ( x != h->second.end() )
			return x->second;

		return std::string();
	}
	return std::string();
}


////////////////////////////////////////


std::string
variable_set::lookup( const std::string &v )
{
	if ( v.empty() )
		return std::string();

	std::string::size_type subvar = v.find_first_of( '$' );
	if ( subvar != std::string::npos )
		return lookup( replace_unix_vars( v, std::bind( &variable_set::lookup, this, std::placeholders::_1 ) ) );

	std::string::size_type apos = v.find_first_of( '[' );
	std::string::size_type cpos = v.find_first_of( ":#/%" );

	precondition( apos == std::string::npos || apos < cpos, "Invalid assumption about array references in variable {0}", v );

	if ( v[0] == '#' )
	{
		precondition( apos == std::string::npos && cpos == std::string::npos, "Unable to get the length of variable {0} and retrieve and index or modify the variable at the same time", v );
		return std::to_string( length( v.substr( 1 ) ) );
	}

	std::vector<std::string> vals;
	std::string vname, ascript;
	if ( apos != std::string::npos )
	{
		vname = v.substr( 0, apos );
		std::string::size_type aend = v.find_first_of( ']', apos );
		ascript = v.substr( apos + 1, aend - apos - 1 );
		if ( ascript == "*" || ascript == "@" )
		{
			auto a = _array_vars.find( vname );
			if ( a != _array_vars.end() )
				vals = a->second;
			else
			{
				auto h = _hash_vars.find( vname );
				if ( h != _hash_vars.end() )
				{
					for ( auto &hv: h->second )
						vals.push_back( hv.second );
				}
				else
				{
					auto s = _simple_vars.find( vname );
					if ( s != _simple_vars.end() )
						vals.push_back( s->second );
				}
			}
		}
		else
			vals.emplace_back( get( vname, ascript ) );
	}
	else if ( cpos != std::string::npos )
	{
		vname = v.substr( 0, cpos );
		vals.emplace_back( get( vname ) );
	}
	else
		return get( v );

	if ( cpos != std::string::npos )
	{
		std::string::size_type ep = v.size() - 1;

		if ( v[cpos] == ':' )
		{
			if ( cpos == ep )
				throw_runtime( "Invalid variable modifier on '{0}', need argument to :", v );

			if ( v[cpos+1] == '-' )
			{
				if ( vals.empty() || ( vals.size() == 1 && vals[0].empty() ) )
				{
					vals.clear();
					vals.emplace_back( v.substr( cpos + 2 ) );
				}
			}
			else if ( v[cpos+1] == '=' )
			{
				if ( vals.empty() || ( vals.size() == 1 && vals[0].empty() ) )
				{
					vals.clear();
					std::string nval = v.substr( cpos + 2 );
					vals.push_back( nval );
					if ( ascript.empty() )
						set( vname, nval );
					else
						set( vname, ascript, nval );
				}
			}
			else
			{
				std::string offpair = v.substr( cpos + 2 );
				size_t pos = 0;
				std::string::size_type off = std::stoull( offpair, &pos );
				std::string::size_type length = std::string::npos;
				if ( pos < offpair.size() )
				{
					length = std::stoull( offpair.substr( pos + 1 ), &pos );
					if ( pos == 0 )
						throw_runtime( "Invalid lenght argument in variable dereference '{0}'", v );
				}
				if ( vals.size() == 1 )
				{
					if ( off < vals[0].size() )
						vals[0] = vals[0].substr( off, length );
					else
						vals.clear();
				}
				else
				{
					if ( off < vals.size() )
					{
						vals.erase( vals.begin(), vals.begin() + off );
						if ( length < vals.size() )
							vals.erase( vals.begin() + length, vals.end() );
					}
					else
						vals.clear();
				}
			}
		}
		else if ( v[cpos] == '#' )
		{
			throw_not_yet();
		}
		else if ( v[cpos] == '%' )
		{
			throw_not_yet();
		}
		else if ( v[cpos] == '/' )
		{
			if ( cpos == ep )
				throw_runtime( "Invalid variable modifier on '{0}', need argument to /", v );

			std::string::size_type replP = v.find_first_of( '/', cpos + 1 );
			if ( replP == std::string::npos )
				throw_runtime( "Invalid variable modifier on '{0}', need replacement to /pattern/replacement", v );
			std::string pat = v.substr( cpos + 1, replP - cpos - 1 );
			std::string repl;
			if ( replP < ep )
				repl = v.substr( replP + 1 );
			for ( auto &v: vals )
			{
				std::string::size_type x = v.find( pat );
				while ( x != std::string::npos )
				{
					v.replace( x, pat.size(), repl );
					x = v.find( pat, x + repl.size() );
				}
			}
		}
		else
			throw_logic( "Unhandled modification character '{0}' in '{1}'", v[cpos], v );
	}

	if ( vals.empty() )
		return std::string();

	if ( vals.size() == 1 )
		return std::move( vals[0] );

	for ( size_t i = 1; i < vals.size(); ++i )
	{
		vals[0].push_back( ' ' );
		vals[0].append( vals[i] );
	}

	return std::move( vals[0] );
}


////////////////////////////////////////


void
variable_set::unset( const std::string &v )
{
	unset_simple( v );
	unset_array( v );
	unset_hash( v );
}


////////////////////////////////////////


size_t
variable_set::length( const std::string &v ) const
{
	auto s = _simple_vars.find( v );
	if ( s != _simple_vars.end() )
		return s->second.size();

	auto a = _array_vars.find( v );
	if ( a != _array_vars.end() )
		return a->second.size();

	auto h = _hash_vars.find( v );
	if ( h != _hash_vars.end() )
		return h->second.size();

	return 0;
}


////////////////////////////////////////


bool
variable_set::defined( const std::string &v ) const
{
	if ( _simple_vars.find( v ) != _simple_vars.end() )
		return true;

	if ( _array_vars.find( v ) != _array_vars.end() )
		return true;

	if ( _hash_vars.find( v ) != _hash_vars.end() )
		return true;

	return false;
}


////////////////////////////////////////


std::string
variable_set::substitute( const std::string &v )
{
	return replace_unix_vars( v,
							  std::bind( &variable_set::lookup, this, std::placeholders::_1 ) );
}


////////////////////////////////////////


void
variable_set::unset_simple( const std::string &v )
{
	auto i = _simple_vars.find( v );
	if ( i != _simple_vars.end() )
		_simple_vars.erase( i );
}


////////////////////////////////////////


void
variable_set::unset_array( const std::string &v )
{
	auto i = _array_vars.find( v );
	if ( i != _array_vars.end() )
		_array_vars.erase( i );
}


////////////////////////////////////////


void
variable_set::unset_hash( const std::string &v )
{
	auto i = _hash_vars.find( v );
	if ( i != _hash_vars.end() )
		_hash_vars.erase( i );
}


////////////////////////////////////////


} // base



