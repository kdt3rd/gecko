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

#include <array>
#include <vector>
#include "plane.h"
#include <engine/computed_value.h>
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

///
/// @brief Class image_buf provides a collection of planes
///
class image_buf : public engine::computed_base
{
public:
	typedef std::vector<plane> plane_list;
	typedef plane_list::iterator iterator;
	typedef plane_list::const_iterator const_iterator;
	typedef plane_list::reverse_iterator reverse_iterator;
	typedef plane_list::const_reverse_iterator const_reverse_iterator;

	image_buf( void ) = default;
	template <typename... Args>
	inline image_buf( const base::cstring &opname, const engine::dimensions &d, Args &&... args )
		: computed_base( image::op_registry(), opname, d, std::forward<Args>( args )... )
	{
		engine::dimensions pd = d;
		pd.z = 0;
		pd.w = 0;
		_planes.reserve( static_cast<size_t>( d.z ) );
		for ( uint16_t p = 0; p != d.z; ++p )
			_planes.push_back( plane( "i.extract", pd, *this, size_t(p) ) );
	}

	image_buf( const image_buf & );
	image_buf( image_buf && );
	image_buf &operator=( const image_buf & );
	image_buf &operator=( image_buf && );
	~image_buf( void );

	inline engine::dimensions dims( void ) const
	{
		engine::dimensions r = node_dims();
		if ( r == engine::nulldim && ! _planes.empty() )
		{
			r = _planes.front().dims();
			r.z = static_cast<uint16_t>( size() );
		}
		return r;
	}

	void add_plane( const plane &p );
	void add_plane( plane &&p );

	inline bool empty( void ) const { return _planes.empty(); }
	inline size_t size( void ) const { return _planes.size(); }
	inline plane &operator[]( size_t i ) { return _planes[i]; }
	inline const plane &operator[]( size_t i ) const { return _planes[i]; }

	inline iterator begin( void ) { return _planes.begin(); }
	inline const_iterator begin( void ) const { return _planes.begin(); }
	inline const_iterator cbegin( void ) const { return _planes.cbegin(); }
	inline iterator end( void ) { return _planes.end(); }
	inline const_iterator end( void ) const { return _planes.end(); }
	inline const_iterator cend( void ) const { return _planes.cend(); }

	inline reverse_iterator rbegin( void ) { return _planes.rbegin(); }
	inline const_reverse_iterator rbegin( void ) const { return _planes.rbegin(); }
	inline const_reverse_iterator crbegin( void ) const { return _planes.crbegin(); }
	inline reverse_iterator rend( void ) { return _planes.rend(); }
	inline const_reverse_iterator rend( void ) const { return _planes.rend(); }
	inline const_reverse_iterator crend( void ) const { return _planes.crend(); }

private:
	std::vector<plane> _planes;
};

engine::hash &operator<<( engine::hash &h, const image_buf &p );

} // namespace image



