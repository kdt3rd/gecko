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

#include "scanline.h"
#include "plane.h"
//#include <iostream>

////////////////////////////////////////

namespace image
{

inline scanline scan_dup( const plane &p, int y )
{
	return scanline( p.line( y ), p.width(), p.stride(), true );
}

inline scanline scan_ref( plane &p, int y )
{
	return scanline( p.line( y ), p.width(), p.stride() );
}

inline scanline scan_ref( const plane &p, int y )
{
	return scanline( p.line( y ), p.width(), p.stride() );
}

////////////////////////////////////////

#if 0

add_node()
{

	// if a node has a reference, it's a terminal no matter what
// if node == n_to_one
//   start subgroup
// else if node == one_to_one
//   if input member of subgroup
//     add to subgroup
//   if another input member of different subgroup
//     merge subgroups
//   else
//     start subgroup

	// subgroup has (all) inputs
	// subgroup has N outputs (????), at least given rules above
}

process_node()
{
	if ( node.in_subgroup() )
	{
//		sub group
		// how to make sure all inputs are computed prior to this?
		//    ->
		// rotate first subgroup item to after last input (or when
		// copying / merging / adding, insert the input prior to the
		// first subgroup item)

		// how to compute this iteratively and not recursively
		// 
		// with a single output, possible to do the same thing for
		// subgroup we assume for nodes, where you just process in
		// order, and have to track a set of scanlines

		// recursive would look something like:
		for ( size_t i = 0; i < subgroup.outputs(); ++i )
		{
			plane p( w, h );
			for ( int y = 0; y < h; ++y )
			{
				scanline dest = scan_ref( p, y );
				function( dest, arg1, arg2, arg3... );
			}
		}

		// iterative might be something like
		scanline_group sg( w );
		for ( size_t i = 0; i < subgroup.size(); ++i )
		{
			scanline dest = sg.checkout();
			node &cur = subgroup[i];
			cur.function( dest, arg1, arg2, arg3... );
			for ( o: cur.outputs() )
				o.set_input( i, dest );
			o.clean_inputs();
		}
	}
}

struct group_process
{
	std::set<node_id> _members;
	node_id _last;
	graph &_graph;
};

struct scanline_group_process
{
	template <size_t I>
	static inline typename scanline_arg_extractor<typename base::function_traits<function>::template get_arg_type<I+1>::type>::type
	extract( std::deque<scanline> &pool, int y, const std::vector<engine::any> &in )
	{
		typedef typename base::function_traits<function>::template get_arg_type<I+1>::type arg_type;
		typedef scanline_arg_extractor<arg_type> extractor_type;
//		if ( y == 0 )
//			std::cout << "   extract arg " << I << " type " << typeid(arg_type).name() << " result type " << typeid(typename extractor_type::type).name() << " (" << typeid(extractor_type).name() << ")" << std::endl;

		return extractor_type::get( pool, y, in[I] );
	}

	inline plane operator()( const engine::dimensions &d, input_graph &i )
	{
		plane ret( static_cast<int>( d.x ), static_cast<int>( d.y ) );

		std::deque<scanline> scanPool;
		for ( int y = start; y < end; ++y )
		{
			scanline dest = scan_ref( ret, y );
			scanPool.push_back( dest );
			scanFunc( dest, extract<S>( scanPool, y, in )... );
		}
		
	}
};
#endif

////////////////////////////////////////

template <typename T>
struct scanline_arg_extractor
{
	typedef T type;
	static inline type get( int , const engine::any &v )
	{
		return engine::any_cast<type>( v );
	}
};

template <>
struct scanline_arg_extractor<scanline>
{
	typedef scanline type;
	static inline type get( int y, const engine::any &v )
	{
		const plane &p = engine::any_cast<plane>( v );
		return scan_ref( p, y );
	}
};

template <>
struct scanline_arg_extractor<const scanline &>
{
	typedef scanline type;
	static inline type get( int y, const engine::any &v )
	{
		const plane &p = engine::any_cast<plane>( v );
		return scan_ref( p, y );
	}
};

template <>
struct scanline_arg_extractor<scanline &>
{
	typedef scanline type;
	static inline type get( int y, const engine::any &v )
	{
		const plane &p = engine::any_cast<plane>( v );
		return scan_dup( p, y );
	}
};

////////////////////////////////////////

template <typename... Args>
struct scanline_plane_operator
{
	typedef plane result_type;
	typedef std::function<void ( scanline &, Args...)> function;

	inline plane operator()( engine::graph &g, const engine::dimensions &d, const function &f, const std::vector<engine::any> &inputs )
	{
		plane ret( static_cast<int>( d.x ), static_cast<int>( d.y ) );

		std::function<void(int,int)> threadFunc = std::bind(
			dispatch_scans, std::ref( ret ),
			std::placeholders::_1, std::placeholders::_2,
			std::cref( f ), std::cref( inputs ) );
		g.dispatch_threads( threadFunc, 0, ret.height() );

		return ret;
	}

private:
	static inline void dispatch_scans( plane &ret, int start, int end, const function &scanFunc, const std::vector<engine::any> &in )
	{
//		std::cout << "  scanline process dispatching " << start << " - " << end << ", " << in.size() << " inputs" << std::endl;
		process_dispatch( ret, start, end, scanFunc, in, base::gen_sequence<sizeof...(Args)>{} );
	}

	template <size_t I>
	static inline typename scanline_arg_extractor<typename base::function_traits<function>::template get_arg_type<I+1>::type>::type
	extract( int y, const std::vector<engine::any> &in )
	{
		typedef typename base::function_traits<function>::template get_arg_type<I+1>::type arg_type;
		typedef scanline_arg_extractor<arg_type> extractor_type;
//		if ( y == 0 )
//			std::cout << "   extract arg " << I << " type " << typeid(arg_type).name() << " result type " << typeid(typename extractor_type::type).name() << " (" << typeid(extractor_type).name() << ")" << std::endl;

		return extractor_type::get( y, in[I] );
	}

	template <size_t... S>
	static inline void process_dispatch( plane &ret, int start, int end, const std::function<void ( scanline &, Args ... )> &scanFunc, const std::vector<engine::any> &in, const base::sequence<S...> & )
	{
		for ( int y = start; y < end; ++y )
		{
			auto dest = scan_ref( ret, y );
			scanFunc( dest, extract<S>( y, in )... );
		}
	}
};

template <typename Functor>
struct scanline_plane_adapter : scanline_plane_adapter<decltype(&Functor::operator())>
{
};

template <typename... Args>
struct scanline_plane_adapter<void (scanline &, Args...)> : scanline_plane_operator<Args...>
{
};

template <typename... Args>
struct scanline_plane_adapter<void (*)(scanline &, Args...)> : scanline_plane_operator<Args...>
{
};

} // namespace image



