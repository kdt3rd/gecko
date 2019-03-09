// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT


#include <imgproc/expr.h>
#include <imgproc/environment.h>
#include <imgproc/function.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	using namespace imgproc;

	auto x = std::make_shared<expr>( identifier_expr( U"x" ) );
	auto y = std::make_shared<expr>( identifier_expr( U"y" ) );
	auto e = std::make_shared<expr>( infix_expr( U"+", x, y ) );

	function f( U"add", U"x", U"y" );
	f.set_result( e );
//	auto args = { type_primary( pod_type::FLOAT32 ), type_primary( pod_type::FLOAT32 ) };
//	std::cout << e << " = " << infer( f, args )->get_type() << std::endl;

	/*
	environment env;
	env[U"true"] = make_type<bool>();
	env[U"false"] = make_type<bool>();
	env[U"x"] = make_type<float>();
	env[U"y"] = make_type<float>();

	auto tr = expr::make<identifier_expr>( U"true" );
	auto x = expr::make<identifier_expr>( U"x" );
	auto y = expr::make<identifier_expr>( U"y" );
	auto add = expr::make<infix_expr>( U"+", x, y );
	auto end = expr::make<if_expr>( tr, add, y );

	type t = infer( *end, env );

	std::cout << end << ": " << t << std::endl;
	*/

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
