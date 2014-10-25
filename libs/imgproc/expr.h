
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <base/variant.h>
#include "type.h"

namespace imgproc
{

////////////////////////////////////////

// Forward declare a generic expression
class expr;

// Forward declare function for lamdba expressions
class function;

////////////////////////////////////////

class number_expr
{
public:
	number_expr( std::u32string t )
		: _value( t )
	{
	}

	const std::u32string &value( void ) const
	{
		return _value;
	}

protected:
	std::u32string _value;
};

////////////////////////////////////////

class identifier_expr
{
public:
	identifier_expr( std::u32string t )
		: _value( t )
	{
	}

	const std::u32string &value( void ) const
	{
		return _value;
	}

protected:
	std::u32string _value;
};

////////////////////////////////////////

class prefix_expr
{
public:
	prefix_expr( const std::u32string &op, const std::shared_ptr<expr> &x )
		: _op( op ), _x( x )
	{
	}

	std::u32string operation( void ) const
	{
		return _op;
	}

	std::shared_ptr<expr> expression( void ) const
	{
		return _x;
	}

private:
	std::u32string _op;
	std::shared_ptr<expr> _x;
};

////////////////////////////////////////

class postfix_expr
{
public:
	postfix_expr( const std::u32string &op, const std::shared_ptr<expr> &x )
		: _op( op ), _x( x )
	{
	}

	std::u32string operation( void ) const
	{
		return _op;
	}

	std::shared_ptr<expr> expression( void ) const
	{
		return _x;
	}

private:
	std::u32string _op;
	std::shared_ptr<expr> _x;
};

////////////////////////////////////////

class infix_expr
{
public:
	infix_expr( const std::u32string &op, const std::shared_ptr<expr> &x, const std::shared_ptr<expr> &y )
		: _op( op ), _x( x ), _y( y )
	{
	}

	std::u32string operation( void ) const
	{
		return _op;
	}

	std::shared_ptr<expr> expression1( void ) const
	{
		return _x;
	}

	std::shared_ptr<expr> expression2( void ) const
	{
		return _y;
	}

private:
	std::u32string _op;
	std::shared_ptr<expr> _x;
	std::shared_ptr<expr> _y;
};

////////////////////////////////////////

class circumfix_expr
{
public:
	circumfix_expr( const std::u32string &op, const std::u32string &cl, const std::shared_ptr<expr> &x )
		: _open( op ), _close( cl ), _x( x )
	{
	}

	const std::u32string &open( void ) const
	{
		return _open;
	}

	const std::u32string &close( void ) const
	{
		return _close;
	}

	std::shared_ptr<expr> expression( void ) const
	{
		return _x;
	}

private:
	std::u32string _open, _close;
	std::shared_ptr<expr> _x;
};

////////////////////////////////////////

class postcircumfix_expr
{
public:
	postcircumfix_expr( const std::u32string &op, const std::u32string &cl, const std::shared_ptr<expr> &x, const std::shared_ptr<expr> &y )
		: _open( op ), _close( cl ), _x( x ), _y( y )
	{
	}

	const std::u32string &open( void ) const
	{
		return _open;
	}

	const std::u32string &close( void ) const
	{
		return _close;
	}

	std::shared_ptr<expr> expression1( void ) const
	{
		return _x;
	}

	std::shared_ptr<expr> expression2( void ) const
	{
		return _y;
	}

private:
	std::u32string _open, _close;
	std::shared_ptr<expr> _x;
	std::shared_ptr<expr> _y;
};

////////////////////////////////////////

class call_expr
{
public:
	call_expr( const std::u32string &func, std::vector<std::shared_ptr<expr>> &&args )
		: _func( func ), _args( std::move( args ) )
	{
	}

	const std::u32string &function( void ) const
	{
		return _func;
	}
	
	const std::vector<std::shared_ptr<expr>> &arguments( void ) const
	{
		return _args;
	}

protected:
	std::u32string _func;
	std::vector<std::shared_ptr<expr>> _args;
};

////////////////////////////////////////

class if_expr
{
public:
	if_expr( const std::shared_ptr<expr> &c, const std::shared_ptr<expr> &t, const std::shared_ptr<expr> &f )
		: _condition( c ), _true( t ), _false( f )
	{
	}

	const std::shared_ptr<expr> &condition( void ) const
	{
		return _condition;
	}

	const std::shared_ptr<expr> &when_true( void ) const
	{
		return _true;
	}

	const std::shared_ptr<expr> &when_false( void ) const
	{
		return _false;
	}

private:
	std::shared_ptr<expr> _condition;
	std::shared_ptr<expr> _true;
	std::shared_ptr<expr> _false;
};

////////////////////////////////////////

class range_expr
{
public:
	range_expr( std::shared_ptr<expr> &r )
		: _start( r )
	{
	}

	range_expr( std::shared_ptr<expr> &start, std::shared_ptr<expr> &end )
		: _start( start ), _end( end )
	{
	}

	range_expr( std::shared_ptr<expr> &start, std::shared_ptr<expr> &end, std::shared_ptr<expr> &by )
		: _start( start ), _end( end ), _by( by )
	{
	}

//	std::string get_size( compile_context &code, std::shared_ptr<scope> &scope ) const;
//	std::string get_offset( compile_context &code, std::shared_ptr<scope> &scope ) const;

	void set_index( size_t idx ) { _index = idx; }

	std::shared_ptr<expr> start( void ) const { return _start; }
	std::shared_ptr<expr> end( void ) const { return _end; }
	std::shared_ptr<expr> by( void ) const { return _by; }

private:
	size_t _index = 0;
	std::shared_ptr<expr> _start;
	std::shared_ptr<expr> _end;
	std::shared_ptr<expr> _by;
};

////////////////////////////////////////

class for_expr
{
public:
	for_expr( void )
	{
	}

	void add_modifier( const std::u32string &mod )
	{
		_mods.push_back( mod );
	}

	void add_variable( std::u32string n )
	{
		_vars.push_back( std::move( n ) );
	}

	void add_range( range_expr &&r )
	{
		r.set_index( _ranges.size() );
		_ranges.emplace_back( std::move( r ) );
	}

	void set_result( const std::shared_ptr<expr> &r )
	{
		_result = r;
	}

	const std::vector<std::u32string> &modifiers( void ) const
	{
		return _mods;
	}


	const std::vector<std::u32string> &variables( void ) const
	{
		return _vars;
	}

	const std::vector<range_expr> &ranges( void ) const
	{
		return _ranges;
	}

	std::shared_ptr<expr> result( void ) const
	{
		return _result;
	}

private:
	std::vector<std::u32string> _mods;
	std::vector<std::u32string> _vars;
	std::vector<range_expr> _ranges;
	std::shared_ptr<expr> _result;
};

////////////////////////////////////////

class assign_expr
{
public:
	assign_expr( std::u32string var, const std::shared_ptr<expr> &e )
		: _var( var ), _expr( e )
	{
	}

	void set_next( const std::shared_ptr<expr> &n )
	{
		_next = n;
	}

	const std::u32string &variable( void ) const
	{
		return _var;
	}

	std::shared_ptr<expr> expression( void ) const
	{
		return _expr;
	}

	std::shared_ptr<expr> next( void ) const
	{
		return _next;
	}

private:
	std::u32string _var;
	std::shared_ptr<expr> _expr;
	std::shared_ptr<expr> _next;
};

////////////////////////////////////////

class lambda_expr
{
public:
	lambda_expr( const std::shared_ptr<function> &f )
		: _func( f )
	{
	}

	const std::shared_ptr<function> &get_function( void ) const
	{
		return _func;
	}


private:
	std::shared_ptr<function> _func;
};

////////////////////////////////////////

class expr : public base::variant<
	prefix_expr,
	postfix_expr,
	infix_expr,
	circumfix_expr,
	postcircumfix_expr,
	number_expr,
	identifier_expr,
	call_expr,
	if_expr,
	range_expr,
	for_expr,
	assign_expr,
	lambda_expr
>
{
public:
	using variant::variant;

	template<typename T, typename ...Args>
	static std::shared_ptr<expr> make( Args ...args )
	{
		auto result = std::make_shared<expr>();
		result->set<T>( args... );
		return result;
	}

	const type &get_type( void ) const
	{
		return _type;
	}

	void set_type( type &&t )
	{
		_type = std::move( t );
	}

	std::shared_ptr<expr> clone( void );

private:
	type _type;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const std::shared_ptr<expr> &e );

////////////////////////////////////////

}

