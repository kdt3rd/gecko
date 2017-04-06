//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <base/contract.h>
#include <base/format.h>
#include <iostream>
#include <sqlite/database.h>

namespace
{

int safemain( void )
{
	sqlite::database db( "/tmp/test.db" );
	db.exec( "CREATE TABLE IF NOT EXISTS test ( id INTEGER PRIMARY KEY, name VARCHAR(32) )" );
	db.exec( "DELETE FROM test" );

	{
		auto stmt = db.prepare( "INSERT INTO test ( name ) VALUES ( ? )" );
		stmt.exec( "Test1" );
		stmt.exec( "Test2" );
		stmt.exec( "Test3" );
	}

	{
		auto sel = db.prepare( "SELECT * FROM test" );
		while ( sel.step() )
			std::cout << sel.get<int64_t>( 0 ) << " = " << sel.get<std::string>( 1 ) << std::endl;
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
