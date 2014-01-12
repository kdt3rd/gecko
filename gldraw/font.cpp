
#include "font.h"

namespace gldraw
{

////////////////////////////////////////

font::font( std::string fam, std::string sty, double sz )
	: _family( std::move(fam) ), _style( std::move(sty) ), _size( sz )
{
}

////////////////////////////////////////

font::~font( void )
{
}

////////////////////////////////////////

}

