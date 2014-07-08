
#pragma once

#include <platform/screen.h>

namespace platform { namespace dummy
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( void );
	~screen( void );

	base::size bounds( void ) const override;
};

////////////////////////////////////////

} }

