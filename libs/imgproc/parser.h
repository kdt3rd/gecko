//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <map>
#include <string>
#include <functional>

#include "expr.h"
#include "function.h"
#include "token.h"
#include "message.h"

namespace imgproc
{

////////////////////////////////////////

class parser
{
public:
	parser( std::map<std::u32string,std::shared_ptr<function>> &funcs, std::istream &in, utf::mode = utf::UTF8 );
	parser( std::map<std::u32string,std::shared_ptr<function>> &funcs, const iterator &tok );

	void parse( void );

	bool has_messages( void ) const { return !_messages.empty(); }
	bool has_errors( void ) const { return _has_errors; }
	const std::vector<message> &messages( void ) const { return _messages; }

private:
	location previous_end( void ) const { return _previous_end; }
	location start_location( void ) const { return _token.start_location(); }
	location end_location( void ) const { return _token.end_location(); }

	bool expect( const std::u32string &c );
	bool expect( token_type t );

	bool is_loop_modifier( const std::u32string &n );

	void next_token( void );

	void add_error( const std::string &msg );
	void add_error( const std::string &msg, const location &l );
	void add_warning( const std::string &msg );
	void add_warning( const std::string &msg, const location &l );
	void add_info( const std::string &msg );
	void add_info( const std::string &msg, const location &l );

	void comments( void );

	std::shared_ptr<expr> expression( void );
	std::shared_ptr<expr> primary_expr( void );
	std::shared_ptr<expr> expr_block( void );
	std::shared_ptr<expr> if_expr( void );
	std::shared_ptr<expr> loop_range( void );
	std::shared_ptr<expr> loop_expr( void );
	std::vector<std::shared_ptr<expr>> arguments( void );
	
	std::unique_ptr<function> get_function( void );

	void id_list( const std::function<void(std::u32string &)> &cb );
	void arg_list( const std::function<void(std::u32string &,std::u32string &)> &cb );
	bool arg( const std::function<void(std::u32string &,std::u32string &)> &cb );

	location _previous_end;
	iterator _token;
	std::map<std::u32string,std::shared_ptr<function>> &_funcs;
	std::vector<std::u32string> _comments;
	std::vector<message> _messages;
	std::map<std::shared_ptr<expr>,std::pair<location,location>> _expr_locs;
	bool _parsing_range = false;
	bool _parsing_assign = false;
	bool _parsing_block = false;
	bool _has_errors = false;
};

////////////////////////////////////////

}

