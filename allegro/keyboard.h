
#pragma once

#include "callegro.h"

#include <stdexcept>

namespace allegro
{

#define KEYCODE( x ) KEY_ ## x = callegro::ALLEGRO_KEY_ ## x
enum key_code
{
	KEYCODE( A ), KEYCODE( B ), KEYCODE( C ), KEYCODE( D ), KEYCODE( E ),
	KEYCODE( F ), KEYCODE( G ), KEYCODE( H ), KEYCODE( I ), KEYCODE( J ),
	KEYCODE( K ), KEYCODE( L ), KEYCODE( M ), KEYCODE( N ), KEYCODE( O ),
	KEYCODE( P ), KEYCODE( Q ), KEYCODE( R ), KEYCODE( S ), KEYCODE( T ),
	KEYCODE( U ), KEYCODE( V ), KEYCODE( W ), KEYCODE( X ), KEYCODE( Y ),
	KEYCODE( Z ),

	KEYCODE( 0 ), KEYCODE( 1 ), KEYCODE( 2 ), KEYCODE( 3 ), KEYCODE( 4 ),
	KEYCODE( 5 ), KEYCODE( 6 ), KEYCODE( 7 ), KEYCODE( 8 ), KEYCODE( 9 ),

	KEYCODE( PAD_0 ), KEYCODE( PAD_1 ), KEYCODE( PAD_2 ), KEYCODE( PAD_3 ), KEYCODE( PAD_4 ),
	KEYCODE( PAD_5 ), KEYCODE( PAD_6 ), KEYCODE( PAD_7 ), KEYCODE( PAD_8 ), KEYCODE( PAD_9 ),

	KEYCODE( F1 ), KEYCODE( F2 ), KEYCODE( F3 ), KEYCODE( F4 ), KEYCODE( F5 ), KEYCODE( F6 ),
	KEYCODE( F7 ), KEYCODE( F8 ), KEYCODE( F9 ), KEYCODE( F10 ), KEYCODE( F11 ), KEYCODE( F12 ),

	KEYCODE( ESCAPE ),
	KEYCODE( TILDE ),
	KEYCODE( MINUS ),
	KEYCODE( EQUALS ),
	KEYCODE( BACKSPACE ),
	KEYCODE( TAB ),
	KEYCODE( OPENBRACE ),
	KEYCODE( CLOSEBRACE ),
	KEYCODE( ENTER ),
	KEYCODE( SEMICOLON ),
	KEYCODE( QUOTE ),
	KEYCODE( BACKSLASH ),
	KEYCODE( BACKSLASH2 ),
	KEYCODE( COMMA ),
	KEYCODE( FULLSTOP ),
	KEYCODE( SLASH ),
	KEYCODE( SPACE ),
	KEYCODE( INSERT ),
	KEYCODE( DELETE ),
	KEYCODE( HOME ),
	KEYCODE( END ),
	KEYCODE( PGUP ),
	KEYCODE( PGDN ),
	KEYCODE( LEFT ),
	KEYCODE( RIGHT ),
	KEYCODE( UP ),
	KEYCODE( DOWN ),
	KEYCODE( PAD_SLASH ),
	KEYCODE( PAD_ASTERISK ),
	KEYCODE( PAD_MINUS ),
	KEYCODE( PAD_PLUS ),
	KEYCODE( PAD_DELETE ),
	KEYCODE( PAD_ENTER ),
	KEYCODE( PRINTSCREEN ),
	KEYCODE( PAUSE ),
	KEYCODE( ABNT_C1 ),
	KEYCODE( YEN ),
	KEYCODE( KANA ),
	KEYCODE( CONVERT ),
	KEYCODE( NOCONVERT ),
	KEYCODE( AT ),
	KEYCODE( CIRCUMFLEX ),
	KEYCODE( COLON2 ),
	KEYCODE( KANJI ),
	KEYCODE( LSHIFT ),
	KEYCODE( RSHIFT ),
	KEYCODE( LCTRL ),
	KEYCODE( RCTRL ),
	KEYCODE( ALT ),
	KEYCODE( ALTGR ),
	KEYCODE( LWIN ),
	KEYCODE( RWIN ),
	KEYCODE( MENU ),
	KEYCODE( SCROLLLOCK ),
	KEYCODE( NUMLOCK ),
	KEYCODE( CAPSLOCK ),
	KEYCODE( PAD_EQUALS ),
	KEYCODE( BACKQUOTE ),
	KEYCODE( SEMICOLON2 ),
	KEYCODE( COMMAND ),
//	KEYCODE( BACK ),
	KEYCODE( MAX ),
};
#undef KEYCODE

#define KEYMOD( x ) MOD_ ## x = callegro::ALLEGRO_KEYMOD_ ## x
enum key_mod
{
	KEYMOD( SHIFT ),
	KEYMOD( CTRL ),
	KEYMOD( ALT ),
	KEYMOD( LWIN ),
	KEYMOD( RWIN ),
	KEYMOD( MENU ),
	KEYMOD( ALTGR ),
	KEYMOD( COMMAND ),
	KEYMOD( SCROLLLOCK ),
	KEYMOD( NUMLOCK ),
	KEYMOD( CAPSLOCK ),
	KEYMOD( INALTSEQ ),
	KEYMOD( ACCENT1 ),
	KEYMOD( ACCENT2 ),
	KEYMOD( ACCENT3 ),
	KEYMOD( ACCENT4 ),
};
#undef KEYMOD

////////////////////////////////////////

class keyboard
{
public:
	keyboard( void )
	{
		if ( callegro::al_is_keyboard_installed() )
			throw std::runtime_error( "only one keyboard allowed" );

		if ( !callegro::al_install_keyboard() )
			throw std::runtime_error( "error installing keyboard" );
	}

	~keyboard( void )
	{
		callegro::al_uninstall_keyboard();
	}

	void update_state( void )
	{
		callegro::al_get_keyboard_state( &m_state );
	}

	bool key_down( int kc )
	{
		return callegro::al_key_down( &m_state, int(kc) );
	}

	const char *key_name( int kc )
	{
		return callegro::al_keycode_to_name( int(kc) );
	}

private:
	callegro::ALLEGRO_KEYBOARD_STATE m_state;

};

////////////////////////////////////////

}

// vim:ft=cpp:
