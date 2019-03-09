// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "function.h"

////////////////////////////////////////

class parser
{
public:
	parser( const std::string &main );

	void parse( std::istream &in );
	void save( std::ostream &out );

private:
	void parse_text( std::istream &in );
	void parse_expr( std::istream &in );
	void parse_code( std::istream &in );
	void parse_directive( std::istream &in );
	void parse_comment( std::istream &in );
	void parse_string( std::istream &in );

	std::string _indent;
	std::vector<std::string> _includes;
	function _func;
};

////////////////////////////////////////

