
#pragma once

#include <string>

namespace draw
{

////////////////////////////////////////

class font
{
public:
	font( void );
	virtual ~font( void );

	virtual std::string family( void ) const = 0;
	virtual std::string style( void ) const = 0;
	virtual double size( void ) const = 0;
};

////////////////////////////////////////

}
