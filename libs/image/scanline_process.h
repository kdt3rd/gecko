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

template <typename T>
struct scanline_type_convert
{
	typedef T type;
};

template <>
struct scanline_type_convert<scanline>
{
	typedef plane type;
};

template <>
struct scanline_type_convert<scanline &>
{
	typedef plane &type;
};

template <>
struct scanline_type_convert<const scanline &>
{
	typedef const plane &type;
};

template <typename> struct scanline_process {};

template <typename... Args>
struct scanline_process<void (*)( scanline &, Args... )>
{
	typedef typename std::function<plane(typename scanline_type_convert<Args>::type...)> function;
};

template <typename ScanArg, typename Input>
struct scan_convert
{
	static inline ScanArg extract( int y, Input && v )
	{
		return std::forward<Input>( v );
	}
};

template <>
struct scan_convert<scanline &, plane &>
{
	static inline scanline extract( int y, plane &v )
	{
		return scan_ref( v, y );
	}
};

template <>
struct scan_convert<scanline &, const plane &>
{
	static inline scanline extract( int y, const plane &v )
	{
		return scan_dup( v, y );
	}
};

template <>
struct scan_convert<const scanline &, const plane &>
{
	static inline scanline extract( int y, const plane &v )
	{
		return scan_ref( v, y );
	}
};

template <typename... Args, typename... Inputs>
inline void
scan_process( plane &ret, int start, int end, void (*scanFunc)( scanline &, Args ... ), Inputs && ...in )
{
	for ( int y = start; y < end; ++y )
	{
		auto dest = scan_ref( ret, y );
		scanFunc( dest, scan_convert<Args,Inputs>::extract( y, std::forward<Inputs>( in ) )... );
	}
}

template <typename... Args>
struct scanline_plane_operator
{
	typedef plane result_type;

	inline plane operator()( engine::graph &g, const engine::dimensions &d, const std::function<void (scanline &, Args...)> &f, const std::vector<engine::any> &inputs )
	{
		plane ret( static_cast<int>( d.x ), static_cast<int>( d.y ) );

		std::function<void(int,int)> threadFunc = std::bind(
			dispatch_scans, std::ref( ret ),
			std::placeholders::_1, std::placeholders::_2,
			std::cref( f ), std::cref( inputs ) );
		g.dispatch_threads( threadFunc, 0, ret.height() );

		return ret;
	}

	static inline void dispatch_scans( plane &ret, int start, int end, const std::function<void ( scanline &, Args ... )> &scanFunc, const std::vector<engine::any> &in )
	{
		for ( int y = start; y < end; ++y )
		{
			auto dest = scan_ref( ret, y );
//		scanFunc( dest, scan_convert<Args,Inputs>::extract( y, std::forward<Inputs>( in ) )... );
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



