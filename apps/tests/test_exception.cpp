
#include <base/contract.h>
#include <fstream>

namespace
{

void open_file( const std::string &name )
{
	try
	{
		std::ifstream file( name );
		file.exceptions( std::ios_base::failbit );
	}
	catch ( ... )
	{
		throw_add_location( "couldn't open file \"{0}\"", name );
	}
}

void tryit( void )
{
	try
	{
		open_file( "doesnt_exist" );
	}
	catch ( ... )
	{
		throw_add( "tryit() failed" );
	}
}


int safemain( void )
{
	try
	{
		tryit();
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}

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
