
#include "data_writer.h"
#include "data_type.h"
#include <exception>

////////////////////////////////////////

data_writer::data_writer( std::string f )
	: _hash( new Botan::SHA_256 ), _filename( std::move( f ) ), _file( std::make_shared<std::ofstream>(_filename.c_str() ) )
{
}

////////////////////////////////////////

data_writer::data_writer( data_writer &&other )
	: _hash( std::move( other._hash ) ), _filename( std::move( other._filename ) ), _file( std::move( other._file ) )
{
}

////////////////////////////////////////

data_writer::~data_writer( void )
{
}

////////////////////////////////////////

void
data_writer::write_null( void )
{
	update( (Botan::byte*)(&data_type::null), 1 );
	*_file << data_type::null;
}

////////////////////////////////////////

void
data_writer::write( bool v )
{
	if ( v )
	{
		update( &data_type::yes, 1 );
		*_file << data_type::yes;
	}
	else
	{
		update( &data_type::no, 1 );
		*_file << data_type::no;
	}
}

////////////////////////////////////////

void data_writer::write( char v )
{
	*_file << data_type::byte;
	_file->write( (char *)&v, sizeof(v) );
	update( &data_type::byte, 1 );
	update( &v, sizeof(v) );
}

////////////////////////////////////////

void data_writer::write( int8_t v )
{
	*_file << data_type::int8;
	_file->write( (char *)&v, sizeof(v) );
	update( &data_type::int8, 1 );
	update( &v, sizeof(v) );
}

////////////////////////////////////////

void data_writer::write( uint8_t v )
{
	*_file << data_type::uint8;
	_file->write( (char *)&v, sizeof(v) );
	update( &data_type::uint8, 1 );
	update( &v, sizeof(v) );
}

////////////////////////////////////////

void
data_writer::write( int16_t v )
{
	uint8_t data[2];
	data[0] = ( v >> 8 ) & 0xFF;
	data[1] = ( v >> 0 ) & 0xFF;
	*_file << data_type::int16;
	_file->write( (char *)data, 2 );
	update( &data_type::int16, 1 );
	update( data, 2 );
}

////////////////////////////////////////

void
data_writer::write( uint16_t v )
{
	uint8_t data[2];
	data[0] = ( v >> 8 ) & 0xFF;
	data[1] = ( v >> 0 ) & 0xFF;
	*_file << data_type::uint16;
	_file->write( (char *)data, 2 );
	update( &data_type::uint16, 1 );
	update( data, 2 );
}

////////////////////////////////////////

void
data_writer::write( int32_t v )
{
	uint8_t data[4];
	data[0] = ( v >> 24 ) & 0xFF;
	data[1] = ( v >> 16 ) & 0xFF;
	data[2] = ( v >>  8 ) & 0xFF;
	data[3] = ( v >>  0 ) & 0xFF;
	*_file << data_type::int32;
	_file->write( (char *)data, 4 );
	update( &data_type::int32, 1 );
	update( data, 4 );
}

////////////////////////////////////////

void
data_writer::write( uint32_t v )
{
	*_file << data_type::uint32;
	update( &data_type::uint32, 1 );
	write32( v );
}

////////////////////////////////////////

void
data_writer::write( int64_t v )
{
	uint8_t data[8];
	data[0] = ( v >> 56 ) & 0xFF;
	data[1] = ( v >> 48 ) & 0xFF;
	data[2] = ( v >> 40 ) & 0xFF;
	data[3] = ( v >> 32 ) & 0xFF;
	data[4] = ( v >> 24 ) & 0xFF;
	data[5] = ( v >> 16 ) & 0xFF;
	data[6] = ( v >>  8 ) & 0xFF;
	data[7] = ( v >>  0 ) & 0xFF;
	*_file << data_type::int64;
	_file->write( (char *)data, 8 );
	update( &data_type::int64, 1 );
	update( data, 8 );
}

////////////////////////////////////////

void
data_writer::write( uint64_t v )
{
	uint8_t data[8];
	data[0] = ( v >> 56 ) & 0xFF;
	data[1] = ( v >> 48 ) & 0xFF;
	data[2] = ( v >> 40 ) & 0xFF;
	data[3] = ( v >> 32 ) & 0xFF;
	data[4] = ( v >> 24 ) & 0xFF;
	data[5] = ( v >> 16 ) & 0xFF;
	data[6] = ( v >>  8 ) & 0xFF;
	data[7] = ( v >>  0 ) & 0xFF;
	*_file << data_type::uint64;
	_file->write( (char *)data, 8 );
	update( &data_type::uint64, 1 );
	update( data, 8 );
}

////////////////////////////////////////

void
data_writer::write( float f )
{
	uint32_t &v = *( reinterpret_cast<uint32_t*>( &f ) );
	uint8_t data[4];
	data[0] = ( v >> 24 ) & 0xFF;
	data[1] = ( v >> 16 ) & 0xFF;
	data[2] = ( v >>  8 ) & 0xFF;
	data[3] = ( v >>  0 ) & 0xFF;
	*_file << data_type::float32;
	_file->write( (char *)data, 4 );
	update( &data_type::float32, 1 );
	update( data, 4 );
}

////////////////////////////////////////

void
data_writer::write( double f )
{
	uint64_t &v = *( reinterpret_cast<uint64_t*>( &f ) );
	uint8_t data[8];
	data[0] = ( v >> 56 ) & 0xFF;
	data[1] = ( v >> 48 ) & 0xFF;
	data[2] = ( v >> 40 ) & 0xFF;
	data[3] = ( v >> 32 ) & 0xFF;
	data[4] = ( v >> 24 ) & 0xFF;
	data[5] = ( v >> 16 ) & 0xFF;
	data[6] = ( v >>  8 ) & 0xFF;
	data[7] = ( v >>  0 ) & 0xFF;
	*_file << data_type::float64;
	_file->write( (char *)data, 8 );
	update( &data_type::float64, 1 );
	update( data, 8 );
}

////////////////////////////////////////

void
data_writer::write( const char *v )
{
	size_t size = strlen( v );
	if ( size < 255 )
	{
		*_file << data_type::string8;
		uint8_t s = uint8_t( size );
		_file->write( (char *)&s, sizeof(s) );
		update( &data_type::string8, 1 );
		update( &s, 1 );
	}
	else
	{
		*_file << data_type::string32;
		update( &data_type::string32, 1 );
		write32( uint32_t( size ) );
	}
	_file->write( v, size );
	update( v, size );
}

////////////////////////////////////////

void
data_writer::write( const std::string &v )
{
	if ( v.size() < 255 )
	{
		*_file << data_type::string8;
		uint8_t s = uint8_t( v.size() );
		_file->write( (char *)&s, sizeof(s) );
		update( &data_type::string8, 1 );
		update( &s, 1 );
	}
	else
	{
		*_file << data_type::string32;
		update( &data_type::string32, 1 );
		write32( uint32_t( v.size() ) );
	}
	_file->write( v.c_str(), v.size() );
	update( v.c_str(), v.size() );
}

////////////////////////////////////////

void
data_writer::write_noop( void )
{
	*_file << data_type::noop;
	update( &data_type::noop, 1 );
}

////////////////////////////////////////

void
data_writer::write_object( size_t size )
{
	if ( size < 255 )
	{
		*_file << data_type::object8;
		uint8_t s = uint8_t( size );
		_file->write( (char *)&s, sizeof(s) );
		update( &data_type::object8, 1 );
		update( &s, 1 );
	}
	else
	{
		*_file << data_type::object32;
		update( &data_type::object32, 1 );
		write32( uint32_t( size ) );
	}
}

////////////////////////////////////////

void
data_writer::write_array( size_t size )
{
	if ( size < 255 )
	{
		*_file << data_type::array8;
		uint8_t s = uint8_t( size );
		_file->write( (char *)&s, sizeof(s) );
		update( &data_type::array8, 1 );
		update( &s, 1 );
	}
	else
	{
		*_file << data_type::array32;
		update( &data_type::array32, 1 );
		write32( uint32_t( size ) );
	}
}

////////////////////////////////////////

void data_writer::write_object( void )
{
	*_file << data_type::object8;
	uint8_t s = 255;
	_file->write( (char *)&s, sizeof(s) );
	update( &data_type::object8, 1 );
	update( &s, 1 );
}

////////////////////////////////////////

void data_writer::write_array( void )
{
	*_file << data_type::array8;
	uint8_t s = 255;
	_file->write( (char *)&s, sizeof(s) );
	update( &data_type::array8, 1 );
	update( &s, 1 );
}

////////////////////////////////////////

void data_writer::write_end( void )
{
	*_file << data_type::end;
	update( &data_type::end, 1 );
}

////////////////////////////////////////

hash256 data_writer::close( void )
{
	return hash256( *_hash );
}

////////////////////////////////////////

void data_writer::write32( uint32_t s )
{
	uint8_t data[4];
	data[0] = ( s >> 24 ) & 0xFF;
	data[1] = ( s >> 16 ) & 0xFF;
	data[2] = ( s >>  8 ) & 0xFF;
	data[3] = ( s >>  0 ) & 0xFF;
	_file->write( (char *)data, 4 );
	update( data, 4 );
}

////////////////////////////////////////

void data_writer::update( const void *data, size_t size )
{
	_hash->update( (Botan::byte *)(data), size );
}

////////////////////////////////////////

