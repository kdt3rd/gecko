
#include "data_reader.h"
#include "data_type.h"
#include <exception>

////////////////////////////////////////

data_reader::data_reader( const std::string &fn )
	: _file( std::make_shared<std::ifstream>( fn ) )
{
}

////////////////////////////////////////

data_reader::data_reader( data_reader &&other )
	: _file( other._file )
{
}

////////////////////////////////////////

data_reader::~data_reader( void )
{
}

////////////////////////////////////////

char
data_reader::next_type( void )
{
	return _file->peek();
}

////////////////////////////////////////

void
data_reader::read_null( void )
{
	char t = _file->get();
	if ( t != data_type::null )
		throw std::runtime_error( "expected type null" );
}

////////////////////////////////////////

void
data_reader::read( bool &v )
{
	char t = _file->get();
	switch ( t )
	{
		case data_type::yes:
			v = true;
			break;
		case data_type::no:
			v = false;
			break;
		default:
			throw std::runtime_error( "expected type boolean" );
	}
}

////////////////////////////////////////

void data_reader::read( int8_t &v )
{
	char t = _file->get();
	if ( t != data_type::int8 )
		throw std::runtime_error( "expected type int8" );
	_file->read( (char *)&v, sizeof(v) );
}

////////////////////////////////////////

void data_reader::read( uint8_t &v )
{
	char t = _file->get();
	if ( t != data_type::uint8 )
		throw std::runtime_error( "expected type uint8" );
	_file->read( (char *)&v, sizeof(v) );
}

////////////////////////////////////////

void
data_reader::read( int16_t &v )
{
	char t = _file->get();
	if ( t != data_type::int16 )
		throw std::runtime_error( "expected type int16" );
	uint8_t data[sizeof(v)];
	_file->read( (char *)data, sizeof(v) );
	v = ( data[0] << 8 ) + data[1];
}

////////////////////////////////////////

void
data_reader::read( uint16_t &v )
{
	char t = _file->get();
	if ( t != data_type::int16 )
		throw std::runtime_error( "expected type uint16" );
	uint8_t data[sizeof(v)];
	_file->read( (char *)data, sizeof(v) );
	v = ( data[0] << 8 ) + data[1];
}

////////////////////////////////////////

void
data_reader::read( int32_t &v )
{
	char t = _file->get();
	if ( t != data_type::int32 )
		throw std::runtime_error( "expected type int32" );
	uint8_t data[sizeof(v)];
	_file->read( (char *)data, sizeof(v) );
	v = ( data[0] << 24 ) + ( data[1] << 16 ) + ( data[2] << 8 ) + data[3];
}

////////////////////////////////////////

void
data_reader::read( uint32_t &v )
{
	char t = _file->get();
	if ( t != data_type::uint32 )
		throw std::runtime_error( "expected type uint32" );
	v = read32();
}

////////////////////////////////////////

void
data_reader::read( int64_t &v )
{
	char t = _file->get();
	if ( t != data_type::int64 )
		throw std::runtime_error( "expected type int64" );
	uint8_t data[sizeof(v)];
	_file->read( (char *)data, sizeof(v) );
	v = ( uint64_t(data[0]) << 56 ) + ( uint64_t(data[1]) << 48 ) + ( uint64_t(data[2]) << 40 ) + ( uint64_t(data[3]) << 32 ) + ( uint64_t(data[4]) << 24 ) + ( uint64_t(data[5]) << 16 ) + ( uint64_t(data[6]) << 8 ) + uint64_t(data[7]);
}

////////////////////////////////////////

void
data_reader::read( uint64_t &v )
{
	char t = _file->get();
	if ( t != data_type::uint64 )
		throw std::runtime_error( "expected type uint64" );
	uint8_t data[sizeof(v)];
	_file->read( (char *)data, sizeof(v) );
	v = ( uint64_t(data[0]) << 56 ) + ( uint64_t(data[1]) << 48 ) + ( uint64_t(data[2]) << 40 ) + ( uint64_t(data[3]) << 32 ) + ( uint64_t(data[4]) << 24 ) + ( uint64_t(data[5]) << 16 ) + ( uint64_t(data[6]) << 8 ) + uint64_t(data[7]);
}

////////////////////////////////////////

void
data_reader::read( float &f )
{
	char t = _file->get();
	if ( t != data_type::float32 )
		throw std::runtime_error( "expected type float32" );
	uint8_t data[sizeof(f)];
	_file->read( (char *)data, sizeof(f) );
	union {
		uint32_t i;
		float f;
	} tmp;
	tmp.i = ( data[0] << 24 ) + ( data[1] << 16 ) + ( data[2] << 8 ) + data[3];
	f = tmp.f;
}

////////////////////////////////////////

void
data_reader::read( double &f )
{
	char t = _file->get();
	if ( t != data_type::float64 )
		throw std::runtime_error( "expected type uint64" );
	uint8_t data[sizeof(f)];
	_file->read( (char *)data, sizeof(f) );
	union {
		uint64_t i;
		double f;
	} tmp;
	tmp.i = ( uint64_t(data[0]) << 56 ) + ( uint64_t(data[1]) << 48 ) + ( uint64_t(data[2]) << 40 ) + ( uint64_t(data[3]) << 32 ) + ( uint64_t(data[4]) << 24 ) + ( uint64_t(data[5]) << 16 ) + ( uint64_t(data[6]) << 8 ) + uint64_t(data[7]);
	f = tmp.f;
}

////////////////////////////////////////

void
data_reader::read( std::string &v )
{
	char t = _file->get();
	uint32_t size = 0;
	switch ( t )
	{
		case data_type::string8:
		{
			uint8_t s = _file->get();
			size = s;
			break;
		}

		case data_type::string32:
			size = read32();
			break;

		default:
			throw std::runtime_error( "expected type string" );
	}

	v.resize( size );
	_file->read( const_cast<char *>( v.data() ), size );
}

////////////////////////////////////////

void
data_reader::read_noop( void )
{
	char t = _file->get();
	if ( t != data_type::noop )
		throw std::runtime_error( "expecte type noop" );
}

////////////////////////////////////////

size_t
data_reader::read_object( void )
{
	char t = _file->get();
	uint32_t size = 0;
	switch ( t )
	{
		case data_type::object8:
		{
			uint8_t s = _file->get();
			size = s;
			if ( size == 255 )
				size = std::numeric_limits<size_t>::max();
			break;
		}

		case data_type::object32:
			size = read32();
			break;

		default:
			throw std::runtime_error( "expecte type object" );
	}

	return size;
}

////////////////////////////////////////

size_t
data_reader::read_array( void )
{
	char t = _file->get();
	uint32_t size = 0;
	switch ( t )
	{
		case data_type::array8:
		{
			uint8_t s = _file->get();
			size = s;
			if ( size == 255 )
				size = std::numeric_limits<size_t>::max();
			break;
		}

		case data_type::array32:
			size = read32();
			break;

		default:
			throw std::runtime_error( "expecte type array" );
	}

	return size;
}

////////////////////////////////////////

void data_reader::read_end( void )
{
	char t = _file->get();
	if ( t != data_type::end )
		throw std::runtime_error( "expecte type end" );
}

////////////////////////////////////////

uint32_t data_reader::read32( void )
{
	uint8_t data[4];
	_file->read( (char *)data, 4 );
	return ( data[0] << 24 ) + ( data[1] << 16 ) + ( data[2] << 8 ) + data[3];
}

////////////////////////////////////////

