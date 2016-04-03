//
// Copyright (c) 2016 Kimball Thurston
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

#include "writer.h"
#include <base/contract.h>
#include <base/file_path.h>
#include <base/string_util.h>

#include "exr_writer.h"
#include <mutex>
#include <atomic>
#include <map>


////////////////////////////////////////


namespace
{

std::once_flag theInitWriters;
std::mutex theWriterMutex;
void initWriters( void )
{
	media::register_exr_writer();
}

static std::vector<std::shared_ptr<media::writer>> theWriters;
static std::map<std::string, std::shared_ptr<media::writer>> theWritersByExtension;

}


////////////////////////////////////////


namespace media
{

const std::string writer::ForceWriterMetadataName = "media::WriterName";


////////////////////////////////////////


writer::writer( base::cstring n )
	: _name( n )
{
}


////////////////////////////////////////


writer::~writer( void )
{
}


////////////////////////////////////////


container
writer::open( const base::uri &u,
			  const track_description &td,
			  const metadata &openParams )
{
	std::call_once( theInitWriters, &initWriters );

	std::shared_ptr<writer> writer;
	if ( ! u.path().empty() )
	{
		std::string ext = base::to_lower( base::file_extension( u.path().back() ) );

		std::unique_lock<std::mutex> lk( theWriterMutex );
		auto wOverride = openParams.find( ForceWriterMetadataName );
		if ( wOverride != openParams.end() )
		{
			const std::string &name = wOverride->second.as<std::string>();
			for ( auto &w: theWriters )
			{
				if ( w->name() == name )
				{
					writer = w;
					break;
				}
			}
			if ( ! writer )
				throw_runtime( "Writer override set to {0}, but specified writer not found", name );
		}
		else
		{
			auto eh = theWritersByExtension.find( ext );
			if ( eh != theWritersByExtension.end() )
				writer = eh->second;
		}
	}

	// because of scoping, we have unlocked the mutex so other writers can find the factory
	if ( writer )
		return writer->create( u, td, openParams );
	else
		throw_runtime( "No writer found for {0} with given parameters", u );
}


////////////////////////////////////////


void
writer::register_writer( const std::shared_ptr<writer> &w )
{
	std::unique_lock<std::mutex> lk( theWriterMutex );

	for ( auto &cw: theWriters )
	{
		if ( cw->name() == w->name() )
			throw_logic( "{0} media writer already registered", w->name() );
	}
	theWriters.push_back( w );

	for ( auto &e: w->extensions() )
	{
		precondition( theWritersByExtension.find( e ) == theWritersByExtension.end(), "Extension {0} already handled by {1}", e, theWritersByExtension[e]->name() );
		theWritersByExtension[e] = w;
	}
}


////////////////////////////////////////


} // media



