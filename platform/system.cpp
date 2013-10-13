
#include "system.h"

namespace platform
{

////////////////////////////////////////

system::system( std::string name, std::string desc )
	: _name( std::move( name ) ), _desc( std::move( desc ) )
{
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

}

