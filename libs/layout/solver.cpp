
#include "solver.h"
#include <base/contract.h>
#include <base/scope_guard.h>

namespace layout
{

////////////////////////////////////////

void solver::add_constraint( const constraint &c )
{
	precondition( _constraints.find( c ) == _constraints.end(), "duplicate constraints" );

	tag t;
	std::unique_ptr<row> rowptr( create_row( c, t ) );
	symbol subj = choose_subject( *rowptr, t );

	if ( !subj && rowptr->dummies() )
	{
		if ( std::abs( rowptr->constant() ) > 1.0e-8 )
			throw_runtime( "unsatisfiable constraint: {0}", *rowptr );
		else
			subj = t.first;
	}

	if ( !subj )
	{
		if ( !add_with_artificial_variable( *rowptr ) )
			throw_runtime( "unsatisfiable constraint: {0}", *this );
	}
	else
	{
		rowptr->solve( subj );
		substitute( subj, *rowptr );
		_rows[subj].swap( rowptr );
	}

	_constraints[c] = t;

	optimize( *_objective );
}

////////////////////////////////////////

void solver::remove_constraint( const constraint &c )
{
	auto i = _constraints.find( c );
	precondition( i != _constraints.end(), "unknown constraint" );

	tag t( i->second );
	_constraints.erase( i );

	remove_constraint_effects( c, t );

	auto r = _rows.find( t.first );
	if ( r != _rows.end() )
	{
		std::unique_ptr<row> rowptr( std::move( r->second ) );
		_rows.erase( r );
	}
	else
	{
		r = get_marker_leaving_row( t.first );
		if ( r == _rows.end() )
			throw_runtime( "unable to find leaving row" );
		symbol leaving( r->first );
		std::unique_ptr<row> rowptr( std::move( r->second ) );
		_rows.erase( r );
		rowptr->solve( leaving, t.first );
		substitute( t.first, *rowptr );
	}

	optimize( *_objective );
}

////////////////////////////////////////

bool solver::has_constraint( const constraint &c )
{
	return _constraints.find( c ) != _constraints.end();
}

////////////////////////////////////////

void solver::add_variable( const variable &v, double str )
{
	precondition( _edits.find( v ) == _edits.end(), "duplicate varible {0}", v );
	precondition( str < required, "bad strength {0}", str );
	constraint c( v, constraint::relation::EQUAL, str );
	add_constraint( c );
	_edits[v] = edit_info( _constraints.at( c ), c, 0.0 );
}

////////////////////////////////////////

void solver::remove_variable( const variable &v )
{
	auto i = _edits.find( v );
	precondition( i != _edits.end(), "unknown variable" );
	remove_constraint( i->second.c );
	_edits.erase( i );
}

////////////////////////////////////////

bool solver::has_variable( const variable &v )
{
	return _edits.find( v ) != _edits.end();
}

////////////////////////////////////////

void solver::add_objective( const expression &e )
{
	std::unique_ptr<row> result( new row( e.constant() ) );

	for ( size_t i = 0; i < e.size(); ++i )
	{
		const term &t = e[i];
		if ( std::abs( t.coefficient() ) > 1e-8 )
		{
			symbol s( get_symbol( t.var() ) );
			auto rit = _rows.find( s );
			if ( rit != _rows.end() )
				result->insert( *rit->second, t.coefficient() );
			else
				result->insert( get_symbol( t.var() ), t.coefficient() );
		}
	}

	if ( result->constant() < 0.0 )
		result->negate();

	_objective->insert( *result );
}

////////////////////////////////////////

void solver::suggest( const variable &v, double x )
{
	auto i = _edits.find( v );
	precondition( i != _edits.end(), "unknown variable" );

	on_scope_exit { dual_optimize(); };
	edit_info &info = i->second;
	double delta = x - info.d;
	info.d = x;

	auto it = _rows.find( info.t.first );
	if ( it != _rows.end() )
	{
		if ( ( *(it->second) += -delta ) < 0.0 )
			_infeasible.push_back( it->first );
		return;
	}

	// TODO remove duplicate code from above?
	it = _rows.find( info.t.second );
	if ( it != _rows.end() )
	{
		if ( ( *(it->second) += -delta ) < 0.0 )
			_infeasible.push_back( it->first );
		return;
	}

	for ( auto &r: _rows )
	{
		double coeff = (*r.second)[info.t.first];
		if ( coeff != 0.0 &&
			( (*r.second) += delta * coeff ) < 0.0 &&
			r.first.kind() != symbol::type::EXTERNAL )
		{
			_infeasible.push_back( r.first );
		}
	}
}

////////////////////////////////////////

void solver::update_variables( void )
{
	var_map oldvars;
	std::swap( oldvars, _vars );
	for ( auto &i: oldvars )
	{
		variable v = i.first;
		auto r = _rows.find( i.second );
		if ( r == _rows.end() )
			v = 0.0;
		else
			v = r->second->constant();
		_vars[v] = i.second;
	}
}

////////////////////////////////////////

void solver::reset( void )
{
	_rows.clear();
	_constraints.clear();
	_vars.clear();
	_edits.clear();
	_infeasible.clear();
	_objective.reset( new row );
	_artificial.reset();
}

////////////////////////////////////////

void solver::print( std::ostream &out ) const
{
	std::map<symbol,const char *> symnames;
	for ( auto &v: _vars )
		symnames[v.second] = v.first.name();

	out << "\nObjective:\n========================================\n";
	print( out, *_objective, symnames );
	out << '\n';

	out << "\nTableau:\n========================================\n";
	for ( auto &r: _rows )
	{
		const char *n = symnames[r.first];
		if ( n == nullptr )
			out << r.first << " | ";
		else
			out << n << " | ";
		print( out, *r.second, symnames );
		out << '\n';
	}

	out << "\nInfeasible:\n========================================\n";
	for ( auto &s: _infeasible )
		out << s << '\n';

	out << "\nConstraints:\n========================================\n";
	for ( auto &c: _constraints )
		out << c.first << '\n';
}

////////////////////////////////////////

void solver::print( std::ostream &out, const row &r, std::map<symbol,const char *> &symnames ) const
{
	auto &cells = r.cells();
	for ( auto i = cells.begin(); i != cells.end(); ++i )
	{
		double c = i->second;
		if ( std::abs( c ) > 1e-8 )
		{
			if ( i != cells.begin() )
			{
				if ( c < 0 )
					out << " - ";
				else
					out << " + ";
			}
			c = std::abs( c );
			if ( std::abs( c - 1.0 ) > 1e-8 )
				out << c << '*';

			const char *n = symnames[i->first];
			if ( n == nullptr )
				out << i->first;
			else
				out << n;
		}
	}

	double c = r.constant();
	if ( std::abs( c ) > 1e-8 )
	{
		if ( c < 0 )
			out << " - " << -c;
		else
			out << " + " << c;
	}
}

////////////////////////////////////////

symbol solver::get_symbol( const variable &v )
{
	auto it = _vars.find( v );
	if ( it != _vars.end() )
		return it->second;
	symbol s( symbol::type::EXTERNAL, _symid++ );
	_vars[v] = s;
	return s;
}

////////////////////////////////////////

std::unique_ptr<row> solver::create_row( const constraint &c, tag &t )
{
	const expression &e = c.get_expr();
	std::unique_ptr<row> result( new row( e.constant() ) );

	for ( size_t i = 0; i < e.size(); ++i )
	{
		const term &t = e[i];
		if ( std::abs( t.coefficient() ) > 1e-8 )
		{
			symbol s( get_symbol( t.var() ) );
			auto rit = _rows.find( s );
			if ( rit != _rows.end() )
				result->insert( *rit->second, t.coefficient() );
			else
				result->insert( get_symbol( t.var() ), t.coefficient() );
		}
	}

	switch ( c.op() )
	{
		case constraint::relation::LESS_EQUAL:
		case constraint::relation::GREATER_EQUAL:
		{
			double coeff = ( c.op() == constraint::relation::LESS_EQUAL ) ? 1.0 : -1.0;
			symbol slack( symbol::type::SLACK, _symid++ );
			t.first = slack;
			result->insert( slack, coeff );
			if ( c.strength() < required )
			{
				symbol err( symbol::type::ERROR, _symid++ );
				t.second = err;
				result->insert( err, -coeff );
				_objective->insert( err, c.strength() );
			}
			break;
		}

		case constraint::relation::EQUAL:
		{
			if ( c.strength() < required )
			{
				symbol errp( symbol::type::ERROR, _symid++ );
				symbol errm( symbol::type::ERROR, _symid++ );
				t.first = errp;
				t.second = errm;
				result->insert( errp, -1.0 );
				result->insert( errm, 1.0 );
				_objective->insert( errp, c.strength() );
				_objective->insert( errm, c.strength() );
			}
			else
			{
				symbol dummy( symbol::type::DUMMY, _symid++ );
				t.first = dummy;
				result->insert( dummy );
			}
			break;
		}
	}

	if ( result->constant() < 0.0 )
		result->negate();

	return std::move( result );
}

////////////////////////////////////////

symbol solver::choose_subject( const row &r, const tag &t )
{
	for ( auto &it: r.cells())
	{
		if ( it.first.kind() == symbol::type::EXTERNAL )
			return it.first;
	}

	if ( t.first.kind() == symbol::type::SLACK || t.first.kind() == symbol::type::ERROR )
	{
		if ( r[t.first] < 0.0 )
			return t.first;
	}
	if ( t.second.kind() == symbol::type::SLACK || t.second.kind() == symbol::type::ERROR )
	{
		if ( r[t.second] < 0.0 )
			return t.second;
	}

	return symbol();
}

////////////////////////////////////////

bool solver::add_with_artificial_variable( const row &r )
{
	symbol art( symbol::type::SLACK, _symid++ );
	_rows[art] = std::unique_ptr<row>( new row( r ) );
	_artificial.reset( new row( r ) );

	optimize( *_artificial );
	bool success = std::abs( _artificial->constant() ) < 1e-8;
	_artificial.reset();

	auto it = _rows.find( art );
	if ( it != _rows.end() )
	{
		std::unique_ptr<row> rptr( std::move( it->second ) );
		_rows.erase( it );

		if ( rptr->cells().empty() )
			return success;
		symbol entering( rptr->pivotable() );
		if ( entering.kind() == symbol::type::INVALID )
			return false;
		rptr->solve( art, entering );
		substitute( entering, *rptr );
		_rows[entering] = std::move( rptr );
	}

	for ( auto &it: _rows )
		it.second->remove( art );
	_objective->remove( art );

	return success;
}

////////////////////////////////////////

void solver::substitute( const symbol &s, const row &r )
{
	for ( auto &it: _rows )
	{
		it.second->substitute( s, r );
		if ( it.first.kind() != symbol::type::EXTERNAL && it.second->constant() < 0.0 )
			_infeasible.push_back( it.first );
	}

	_objective->substitute( s, r );
	if ( _artificial )
		_artificial->substitute( s, r );
}

////////////////////////////////////////

void solver::optimize( const row &objective )
{
	while ( true )
	{
		symbol entering( _objective->entering() );
		if ( entering.kind() == symbol::type::INVALID )
			return;
		auto rit = get_leaving_row( entering );
		if ( rit == _rows.end() )
			throw_runtime( "objective is unbounded" );

		symbol leaving( rit->first );
		std::unique_ptr<row> r = std::move( rit->second );
		_rows.erase( rit );
		r->solve( leaving, entering );
		substitute( entering, *r );
		_rows[entering] = std::move( r );

	}
}

////////////////////////////////////////

void solver::dual_optimize( void )
{
	while ( !_infeasible.empty() )
	{
		symbol leaving( _infeasible.back() );
		_infeasible.pop_back();
		auto it = _rows.find( leaving );
		if ( it != _rows.end() && it->second->constant() < 0.0 )
		{
			symbol entering( get_dual_entering_symbol( *it->second ) );
			if ( entering.kind() == symbol::type::INVALID )
				throw_runtime( "dual optimize failed" );
			std::unique_ptr<row> r = std::move( it->second );
			_rows.erase( it );
			r->solve( leaving, entering );
			substitute( entering, *r );
			_rows[entering] = std::move( r );
		}
	}
}

////////////////////////////////////////

symbol solver::get_dual_entering_symbol( const row &r )
{
	symbol result;
	double ratio = std::numeric_limits<double>::max();
	for ( const auto &it: r.cells() )
	{
		if ( it.second > 0.0 && it.first.kind() != symbol::type::DUMMY )
		{
			double coeff = _objective->coefficient( it.first );
			double r = coeff / it.second;
			if ( r < ratio )
			{
				ratio = r;
				result = it.first;
			}
		}

	}
	return result;
}

////////////////////////////////////////

solver::row_map::iterator solver::get_leaving_row( const symbol &enter )
{
	row_map::iterator found = _rows.end();
	double ratio = std::numeric_limits<double>::max();
	for ( auto it = _rows.begin(); it != _rows.end(); ++it )
	{
		if ( it->first.kind() != symbol::type::EXTERNAL )
		{
			double c = it->second->coefficient( enter );
			if ( c < 0.0 )
			{
				double tmpratio = - it->second->constant() / c;
				if ( tmpratio < ratio )
				{
					ratio = tmpratio;
					found = it;
				}
			}
		}
	}
	return found;
}

////////////////////////////////////////

solver::row_map::iterator solver::get_marker_leaving_row( const symbol &marker )
{
	double r1 = std::numeric_limits<double>::max();
	double r2 = std::numeric_limits<double>::max();
	row_map::iterator result1 = _rows.end();
	row_map::iterator result2 = _rows.end();
	row_map::iterator result3 = _rows.end();
	for ( auto it = _rows.begin(); it != _rows.end(); ++it )
	{
		double c = it->second->coefficient( marker );
		if ( c == 0.0 )
			continue;
		if ( it->first.kind() == symbol::type::EXTERNAL )
		{
			result3 = it;
		}
		else if ( c < 0.0 )
		{
			double r = - it->second->constant() / c;
			if ( r < r1 )
			{
				r1 = r;
				result1 = it;
			}
		}
		else
		{
			double r = - it->second->constant() / c;
			if ( r < r2 )
			{
				r2 = r;
				result2 = it;
			}
		}
	}

	if ( result1 != _rows.end() )
		return result1;
	if ( result2 != _rows.end() )
		return result2;
	return result3;
}

////////////////////////////////////////

void solver::remove_constraint_effects( const constraint &c, const tag &t )
{
	if ( t.first.kind() == symbol::type::ERROR )
		remove_marker_effects( t.first, c.strength() );
	if ( t.second.kind() == symbol::type::ERROR )
		remove_marker_effects( t.second, c.strength() );
}

////////////////////////////////////////

void solver::remove_marker_effects( const symbol &s, double str )
{
	auto it = _rows.find( s );
	if ( it != _rows.end() )
		_objective->insert( *(it->second), -str );
	else
		_objective->insert( s, -str );
}

////////////////////////////////////////

}
