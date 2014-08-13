
#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iterator>

namespace imgproc
{

////////////////////////////////////////

class expr
{
public:
	virtual ~expr( void );

	virtual bool is_primary( void ) const { return false; }
	virtual bool is_list( void ) const { return false; }
	virtual bool is_call( void ) const { return false; }
	virtual bool can_be_feature( void ) const { return false; }

	virtual void write( std::ostream &out ) const = 0;
};

////////////////////////////////////////

class func
{
public:
	func( std::u32string n )
		: _name( std::move( n ) )
	{
	}

	void add_arg( std::u32string a )
	{
		_args.push_back( std::move( a ) );
	}

	void set_result( const std::shared_ptr<expr> &r )
	{
		_result = r;
	}

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

	virtual bool is_primary( void ) const { return true; }

	const std::string &message( void ) const { return _msg; }

	virtual void write( std::ostream &out ) const;

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

	virtual bool is_primary( void ) const { return true; }

	const std::u32string &value( void ) const { return _value; }

protected:
	std::u32string _value;
};

////////////////////////////////////////

class string_expr : public value_expr
{
public:
	string_expr( std::u32string t )
		: value_expr( t )
	{
	}

	virtual ~string_expr( void )
	{
	}

	virtual void write( std::ostream &out ) const;
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
};

////////////////////////////////////////

class char_expr : public value_expr
{
public:
	char_expr( std::u32string t )
		: value_expr( t )
	{
	}

	virtual ~char_expr( void )
	{
	}

	virtual void write( std::ostream &out ) const;

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

	virtual bool can_be_feature( void ) const { return true; }

	virtual void write( std::ostream &out ) const;
};

////////////////////////////////////////

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

////////////////////////////////////////

class feature_expr : public value_expr
{
public:
	feature_expr( std::u32string f )
		: value_expr( std::move( f ) )
	{
	}

	virtual bool can_be_feature( void ) const { return true; }

	virtual ~feature_expr( void )
	{
	}

	virtual void write( std::ostream &out ) const;
};

////////////////////////////////////////

class aggregate_expr : public expr
{
public:
	aggregate_expr( void )
	{
	}

	aggregate_expr( const std::shared_ptr<expr> &e )
		: _value( e )
	{
	}

	virtual ~aggregate_expr( void )
	{
	}

	const std::shared_ptr<expr> &value( void ) const { return _value; }

	virtual void write( std::ostream &out ) const;

private:
	std::shared_ptr<expr> _value;
};

////////////////////////////////////////

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

////////////////////////////////////////

class call_expr : public expr
{
public:
	call_expr( void )
	{
	}

	call_expr( const std::shared_ptr<expr> &e )
		: _value( e )
	{
	}

	virtual ~call_expr( void )
	{
	}

	virtual bool is_call( void ) const { return true; }

	const std::shared_ptr<expr> &value( void ) const { return _value; }

	virtual void write( std::ostream &out ) const;

protected:
	std::shared_ptr<expr> _value;
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
			throw std::runtime_error( "empty expression list" );

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

private:
	std::shared_ptr<expr> _value;
	std::shared_ptr<expr> _next;
};

////////////////////////////////////////

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
			throw std::runtime_error( "empty expression list" );

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

	virtual bool is_list( void ) const { return true; }

	virtual void write( std::ostream &out ) const;

private:
	std::shared_ptr<expr> _value;
	std::shared_ptr<expr> _next;
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

	void write( std::ostream &out ) const override
	{
		out << "for ( ";
		std::copy( _vars.begin(), _vars.end(), std::ostream_iterator<std::u32string>( out, ", " ) );
		out << " ) ";
	}

private:
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

	virtual void write( std::ostream &out ) const override
	{
		out << "if ( ";
		_condition->write( out );
		out << " ) ";
		_true->write( out );
		if ( _false )
		{
			out << " else ";
			_false->write( out );
		}
	}

private:
	std::shared_ptr<expr> _condition;
	std::shared_ptr<expr> _true;
	std::shared_ptr<expr> _false;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const expr &e )
{
	e.write( out );
	return out;
}

////////////////////////////////////////

}

