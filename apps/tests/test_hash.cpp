
#include <base/sha256.h>
#include <base/contract.h>
#include <sstream>
#include <iostream>

namespace
{

void check( const std::string &msg )
{
	base::sha256 hash;
	hash( msg.c_str(), msg.size() );
	std::cout << "Message: \"" << msg << "\"\n  " << hash.hash_string() << std::endl;
}

int safemain( void )
{
	check( "" );
	check( "abc" );
	check( "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu" );
	check( "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" );
	return 0;
}

}

int main( void )
{
	try
	{
		return safemain();
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
