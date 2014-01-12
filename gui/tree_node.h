
#pragma once

#include "container.h"

namespace gui
{

////////////////////////////////////////

class tree_node : public container<tree_layout>
{
public:
	using container<tree_layout>::container;

	bool collapsed( void ) const { return _collapsed; }
	void set_collapsed( bool c = true );

	void compute_minimum( void ) override;
	void compute_layout( void ) override;

	bool mouse_press( const core::point &p, int button ) override;
	bool mouse_release( const core::point &p, int button ) override;
	bool mouse_move( const core::point &p ) override;

	void paint( const std::shared_ptr<gldraw::canvas> &c ) override;

private:
	bool _collapsed = false;
};

////////////////////////////////////////

}
