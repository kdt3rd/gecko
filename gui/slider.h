
#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class slider : public widget
{
public:
	slider( void );
	~slider( void );

	void set_value( double v );
	void set_range( double min, double max );

	virtual void paint( const std::shared_ptr<draw::canvas> &c );

	virtual void compute_minimum( void );

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );
	virtual bool mouse_release( const draw::point &p, int button );

private:
	bool _tracking = false;
	bool _pressed = false;
	double _handle = 12.0;
	double _start = 0.0;
	double _value = 0.5;
	double _min = 0.0, _max = 1.0;
};

////////////////////////////////////////

}

