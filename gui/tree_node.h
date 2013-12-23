
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
	void set_collapse( bool c = true ) { _collapsed = c; }

	virtual void compute_minimum( void );
	virtual void compute_layout( void );

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_release( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );

	virtual void paint( const std::shared_ptr<draw::canvas> &c );

private:
	bool _collapsed = false;
};

////////////////////////////////////////

}
