
#include <algorithm>
#include <mutex>
#include "platform.h"

std::vector<platform::platform> *_list = nullptr;
std::mutex lock;

namespace platform
{

////////////////////////////////////////


////////////////////////////////////////

platform::platform( std::string name, std::string render, const std::function<std::shared_ptr<system>(void)> &creator )
	: _name( std::move(name) ), _render( std::move(render) ), _creator( creator )
{
}

////////////////////////////////////////

void platform::enroll( std::string name, std::string render, const std::function<std::shared_ptr<system>(void)> &creator )
{
	std::lock_guard<std::mutex> guard( lock );

	if ( _list == nullptr )
		_list = new std::vector<platform::platform>;

	_list->emplace_back( name, render, creator );
}

////////////////////////////////////////

const std::vector<platform> &platform::list( void )
{
	init();

	std::lock_guard<std::mutex> guard( lock );
	if ( _list == nullptr )
		_list = new std::vector<platform::platform>;

	return *_list;
}

////////////////////////////////////////

}

