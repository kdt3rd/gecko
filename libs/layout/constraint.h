
#pragma once

#include <memory>
#include "expression.h"

namespace layout
{

constexpr double required = 1001001000.0;
constexpr double strong =      1000000.0;
constexpr double medium =         1000.0;
constexpr double weak =              1.0;

////////////////////////////////////////

class constraint
{
public:
	enum class relation
	{
		LESS_EQUAL,
		GREATER_EQUAL,
		EQUAL
	};

	constraint( void )
	{
	}

	constraint( const expression &e, relation o, double str = required )
		: _data( std::make_shared<data>( e, o, str ) )
	{
	}

	double strength( void ) const
	{
		precondition( _data, "null constraint data" );
		return _data->_strength;
	}

	const expression &get_expr( void ) const
	{
		precondition( _data, "null constraint data" );
		return _data->_expr;
	}

	relation op( void ) const
	{
		precondition( _data, "null constraint data" );
		return _data->_op;
	}

	bool operator<( const constraint &c ) const
	{
		return _data < c._data;
	}

	void set_strength( double str )
	{
		precondition( _data, "null constraint data" );
		_data->_strength = str;
	}

private:
	struct data
	{
		data( const expression &e, relation o, double s )
			: _expr( e ), _op( o ), _strength( s )
		{
		}

		expression _expr;
		relation _op;
		double _strength;
	};

	std::shared_ptr<data> _data;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const constraint &c )
{
	out << c.get_expr();
	switch ( c.op() )
	{
		case constraint::relation::LESS_EQUAL: out << " <= 0"; break;
		case constraint::relation::GREATER_EQUAL: out << " >= 0"; break;
		case constraint::relation::EQUAL: out << " == 0"; break;
	}

	return out;
}

////////////////////////////////////////

inline constraint operator<=( const expression &e1, const expression &e2 )
{
	return constraint( e1 - e2, constraint::relation::LESS_EQUAL );
}

////////////////////////////////////////

inline constraint operator>=( const expression &e1, const expression &e2 )
{
	return constraint( e1 - e2, constraint::relation::GREATER_EQUAL );
}

////////////////////////////////////////

inline constraint operator==( const expression &e1, const expression &e2 )
{
	return constraint( e1 - e2, constraint::relation::EQUAL );
}

////////////////////////////////////////

inline constraint operator||( constraint &&c, double str )
{
	c.set_strength( str );
	return std::move( c );
}

////////////////////////////////////////

inline constraint operator||( double str, constraint &&c )
{
	c.set_strength( str );
	return std::move( c );
}

////////////////////////////////////////

}

