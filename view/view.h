
#pragma once

#include <layout/area.h>
#include <draw/canvas.h>
#include <draw/rect.h>
#include "delegate.h"

namespace view
{

////////////////////////////////////////

class view
{
public:
	view( void );
	virtual ~view( void );

	void set_delegate( delegate *d ) { _delegate = d; }
	void reset_delegate( void ) { _delegate = nullptr; }
	delegate *get_delegate( void ) { return _delegate; }

	virtual void layout( const std::shared_ptr<draw::canvas> &canvas );
	virtual void paint( const std::shared_ptr<draw::canvas> &canvas ) = 0;

	void invalidate( const draw::rect &r ) { if ( _delegate ) _delegate->invalidate( r ); }

private:
	delegate *_delegate;
};

////////////////////////////////////////

}

