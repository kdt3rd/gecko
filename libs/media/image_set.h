//
// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

#include "image_frame.h"
#include "metadata.h"

////////////////////////////////////////

namespace media
{

///
/// @brief Class image_set provides...
///
class image_set
{
public:
    
	std::shared_ptr<image_part> operator[]( int64_t f )
	{
		return at( f );
	}

	std::shared_ptr<image_part> at( int64_t f )
	{
		return std::shared_ptr<image_frame>( doRead( f ) );
	}

	inline const metadata &info( void ) const;
private:

};

} // namespace media
