
#pragma once

namespace layout
{

////////////////////////////////////////

class layout
{
public:
	virtual ~layout( void )
	{
	}

	virtual void recompute_minimum( void ) = 0;
	virtual void recompute_layout( void ) = 0;
};

////////////////////////////////////////

}

