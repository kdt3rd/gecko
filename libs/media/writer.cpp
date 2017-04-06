//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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
			  const std::vector<track_description> &td,
			  const metadata &openParams )
{
	std::call_once( theInitWriters, &initWriters );

	std::shared_ptr<writer> writer;
	if ( ! u.path().empty() )
	{
		std::string ext = base::to_lower( base::file_extension( u.path().back() ) );

		//std::cout << "output extension: " << ext << std::endl;
		std::unique_lock<std::mutex> lk( theWriterMutex );
		auto wOverride = openParams.find( ForceWriterMetadataName );
		if ( wOverride != openParams.end() )
		{
			using namespace base;
			const std::string &name = any_cast<const std::string &>( wOverride->second );
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



