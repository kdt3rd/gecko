
#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iterator>
#include <base/contract.h>
#include "scope.h"

namespace imgproc
{

////////////////////////////////////////

class expr
{
public:
	virtual ~expr( void );

	virtual void write( std::ostream &out ) const = 0;
	virtual type result_type( std::shared_ptr<scope> &scope ) const = 0;
};

////////////////////////////////////////

class prefix_expr : public expr
{
public:
	prefix_expr( const std::u32string &op, const std::shared_ptr<expr> &x );

	void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::u32string _op;
	std::shared_ptr<expr> _x;
};

////////////////////////////////////////

class postfix_expr : public expr
{
public:
	postfix_expr( const std::u32string &op, const std::shared_ptr<expr> &x );

	void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::u32string _op;
	std::shared_ptr<expr> _x;
};

////////////////////////////////////////

class infix_expr : public expr
{
public:
	infix_expr( const std::u32string &op, const std::shared_ptr<expr> &x, const std::shared_ptr<expr> &y );

	void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::u32string _op;
	std::shared_ptr<expr> _x;
	std::shared_ptr<expr> _y;
};

////////////////////////////////////////

class circumfix_expr : public expr
{
public:
	circumfix_expr( const std::u32string &op, const std::u32string &cl, const std::shared_ptr<expr> &x );

	void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::u32string _open, _close;
	std::shared_ptr<expr> _x;
};

////////////////////////////////////////

class postcircumfix_expr : public expr
{
public:
	postcircumfix_expr( const std::u32string &op, const std::u32string &cl, const std::shared_ptr<expr> &x, const std::shared_ptr<expr> &y );

	void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::u32string _open, _close;
	std::shared_ptr<expr> _x;
	std::shared_ptr<expr> _y;
};

////////////////////////////////////////

class func
{
public:
	func( std::u32string n )
		: _name( std::move( n ) )
	{
	}

	template<typename ...Args>
	func( std::u32string n, const Args &...args )
		: _name( std::move( n ) )
	{
		std::array<std::u32string,sizeof...(args)> list
		{{
			args...
		}};

		for ( auto arg: list )
			add_arg( arg );
	}

	void add_arg( std::u32string a )
	{
		_args.push_back( std::move( a ) );
	}

	void set_result( const std::shared_ptr<expr> &r )
	{
		_result = r;
	}

	const std::u32string &name( void ) const
	{
		return _name;
	}

	const std::vector<std::u32string> &args( void ) const
	{
		return _args;
	}

	const std::shared_ptr<expr> result( void ) const { return _result; }

	void write( std::ostream &out ) const;

private:
	std::u32string _name;
	std::vector<std::u32string> _args;
	std::shared_ptr<expr> _result;
};

////////////////////////////////////////

class error_expr : public expr
{
public:
	error_expr( std::string m )
		: _msg( m )
	{
	}

	virtual ~error_expr( void );

	const std::string &message( void ) const { return _msg; }

	virtual void write( std::ostream &out ) const;
	type result_type( std::shared_ptr<scope> &scope ) const override;

protected:
	std::string _msg;
};

////////////////////////////////////////

class value_expr : public expr
{
public:
	value_expr( std::u32string t )
		: _value( t )
	{
	}

	value_expr( value_expr &v )
		: _value( v._value )
	{
	}

	value_expr( value_expr &&v )
		: _value( std::move( v._value ) )
	{
	}

	virtual ~value_expr( void );

	const std::u32string &value( void ) const { return _value; }

protected:
	std::u32string _value;
};

////////////////////////////////////////

class number_expr : public value_expr
{
public:
	number_expr( std::u32string t )
		: value_expr( t )
	{
	}

	virtual ~number_expr( void )
	{
	}

	virtual void write( std::ostream &out ) const;
	type result_type( std::shared_ptr<scope> &scope ) const override;
};

////////////////////////////////////////

class identifier_expr : public value_expr
{
public:
	identifier_expr( std::u32string t )
		: value_expr( t )
	{
	}

	virtual ~identifier_expr( void )
	{
	}

	virtual void write( std::ostream &out ) const;
	type result_type( std::shared_ptr<scope> &scope ) const override;
};

////////////////////////////////////////

/*
class operator_expr : public value_expr
{
public:
	operator_expr( std::u32string t )
		: value_expr( t )
	{
	}

	virtual ~operator_expr( void )
	{
	}

	virtual void write( std::ostream &out ) const;
};
*/

////////////////////////////////////////

class arguments_expr : public expr
{
public:
	template<typename iter>
	arguments_expr( iter begin, const iter &end )
	{
		if ( begin == end )
			throw_runtime( "empty expression list" );

		_value = *begin;
		++begin;

		if ( begin != end )
			_next = std::make_shared<arguments_expr>( begin, end );
	}

	virtual ~arguments_expr( void )
	{
	}

	const std::shared_ptr<expr> &value( void ) const { return _value; }
	const std::shared_ptr<expr> &next( void ) const { return _next; }

	virtual void write( std::ostream &out ) const;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::shared_ptr<expr> _value;
	std::shared_ptr<expr> _next;
};

////////////////////////////////////////

/*
class tuple_expr : public expr
{
public:
	tuple_expr( void )
	{
	}

	tuple_expr( const std::shared_ptr<expr> &e )
		: _value( e )
	{
	}

	virtual ~tuple_expr( void )
	{
	}

	const std::shared_ptr<expr> &value( void ) const { return _value; }

	virtual void write( std::ostream &out ) const;

protected:
	std::shared_ptr<expr> _value;
};
*/

////////////////////////////////////////

class call_expr : public expr
{
public:
	call_expr( void )
	{
	}

	call_expr( const std::u32string &func, std::vector<std::shared_ptr<expr>> &&args )
		: _func( func ), _args( std::move( args ) )
	{
	}

	virtual ~call_expr( void )
	{
	}

	const std::u32string &function( void ) const { return _func; }
	const std::vector<std::shared_ptr<expr>> &arguments( void ) const { return _args; }

	virtual void write( std::ostream &out ) const;
	type result_type( std::shared_ptr<scope> &scope ) const override;

protected:
	std::u32string _func;
	std::vector<std::shared_ptr<expr>> _args;
};

////////////////////////////////////////

class chain_expr : public expr
{
public:
	chain_expr( const std::shared_ptr<expr> &e, const std::shared_ptr<expr> &next )
		: _value( e ), _next( next )
	{
	}

	template<typename iter>
	chain_expr( iter begin, const iter &end )
	{
		if ( begin == end )
			throw_runtime( "empty expression list" );

		_value = *begin;
		++begin;

		if ( begin != end )
			_next.reset( new chain_expr( begin, end ) );
	}

	virtual ~chain_expr( void )
	{
	}

	const std::shared_ptr<expr> &value( void ) const { return _value; }
	const std::shared_ptr<expr> &next( void ) const { return _next; }

	virtual void write( std::ostream &out ) const;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::shared_ptr<expr> _value;
	std::shared_ptr<expr> _next;
};

////////////////////////////////////////

/*
class list_expr : public expr
{
public:
	list_expr( const std::shared_ptr<expr> &e, const std::shared_ptr<expr> &next )
		: _value( e ), _next( next )
	{
	}

	template<typename iter>
	list_expr( iter begin, const iter &end )
	{
		if ( begin == end )
			throw_runtime( "empty expression list" );

		_value = *begin;
		++begin;

		if ( begin != end )
			_next.reset( new list_expr( begin, end ) );
	}

	virtual ~list_expr( void )
	{
	}

	const std::shared_ptr<expr> &value( void ) const { return _value; }
	const std::shared_ptr<expr> &next( void ) const { return _next; }

	virtual void write( std::ostream &out ) const;

private:
	std::shared_ptr<expr> _value;
	std::shared_ptr<expr> _next;
};
*/

////////////////////////////////////////

class block_expr : public expr
{
public:
	block_expr( const std::shared_ptr<expr> &list )
		: _value( list )
	{
	}

	virtual ~block_expr( void )
	{
	}

	const std::shared_ptr<expr> &value( void ) const { return _value; }

	virtual void write( std::ostream &out ) const;

private:
	std::shared_ptr<expr> _value;
};

////////////////////////////////////////

class for_expr : public expr
{
public:
	for_expr( void )
	{
	}

	virtual ~for_expr( void )
	{
	}

	void set_modifier( const std::u32string &mod )
	{
		_mod = mod;
	}

	void add_variable( std::u32string n )
	{
		_vars.push_back( std::move( n ) );
	}

	void add_range( const std::shared_ptr<expr> &r )
	{
		_ranges.push_back( r );
	}

	void set_result( const std::shared_ptr<expr> &r )
	{
		_result = r;
	}

	const std::vector<std::u32string> &variables( void ) const
	{
		return _vars;
	}

	const std::vector<std::shared_ptr<expr>> &ranges( void ) const
	{
		return _ranges;
	}

	void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::u32string _mod;
	std::vector<std::u32string> _vars;
	std::vector<std::shared_ptr<expr>> _ranges;
	std::shared_ptr<expr> _result;
};

////////////////////////////////////////

class if_expr : public expr
{
public:
	if_expr( void )
	{
	}

	virtual ~if_expr( void )
	{
	}

	void set_condition( const std::shared_ptr<expr> &c )
	{
		_condition = c;
	}

	void set_result( const std::shared_ptr<expr> &r )
	{
		_true = r;
	}

	void set_else( const std::shared_ptr<expr> &r )
	{
		_false = r;
	}

	virtual void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::shared_ptr<expr> _condition;
	std::shared_ptr<expr> _true;
	std::shared_ptr<expr> _false;
};

////////////////////////////////////////

class range_expr : public expr
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

	virtual void write( std::ostream &out ) const override;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::shared_ptr<expr> _start;
	std::shared_ptr<expr> _end;
	std::shared_ptr<expr> _by;

};

////////////////////////////////////////

class assign_expr : public expr
{
public:
	assign_expr( std::u32string var, const std::shared_ptr<expr> &e )
		: _var( var ), _expr( e )
	{
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

	void set_next( const std::shared_ptr<expr> &n )
	{
		_next = n;
	}

	virtual void write( std::ostream &out ) const;
	type result_type( std::shared_ptr<scope> &scope ) const override;

private:
	std::u32string _var;
	std::shared_ptr<expr> _expr;
	std::shared_ptr<expr> _next;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const expr &e )
{
	e.write( out );
	return out;
}

////////////////////////////////////////

}

