
#include <algorithm>
#include <mutex>
#include "platform.h"

namespace
{
	std::vector<platform::platform> *platforms = nullptr;
	std::mutex lock;
}

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

	if ( platforms == nullptr )
		platforms = new std::vector<::platform::platform>;

	platforms->emplace_back( name, render, creator );
}

////////////////////////////////////////

namespace
{
	std::once_flag flag;
}

const std::vector<platform> &platform::list( void )
{
//	std::call_once( flag, [](){ init(); } );
	init();

	std::lock_guard<std::mutex> guard( lock );
	if ( platforms == nullptr )
		platforms = new std::vector<::platform::platform>;

	return *platforms;
}

////////////////////////////////////////

platform &platform::common( void )
{
//	std::call_once( flag, [](){ init(); } );
	init();

	std::lock_guard<std::mutex> guard( lock );
	if ( platforms == nullptr )
		throw std::runtime_error( "no platforms found" );

	return platforms->front();
}

////////////////////////////////////////

////////////////////////////////////////

}

