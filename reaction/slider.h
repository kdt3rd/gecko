
#pragma once

#include <memory>
#include "reaction.h"
#include <core/action.h>

namespace reaction
{

////////////////////////////////////////

class slider : public reaction
{
public:
	slider( void );
	~slider( void );

	void set_handle_size( double s ) { _handle = s; }

	action<void(double)> move_began;
	action<void(double)> changed;
	action<void(double)> move_ended;

	virtual bool mouse_press( const layout::simple_area &area, const draw::point &p, int button );
	virtual bool mouse_move( const layout::simple_area &area, const draw::point &p );
	virtual bool mouse_release( const layout::simple_area &area, const draw::point &p, int button );

private:
	bool _tracking = false;
	double _start = 0.0;
	double _value = 0.5;
	double _handle = 10.0;
};

////////////////////////////////////////

}

