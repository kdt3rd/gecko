
#pragma once

#include <string>

namespace draw
{

////////////////////////////////////////

class font
{
public:
	font( std::string fam, std::string sty, double sz );
	virtual ~font( void );

	std::string family( void ) const { return _family; }
	std::string style( void ) const { return _style; }
	double size( void ) const { return _size; }

private:
	std::string _family;
	std::string _style;
	double _size;
};

////////////////////////////////////////

}
