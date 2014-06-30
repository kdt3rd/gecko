
#include <base/contract.h>
#include <script/cocoa/font_manager.h>

namespace
{

int safemain( int argc, char **argv )
{
	auto fontmgr = script::cocoa::font_manager();

	auto style = fontmgr.get_families();
	for ( auto fam: style )
		std::cout << fam << std::endl;

	return 0;
}

}

////////////////////////////////////////

int main( int argc, char *argv[] )
{
	int ret = -1;
	try
	{
		ret = safemain( argc, argv );
	}
	catch ( std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
	return ret;
}

////////////////////////////////////////

