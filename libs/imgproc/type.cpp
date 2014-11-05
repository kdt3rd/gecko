
#include "unifier.h"
#include <algorithm>

namespace
{

using namespace imgproc;

const std::type_index t_function( typeid( void(void) ) );
const std::type_index t_uint8( typeid( uint8_t ) );
const std::type_index t_int8( typeid( int8_t ) );
const std::type_index t_uint16( typeid( uint16_t ) );
const std::type_index t_int16( typeid( int16_t ) );
const std::type_index t_uint32( typeid( uint32_t ) );
const std::type_index t_int32( typeid( int32_t ) );
const std::type_index t_uint64( typeid( uint64_t ) );
const std::type_index t_int64( typeid( int64_t ) );
const std::type_index t_float32( typeid( float ) );
const std::type_index t_float64( typeid( double ) );
const std::type_index t_boolean( typeid( bool ) );

class printer
{
public:
	printer( std::ostream &out )
		: _out( out )
	{
	}

	void operator()( const type_variable &t )
	{
		_out << "var(" << t.id() << ')';
	}

	void operator()( const type_operator &t )
	{
		_out << "t(" << t.name() << ")";
		if ( !t.empty() )
		{
			_out << ": {";
			for ( const auto &st: t )
			{
				_out << ' ';
				visit( *this, st );
			}
			_out << " }";
		}
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	void operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
	}

private:
	std::ostream &_out;
};

}

////////////////////////////////////////

namespace imgproc
{

////////////////////////////////////////

std::string type_operator::name( void ) const
{
	std::stringstream tmp;
	tmp << _type;
	return tmp.str();
}

////////////////////////////////////////

void type_operator::add( type t )
{
	_types.emplace_back( std::move( t ) );
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const type &t )
{
	printer p( out );
	base::visit( p, t );
	return out;
}

////////////////////////////////////////

}

