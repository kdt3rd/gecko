
#pragma once

#include <botan/sha2_32.h>

////////////////////////////////////////

class hash256
{
public:
	hash256( void );
	hash256( const hash256 &other );
	hash256( const Botan::byte *other );
	hash256( Botan::SHA_256 &sha );

	const Botan::byte *hash( void ) { return _hash; }
	std::string string( void ) const;

private:
	Botan::byte _hash[32];
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const hash256 &h )
{
	out << h.string();
	return out;
}

////////////////////////////////////////

