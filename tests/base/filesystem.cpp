
#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <base/unit_test.h>
#include <base/posix_file_system.h>
#include <sstream>
#include <iostream>

#include <unistd.h>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0] );

	base::unit_test fstest( "fs" );
	fstest.setup( options );

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.add_help();
	options.parse( argc, argv );
	errhandler.dismiss();

	fstest["register"] = [&]( void )
	{
		try
		{
			base::file_system::add( "file", std::make_shared<base::posix_file_system>() );
			fstest.failure( "registration succeeded, should have already defined automatically" );
		}
		catch ( std::exception &e )
		{
			fstest.success( "registration correctly failed for pre-registered fs: {0}", e.what() );
		}
	};

	base::uri tmppath( "file", "", "tmp", "test_posix_fs" );
	base::uri file1( tmppath, "test1" );
	base::uri file2( tmppath, "test2" );
	base::uri badpath( tmppath, "extra/bad" );
	auto fs = base::file_system::get( tmppath );

	fstest["mkdir"] = [&]( void )
	{
		try
		{
			fs->mkdir( tmppath );
			fstest.success( "mkdir successful" );
		}
		catch ( std::exception &e )
		{
			fstest.failure( "mkdir failed ({0})", e.what() );
		}

		try
		{
			fs->mkdir( badpath );
			fstest.failure( "mkdir expected to fail but did not" );
		}
		catch ( std::exception &e )
		{
			fstest.success( "mkdir failed  as expected ({0})", e.what() );
		}
	};

	fstest["list"] = [&]( void )
	{
		fstest.run( "ostream" );
		fstest.run( "iostream" );
		fstest.message( "listing directory {0}", tmppath );
		auto dir = fs->readdir( tmppath );
		while ( ++dir )
			fstest.message( "entry {0}", *dir );
		fstest.success( "listed {0}", tmppath );
	};

	fstest["lstat"] = [&]( void )
	{
		struct stat statbuf = {};
		fs->lstat( tmppath, &statbuf );
		if ( S_ISDIR( statbuf.st_mode ) )
			fstest.success( "path {0} is a directory", tmppath );
		else
			throw_runtime( "{0} is not a directory per lstat", tmppath );
	};

	fstest["stat"] = [&]( void )
	{
		struct stat statbuf = {};
		fs->stat( tmppath, &statbuf );
		if ( S_ISDIR( statbuf.st_mode ) )
			fstest.success( "path {0} is a directory", tmppath );
		else
			throw_runtime( "{0} is not a directory per stat", tmppath );
	};

	fstest["statfs"] = [&]( void )
	{
		struct statvfs fsinfo;
		fs->statfs( tmppath, &fsinfo );
		fstest.success( "file system at {0} has {1} bytes free", tmppath, (fsinfo.f_bsize*fsinfo.f_bavail) );
	};

	fstest["ostream"] = [&]( void )
	{
		fstest.run( "mkdir" );
		base::ostream testoutput = fs->open_write( file1 );
		testoutput << "Hello, world!" << std::endl;
		if ( testoutput )
			fstest.success( "data written to {0}", file1 );
		else
			throw_runtime( "failed to write data to {0}", file1 );
	};

   	fstest["istream"] = [&]( void )
	{
		fstest.run( "ostream" );
		base::istream testinput = fs->open_read( file1 );
		std::string l;
		std::getline( testinput, l );
		if ( l == "Hello, world!" )
			fstest.success( "data read from {0}", file1 );
		else
		{
			fstest.failure( "read incorrect data from {0}", file1 );
			fstest.message( "read data: {0}", l );
		}
	};

	fstest["iostream"] = [&]( void )
	{
		std::string msg = "The answer to the question of the Life, the Universe, and Everything is: 42";
		base::iostream testio = fs->open( file2 );
		testio << msg << std::endl;
		if ( testio )
			fstest.success( "data written to {0}", file2 );
		else
			throw_runtime( "failed to write data to {0}", file2 );

		if ( ! testio.seekg( 0 ) )
			throw_runtime( "unable to seek to the beginning" );

		std::string readl;
		std::getline( testio, readl );
		if ( testio )
			fstest.success( "data read from {0}", file2 );
		else
			throw_runtime( "failed to read data from {0}", file2 );
		if ( readl == msg )
			fstest.success( "data read matches write in {0}", file2 );
		else
			throw_runtime( "data read doesn't matches write in {0}" );
	};

	fstest["exists"] = [&]( void )
	{
		fstest.run( "ostream" );
		if ( fs->exists( file1 ) )
			fstest.success( "{0} exists", file1 );
		else
			throw_runtime( "{0} does not exist", file1 );
	};

	fstest["access"] = [&]( void )
	{
		fstest.run( "ostream" );
		if ( fs->access( file1, R_OK ) )
			fstest.success( "read access okay on {0}", file1 );
		else
			fstest.failure( "read access not okay on {0}", file1 );

		if ( fs->access( file1, W_OK ) )
			fstest.success( "write access okay on {0}", file1 );
		else
			fstest.failure( "write access not okay on {0}", file1 );

		if ( fs->access( file1, X_OK ) )
			fstest.failure( "exec access okay on {0}", file1 );
		else
			fstest.success( "exec access not okay on {0}", file1 );
	};

	fstest["mkdir_all"] = [&]( void )
	{
		base::uri tmppathmkdir( tmppath, "blah", "foo" );
		fs->mkdir_all( tmppathmkdir );
		fstest.success( "mkdir_all successful" );
	};

	fstest["symlink"] = [&]( void )
	{
		fstest.run( "ostream" );
		base::uri target( tmppath, "symlink" );
		fs->symlink( file1, target );

		struct stat buf;
		fs->lstat( target, &buf );
		if ( S_ISLNK( buf.st_mode ) )
			fstest.success( "created symlink" );
		else
			fstest.failure( "symlink failed" );
	};

	fstest["link"] = [&]( void )
	{
		fstest.run( "ostream" );
		base::uri target( tmppath, "hardlink" );
		fs->link( file1, target );
		fstest.success( "able to hard link file" );

		struct stat buf;
		fs->lstat( target, &buf );
		if ( buf.st_nlink == 2 )
			fstest.success( "linked file has two links" );
		else
			fstest.failure( "linked file should have two links, got {0}", buf.st_nlink );
	};

	fstest["unlink"] = [&]( void )
	{
		fstest.run( "symlink" );
		base::uri target( tmppath, "symlink" );
		fs->unlink( target );
		if ( fs->exists( target ) )
			throw_runtime( "file {0} still exists", target );
		fstest.success( "able to remove file" );
	};

	fstest["rename"] = [&]( void )
	{
		fstest.run( "mkdir" );
		base::uri target( tmppath, "newblah" );
		base::uri source( tmppath, "blah" );
		fs->rename( source, target );
	};

	fstest.cleanup() = [&]( void )
	{
		fs->rmdir_all( tmppath );
	};

	fstest.run( options );
	fstest.clean();

	return - static_cast<int>( fstest.failure_count() );
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
	return -1;
}
