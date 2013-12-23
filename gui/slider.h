
#pragma once

#include "widget.h"
#include <reaction/slider.h>

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

private:
	bool _pressed = false;
	double _value = 0.5;
	double _min = 0.0, _max = 1.0;
};

////////////////////////////////////////

}

