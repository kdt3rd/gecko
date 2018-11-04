//
// Copyright (c) 2015-2018 Kimball Thurston and Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <base/const_string.h>
#include "layer.h"
#include "sample_data.h"

namespace media
{

class image;
class data;

////////////////////////////////////////

/// @brief a frame is a concept for the video track
///
/// concepts available within a frame
///
/// @sa layer - sub-image for a frame (beauty, AOV, light_group1, etc)
/// @sa view - a particular view of a layer (left, right, mono, *empty* / none)
/// @sa image - actual image with planes (red, green, blue, etc.) for the layer
/// @sa data - actual data (deep, etc.) for the view
///
/// Some of these aren't always both available within a file format
///
class frame : public sample_data
{
public:
	using layer_list = base::small_vector<layer, 1>;
	using named_image = std::pair< std::string, std::shared_ptr<image> >;
	using named_image_list = base::small_vector<named_image, 1>;

	frame( void ) = default;
	frame( int64_t fn ) : _number( fn ) {}
	~frame( void ) override;

	int64_t number( void ) const
	{
		return _number;
	}

	size_t item_count( void ) const override { return layer_count(); }

	const layer_list &layers( void ) const { return _layers; }
	size_t layer_count( void ) const { return _layers.size(); }

	bool layer_has_view( size_t layerIdx, size_t viewIdx ) const;
	bool layer_has_view( size_t layerIdx, base::cstring v ) const;
	bool layer_has_view( base::cstring layer, size_t viewIdx ) const;
	bool layer_has_view( base::cstring layer, base::cstring v ) const;

	std::shared_ptr<image> find_image( size_t layerIdx, size_t viewIdx ) const;
	std::shared_ptr<image> find_image( size_t layerIdx, base::cstring v ) const;
	std::shared_ptr<image> find_image( base::cstring layer, size_t viewIdx ) const;
	std::shared_ptr<image> find_image( base::cstring layer, base::cstring v ) const;

	/// deep data and other data attached to a frame but not specifically
	/// an image
	std::shared_ptr<data> find_data( size_t layerIdx, size_t viewIdx ) const;
	std::shared_ptr<data> find_data( size_t layerIdx, base::cstring v ) const;
	std::shared_ptr<data> find_data( base::cstring layer, size_t viewIdx ) const;
	std::shared_ptr<data> find_data( base::cstring layer, base::cstring v ) const;

	named_image_list images_for_view( base::cstring v ) const;

	layer &register_layer( base::cstring lname );

	class const_image_iterator
	{
	public:
		const_image_iterator( void ) = default;
		inline const_image_iterator( const frame *f );

		inline const_image_iterator &operator++( void );
		const_image_iterator operator++( int ) const;
		bool operator==( const const_image_iterator &o ) const;
		bool operator!=( const const_image_iterator &o ) const;

		const layer &access_layer( void ) const;
		const view &access_view( void ) const;

		image & operator*( void ) const;
	private:
		void advance( bool incfirst );

		const frame *_frame = nullptr;
		size_t _cur_layer = 0;
		size_t _cur_view = 0;
	};

	const_image_iterator image_begin( void ) const { return const_image_iterator( this ); }
	const_image_iterator image_end( void ) const { return const_image_iterator(); }

	// TODO: merge data and image? Need to work w/ deep more
	class const_data_iterator
	{
	public:
		const_data_iterator( void ) = default;
		inline const_data_iterator( const frame *f );

		inline const_data_iterator &operator++( void );
		const_data_iterator operator++( int ) const;
		bool operator==( const const_data_iterator &o ) const;
		bool operator!=( const const_data_iterator &o ) const;

		const layer &access_layer( void ) const;
		const view &access_view( void ) const;

		data & operator*( void ) const;
	private:
		void advance( bool incfirst );

		const frame *_frame = nullptr;
		size_t _cur_layer = 0;
		size_t _cur_view = 0;
	};

	const_data_iterator data_begin( void ) const { return const_data_iterator( this ); }
	const_data_iterator data_end( void ) const { return const_data_iterator(); }

	// TODO: metadata here or no?

protected:
	frame( const frame & ) = delete;
	frame &operator=( const frame & ) = delete;
	frame( frame && ) = delete;
	frame &operator=( frame && ) = delete;

private:
	int64_t _number = -1;

	layer_list _layers;
};

////////////////////////////////////////

inline
frame::const_image_iterator::const_image_iterator( const frame *f )
	: _frame( f ), _cur_layer( 0 ), _cur_view( 0 )
{
	advance( false );
}

inline frame::const_image_iterator &
frame::const_image_iterator::operator++( void )
{
	advance( true );
	return *this;
}

inline frame::const_image_iterator
frame::const_image_iterator::operator++( int ) const
{
	const_image_iterator i{ *this };
	++i;
	return i;
}

inline bool
frame::const_image_iterator::operator==( const const_image_iterator &o ) const
{
	return _frame == o._frame && _cur_layer == o._cur_layer && _cur_view == o._cur_view;
}

inline bool frame::const_image_iterator::operator!=( const const_image_iterator &o ) const
{
	return ! (*this == o );
}

inline const layer &
frame::const_image_iterator::access_layer( void ) const
{
	return _frame->layers()[_cur_layer];
}

inline const view &
frame::const_image_iterator::access_view( void ) const
{
	return _frame->layers()[_cur_layer][_cur_view];
}

inline image &
frame::const_image_iterator::operator*( void ) const
{
	return *( static_cast<const std::shared_ptr<image> &>( _frame->layers()[_cur_layer][_cur_view] ) );
}

inline void
frame::const_image_iterator::advance( bool incfirst )
{
	if ( ! _frame )
		return;

	const layer_list &ll = _frame->layers();
	while ( _cur_layer < ll.size() )
	{
		const layer &l = ll[_cur_layer];
		if ( incfirst )
			++_cur_view;

		while ( _cur_view < l.view_count() )
		{
			if ( ! static_cast<const std::shared_ptr<image> &>( l[_cur_view] ) )
				++_cur_view;
		}

		if ( _cur_view >= l.view_count() )
		{
			_cur_view = 0;
			++_cur_layer;
			incfirst = false;
		}
	}

	if ( _cur_layer >= ll.size() )
	{
		_frame = nullptr;
		_cur_layer = 0;
		_cur_view = 0;
	}
}

////////////////////////////////////////

inline
frame::const_data_iterator::const_data_iterator( const frame *f )
	: _frame( f ), _cur_layer( 0 ), _cur_view( 0 )
{
	advance( false );
}

inline frame::const_data_iterator &
frame::const_data_iterator::operator++( void )
{
	advance( true );
	return *this;
}

inline frame::const_data_iterator
frame::const_data_iterator::operator++( int ) const
{
	const_data_iterator i{ *this };
	++i;
	return i;
}

inline bool
frame::const_data_iterator::operator==( const const_data_iterator &o ) const
{
	return _frame == o._frame && _cur_layer == o._cur_layer && _cur_view == o._cur_view;
}

inline bool frame::const_data_iterator::operator!=( const const_data_iterator &o ) const
{
	return ! (*this == o );
}

inline data &
frame::const_data_iterator::operator*( void ) const
{
	return *( static_cast<const std::shared_ptr<data> &>( _frame->layers()[_cur_layer][_cur_view] ) );
}

inline void
frame::const_data_iterator::advance( bool incfirst )
{
	if ( ! _frame )
		return;

	const layer_list &ll = _frame->layers();
	while ( _cur_layer < ll.size() )
	{
		const layer &l = ll[_cur_layer];
		if ( incfirst )
			++_cur_view;

		while ( _cur_view < l.view_count() )
		{
			if ( ! static_cast<const std::shared_ptr<data> &>( l[_cur_view] ) )
				++_cur_view;
		}

		if ( _cur_view >= l.view_count() )
		{
			_cur_view = 0;
			++_cur_layer;
			incfirst = false;
		}
	}

	if ( _cur_layer >= ll.size() )
	{
		_frame = nullptr;
		_cur_layer = 0;
		_cur_view = 0;
	}
}

////////////////////////////////////////

}

