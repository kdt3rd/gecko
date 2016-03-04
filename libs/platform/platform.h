
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
	platform( std::string name, std::string render, const std::function<std::shared_ptr<system>(void)> &creator );
	const std::string &name( void ) const { return _name; }
	const std::string &render( void ) const { return _render; }

	std::shared_ptr<system> create( void ) const { return _creator(); }

	static void enroll( std::string name, std::string render, const std::function<std::shared_ptr<system>(void)> &creator );
	static const std::vector<platform> &list( void );

	static platform &common( void );

private:
	static void init( void );

	platform( void ) = delete;
	std::string _name;
	std::string _render;
	std::function<std::shared_ptr<system>(void)> _creator;
};

////////////////////////////////////////

}

