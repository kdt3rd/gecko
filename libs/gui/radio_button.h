//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "widget.h"
#include <base/signal.h>
#include <draw/shape.h>

namespace gui
{

////////////////////////////////////////

class radio_button_w : public widget
{
public:
	radio_button_w( void );
	~radio_button_w( void );

	void build( context &ctxt ) override;
	void paint( context &ctxt ) override;

	bool mouse_press( const point &p, int button ) override;
	bool mouse_release( const point &p, int button ) override;
	bool mouse_move( const point &p ) override;

	bool is_checked( void ) const
	{
		return _state;
	}

	void set_state( bool s );

	signal<void(void)> when_selected;

public:
	draw::shape _checked;
	draw::shape _unchecked;

	bool _state = false;
	bool _tracking = false;
	bool _current = false;
};

////////////////////////////////////////

using radio_button = widget_ptr<radio_button_w>;

}

