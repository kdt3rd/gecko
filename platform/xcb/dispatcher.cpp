
#include <iostream>
#include <stdlib.h>
#include <core/contract.h>
#include "dispatcher.h"
#include <xcb/xcb.h>

namespace xcb
{

////////////////////////////////////////

dispatcher::dispatcher( xcb_connection_t *c )
	: _connection( c )
{
	precondition( _connection, "null connection" );
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	xcb_flush( _connection );
	_exit_code = 0;

	std::shared_ptr<xcb_generic_event_t> event;
	bool done = false;
	while ( !done )
	{
		event.reset( xcb_wait_for_event( _connection ), free );
		if ( !event )
			break;
		switch ( event->response_type & ~0x80 )
		{
			case XCB_EXPOSE:
			{
				xcb_expose_event_t *ev = reinterpret_cast<xcb_expose_event_t*>( event.get() );
				if ( ev->count == 0 )
					_windows[ev->window]->exposed();
				break;
			}

			case XCB_CONFIGURE_NOTIFY:
			{
				xcb_configure_notify_event_t *ev = reinterpret_cast<xcb_configure_notify_event_t*>( event.get() );
				_windows[ev->window]->resized( ev->width, ev->height );
				break;
			}
		}
	/*
		SDL_Event event;
		if ( SDL_WaitEvent( &event ) == 0 )
			throw std::runtime_error( SDL_GetError() );

		switch ( event.type )
		{
			case SDL_QUIT:
				done = true;
				break;

			case SDL_WINDOWEVENT:
				switch ( event.window.event )
				{
					case SDL_WINDOWEVENT_CLOSE:
						_windows[event.window.windowID]->closed();
						break;

					case SDL_WINDOWEVENT_SHOWN:
						_windows[event.window.windowID]->shown();
						break;

					case SDL_WINDOWEVENT_HIDDEN:
						_windows[event.window.windowID]->hidden();
						break;

					case SDL_WINDOWEVENT_MINIMIZED:
						_windows[event.window.windowID]->minimized();
						break;

					case SDL_WINDOWEVENT_MAXIMIZED:
						_windows[event.window.windowID]->maximized();
						break;

					case SDL_WINDOWEVENT_RESTORED:
						_windows[event.window.windowID]->restored();
						break;

					case SDL_WINDOWEVENT_EXPOSED:
					{
						SDL_Event peek[5];
						int n = SDL_PeepEvents( peek, 5, SDL_PEEKEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT );
						if ( n < 0 )
							throw std::runtime_error( SDL_GetError() );
						bool doit = true;
						for ( int i = 0; i < n; ++i )
						{
							if ( peek[i].window.event == SDL_WINDOWEVENT_EXPOSED &&
								 peek[i].window.windowID == event.window.windowID )
							{
								doit = false;
								break;
							}

						}
						if ( doit )
							_windows[event.window.windowID]->exposed();
						break;
					}

					case SDL_WINDOWEVENT_MOVED:
						_windows[event.window.windowID]->moved( event.window.data1, event.window.data2 );
						break;

					case SDL_WINDOWEVENT_RESIZED:
						_windows[event.window.windowID]->resized( event.window.data1, event.window.data2 );
						break;
				}
		}
		break;
		*/
	}

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;

	/*
	SDL_Event event;
	event.type = SDL_QUIT;
	if ( SDL_PushEvent( &event) == 0 )
		throw std::runtime_error( SDL_GetError() );
	*/
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &w )
{
	_windows[w->id()] = w;
}

////////////////////////////////////////

}

