//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>
#include <map>
#include <atomic>
#include <array>
#include <vector>
#include <stack>
#include <string>
#include <platform/dispatcher.h>
#include <base/pipe.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace platform { namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::dispatcher.
///
/// Dispatcher implemented using Xlib.
class dispatcher : public ::platform::dispatcher
{
public:
	/// @brief Constructor.
	dispatcher( const std::shared_ptr<Display> &dpy, const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	~dispatcher( void );

	int execute( void ) override;
	void exit( int code ) override;
	void add_waitable( const std::shared_ptr<waitable> &w ) override;
	void remove_waitable( const std::shared_ptr<waitable> &w ) override;

	/// @brief Add a window.
	///
	/// Add a window for the dispatcher to handle events.
	void add_window( const std::shared_ptr<window> &w );

	void remove_window( const std::shared_ptr<window> &w );

	std::pair<std::vector<uint8_t>, std::string> query_selection( bool mouseSel, const std::vector<std::string> &reqTypes = std::vector<std::string>() );
	std::pair<std::vector<uint8_t>, std::string> query_selection( const std::string &clipboardName, const std::vector<std::string> &reqTypes = std::vector<std::string>() );

private:
	void wake_up_executor( void );
	void run_event_loop_until( std::atomic<bool> *b );
	bool drain_xlib_events( void );

	struct Property
	{
		unsigned char *data = nullptr;
		int format = 0;
		int nitems = 0;
		Atom type = None;
	};
	Property read_property( Atom sel );

	void dispatchKeyPress( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchKeyRelease( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchButtonPress( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchButtonRelease( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchMotionNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchEnterNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchLeaveNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchFocusIn( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchFocusOut( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchKeymapNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchExpose( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchGraphicsExpose( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchNoExpose( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchVisibilityNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchCreateNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchDestroyNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchUnmapNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchMapNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchMapRequest( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchReparentNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchConfigureNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchConfigureRequest( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchGravityNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchResizeRequest( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchCirculateNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchCirculateRequest( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchPropertyNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchSelectionClear( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchSelectionRequest( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchSelectionNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchColormapNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchClientMessage( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchMappingNotify( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchGenericEvent( const std::shared_ptr<window> &w, XEvent &event );
	void dispatchUNKNOWN( const std::shared_ptr<window> &w, XEvent &event );

	typedef void (dispatcher::*event_dispatch_func)( const std::shared_ptr<window> &, XEvent & );

	std::array<event_dispatch_func, LASTEvent> _dispatch;

	int _exit_code = 0;
	base::pipe _wait_pipe;
	std::atomic<bool> _exit_requested{false};

	std::shared_ptr<Display> _display;

	Atom _atom_delete_window = None;
	Atom _atom_quit_app = None;

	XIM _xim;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;

	Window _clipboard_win = 0;
	std::map<Window, std::shared_ptr<window>> _windows;

	Atom _sel_targets = None;
	Atom _sel_primary = None;
	Atom _sel_clip = None;
	std::map<std::string, Atom> _sel_custom_clips;

	// probably shouldn't allow a stack, but who knows...
	struct SelectionRequestInfo
	{
		SelectionRequestInfo( void ) = default;
		SelectionRequestInfo( std::vector<uint8_t> *r, std::string *rt, const std::vector<std::string> *rqt, std::atomic<bool> *f, Atom s ) : result( r ), resulttype( rt ), reqTypes( rqt ), fin( f ), sel( s ), requested( None ), sent_request( false ) {}
		
		~SelectionRequestInfo( void ) = default;
		SelectionRequestInfo( const SelectionRequestInfo & ) = default;
		SelectionRequestInfo( SelectionRequestInfo && ) = default;
		SelectionRequestInfo &operator=( const SelectionRequestInfo & ) = default;
		SelectionRequestInfo &operator=( SelectionRequestInfo && ) = default;
		std::vector<uint8_t> *result = nullptr;
		std::string *resulttype = nullptr;
		const std::vector<std::string> *reqTypes = nullptr;
		std::atomic<bool> *fin = nullptr;
		Atom sel = None;
		Atom requested = None;
		bool sent_request = false;
	};
	std::stack<SelectionRequestInfo> _sel_stack;
};

////////////////////////////////////////

} }

