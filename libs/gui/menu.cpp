//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "menu.h"
#include "label.h"
#include "button.h"
#include <platform/window.h>

namespace gui
{

////////////////////////////////////////

menu::menu( const std::shared_ptr<platform::window> &w )
	: window( w )
{
	precondition( bool(_window), "null window" );
	_window->set_popup();
	std::cout << "Created menu: " << w << std::endl;

    _container = std::make_shared<gui::simple_container>( direction::DOWN );
    _container->set_spacing( 12, 6 );
    _container->set_pad( 12, 12, 12, 12 );

	set_widget( _container );
}

////////////////////////////////////////

menu::~menu( void )
{
}

////////////////////////////////////////

void menu::add_entry( const std::string &n )
{
	auto button = std::make_shared<gui::button>( n );
	button->when_activated.connect( [this]()
	{
		this->hide();
	} );
	_container->add( button );
}

////////////////////////////////////////


}

