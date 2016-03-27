//
// Copyright (c) 2016 Kimball Thurston & Ian Godin
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

#include "container.h"
// remove this if we create video_track.cpp
#include "video_track.h"
#include <stdexcept>
#include <base/file_path.h>

#include "exr_reader.h"
#include <mutex>

namespace
{

struct container_factory
{
	std::function<media::container(const base::uri&)> func;
	std::vector<std::string> extensions;
	std::vector<std::vector<uint8_t>> magics;
};

std::once_flag theInitCreators;
std::mutex theCreatorMutex;
void initCreators( void )
{
	media::register_exr_reader();
}

static std::map<std::string, container_factory> theCreators;
static std::map<std::string, std::string> theCreatorsByExtension;

}

namespace media
{


////////////////////////////////////////


track::~track( void )
{
}


////////////////////////////////////////


video_track::~video_track( void )
{
}


////////////////////////////////////////

//container::container( void )
//{
//}

////////////////////////////////////////

//container::~container( void )
//{
//}
container
container::create( const base::uri &u )
{
	try
	{
		std::call_once( theInitCreators, &initCreators );

		std::function<container(const base::uri &)> factory;
		if ( ! u.path().empty() )
		{
			std::string ext = base::file_extension( u.path().back() );

			std::unique_lock<std::mutex> lk( theCreatorMutex );
			auto eh = theCreatorsByExtension.find( ext );
			if ( eh != theCreatorsByExtension.end() )
			{
				auto fact = theCreators.find( eh->second );
				postcondition( fact != theCreators.end(), "Extension registered but handler {0} not found", eh->second );
				factory = fact->second.func;
			}
		}

		// because of scoping, we have unlocked the mutex so other readers can find the factory
		if ( factory )
			return factory( u );
	}
	catch ( std::exception &e )
	{
		std::cout << "WARNING: Unable to open media container based on file extension, falling back to scanning headers: " << e.what() << std::endl;
	}
	return scan_header( u );
}


////////////////////////////////////////


void
container::register_media_type( const std::string &name,
								const std::function<container(const base::uri &)> &factory,
								const std::vector<std::string> &extlist,
								const std::vector<std::vector<uint8_t>> &magics )
{
	std::unique_lock<std::mutex> lk( theCreatorMutex );

	precondition( theCreators.find( name ) == theCreators.end(), "{0} container creator already registered", name );
	container_factory &cf = theCreators[name];
	cf.func = factory;
	cf.extensions = extlist;
	cf.magics = magics;
	for ( auto &e: extlist )
	{
		precondition( theCreatorsByExtension.find( e ) == theCreatorsByExtension.end(), "Extension {0} already handled by {1}", e, theCreatorsByExtension[e] );
		theCreatorsByExtension[e] = name;
	}
}

////////////////////////////////////////

container
container::scan_header( const base::uri &u )
{
	throw_not_yet();
}

////////////////////////////////////////

}

