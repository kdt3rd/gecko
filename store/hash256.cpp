
#include "hash256.h"
#include <cstring>

////////////////////////////////////////

hash256::hash256( void )
{
}

////////////////////////////////////////

hash256::hash256( const hash256 &other )
{
	std::memcpy( _hash, other._hash, 32 );
}

////////////////////////////////////////

hash256::hash256( const Botan::byte *other )
{
	std::memcpy( _hash, other, 32 );
}

////////////////////////////////////////

hash256::hash256( Botan::SHA_256 &sha )
{
	sha.final( _hash );
}

////////////////////////////////////////

std::string hash256::string( void ) const
{
	std::string tmp;
	tmp.resize( 64 );
	for ( size_t i = 0; i < 32; ++i )
		sprintf( &tmp[i*2], "%02X", _hash[i] );
	return tmp;
}


////////////////////////////////////////

