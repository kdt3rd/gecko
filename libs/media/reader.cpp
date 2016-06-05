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

#include "reader.h"
#include <base/contract.h>
#include <base/file_path.h>
#include <base/string_util.h>
#include <base/file_system.h>

#include "exr_reader.h"
#include "tiff_reader.h"
#include <mutex>
#include <map>

namespace
{

std::once_flag theInitReaders;
std::mutex theReaderMutex;
void initReaders( void )
{
	media::register_exr_reader();
	media::register_tiff_reader();
}

static std::vector<std::shared_ptr<media::reader>> theReaders;
static std::map<std::string, std::shared_ptr<media::reader>> theReadersByExtension;
static size_t theMaxHeaderMagicSize = 0;

}


////////////////////////////////////////


namespace media
{


////////////////////////////////////////


reader::reader( base::cstring n )
	: _name( n )
{
}


////////////////////////////////////////


reader::~reader( void )
{
}


////////////////////////////////////////


container
reader::open( const base::uri &u,
			  const metadata &openParams )
{
	try
	{
		std::call_once( theInitReaders, &initReaders );

		std::shared_ptr<reader> reader;
		if ( ! u.path().empty() )
		{
			std::string ext = base::to_lower( base::file_extension( u.path().back() ) );

			std::unique_lock<std::mutex> lk( theReaderMutex );
			auto eh = theReadersByExtension.find( ext );
			if ( eh != theReadersByExtension.end() )
				reader = eh->second;
		}

		// because of scoping, we have unlocked the mutex so other readers can find the factory
		if ( reader )
			return reader->create( u, openParams );
	}
	catch ( std::exception &e )
	{
		std::cout << "WARNING: Unable to open media container based on file extension, falling back to scanning headers:" << std::endl;
		base::print_exception( std::cout, e );
	}

	try
	{
		return scan_header( u, openParams );
	}
	catch ( std::exception &e )
	{
		std::cout << "WARNING: Unable to find media reader based on magic number, scanning directory:" << std::endl;
		base::print_exception( std::cout, e );
	}

	throw_not_yet();
}


////////////////////////////////////////


void
reader::register_reader( const std::shared_ptr<reader> &r )
{
	std::unique_lock<std::mutex> lk( theReaderMutex );

	for ( auto &cr: theReaders )
	{
		if ( cr->name() == r->name() )
			throw_logic( "{0} media reader already registered", r->name() );
	}
	theReaders.push_back( r );

	for ( auto &e: r->extensions() )
	{
		precondition( theReadersByExtension.find( e ) == theReadersByExtension.end(), "Extension {0} already handled by {1}", e, theReadersByExtension[e]->name() );
		theReadersByExtension[e] = r;
	}

	for ( auto &m: r->magic_numbers() )
	{
		theMaxHeaderMagicSize = std::max( theMaxHeaderMagicSize, m.size() );
	}
}

////////////////////////////////////////

container
reader::scan_header( const base::uri &u, const metadata &openParams )
{
	if ( theMaxHeaderMagicSize == 0 )
		throw_runtime( "Header scanning not possible: no readers registered with magic numbers" );

	std::vector<char> fheader( theMaxHeaderMagicSize, 0 );
	std::streamsize n = 0;
	{
		auto fs = base::file_system::get( u );
		base::istream stream = fs->open_read( u );

		if ( stream.read( fheader.data(), static_cast<std::streamsize>( theMaxHeaderMagicSize ) ) )
		{
			n = stream.gcount();
		}
	}
	
	std::unique_lock<std::mutex> lk( theReaderMutex );

	for ( auto &r: theReaders )
	{
		for ( auto &m: r->magic_numbers() )
		{
			if ( n >= static_cast<std::streamsize>( m.size() ) )
			{
				if ( std::equal( m.begin(), m.end(), fheader.begin() ) )
				{
					return r->create( u, openParams );
				}
			}
		}
	}

	throw_runtime( "Unable to find reader by magic number" );
}


////////////////////////////////////////


} // media



