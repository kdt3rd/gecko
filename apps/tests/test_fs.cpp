
#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <base/posix_file_system.h>
#include <sstream>
#include <iostream>

#include <unistd.h>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "",      "<dir> ...",    base::cmd_line::args,     "List of directories to show", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	base::file_system::add( "file", std::make_shared<base::posix_file_system>() );

	if ( auto &opt = options["<dir> ..."] )
	{
		for ( auto &v: opt.values() )
		{
			std::string pname( v );
			base::uri path( pname );
			std::cout << "Path = " << pname << '\n';
			auto dir = base::file_system::get( path )->readdir( path );
			while ( ++dir )
				std::cout << *dir << std::endl;
		}
	}

	std::cout << "\nTesting misc posix_file_system functions:\n";
	base::uri tmppath( "file:/tmp" );
	auto fs = base::file_system::get( tmppath );
	struct stat statbuf = {0};
	fs->lstat( tmppath, &statbuf );
	if ( S_ISDIR( statbuf.st_mode ) )
		std::cout << "Path " << tmppath << ": is correctly lstat-ed as a directory" << std::endl;
	else
		throw std::runtime_error( "/tmp is not a directory per lstat" );

	fs->stat( tmppath, &statbuf );
	if ( S_ISDIR( statbuf.st_mode ) )
		std::cout << "Path " << tmppath << ": is correctly stat-ed as a directory" << std::endl;
	else
		throw std::runtime_error( "/tmp is not a directory per stat" );

	struct statvfs fsinfo;
	fs->statfs( tmppath, &fsinfo );
	std::cout << "Filesystem at " << tmppath << " has " << (fsinfo.f_bsize*fsinfo.f_bavail) << " bytes free" << std::endl;

	base::uri tmpfn( tmppath, "test_posix_fs" );
	{
		std::cout << "Writing test data to " << tmpfn << std::endl;
		base::ostream testoutput = fs->open_write( tmpfn );
		testoutput << "Hello, world!" << std::endl;
	}
	{
		std::cout << "Reading data to " << tmpfn << std::endl;
		base::istream testinput = fs->open_read( tmpfn );
		std::string l;
		std::getline( testinput, l );
		if ( l == "Hello, world!" )
			std::cout << "SUCCESS: Able to read data from written file" << std::endl;
		else
			std::cerr << "ERROR: Unble to read same data as was written to file, got '" << l << "'" << std::endl;
	}
	{
		std::cout << "Testing iostream to " << tmpfn << std::endl;
		base::iostream testio = fs->open( tmpfn );
		testio << "The answer to the question of the Life, the Universe, and Everything is: 42" << std::endl;
		if ( ! testio.seekg( 0 ) )
			throw std::runtime_error( "Unable to seek to the beginning" );
		std::string readl;
		std::getline( testio, readl );
		std::cout << "Read: " << readl << std::endl;
	}
	if ( fs->exists( tmpfn ) )
		std::cout << "File " << tmpfn << " exists." << std::endl;
	else
		throw std::runtime_error( "File " + tmpfn.pretty() + " does not exist" );
	
	if ( fs->access( tmpfn, R_OK ) )
		std::cout << "Read OK on " << tmpfn << std::endl;
	else
		std::cerr << "ERROR: Read NOT OK on " << tmpfn << std::endl;
	if ( fs->access( tmpfn, W_OK ) )
		std::cout << "Write OK on " << tmpfn << std::endl;
	else
		std::cerr << "ERROR: Write NOT OK on " << tmpfn << std::endl;
	if ( fs->access( tmpfn, X_OK ) )
		std::cout << "Execute OK on " << tmpfn << std::endl;
	else
		std::cout << "Execute NOT OK on " << tmpfn << std::endl;

	base::uri tmppathmkdir( tmppath, "mumble" );
	tmppathmkdir /= "blah";
	try
	{
		fs->mkdir( tmppathmkdir );
	}
	catch ( std::exception &e )
	{
		base::print_exception( std::cout, e );
		std::cout << "Successfully trap failure " << tmppathmkdir << std::endl;
	}

	fs->mkdir_all( tmppathmkdir );
	base::uri tmpsubpathtest( tmppathmkdir, "foobar" );
	fs->symlink( tmpfn, tmpsubpathtest );
	fs->lstat( tmpsubpathtest, &statbuf );
	if ( S_ISLNK( statbuf.st_mode ) )
		std::cout << "Able to create symlink " << tmpsubpathtest << std::endl;
	fs->unlink( tmpsubpathtest );
	if ( fs->exists( tmpsubpathtest ) )
		throw std::runtime_error( "File " + tmpsubpathtest.pretty() + " still exists" );
	std::cout << "SUCCESS: Able to remove symlink file" << std::endl;
	fs->link( tmpfn, tmpsubpathtest );
	std::cout << "SUCCESS: Able to create hard link file" << std::endl;
	base::uri tmpsubpathtest2( tmppathmkdir, "foobaz" );
	fs->rename( tmpsubpathtest, tmpsubpathtest2 );
	fs->lstat( tmpsubpathtest2, &statbuf );
	if ( statbuf.st_nlink == 2 )
		std::cout << "SUCCESS: hard link is preserved across rename" << std::endl;
	std::cout << "Contents of " << tmppathmkdir << " after rename" << std::endl;
	auto dir = fs->readdir( tmppathmkdir );
	while ( ++dir )
		std::cout << *dir << std::endl;

	fs->rmdir_all( tmppathmkdir );
	fs->lstat( tmpfn, &statbuf );
	if ( statbuf.st_nlink != 1 )
		throw std::runtime_error( "remove failed" );
	base::uri cleanuppath( tmppath, "mumble" );
	fs->rmdir( cleanuppath );
	try
	{
		fs->rmdir( tmpfn );
	}
	catch ( std::exception &e )
	{
		base::print_exception( std::cout, e );
		std::cout << "SUCCESS: attempt to remove file via rmdir fails" << std::endl;
	}
	fs->unlink( tmpfn );

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
