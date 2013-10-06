
#pragma once

extern "C" {
#include <allegro5/allegro.h>
}

namespace allegro
{

////////////////////////////////////////

#define EVENT_TYPE( x ) EVENT_ ## x = ALLEGRO_EVENT_ ## x
enum event_type
{
	EVENT_TYPE( KEY_DOWN ),
	EVENT_TYPE( KEY_UP ),
	EVENT_TYPE( KEY_CHAR ),
	EVENT_TYPE( MOUSE_AXES ),
	EVENT_TYPE( MOUSE_BUTTON_DOWN ),
	EVENT_TYPE( MOUSE_BUTTON_UP ),
	EVENT_TYPE( MOUSE_WARPED ),
	EVENT_TYPE( MOUSE_ENTER_DISPLAY ),
	EVENT_TYPE( MOUSE_LEAVE_DISPLAY ),
	EVENT_TYPE( TIMER ),
	EVENT_TYPE( DISPLAY_EXPOSE ),
	EVENT_TYPE( DISPLAY_RESIZE ),
	EVENT_TYPE( DISPLAY_CLOSE ),
	EVENT_TYPE( DISPLAY_LOST ),
	EVENT_TYPE( DISPLAY_FOUND ),
	EVENT_TYPE( DISPLAY_SWITCH_OUT ),
	EVENT_TYPE( DISPLAY_SWITCH_IN ),
	EVENT_TYPE( DISPLAY_ORIENTATION ),
//	EVENT_TYPE( DISPLAY_HALT_DRAWING ),
//	EVENT_TYPE( DISPLAY_RESUME_DRAWING ),
//	EVENT_TYPE( DISPLAY_CONNECTED ),
//	EVENT_TYPE( DISPLAY_DISCONNECTED ),
};

class event
{
public:
	int type( void ) const { return m_event.type; }

	double timestamp( void ) const { return m_event.any.timestamp; }

	// Keyboard event
	int keycode( void ) const { return m_event.keyboard.keycode; }
	int unicode( void ) const { return m_event.keyboard.unichar; }
	int modifiers( void ) const { return m_event.keyboard.modifiers; }
	bool repeat( void ) const { return m_event.keyboard.repeat; }

	int x( void ) const { return m_event.mouse.x; }
	int y( void ) const { return m_event.mouse.y; }
	int z( void ) const { return m_event.mouse.z; }
	int w( void ) const { return m_event.mouse.w; }
	int dx( void ) const { return m_event.mouse.dx; }
	int dy( void ) const { return m_event.mouse.dy; }
	int dz( void ) const { return m_event.mouse.dz; }
	int dw( void ) const { return m_event.mouse.dw; }
	int button( void ) const { return m_event.mouse.button - 1; }

	int width( void ) const { return m_event.display.width; }
	int height( void ) const { return m_event.display.height; }

	bool acknowledge_resize( void ) { return al_acknowledge_resize( m_event.display.source ); }


	ALLEGRO_EVENT *internal( void ) { return &m_event; }

private:
	ALLEGRO_EVENT m_event;
};

////////////////////////////////////////

}

// vim:ft=cpp:
