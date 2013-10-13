
#include <sstream>
#include <stdexcept>
#include "callegro.h"
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
	if ( !callegro::al_install_system( ALLEGRO_VERSION_INT, NULL ) )
		throw runtime_error( "Can't initialize allegro" );

	if ( !callegro::al_init_image_addon() )
		throw runtime_error( "Can't initialize allegro image" );

	if ( !callegro::al_init_primitives_addon() )
		throw runtime_error( "Can't initialize allegro primitive" );

	callegro::al_init_font_addon();
	if ( !callegro::al_init_ttf_addon() )
		throw runtime_error( "Can't initialize allegro ttf" );

	callegro::al_register_assert_handler( handler );
}

////////////////////////////////////////

system::~system( void )
{
	callegro::al_uninstall_system();
	callegro::al_shutdown_font_addon();
	callegro::al_shutdown_primitives_addon();
	callegro::al_shutdown_image_addon();
}

////////////////////////////////////////

}
