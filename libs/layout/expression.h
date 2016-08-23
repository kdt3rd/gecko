
#pragma once

#include <cmath>
#include <memory>
#include <functional>
#include <base/contract.h>

namespace layout
{

class expression;
class term;

class variable
{
public:
	variable( const char *name )
		: _data( std::make_shared<data>( name, 0.0 ) )
	{
	}

	variable( std::string &&n )
		: _data( std::make_shared<data>( std::move( n ), 0.0 ) )
	{
	}

	variable( const std::string &n )
		: _data( std::make_shared<data>( std::string( n ), 0.0 ) )
	{
	}

	const char *name( void ) const
	{
		return _data->_name.c_str();
	}

	variable &operator=( double x )
	{
		_data->_value = x;
		return *this;
	}

	double value( void ) const
	{
		return _data->_value;
	}

	bool less( const variable &v ) const
	{
		return _data < v._data;
	}

private:
	struct data
	{
		data( std::string &&n, double v )
			: _name( std::move( n ) ), _value( v )
		{
		}
		std::string _name;
		double _value = 0.0;
	};

	std::shared_ptr<data> _data;
};

class term
{
public:
	term( const variable &v, double coeff = 1.0 )
		: _variable( v ), _coefficient( coeff )
	{
	}

	term &operator*=( double c )
	{
		_coefficient *= c;
		return *this;
	}

	term &operator/=( double c )
	{
		_coefficient /= c;
		return *this;
	}

	term operator-( void ) const
	{
		term result( *this );
		result *= -1.0;
		return result;
	}

	const variable &var( void ) const
	{
		return _variable;
	}

	double coefficient( void ) const
	{
		return _coefficient;
	}

	double value( void ) const
	{
		return _variable.value() * _coefficient;
	}

private:
	variable _variable;
	double _coefficient;
};

class expression
{
public:
	expression( double c = 0.0 )
		: _constant( c )
	{
	}

	expression( const term &t, double c = 0.0 )
		: _constant( c )
	{
		_terms.push_back( t );
	}

	expression( const variable &v, double c = 0.0 )
		: _constant( c )
	{
		_terms.push_back( term( v ) );
	}

	expression &operator+=( double c )
	{
		_constant += c;
		return *this;
	}

	expression &operator-=( double c )
	{
		_constant -= c;
		return *this;
	}

	expression &operator*=( double x )
	{
		for ( auto &t: _terms )
			t *= x;
		_constant *= x;
		return *this;
	}

	expression &operator/=( double x )
	{
		for ( auto &t: _terms )
			t /= x;
		_constant /= x;
		return *this;
	}

	expression &operator+=( const variable &v )
	{
		_terms.push_back( v );
		return *this;
	}

	expression &operator+=( const term &t )
	{
		_terms.push_back( t );
		return *this;
	}

	expression &operator-=( const term &t )
	{
		_terms.push_back( -t );
		return *this;
	}

	expression &operator+=( const expression &e )
	{
		_terms.insert( _terms.end(), e._terms.begin(), e._terms.end() );
		_constant += e._constant;
		return *this;
	}

	size_t size( void ) const
	{
		return _terms.size();
	}

	const term &operator[]( size_t i ) const
	{
		return _terms.at( i );
	}

	term &operator[]( size_t i )
	{
		return _terms.at( i );
	}

	double constant( void ) const
	{
		return _constant;
	}

	double value( void ) const
	{
		double result = _constant;
		for ( auto &t: _terms )
			result += t.value();
		return result;
	}

private:
	std::vector<term> _terms;
	double _constant;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const variable &v )
{
	out << v.name();
	return out;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const term &t )
{
	if ( std::abs( t.coefficient() - 1.0 ) < 1e-8 )
		out << t.var();
	else
		out << t.coefficient() << "*" << t.var();
	return out;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const expression &e )
{
	for ( size_t t = 0; t < e.size(); ++t )
	{
		if ( t > 0 )
		{
			const term &x = e[t];
			double c = x.coefficient();
			if ( c < 0.0 )
				out << " - ";
			else
				out << " + ";
			c = std::abs( c );

			if ( std::abs( c - 1.0 ) < 1e-8 )
				out << x.var();
			else
				out << c << '*' << x.var();
		}
		else
			out << e[t];
	}

	double c = e.constant();
	if ( std::abs( c ) > 1e-8 )
	{
		if ( c < 0.0 )
			out << " - " << std::abs( c );
		else
			out << " + " << c;
	}

	return out;
}

////////////////////////////////////////

inline term operator*( const variable &a, double b )
{
	return term( a, b );
}

////////////////////////////////////////

inline term operator/( const variable &a, double b )
{
	return term( a, 1.0 / b );
}

////////////////////////////////////////

inline term operator-( const variable &a )
{
	return term( a, -1.0 );
}

////////////////////////////////////////

inline expression operator+( const term &a, const term &b )
{
	expression e( a );
	e += b;
	return e;
}

////////////////////////////////////////

inline expression operator+( const variable &a, const variable &b )
{
	return term( a ) + term( b );
}

////////////////////////////////////////

inline expression operator-( const term &a, const term &b )
{
	expression e( a );
	e += -b;
	return e;
}

////////////////////////////////////////

inline expression operator-( const variable &a, const variable &b )
{
	return term( a ) - term( b );
}

////////////////////////////////////////

inline expression operator+( const term &a, const variable &b )
{
	expression e( a );
	e += b;
	return e;
}

////////////////////////////////////////

inline expression operator+( const variable &a, const term &b )
{
	expression e( a );
	e += b;
	return e;
}

////////////////////////////////////////

inline term operator*( const term &a, double b )
{
	term result( a );
	result *= b;
	return result;
}

////////////////////////////////////////

inline term operator/( const term &a, double b )
{
	term result( a );
	result /= b;
	return result;
}

////////////////////////////////////////

inline expression operator+( const expression &a, double b )
{
	expression result( a );
	result += b;
	return result;
}

////////////////////////////////////////

inline expression operator-( const expression &a, double b )
{
	expression result( a );
	result -= b;
	return result;
}

////////////////////////////////////////

inline expression operator*( const expression &a, double b )
{
	expression result( a );
	result *= b;
	return result;
}

////////////////////////////////////////

inline expression operator/( const expression &a, double b )
{
	expression result( a );
	result /= b;
	return result;
}

////////////////////////////////////////

inline expression operator+( const expression &a, const variable &b )
{
	expression result( a );
	result += b;
	return result;
}

////////////////////////////////////////

inline expression operator+( const expression &a, const term &b )
{
	expression result( a );
	result += b;
	return result;
}

////////////////////////////////////////

inline expression operator-( const expression &a, const term &b )
{
	expression result( a );
	result -= b;
	return result;
}

////////////////////////////////////////

inline expression operator-( const expression &a, const variable &b )
{
	expression result( a );
	result -= b;
	return result;
}

////////////////////////////////////////

inline expression operator-( const expression &a )
{
	expression result( a * -1.0 );
	return result;
}

////////////////////////////////////////

inline expression operator+( const expression &a, const expression &b )
{
	expression result( a );
	result += b;
	return result;
}

////////////////////////////////////////

inline expression operator-( const expression &a, const expression &b )
{
	expression result( a );
	result += -b;
	return result;
}

////////////////////////////////////////

inline expression operator-( const term &a, const variable &b )
{
	expression e( a );
	e += -b;
	return e;
}

////////////////////////////////////////

inline expression operator+( const variable &v, double x )
{
	return expression( v, x );
}

////////////////////////////////////////

inline expression operator-( const variable &v, double x )
{
	return expression( v, -x );
}

////////////////////////////////////////

}

////////////////////////////////////////

namespace std
{
template<>
struct less<layout::variable>
{
	bool operator()( const layout::variable &a, const layout::variable &b ) const
	{
		return a.less( b );
	}
};
}

