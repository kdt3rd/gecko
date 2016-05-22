//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <memory>

#include <base/const_string.h>
#include <base/contract.h>

#include "graph.h"
#include <iostream>

////////////////////////////////////////

namespace engine
{

///
/// @brief computed_base provides a pattern for values computed using
/// an engine graph.
///
/// The idea behind this is to enable transparent construction of C++
/// code, but when a particular value is actually needed, the graph
/// can be processed at that point.
///
/// An example of this might be if there's a simple sum operator that
/// loops over a plane and sums all the pixel components. if that
/// operator is used in the graph, and passed as an argument to
/// another operator, this happens all internal to the graph. if
/// however, there's some c++ code that needs that value for a
/// conditional for some reason, the graph which acts as the value is
/// processed and the result improved. So you want to be able to say:
///
/// auto x = sum( p );
/// if ( static_cast<float>( x ) < 0.1F )
///   std::cout << "do something" << std::endl;
///
/// compared to
/// p = solarize( p, sum( p ) / static_cast<double>( p.width() * p.height() ) );
///
/// where the value can just stay a value and be processed later when
/// the item referencing p is evaluated.
///
class computed_base
{
public:
	computed_base( void ) = default;
	~computed_base( void )
	{
		clear_graph();
	}
	computed_base( const computed_base & ) = default;
	computed_base( computed_base && ) = default;
	computed_base &operator=( const computed_base & ) = default;
	computed_base &operator=( computed_base && ) = default;

	template <typename... Args>
	explicit computed_base( const registry &r, const base::cstring &opname, const dimensions &d, Args &&... args )
	{
		_graph = find_or_create_graph( r, std::forward<Args>( args )... );
		_id = _graph->add_node( opname, d, { check_or_add( *_graph, std::forward<Args>( args ) )... } );
		_graph->reference( _id );
	}

	inline const std::shared_ptr<graph> &graph_ptr( void ) const { return _graph; }
	inline node_id id( void ) const { return _id; }
	inline dimensions node_dims( void ) const
	{
		dimensions r = nulldim;
		if ( _graph )
			r = (*_graph)[_id].dims();
		return r;
	}
	inline bool pending( void ) const
	{
		if ( _graph )
		{
			const node &n = (*_graph)[_id];
			if ( n.value().empty() )
				return true;
		}
			
		return false;
	}

#if 0
	inline T compute( void ) const
	{
		if ( ! _graph )
			throw_runtime( "No graph to compute with" );
		const any &v = _graph->get_value( _id );
//		return std::experimental::any_cast<T>( v );
		return any_cast<T>( v );
	}
#else
	inline const any &compute( void ) const
	{
		if ( ! _graph )
			throw_runtime( "No graph to compute with" );
		return _graph->get_value( _id );
	}
#endif

	inline void clear_graph( void ) noexcept
	{
		if ( _graph )
		{
			_graph->unreference( _id );
			_graph.reset();
		}
		_id = nullnode;
	}

	inline bool compute_hash( hash &v ) const
	{
		if ( _graph )
		{
			v << (*_graph)[_id].hash_value();
			return true;
		}
		return false;
	}
protected:
	template <typename X, bool>
	struct check_delegate;

	template <typename X>
	struct check_delegate<X, false>
	{
		static inline node_id process( graph &g, X &&v )
		{
			return g.add_constant( std::forward<X>( v ) );
		}

		static inline std::shared_ptr<graph> find_graph( const registry &, X && )
		{
			return std::shared_ptr<graph>();
		}
	};
	template <typename X>
	struct check_delegate<X, true>
	{
		static inline node_id process( graph &g, X &&v )
		{
			const std::shared_ptr<graph> &rg = v.graph_ptr();

			if ( rg )
			{
				node_id r;

				if ( &(rg->op_registry()) == &(g.op_registry()) )
					r = g.move_node( *(rg), v.id() );
				else if ( &(rg->op_registry()) == &(registry::pod_registry()) )
					r = g.move_node( *(rg), v.id() );
				else
					throw_runtime( "mixed graphs with different registries not allowed" );

				g.tag_rvalue( r );
				return r;
			}

			return g.add_constant( std::move( v ) );
		}

		static inline std::shared_ptr<graph> find_graph( const registry &reg, X &&v )
		{
			const std::shared_ptr<graph> &r = v.graph_ptr();
			if ( r && &(r->op_registry()) == &reg )
				return r;

			return std::shared_ptr<graph>();
		}
	};
	template <typename X>
	struct check_delegate<const X &, true>
	{
		static inline node_id process( graph &g, const X &v )
		{
			const std::shared_ptr<graph> &r = v.graph_ptr();

			if ( r )
			{
				if ( &(r->op_registry()) == &(g.op_registry()) )
					return g.copy_node( *(r), v.id() );
				else if ( &(r->op_registry()) == &(registry::pod_registry()) )
					return g.copy_node( *(r), v.id() );
				else
					throw_runtime( "mixed graphs with different registries not allowed" );
			}

			return g.add_constant( v );
		}

		static inline std::shared_ptr<graph> find_graph( const registry &reg, const X &v )
		{
			const std::shared_ptr<graph> &r = v.graph_ptr();
			if ( r && &(r->op_registry()) == &reg )
				return r;

			return std::shared_ptr<graph>();
		}
	};

	template <typename X>
	inline node_id check_or_add( graph &g, X &&v )
	{
		return check_delegate<X, std::is_base_of<computed_base, typename std::decay<X>::type>::value>::process( g, std::forward<X>( v ) );
	}

	template <typename X>
	inline std::shared_ptr<graph> check_for_graph( const registry &reg, X &&v )
	{
		return check_delegate<X, std::is_base_of<computed_base, typename std::decay<X>::type>::value>::find_graph( reg, std::forward<X>( v ) );
	}

	template <typename X, typename... Args>
	inline std::shared_ptr<graph>
	find_or_create_graph( const registry &reg, X &&x, Args &&... args )
	{
		std::cout << " cv " << this << ": find_or_create_graph" << std::endl;
		std::shared_ptr<graph> r = check_for_graph( reg, std::forward<X>( x ) );
		if ( r )
			return r;
		return find_or_create_graph( reg, std::forward<Args>( args )... );
	}

	inline std::shared_ptr<graph>
	find_or_create_graph( const registry &reg )
	{
		return std::make_shared<graph>( reg );
	}

	inline void
	adopt( computed_base &&o )
	{
		_graph = std::move( o._graph );
		_id = std::move( o._id );
	}

	std::shared_ptr<graph> _graph;
	node_id _id = nullnode;
};

/// @brief computed_value is a wrapper for values in the graph such
/// that they can be computed and passed in as arguments.
template <typename V>
class computed_value : public computed_base
{
public:
	static_assert( ! std::is_base_of<computed_base, typename std::decay<V>::type>::value, "computed_value shouldn't be created for things that are already computed" );

	inline computed_value( void ) = default;

	template <typename A>
	inline computed_value( A &&v )
	{
		_graph = std::make_shared<graph>( registry::pod_registry() );
		_id = _graph->add_constant( static_cast<V>( std::forward<A>( v ) ) );
	}

	template <typename... Args>
	inline computed_value( const registry &r, const base::cstring &opname, const dimensions &d, Args &&... args )
		: computed_base( r, opname, d, std::forward<Args>( args )... )
	{
	}

	inline computed_value( const computed_value &v ) = default;
	inline computed_value( computed_value &&v ) = default;

	template <typename A>
	inline computed_value &operator=( A &&v )
	{
		_graph = std::make_shared<graph>( registry::pod_registry() );
		_id = _graph->add_constant( static_cast<V>( std::forward<A>( v ) ) );
		return *this;
	}

	inline computed_value &operator=( const computed_value &v ) = default;
	inline computed_value &operator=( computed_value &&v ) = default;

	inline operator V( void ) const
	{
		if ( _graph )
		{
			return engine::any_cast<V>( _graph->get_value( _id ) );
		}

		throw_runtime( "Attempt to evaluate uninitialized computed_value" );
	}

};

} // namespace engine



