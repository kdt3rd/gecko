// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace media
{

///
/// @brief Class sample_data provides a base class for tracks to read
/// samples.
///
/// This allows generic access and management for track data, although
/// is more of an architectural concept than anything that is
/// particularly useful itself.
class sample_data
{
public:
	sample_data( void ) = default;
	virtual ~sample_data( void );

	// mark it as non-copyable (it should be stored in a smart pointer)
	sample_data( const sample_data & ) = delete;
	sample_data &operator=( const sample_data & ) = delete;
	sample_data( sample_data && ) = delete;
	sample_data &operator=( sample_data && ) = delete;

	/// @brief how many items are in this sample_data
	///
	/// An image frame will probably only have 1, but other kinds of
	/// data may have multiple samples (i.e. audio)
	virtual size_t item_count( void ) const = 0;

private:

};

} // namespace media



