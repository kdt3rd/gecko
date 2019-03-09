// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#include "parser.h"
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <base/file_path.h>
#include <base/streambuf.h>
#include <utf/utf.h>
#include <fstream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option( 'f', "function", "<function>", base::cmd_line::arg<1>, "Function name", false ),
		base::cmd_line::option( 'o', "output", "<output>", base::cmd_line::arg<1>, "Output file", false ),
		base::cmd_line::option(  0 , "",         "<html>",     base::cmd_line::arg<1>, "HTML template to process", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	std::string name = "default";
	if ( auto &opt = options["function"] )
		name = opt.value();

	std::string infile = options["<html>"].value();
	std::string outfile = base::remove_extension( infile ) + ".cpp";
	if ( auto &opt = options["output"] )
		outfile = opt.value();

	std::string func = base::basename( infile );
	if ( auto &opt = options["function"] )
		func = opt.value();

	if ( infile == outfile )
		throw_runtime( "cannot overwrite input file" );

	// Parsed information
	std::ifstream in( infile );
	parser p( func );
	p.parse( in );

	std::ofstream out( outfile );
	p.save( out );

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
