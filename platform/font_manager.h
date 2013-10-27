
#pragma once

#include <string>
#include <set>
#include <memory>
#include <draw/font.h>

namespace platform
{

////////////////////////////////////////

class font_manager
{
public:
	font_manager( void );
	virtual ~font_manager( void );

	const std::string &name( void ) const { return _name; }
	const std::string &version( void ) const { return _version; }

	virtual std::set<std::string> get_foundries( void ) = 0;
	virtual std::set<std::string> get_families( void ) = 0;
	virtual std::set<std::string> get_styles( void ) = 0;

	virtual std::shared_ptr<draw::font> get_font( const std::string &family, const std::string &style, double pixsize ) = 0;

protected:
	void set_manager_name( std::string s ) { _name = std::move( s ); }
	void set_manager_version( std::string v ) { _version = std::move( v ); }

	std::string _name;
	std::string _version;
};

////////////////////////////////////////

}
