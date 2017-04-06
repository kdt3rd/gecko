//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cmath>

namespace layout
{

////////////////////////////////////////

class row
{
public:
	row( double c = 0.0 )
		: _constant( c )
	{
	}

	const std::map<symbol,double> &cells( void ) const
	{
		return _cells;
	}

	double constant( void ) const
	{
		return _constant;
	}

	double operator[]( const symbol &s ) const
	{
		return coefficient( s );
	}

	double coefficient( const symbol &s ) const
	{
		auto i = _cells.find( s );
		if ( i != _cells.end() )
			return i->second;
		return 0.0;
	}

	bool dummies( void ) const
	{
		for ( auto &p: _cells )
		{
			if ( p.first.kind() != symbol::type::DUMMY )
				return false;
		}
		return true;
	}

	double operator+=( double v )
	{
		_constant += v;
		return _constant;
	}

	//@brief Insert symbol into row.
	//@param sym Symbol to insert.
	//@param c Coefficient to add to symbol.
	void insert( const symbol &sym, double c = 1.0 )
	{
		double n = ( _cells[sym] += c );
		if ( std::abs( n ) < 1.0e-8 )
			_cells.erase( sym );
	}

	void insert( const row &r, double c = 1.0 )
	{
		_constant += r.constant() * c;
		for ( auto &p: r.cells() )
			insert( p.first, p.second * c );
	}

	void negate( void )
	{
		_constant = -_constant;
		for ( auto &p: _cells )
			p.second = -p.second;
	}

	void remove( const symbol &sym )
	{
		_cells.erase( sym );
	}

	void solve( const symbol &s )
	{
		precondition( _cells.find( s ) != _cells.end(), "symbol {0} not found", s );
		double c = -1.0 / _cells[s];
		_cells.erase( s );
		_constant *= c;
		for ( auto &p: _cells )
			p.second *= c;
	}

	void solve( const symbol &lhs, const symbol &rhs )
	{
		insert( lhs, -1.0 );
		solve( rhs );
	}

	void substitute( const symbol &s, const row &r )
	{
		auto i = _cells.find( s );
		if ( i != _cells.end() )
		{
			double c = i->second;
			_cells.erase( i );
			insert( r, c );
		}
	}

	symbol pivotable( void ) const
	{
		for ( auto &p: _cells )
		{
			const symbol &sym = p.first;
			if ( sym.kind() == symbol::type::SLACK || sym.kind() == symbol::type::ERROR )
				return sym;
		}
		return symbol();
	}

	symbol entering( void ) const
	{
		for ( auto &p: _cells )
		{
			const symbol &sym = p.first;
			if ( sym.kind() != symbol::type::DUMMY && p.second < 0.0 )
				return sym;
		}
		return symbol();
	}

private:
	std::map<symbol,double> _cells;
	double _constant = 0.0;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const row &r )
{
	for ( auto &p: r.cells() )
		out << p.second << '*' << p.first << " + ";
	out << r.constant();
	return out;
}

////////////////////////////////////////

}
