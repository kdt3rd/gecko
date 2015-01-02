
#pragma once

#include <base/size.h>
#include <platform/screen.h>

namespace platform { namespace mswin
{

////////////////////////////////////////

/// @brief Microsoft Windows screen.
class screen : public platform::screen
{
public:
	screen( void );
	virtual ~screen( void );

	bool is_default( void ) const override;

	virtual base::size bounds( void ) const override;

	base::size dpi( void ) const override;
};

////////////////////////////////////////

} }

