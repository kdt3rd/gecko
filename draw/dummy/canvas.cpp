
#include "canvas.h"
#include "font.h"
#include <draw/path.h>
#include <core/contract.h>

namespace dummy
{

////////////////////////////////////////

canvas::canvas( void )
{
}

////////////////////////////////////////

canvas::~canvas( void )
{
}

////////////////////////////////////////

void canvas::fill( const draw::paint &c )
{
}

////////////////////////////////////////

void canvas::fill( const draw::rect &r, const draw::paint &c )
{
}

////////////////////////////////////////

void canvas::draw_path( const draw::path &path, const draw::paint &c )
{
}

////////////////////////////////////////

void canvas::draw_text( const std::shared_ptr<draw::font> &font, const draw::point &p, const std::string &utf8, const draw::paint &c )
{
}

////////////////////////////////////////

void canvas::present( void )
{
}

////////////////////////////////////////

void canvas::screenshot_png( const char *filename )
{
	throw std::runtime_error( "screenshot not implemented");
}

////////////////////////////////////////

}
