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
#include <type_traits>

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

struct plane_scan_binder
{
	plane_scan_binder( void ) = default;
	explicit plane_scan_binder( const plane &p, scanline &s ) : _plane( &p ), _scan( &s ) {}
	explicit plane_scan_binder( scanline &s ) : _scan( &s ) {}

	inline void update( int y ) { if ( _plane ) (*_scan) = scan_ref( *_plane, y ); }
	inline void set( const scanline &s ) { (*_scan) = s; }
	inline void clear( void ) { _scan->clear(); }

	operator const scanline &( void ) const { return (*_scan); }

private:
	const plane *_plane = nullptr;
	scanline *_scan;
};

/// we define our own form of std::reference_wrapper that can be default
/// constructed
template <typename T>
struct engine_ref
{
	typedef T type;
	inline engine_ref( void ) noexcept {}
	inline engine_ref( T &x ) noexcept : _m( &x ) {}
	engine_ref( T &&x ) = delete;
	engine_ref( const engine_ref & ) noexcept = default;
	engine_ref &operator=( const engine_ref & ) noexcept = default;

	inline operator T &( void ) const noexcept { return get(); }
	inline T &get( void ) const noexcept { return *_m; }

	type *_m;
};

template <typename T>
inline engine_ref<T> engref( T &t ) noexcept
{
	return engine_ref<T>( t );
}

template <typename T>
inline engine_ref<const T> cengref( const T &t ) noexcept
{
	return engine_ref<const T>( t );
}

template <typename T>
struct arg_type_adapter
{
	typedef typename std::decay<T>::type base_type;
	typedef engine_ref<const base_type> type;

	static inline const base_type &extract( const type &b )
	{
		return static_cast<const base_type &>( b );
	}

	static inline size_t prebind( std::vector<plane_scan_binder> &, std::vector<scanline> &s )
	{
		return size_t(-1);
	}
	static inline type get( std::vector<plane_scan_binder> &b, std::vector<scanline> &s, size_t i, const engine::any &v, engine::node_id , engine::subgroup &, engine::subgroup_function * )
	{
		precondition( i == size_t(-1), "item with no binder has index to binder list" );
		return cengref( base::any_cast<const base_type &>( v ) );
	}
};

template <>
struct arg_type_adapter<scanline &>
{
	typedef plane_scan_binder base_type;
	typedef engine_ref<const base_type> type;

	static inline const scanline &extract( const type &b )
	{
		const base_type &pb = static_cast<const base_type &>( b );
		return static_cast<const scanline &>( pb );
	}

	static inline size_t prebind( std::vector<plane_scan_binder> &b, std::vector<scanline> &s )
	{
		size_t i = b.size();
		b.push_back( plane_scan_binder() );
		s.push_back( scanline() );
		return i;
	}

	static inline type get( std::vector<plane_scan_binder> &b, std::vector<scanline> &s, size_t i, const engine::any &v, engine::node_id n, engine::subgroup &sg, engine::subgroup_function *sgf )
	{
		precondition( i != size_t(-1), "item with binder has not binder" );
		if ( ! v.empty() )
			b[i] = plane_scan_binder( base::any_cast<const plane &>( v ), s[i] );
		else
		{
			b[i] = plane_scan_binder( s[i] );
			sg.func( n ).add_output( sgf, i );
		}

		return cengref( b[i] );
	}
};

template <>
struct arg_type_adapter<const scanline &>
{
	typedef plane_scan_binder base_type;
	typedef engine_ref<const base_type> type;

	static inline const scanline &extract( const type &b )
	{
		const base_type &pb = static_cast<const base_type &>( b );
		return static_cast<const scanline &>( pb );
	}

	static inline size_t prebind( std::vector<plane_scan_binder> &b, std::vector<scanline> &s )
	{
		size_t i = b.size();
		b.push_back( plane_scan_binder() );
		s.push_back( scanline() );
		return i;
	}

	static inline type get( std::vector<plane_scan_binder> &b, std::vector<scanline> &s, size_t i, const engine::any &v, engine::node_id n, engine::subgroup &sg, engine::subgroup_function *sgf )
	{
		precondition( i != size_t(-1), "item with binder has not binder" );
		if ( ! v.empty() )
			b[i] = plane_scan_binder( base::any_cast<const plane &>( v ), s[i] );
		else
		{
			b[i] = plane_scan_binder( s[i] );
			sg.func( n ).add_output( sgf, i );
		}
		
		return cengref( b[i] );
	}
};

class scanline_plane_functor : public engine::subgroup_function
{
public:
	using engine::subgroup_function::subgroup_function;

	virtual void call( scanline &dest ) = 0;

	virtual const std::vector<scanline> &inputs( void ) const = 0;
	virtual void set_input( size_t i, const scanline &s ) = 0;
};

template <typename... Args>
class scanline_plane_operator : public scanline_plane_functor
{
public:
	typedef plane result_type;
	typedef std::function<void ( scanline &, Args...)> function;
	typedef std::tuple<typename arg_type_adapter<Args>::type...> scan_args;

	scanline_plane_operator( void )
	{}
	explicit scanline_plane_operator( const function &f )
		: _func( f )
	{}
	virtual ~scanline_plane_operator( void )
	{}

	virtual const std::vector<scanline> &inputs( void ) const override
	{
		return _inputs;
	}

	virtual void update_inputs( int d ) override
	{
		for ( auto &b: _binders )
			b.update( d );
	}

	virtual void deref_inputs( void ) override
	{
		for ( auto &b: _binders )
			b.clear();
	}

	virtual void bind( engine::subgroup &sg, engine::node &n ) override
	{
		process_bind( sg, n, base::gen_sequence<sizeof...(Args)>{} );
	}

	virtual void call( scanline &dest ) override
	{
		process_call( dest, base::gen_sequence<sizeof...(Args)>{} );
	}

	virtual void set_input( size_t i, const scanline &s )
	{
		_binders[i].set( s );
	}

private:
	template <size_t... S>
	inline void process_bind( engine::subgroup &sg, engine::node &n, const base::sequence<S...> & )
	{
		size_t b[] = { arg_type_adapter<Args>::prebind( _binders, _inputs )... };
		_args = std::make_tuple( arg_type_adapter<Args>::get( _binders, _inputs, b[S], sg.gref()[n.input(S)].value(), n.input(S), sg, this )... );
	}

	template <size_t... S>
	inline void process_call( scanline &dest, const base::sequence<S...> & )
	{
		_func( dest, arg_type_adapter<Args>::extract( std::get<S>( _args ) )... );
	}

	std::vector<scanline> _inputs;
	std::vector<plane_scan_binder> _binders;
	function _func;
	scan_args _args;
};

void scanline_thread_process( int start, int end, engine::subgroup &sg, int w, int h );
void dispatch_scan_processing( engine::subgroup &sg, const engine::dimensions &dims );


#if 0

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
		const plane &p = engine::any_cast<const plane &>( v );
		return scan_ref( p, y );
	}
};

template <>
struct scanline_arg_extractor<const scanline &>
{
	typedef scanline type;
	static inline type get( int y, const engine::any &v )
	{
		const plane &p = engine::any_cast<const plane &>( v );
		return scan_ref( p, y );
	}
};

template <>
struct scanline_arg_extractor<scanline &>
{
	typedef scanline type;
	static inline type get( int y, const engine::any &v )
	{
		const plane &p = engine::any_cast<const plane &>( v );
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
#endif

template <typename Functor>
struct scanline_plane_adapter : public scanline_plane_adapter<decltype(&Functor::operator())>
{
	using scanline_plane_adapter<decltype(&Functor::operator())>::scanline_plane_adapter;
};

template <typename... Args>
struct scanline_plane_adapter<void (scanline &, Args...)> : public scanline_plane_operator<Args...>
{
	scanline_plane_adapter( void ) {}
	scanline_plane_adapter( const std::function<void(scanline &, Args...)> &f )
		: scanline_plane_operator<Args...>( f )
	{}
};

template <typename... Args>
struct scanline_plane_adapter<void (*)(scanline &, Args...)> : public scanline_plane_operator<Args...>
{
	scanline_plane_adapter( void ) {}
	scanline_plane_adapter( const std::function<void(scanline &, Args...)> &f )
		: scanline_plane_operator<Args...>( f )
	{}
};

} // namespace image



