// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "menu.h"

#include "button.h"
#include "label.h"

#include <platform/window.h>

namespace gui
{
////////////////////////////////////////

menu::menu( const std::shared_ptr<platform::window> &w ) : window( w )
{
    precondition( bool( _window ), "null window" );
    _window->set_popup();
    std::cout << "Created menu: " << w << std::endl;

    _container    = std::make_shared<gui::simple_container>( direction::DOWN );
    coord twelveH = w->from_native_horiz( 12 );
    coord twelveV = w->from_native_vert( 12 );
    coord sixV    = w->from_native_vert( 6 );
    _container->set_spacing( twelveH, sixV );
    _container->set_pad( twelveH, twelveH, twelveV, twelveV );

    set_widget( _container );
}

////////////////////////////////////////

menu::~menu( void ) {}

////////////////////////////////////////

void menu::add_entry( const std::string &n )
{
    auto button = std::make_shared<gui::button>( n );
    button->when_activated.connect( [this]() { this->hide(); } );
    _container->add( button );
}

////////////////////////////////////////

} // namespace gui
