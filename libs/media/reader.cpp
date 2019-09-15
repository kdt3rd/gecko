// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "reader.h"

#include "exr_reader.h"
#include "png_reader.h"
#include "tiff_reader.h"

#include <base/contract.h>
#include <base/file_path.h>
#include <base/file_system.h>
#include <base/string_util.h>
#include <map>
#include <mutex>

namespace
{
std::once_flag theInitReaders;
std::mutex     theReaderMutex;
void           initReaders( void )
{
    media::register_exr_reader();
    //	media::register_png_reader();
    //	media::register_tiff_reader();
}

static std::vector<std::shared_ptr<media::reader>> theReaders;
static std::map<std::string, std::shared_ptr<media::reader>>
              theReadersByExtension;
static size_t theMaxHeaderMagicSize = 0;

} // namespace

////////////////////////////////////////

namespace media
{
////////////////////////////////////////

reader::reader( base::cstring n ) : _name( n ) {}

////////////////////////////////////////

reader::~reader( void ) {}

////////////////////////////////////////

parameter_set reader::default_parameters( void ) const
{
    return initialize_parameters( parameters() );
}

////////////////////////////////////////

container reader::open( const base::uri &u, const parameter_set &openParams )
{
    try
    {
        std::call_once( theInitReaders, &initReaders );

        std::shared_ptr<reader> reader;
        if ( !u.path().empty() )
        {
            std::string ext =
                base::to_lower( base::file_extension( u.path().back() ) );

            std::unique_lock<std::mutex> lk( theReaderMutex );
            auto                         eh = theReadersByExtension.find( ext );
            if ( eh != theReadersByExtension.end() )
                reader = eh->second;
        }

        // because of scoping, we have unlocked the mutex so other readers can find the factory
        if ( reader )
            return reader->create( u, openParams );
    }
    catch ( std::exception &e )
    {
        std::cout
            << "WARNING: Unable to open media container based on file extension, falling back to scanning headers:"
            << std::endl;
        base::print_exception( std::cout, e );
    }

    try
    {
        return scan_header( u, openParams );
    }
    catch ( std::exception &e )
    {
        std::cout
            << "WARNING: Unable to find media reader based on magic number, scanning directory:"
            << std::endl;
        base::print_exception( std::cout, e );
    }

    throw_not_yet();
}

////////////////////////////////////////

parameter_set reader::query_parameters( const base::uri &u, bool temp_open )
{
    if ( temp_open )
    {
        parameter_set r;
        container     c = open( u, r );
        return c.parameters();
    }

    parameter_set r;
    std::call_once( theInitReaders, &initReaders );

    std::shared_ptr<reader> reader;
    if ( !u.path().empty() )
    {
        std::string ext =
            base::to_lower( base::file_extension( u.path().back() ) );

        std::unique_lock<std::mutex> lk( theReaderMutex );
        auto                         eh = theReadersByExtension.find( ext );
        if ( eh != theReadersByExtension.end() )
            reader = eh->second;
    }

    // because of scoping, we have unlocked the mutex
    if ( reader )
        r = reader->default_parameters();
    else
        throw_runtime( "Unable to determine media reader based on extension" );

    return r;
}

////////////////////////////////////////

void reader::register_reader( const std::shared_ptr<reader> &r )
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
        precondition(
            theReadersByExtension.find( e ) == theReadersByExtension.end(),
            "Extension {0} already handled by {1}",
            e,
            theReadersByExtension[e]->name() );
        theReadersByExtension[e] = r;
    }

    for ( auto &m: r->magic_numbers() )
    {
        theMaxHeaderMagicSize = std::max( theMaxHeaderMagicSize, m.size() );
    }
}

////////////////////////////////////////

container
reader::scan_header( const base::uri &u, const parameter_set &openParams )
{
    if ( theMaxHeaderMagicSize == 0 )
        throw_runtime(
            "Header scanning not possible: no readers registered with magic numbers" );

    std::vector<char> fheader( theMaxHeaderMagicSize, 0 );
    std::streamsize   n = 0;
    {
        auto          fs     = base::file_system::get( u );
        base::istream stream = fs->open_read( u );

        if ( stream.read(
                 fheader.data(),
                 static_cast<std::streamsize>( theMaxHeaderMagicSize ) ) )
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

} // namespace media
