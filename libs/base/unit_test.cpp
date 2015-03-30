
#include "unit_test.h"
#include "ansi.h"
#include "contract.h"

namespace base
{

////////////////////////////////////////

unit_test::unit_test( const std::string &n, const std::function<void(void)> &t )
	: _name( n ), _test( t )
{
	auto i = _tests.insert( std::make_pair( n, this ) );
	if ( !i.second )
		throw_runtime( "duplicate unit_test named '{0}'", n );
}

////////////////////////////////////////

unit_test::~unit_test( void )
{
	_tests.erase( _name );
}

////////////////////////////////////////

void unit_test::test( void ) const
{
	try
	{
		_test();
		std::clog << ansi::green << "SUCCESS " << ansi::reset << _name << std::endl;
	}
	catch ( std::exception &e )
	{
		std::clog << ansi::red << "FAILURE " << ansi::reset << _name << std::endl;
		print_exception( std::clog, e, 1 );
	}
}

////////////////////////////////////////

void unit_test::run( void )
{
	for ( auto &t: _tests )
		t.second->test();
}

////////////////////////////////////////

void unit_test::run( const std::string &n )
{
	_tests.at( n )->test();
}

////////////////////////////////////////

}

