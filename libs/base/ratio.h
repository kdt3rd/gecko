
#pragma once

#include <cstdint>
#include <iostream>

namespace base
{

////////////////////////////////////////

class ratio
{
public:
	ratio( int64_t n, int64_t d )
		: _num( n ), _den( d )
	{
		simplify();
	}

	int64_t numerator( void ) const
	{
		return _num;
	}

	int64_t denominator( void ) const
	{
		return _den;
	}

	double value( void ) const
	{
		return double(_num) / double(_den);
	}

private:
	void simplify( void );

	int64_t _num;
	int64_t _den;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const ratio &r )
{
	out << r.numerator() << '/' << r.denominator();
	return out;
}

////////////////////////////////////////

}

