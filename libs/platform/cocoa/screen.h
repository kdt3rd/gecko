//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/screen.h>

namespace platform { namespace cocoa
{

class system;

////////////////////////////////////////

/// @brief Cocoa implementation of screen.
class screen : public platform::screen
{
public:
	screen( void ) = delete;
	screen( void *scr );
	~screen( void );

	bool is_default( void ) const override;

	double refresh_rate( void ) const override;

	rect bounds( bool avail ) const override;
	base::dsize dpi( void ) const override;

private:
	friend class platform::cocoa::system;

	void *_nsscreen;
};

////////////////////////////////////////

} }

