
#include "popup.h"
#include <platform/window.h>

namespace gui
{

////////////////////////////////////////

popup::popup( const std::shared_ptr<platform::window> &w )
	: window( w )
{
	precondition( bool(_window), "null window" );
	_window->set_popup();
}

}
