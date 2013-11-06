
#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <windows.h>

namespace
{
	int CALLBACK fill_font_family( const LOGFONT *lpelfe, const TEXTMETRIC *lpntm, DWORD FontType, LPARAM lParam )
	{
		if ( lpelfe )
		{
			std::set<std::string> *list = reinterpret_cast<std::set<std::string>*>( lParam );
			list->insert( lpelfe->lfFaceName );
		}
		return 1;
	}
}

namespace mswin
{

////////////////////////////////////////

font_manager::font_manager( void )
{
	set_manager_name( "mswin" );
	set_manager_version( "1" );
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	HDC hdc = GetDC( nullptr );

	LOGFONT lf;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfFaceName[0] = '\0';
	lf.lfPitchAndFamily = 0;

	std::set<std::string> ret;
	EnumFontFamiliesEx( hdc, &lf, (FONTENUMPROC)&fill_font_family, (LPARAM)&ret, 0 );
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( void )
{
	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::shared_ptr<draw::font> font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	std::shared_ptr<draw::font> ret;
	return ret;
}

////////////////////////////////////////

}

