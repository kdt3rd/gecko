
#include <sstream>
#include <stdexcept>
extern "C" {
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
}
#include "system.h"

using namespace std;

namespace allegro
{

////////////////////////////////////////

static void handler( char const *expr, char const *file, int line, char const *func )
{
	stringstream str;
	str << "Error in " << file << " line " << line << " in function " << func << '\n';
	str << "  " << expr << '\n';
	throw runtime_error( str.str() );
}

////////////////////////////////////////

system::system( void )
{
	if ( !al_install_system( ALLEGRO_VERSION_INT, NULL ) )
		throw runtime_error( "Can't initialize allegro" );

	if ( !al_init_image_addon() )
		throw runtime_error( "Can't initialize allegro image" );

	if ( !al_init_primitives_addon() )
		throw runtime_error( "Can't initialize allegro primitive" );

	al_register_assert_handler( handler );
}

////////////////////////////////////////

system::~system( void )
{
	al_uninstall_system();
	al_shutdown_image_addon();
}

////////////////////////////////////////

}
