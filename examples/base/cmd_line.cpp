// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT

#include <base/cmd_line.h>
#include <iostream>

void example( int argc, char *argv[] )
{
    base::cmd_line options(
        argv[0], // The program name (for help messages)
        // Example: --file test.txt
        base::cmd_line::option(
            0,
            "file",
            "<file>",
            base::cmd_line::arg<1>,
            "File to show",
            false ),

        // Example: -l log.txt
        base::cmd_line::option(
            'l', "", "[<log>]", base::cmd_line::arg<0, 1>, "Log files", false ),

        // Example: -v -v -v
        base::cmd_line::option(
            'v',
            "",
            "",
            base::cmd_line::counted,
            "Verbose (can be specified many times)",
            false ),

        // Example --range 0 100
        base::cmd_line::option(
            'r',
            "range",
            "<start> <end>",
            base::cmd_line::arg<2>,
            "Frame range",
            false ),

        // A single argument listed after any option.
        base::cmd_line::option(
            0, "", "<arg>", base::cmd_line::arg<1>, "Test file", true ),

        // This will catch any extra arguments at the end.
        base::cmd_line::option(
            0,
            "",
            "<file> ...",
            base::cmd_line::args,
            "List of files to show",
            true ) );

    // Add standard help option.
    options.add_help();

    // Parse the arguments
    try
    {
        options.parse( argc, argv );
    }
    catch ( std::exception &e )
    {
        // Print a help message.
        std::cerr << options << std::endl;

        // Print the error.
        base::print_exception( std::cerr, e );

        // Finally exit.
        return;
    }

    // If help was specified
    if ( options["help"] )
    {
        // Print a help message.
        std::cerr << options << std::endl;
        return;
    }

    if ( auto &opt = options["file"] )
        std::cout << base::format( "file option given is {0}", opt.value() )
                  << std::endl;
    else
        std::cout << "no file option given" << std::endl;

    if ( auto &opt = options["l"] )
    {
        if ( opt.count() == 1 )
            test.success( "log option is '{0}'", opt[0] );
        else
            test.success( "log option is empty" );
    }
    else
        test.success( "log option was not specified" );

    test["verbose"] = [&]( void ) {
        if ( auto &opt = options["v"] )
            test.success( "verbose was specified {0} times", opt.count() );
        else
            test.success( "verbose was not specified" );
    };

    test["multi"] = [&]( void ) {
        if ( auto &opt = options["range"] )
            test.success( "range specified as {0} to {1}", opt[0], opt[1] );
        else
            test.success( "range not specified" );
    };

    test["required"] = [&]( void ) {
        if ( auto &opt = options["<arg>"] )
            test.success( "Argument is {0}", opt.value() );
        else
            test.failure( "Argument is missing" );
    };

    test["list"] = [&]( void ) {
        if ( auto &opt = options["<file> ..."] )
            test.success(
                "List is {0}", base::infix_separated( ", ", opt.values() ) );
        else
            test.failure( "List is missing" );
    };

    test.run();
    test.clean();
}
