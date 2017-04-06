//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "area.h"
#include "solver.h"

namespace layout
{

////////////////////////////////////////

class layout : public area
{
public:
	layout( void )
		: area( "layout" )
	{
	}

	void minimize( const expression &e )
	{
		_solver.add_objective( e );
	}

	void add_constraint( const constraint &c )
	{
		if ( c.strength() > strong )
		{
			constraint tmp( c );
			tmp.set_strength( strong );
			_solver.add_constraint( tmp );
		}
		else
			_solver.add_constraint( c );
	}

	void add_variable( const variable &v, double str = strong )
	{
		_solver.add_variable( v, str );
	}

	void suggest( const variable &v, double x )
	{
		if ( !_solver.has_variable( v ) )
			_solver.add_variable( v );
		_solver.suggest( v, x );
	}

	virtual void update( void )
	{
		_solver.update_variables();
	}

	virtual void reset( void )
	{
		_solver.reset();
	}

	layout &operator<<( const constraint &c )
	{
		add_constraint( c );
		return *this;
	}

	layout &operator<<( const variable &v )
	{
		add_variable( v );
		return *this;
	}

protected:

	solver _solver;
};

////////////////////////////////////////

}

