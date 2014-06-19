
#pragma once

#include <core/size.h>
#include <platform/screen.h>

namespace mswin
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( void );
	virtual ~screen( void );

	bool is_default( void ) const override;

	virtual core::size bounds( void ) const override;

	core::point dpi( void ) const override;
};

////////////////////////////////////////

}

