
#pragma once

#include <type_traits>

namespace core
{

////////////////////////////////////////

template<typename T>
class data_resource
{
public:
	typedef typename std::decay<T>::type D;

	data_resource( D data )
		: _data( data )
	{
	}

	std::string operator()( const std::string &filename )
	{
		D d = _data;
		while ( d->name != nullptr )
		{
			if ( filename == d->name )
				return std::string( d->start, d->end );
			++d;
		}
		throw std::runtime_error( "resource file not found" );
	}


private:
	const D _data;
};

////////////////////////////////////////

template<typename T>
data_resource<T> resource( T &data )
{
	return data_resource<T>( data );
}

////////////////////////////////////////

}

