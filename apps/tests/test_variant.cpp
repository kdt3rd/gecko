
#include <base/contract.h>
#include <base/variant.h>
#include <iostream>

namespace
{

class visitor
{
public:
	void operator()( int x )
	{
		std::cout << "Int: " << x << std::endl;
	}

	void operator()( float x )
	{
		std::cout << "Float: " << x << std::endl;
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	void operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for desired type" );
	}
};

int safemain( void )
{
	using variant = base::variant<int,float>;

	variant x;
	visitor v;

	x.set<int>( 42 );
	variant::visit( v, x );

	x.set<float>( 3.141592 );
	variant::visit( v, x );

	return 1;
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
