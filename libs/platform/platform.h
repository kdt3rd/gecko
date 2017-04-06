//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "system.h"

namespace platform
{

////////////////////////////////////////

class platform
{
public:
	const std::string &name( void ) const { return _name; }
	const std::string &renderer( void ) const { return _render; }

	std::shared_ptr<system> create( void ) const { return _creator(); }

	static const std::vector<platform> &list( void );
	static const platform &common( void );

	platform( void ) = default;
	platform( const platform & ) = default;
	platform &operator=( const platform & ) = default;
	platform( platform && ) = default;
	platform &operator=( platform && ) = default;
	~platform( void ) = default;

private:
	// each O.S. need to implement this
	static std::vector<platform> &init( void );

	// TODO: support passing a display name (valid for Xlib, wayland anyway)
	platform( std::string name, std::string render, const std::function<std::shared_ptr<system>(void)> &creator );

	std::string _name;
	std::string _render;
	std::function<std::shared_ptr<system>(void)> _creator;
};

////////////////////////////////////////

}

