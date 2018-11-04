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

parameter_set writer::default_parameters( void ) const
{
	return initialize_parameters( parameters() );
}

////////////////////////////////////////

std::shared_ptr<writer> writer::find_by_ext( const base::uri &u )
{
	std::call_once( theInitWriters, &initWriters );

	std::shared_ptr<writer> writer;
	if ( ! u.path().empty() )
	{
		std::string ext = base::to_lower( base::file_extension( u.path().back() ) );

		//std::cout << "output extension: " << ext << std::endl;
		std::unique_lock<std::mutex> lk( theWriterMutex );
		auto eh = theWritersByExtension.find( ext );
		if ( eh != theWritersByExtension.end() )
			writer = eh->second;
	}
	return writer;
}

////////////////////////////////////////

std::shared_ptr<writer> writer::find_by_name( const std::string &n )
{
	std::call_once( theInitWriters, &initWriters );

	std::shared_ptr<writer> writer;
	std::unique_lock<std::mutex> lk( theWriterMutex );

	for ( auto &w: theWriters )
	{
		if ( w->name() == n )
		{
			writer = w;
			break;
		}
	}
	return writer;
}

////////////////////////////////////////

parameter_set writer::parameters_by_ext( const base::uri &u, const std::string &opts )
{
	auto w = find_by_ext( u );
	if ( w )
		return initialize_parameters( w->parameters(), opts );
	return parameter_set();
}

////////////////////////////////////////

std::vector< std::shared_ptr<writer> > writer::available( void )
{
	std::call_once( theInitWriters, &initWriters );
	std::unique_lock<std::mutex> lk( theWriterMutex );
	return theWriters;
}

////////////////////////////////////////

container
writer::open(
	const base::uri &u,
	const parameter_set &params,
	const std::string &forcewriter )
{
	std::vector<track_description> td;
	return open( u, td, params, forcewriter );
}

////////////////////////////////////////

container
writer::open(
	const base::uri &u,
	const std::vector<track_description> &td,
	const parameter_set &params,
	const std::string &forcewriter )
{
	std::shared_ptr<writer> writer;
	if ( forcewriter.empty() )
	{
		writer = find_by_ext( u );
		if ( ! writer )
			throw_runtime( "No writer found for output path {0}, please specify writer manually", u );
	}
	else
	{
		writer = find_by_name( forcewriter );
		if ( ! writer )
			throw_runtime( "Writer override set to {0}, but specified writer not found", forcewriter );
	}

	return writer->create( u, td, params );
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



