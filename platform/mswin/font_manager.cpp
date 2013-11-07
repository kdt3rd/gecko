
#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <set>
#include <draw/cairo/font.h>
#include <core/contract.h>
#include <windows.h>
#include <cairo-win32.h>

namespace
{
	int CALLBACK fill_font_family( const LOGFONT *lpelfe, const TEXTMETRIC *lpntm, DWORD FontType, LPARAM lParam )
	{
		if ( lpelfe )
		{
			std::vector<std::wstring> *list = reinterpret_cast<std::vector<std::wstring>*>( lParam );
			list->emplace_back( lpelfe->lfFaceName );
		}
		return 1;
	}

	int CALLBACK fill_logfont( const LOGFONT *lpelfe, const TEXTMETRIC *lpntm, DWORD FontType, LPARAM lParam )
	{
		if ( lpelfe )
		{
			std::vector<LOGFONT> *list = reinterpret_cast<std::vector<LOGFONT>*>( lParam );
			list->emplace_back( *lpelfe );
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

	std::vector<std::wstring> list;
	EnumFontFamiliesEx( hdc, &lf, (FONTENUMPROC)&fill_font_family, (LPARAM)&list, 0 );

	std::set<std::string> ret;
	char buf[LF_FACESIZE];
	for ( auto ws: list )
	{
		int n = WideCharToMultiByte( CP_UTF8, 0, ws.c_str(), -1, buf, LF_FACESIZE, NULL, NULL );
		ret.emplace( buf, buf + n );
	}

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
	/*
	precondition( family.size() < LF_FACESIZE, "family name too long" );

	LOGFONTW lf;

	int n = MultiByteToWideChar( CP_UTF8, 0, family.c_str(), family.size(), lf.lfFaceName, LF_FACESIZE );
	if ( n <= 0 )
		throw std::runtime_error( "can't convert to utf-16" );

	lf.lfWidth = 0;
	lf.lfHeight = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	auto cfont = cairo_win32_font_face_create_for_logfontw( &lf );
	std::cout << "CFont created" << std::endl;

	std::shared_ptr<draw::font> ret;
	ret = std::make_shared<cairo::font>( cfont, family, style, pixsize );
	std::cout << "Font created" << std::endl;

	return ret;
	*/

//	std::cout << "Creating toy font face" << std::endl;
//	auto cfont = cairo_toy_font_face_create( family.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
//	std::cout << "Done" << std::endl;
	std::shared_ptr<draw::font> ret;
//	ret = std::make_shared<cairo::font>( cfont, family, style, pixsize );
//	std::cout << "Returning" << std::endl;
	return ret;
}

////////////////////////////////////////

}

