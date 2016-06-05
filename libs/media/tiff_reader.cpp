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

#include "tiff_reader.h"

#if defined(HAVE_LIBTIFF)
# include "file_per_sample_reader.h"
# include "image_frame.h"
# include "video_track.h"
# include "file_sequence.h"
# include <base/contract.h>
# include <base/scope_guard.h>
# include <base/string_util.h>
# include <base/file_system.h>
# include <base/env.h>

# include <tiffio.h>
# include <tiffio.hxx>

////////////////////////////////////////

namespace media
{

////////////////////////////////////////

namespace
{

static void tiff_warning_handler( const char *p, const char *fmt, va_list args )
{
}

static void tiff_error_handler( const char *p, const char *fmt, va_list args )
{
}

static void tiff_warning_handler_ext( thandle_t sp, const char *p, const char *fmt, va_list args )
{
}

static void tiff_error_handler_ext( thandle_t sp, const char *p, const char *fmt, va_list args )
{
}


////////////////////////////////////////

class tiff_read_track : public video_track
{
public:
	tiff_read_track( int64_t b, int64_t e, file_sequence &&fseq )
			: video_track( "<image>", b, e, sample_rate( 24, 1 ),
						   media::track_description( media::TRACK_VIDEO ) ),
			  _files( std::move( fseq ) )
	{}

	virtual image_frame *doRead( int64_t f );

	virtual void doWrite( int64_t , const image_frame & )
	{
		throw_logic( "reader asked to write a frame" );
	}

private:
	file_sequence _files;
};

////////////////////////////////////////

image_frame *
tiff_read_track::doRead( int64_t f )
{
	std::unique_ptr<image_frame> ret;

	base::uri u = _files.get_frame( f );
	std::string p = u.full_path();
	auto fs = base::file_system::get( u );
	base::istream stream = fs->open_read( u );

	auto terr = TIFFSetErrorHandler( tiff_error_handler );
	auto twarn = TIFFSetWarningHandler( tiff_warning_handler );
	auto terre = TIFFSetErrorHandlerExt( tiff_error_handler_ext );
	auto twarne = TIFFSetWarningHandlerExt( tiff_warning_handler_ext );
	on_scope_exit
	{
		TIFFSetErrorHandler( terr );
		TIFFSetWarningHandler( twarn );
		TIFFSetErrorHandlerExt( terre );
		TIFFSetWarningHandlerExt( twarne );
	};

	// We're just reading the header, so don't open for mapping
	TIFF *t = TIFFStreamOpen( p.c_str(), &stream );
	if ( t )
	{
		on_scope_exit{ TIFFFlushData( t ); TIFFClose( t ); };
		uint16_t bps, sf;
		float offX, offY;
		uint32_t sizeY, sizeX;

		TIFFGetFieldDefaulted( t, TIFFTAG_BITSPERSAMPLE, &bps );
		if ( ! TIFFGetField( t, TIFFTAG_SAMPLEFORMAT, &sf ) )
			sf = SAMPLEFORMAT_VOID;

		if ( ! TIFFGetField( t, TIFFTAG_XPOSITION, &offX ) )
			offX = 0.F;
		if ( ! TIFFGetField( t, TIFFTAG_YPOSITION, &offY ) )
			offY = 0.F;
		// Need to multiply offX by resolution.

		TIFFGetFieldDefaulted( t, TIFFTAG_IMAGEWIDTH, &sizeX );
		TIFFGetFieldDefaulted( t, TIFFTAG_IMAGELENGTH, &sizeY );

		uint16_t photo = 0;
		TIFFGetFieldDefaulted( t, TIFFTAG_PHOTOMETRIC, &photo );

		uint16_t samps = 0;
		if ( PHOTOMETRIC_PALETTE == photo )
		{
			// color, 3 planes
			samps = 3;
		}
		else
		{
			TIFFGetFieldDefaulted( t, TIFFTAG_SAMPLESPERPIXEL, &samps );
		}

		if ( sf == SAMPLEFORMAT_COMPLEXINT || sf == SAMPLEFORMAT_COMPLEXIEEEFP )
			throw_not_yet();

		uint16_t orient = 0;
		TIFFGetFieldDefaulted( t, TIFFTAG_ORIENTATION, &orient );
		switch ( orient )
		{
			default:
				
				break;
		}

		uint16_t isP = 0;
		TIFFGetFieldDefaulted( t, TIFFTAG_PLANARCONFIG, &isP );

		const char *channames[] = { "R", "G", "B", "A" };
		int64_t w = static_cast<int64_t>( sizeX );
		int64_t h = static_cast<int64_t>( sizeY );
		ret.reset( new image_frame( w, h ) );
		for ( uint16_t s = 0; s < samps; ++s )
		{
			image_buffer imgbuf;
			switch ( bps )
			{
				case 1:
					// don't handle 1-bit yet
					throw_not_yet();
					break;
				case 8:
					if ( sf == SAMPLEFORMAT_INT )
						imgbuf = image_buffer::simple_buffer<int8_t>( w, h );
					else
						imgbuf = image_buffer::simple_buffer<uint8_t>( w, h );
					break;
				case 16:
					std::cout << "initializing " << w << 'x' << h << " 16-bit image" << std::endl;
					if ( sf == SAMPLEFORMAT_INT )
						imgbuf = image_buffer::simple_buffer<int16_t>( w, h );
					else
						imgbuf = image_buffer::simple_buffer<uint16_t>( w, h );
					break;
				case 32:
					if ( sf == SAMPLEFORMAT_UINT )
						imgbuf = image_buffer::simple_buffer<uint32_t>( w, h );
					else if ( sf == SAMPLEFORMAT_INT )
						imgbuf = image_buffer::simple_buffer<int32_t>( w, h );
					else // void or ieeefp
						imgbuf = image_buffer::simple_buffer<float>( w, h );
					break;
				case 64:
					if ( sf == SAMPLEFORMAT_UINT )
						imgbuf = image_buffer::simple_buffer<uint64_t>( w, h );
					else if ( sf == SAMPLEFORMAT_INT )
						imgbuf = image_buffer::simple_buffer<int64_t>( w, h );
					else // void or ieeefp
						imgbuf = image_buffer::simple_buffer<double>( w, h );
					break;
				default:
					
					break;
			}

			if ( s < 4 )
				ret->add_channel( channames[s], imgbuf );
			else
				ret->add_channel( imgbuf );
		}

		if ( TIFFIsTiled( t ) )
		{
			uint32_t tileWidth = sizeX, tileLength = sizeY;
			
			TIFFGetField( t, TIFFTAG_TILEWIDTH, &tileWidth );
			TIFFGetField( t, TIFFTAG_TILELENGTH, &tileLength );

			throw_not_yet();
			if ( PLANARCONFIG_SEPARATE == isP )
			{
			}
			else
			{
			}
		}
		else
		{
			tmsize_t ssize = TIFFScanlineSize( t );
			if ( PLANARCONFIG_SEPARATE == isP )
			{
				std::cout << "reading planar TIFF straight to image" << std::endl;
				// need to read it in order of samples to prevent random access
				for ( uint16_t s = 0; s < samps; ++s )
				{
					uint8_t *dptr = reinterpret_cast<uint8_t *>( ret->at( s ).data() );
					precondition( ret->at( s ).ystride_bytes() == ssize, "invalid line stride assumption about scanline size" );
					for ( uint32_t row = 0; row < sizeY; ++row )
					{
						TIFFReadScanline( t, dptr, row, s );
						dptr += ssize;
					}
				}
			}
			else
			{
				std::unique_ptr<uint8_t[]> tmpbuf = std::unique_ptr<uint8_t[]>( new uint8_t[TIFFScanlineSize( t )] );
				std::cout << "reading interleaved TIFF to temporary buffer then copying to image" << std::endl;
				for ( uint32_t row = 0; row < sizeY; ++row )
				{
					TIFFReadScanline( t, tmpbuf.get(), row );
					for ( uint16_t s = 0; s < samps; ++s )
					{
						image_buffer &ib = ret->at( s );
						if ( row == 0 )
						{
							std::cout << " ---> ib " << s << " xstride " << ib.xstride_bytes() << " ystride " << ib.ystride_bytes() << std::endl;
						}
						uint8_t *dptr = reinterpret_cast<uint8_t *>( ib.data() );
						dptr += ib.ystride_bytes() * row;
						const uint8_t *sptr = tmpbuf.get();
						switch ( ib.bits() )
						{
							case 8:
								for ( uint32_t x = 0; x < sizeX; ++x )
									dptr[x] = sptr[x*samps + s];
								break;
							case 16:
							{
								uint16_t *dst16 = reinterpret_cast<uint16_t *>( dptr );
								const uint16_t *src16 = reinterpret_cast<const uint16_t *>( sptr );
								for ( uint32_t x = 0; x < sizeX; ++x )
									dst16[x] = src16[x*samps + s];
								break;
							}
							case 32:
							{
								uint32_t *dst32 = reinterpret_cast<uint32_t *>( dptr );
								const uint32_t *src32 = reinterpret_cast<const uint32_t *>( sptr );
								for ( uint32_t x = 0; x < sizeX; ++x )
									dst32[x] = src32[x*samps + s];
								break;
							}
							case 64:
							{
								uint64_t *dst64 = reinterpret_cast<uint64_t *>( dptr );
								const uint64_t *src64 = reinterpret_cast<const uint64_t *>( sptr );
								for ( uint32_t x = 0; x < sizeX; ++x )
									dst64[x] = src64[x*samps + s];
								break;
							}
							default:
								throw_not_yet();
						}
					}
				}
			}
		}

	}
	return ret.release();
}

////////////////////////////////////////

class TIFFReader : public file_per_sample_reader
{
public:
	TIFFReader( void )
			: file_per_sample_reader( "TIFF" )
	{
		_description = "TIFF Reader";
		_extensions.emplace_back( "tif" );
		_extensions.emplace_back( "tiff" );

		std::vector<uint8_t> magicBE{0x4D, 0x4D};
		_magics.emplace_back( std::move( magicBE ) );
		std::vector<uint8_t> magicLE{0x49, 0x49};
		_magics.emplace_back( std::move( magicLE ) );

		// MDI
		std::vector<uint8_t> magicMDILE{0x50, 0x45};
		_magics.emplace_back( std::move( magicMDILE ) );
		std::vector<uint8_t> magicMDIBE{0x45, 0x50};
		_magics.emplace_back( std::move( magicMDIBE ) );
	}
	virtual ~TIFFReader( void ) = default;

	virtual container create( const base::uri &u, const metadata &params );
};

container
TIFFReader::create( const base::uri &u, const metadata & )
{
	container result;

	int64_t start, last;
	file_sequence fseq( u );
	auto fs = scan_samples( start, last, fseq );

	result.add_track( std::make_shared<tiff_read_track>( start, last, std::move( fseq ) ) );

	return result;
}

}
#endif // HAVE_LIBTIFF

////////////////////////////////////////

void register_tiff_reader( void )
{
#if defined(HAVE_LIBTIFF)
	reader::register_reader( std::make_shared<TIFFReader>() );
#endif
}

} // namespace media

