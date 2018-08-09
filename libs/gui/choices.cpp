//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "choices.h"
#include "label.h"
#include "radio_button.h"

namespace gui
{

////////////////////////////////////////

choices_w::choices_w( void )
	: composite( std::unique_ptr<layout::grid>( new layout::grid ) )
{
	_layout = std::dynamic_pointer_cast<layout::grid>( layout_target() );
	//_layout->set_spacing( 3, 3 );
	_layout->add_columns( 1, 0.0 );
	_layout->add_columns( 1, 1.0 );
}

////////////////////////////////////////

choices_w::~choices_w( void )
{
}

////////////////////////////////////////

void choices_w::add_choice( const std::string &l )
{
	size_t choice = _radios.size();
	auto self = std::dynamic_pointer_cast<choices_w>( shared_from_this() );
	_labels.emplace_back( std::make_shared<label_w>( l ) );
	_radios.emplace_back( std::make_shared<radio_button_w>() );
	if ( choice == 0 )
		_radios.back()->set_state( true );

	_radios.back()->when_selected.connect( [=](void)
	{
		self->choose( choice );
	} );


	size_t n = _layout->add_rows( 1 );
	_layout->add( _radios.back()->layout_target(), 0, n );
	_layout->add( _labels.back()->layout_target(), 1, n );
}

////////////////////////////////////////

void choices_w::build( context &ctxt )
{
	_layout->set_spacing( ctxt.from_native_horiz( 8 ), ctxt.from_native_vert( 8 ) );
	composite::build( ctxt );
}

////////////////////////////////////////

void choices_w::for_subwidgets( const std::function<void(const std::shared_ptr<widget>&)> &f )
{
	for ( auto &l: _labels )
		f( l );
	for ( auto &r: _radios )
		f( r );
}

////////////////////////////////////////

void choices_w::choose( size_t c )
{
	for ( size_t i = 0; i < _radios.size(); ++i )
	{
		auto &r = _radios[i];
		r->set_state( i == c );
	}
	when_activated( c );
}

////////////////////////////////////////

}

