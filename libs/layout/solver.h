//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <map>
#include <memory>
#include "constraint.h"
#include "symbol.h"
#include "row.h"

namespace layout
{

////////////////////////////////////////

class solver
{
public:

	void add_constraint( const constraint &c );
	void remove_constraint( const constraint &c );
	bool has_constraint( const constraint &c );

	void add_variable( const variable &v, double str = strong );
	void remove_variable( const variable &v );
	bool has_variable( const variable &v );

	void add_objective( const expression &e );

	void suggest( const variable &v, double x );

	void update_variables( void );

	void reset( void );

	void print( std::ostream &out ) const;

private:
	void print( std::ostream &out, const row &r, std::map<symbol,const char *> &symnames ) const;

	typedef std::pair<symbol,symbol> tag;
	struct edit_info
	{
		edit_info( void )
		{
		}

		edit_info( tag tt, constraint cc, double dd )
			: t( tt ), c( cc ), d( dd )
		{
		}

		tag t;
		constraint c;
		double d = 0.0;
	};

	typedef std::map<variable,symbol> var_map;
	typedef std::map<symbol,std::unique_ptr<row>> row_map;

	symbol get_symbol( const variable &v );

	std::unique_ptr<row> create_row( const constraint &c, tag &t );

	symbol choose_subject( const row &r, const tag &t );
	bool add_with_artificial_variable( const row &r );

	void substitute( const symbol &s, const row &r );

	void optimize( const row &objective );
	void dual_optimize( void );

	symbol get_dual_entering_symbol( const row &r );
	row_map::iterator get_leaving_row( const symbol &enter );
	row_map::iterator get_marker_leaving_row( const symbol &marker );

	void remove_constraint_effects( const constraint &c, const tag &t );
	void remove_marker_effects( const symbol &s, double str );

	row_map _rows;
	var_map _vars;
	std::unique_ptr<row> _objective = std::unique_ptr<row>( new row );
	std::unique_ptr<row> _artificial;
	std::vector<symbol> _infeasible;
	std::map<constraint,tag> _constraints;
	std::map<variable,edit_info> _edits;
	size_t _symid = 1;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const solver &s )
{
	s.print( out );
	return out;
}

////////////////////////////////////////

}
