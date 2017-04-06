//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once


////////////////////////////////////////



namespace media
{

///
/// @brief Class sample_data provides...
///
class sample_data
{
public:
	sample_data( void ) = default;
	virtual ~sample_data( void );

	virtual size_t item_count( void ) const = 0;

private:

};

} // namespace media



