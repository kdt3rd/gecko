
#include <base/sha256.h>
#include <base/sha160.h>
#include <base/base64.h>
#include <base/contract.h>
#include <sstream>
#include <iostream>

namespace
{

template<typename HASH>
void check( const std::string &msg, const std::string &result )
{
	HASH h;
	h( msg.c_str(), msg.size() );
	std::string test = h.hash_string();

	if ( test == result )
		std::cout << base::ansi::green << "PASSED " << base::ansi::reset << test << std::endl;
	else
		std::cout << base::ansi::red << "FAILED " << base::ansi::reset << test << std::endl;
}

void base64( const std::string &msg, const std::string &result )
{
	std::string enc = base::base64_encode( msg );
	std::string back = base::base64_decode( enc );
	if ( enc == result && back == msg )
		std::cout << base::ansi::green << "PASSED " << base::ansi::reset << enc << std::endl;
	else
		std::cout << base::ansi::red << "FAILED " << base::ansi::reset << enc << ' ' << back << std::endl;
}

int safemain( void )
{
	check<base::sha160>( "",
		"da39a3ee5e6b4b0d3255bfef95601890afd80709" );
	check<base::sha160>( "abc",
		"a9993e364706816aba3e25717850c26c9cd0d89d" );
	check<base::sha160>( "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		"84983e441c3bd26ebaae4aa1f95129e5e54670f1" );
	check<base::sha160>( "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		"a49b2446a02c645bf419f995b67091253a04a259" );

	check<base::sha256>( "",
		"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" );
	check<base::sha256>( "abc",
		"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" );
	check<base::sha256>( "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1" );
	check<base::sha256>( "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		"cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1" );

	base64( "",       "" );
	base64( "f",      "Zg==" );
	base64( "fo",     "Zm8=" );
	base64( "foo",    "Zm9v" );
	base64( "foob",   "Zm9vYg==" );
	base64( "fooba",  "Zm9vYmE=" );
	base64( "foobar", "Zm9vYmFy" );

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
