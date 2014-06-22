
#pragma once

#include <base/size.h>
#include <platform/screen.h>

namespace platform { namespace mswin
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( void );
	virtual ~screen( void );

	bool is_default( void ) const override;

	virtual base::size bounds( void ) const override;

	base::point dpi( void ) const override;
};

////////////////////////////////////////

} }

