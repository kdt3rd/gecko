
#include <base/contract.h>
#include <base/variant.h>
#include <iostream>
#include <base/tuple_util.h>

namespace
{

class visitor
{
public:
	bool operator()( int x )
	{
		std::cout << "Int: " << x << std::endl;
		return true;
	}

	bool operator()( float x )
	{
		std::cout << "Float: " << x << std::endl;
		return false;
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	void operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
	}
};

class visitor2
{
public:
	void operator()( int x, int y )
	{
		std::cout << "Int: " << x << ' ' << y << std::endl;
	}

	void operator()( float x, float y )
	{
		std::cout << "Float: " << x << ' ' << y << std::endl;
	}

	void operator()( int x, float y )
	{
		std::cout << "Int/Float: " << x << ' ' << y << std::endl;
	}

	void operator()( float x, int y )
	{
		std::cout << "Float/Int: " << x << ' ' << y << std::endl;
	}

	// Compile-time check for any missing operator() implementation
	template<typename T1, typename T2>
	void operator()( T1 a, T2 b )
	{
		static_assert( base::always_false<T1,T2>::value, "missing operator() for variant types" );
	}
};


int safemain( void )
{
	using variant = base::variant<int,float>;

	variant x;
	visitor v;

	x.set<int>( 42 );
	std::cout << "int = " << base::visit<bool>( v, x ) << std::endl;

	x.set<float>( 3.141592 );
	std::cout << "float = " << base::visit<bool>( v, x ) << std::endl;

	variant y;

	visitor2 v2;
	y.set<int>( 12 );
	base::visit<void>( v2, x, y );

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
