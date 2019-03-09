// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>
#include <string>
#include <vector>
#include <base/string_util.h>
#include <base/format.h>

////////////////////////////////////////

class function
{
public:
	function( std::string n, size_t indent = 1 );

	void set_name( std::string n )
	{
		_name = std::move( n );
	}

	void add( char c )
	{
		_builder.push_back( c );
	}

	void add( const std::string &s )
	{
		_builder.append( s );
	}

	void add( const char *s )
	{
		_builder.append( s );
	}

	void push_text( bool trim = false );
	void push_code( void );
	void push_expr( void );

	void save( std::ostream &out );

	void indent( void )
	{
		push_code();
		_indent.push_back( '\t' );
	}

	void unindent( void )
	{
		push_code();
		_indent.pop_back();
	}

	void add_arg( std::string n )
	{
		_args.emplace_back( std::move( n ) );
	}

private:
	void emit_proto( std::ostream &os );

	std::string _name;
	std::vector<std::string> _args;
	std::string _code;
	std::string _builder;
	std::string _indent;
};

////////////////////////////////////////

