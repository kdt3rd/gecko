
#pragma once

#include "layout.h"

namespace layout
{

////////////////////////////////////////

class grid : public layout
{
public:
	grid( size_t w, size_t h );

	void add( area &a, size_t x, size_t y, size_t w = 1, size_t h = 1 );

	void reset( void ) override;

	void set_spacing( double h, double v );
	void set_spacing( double s )
	{
		set_spacing( s, s );
	}

	void set_padding( double h, double v );
	void set_padding( double p )
	{
		set_padding( p, p );
	}

private:
	void create_tabs( size_t w, size_t h );

	double _default_hspacing = 3.0;
	variable _hspacing;
	double _default_vspacing = 3.0;
	variable _vspacing;
	double _default_hpadding = 3.0;
	variable _hpadding;
	double _default_vpadding = 3.0;
	variable _vpadding;
	std::vector<variable> _htabs;
	std::vector<variable> _vtabs;
	std::vector<constraint> _hcons;
	std::vector<constraint> _vcons;
};

////////////////////////////////////////

}

