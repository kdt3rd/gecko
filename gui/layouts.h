
#pragma once

#include "widget.h"

#include <layout/box_layout.h>
#include <layout/form_layout.h>
//#include <layout/grid_layout.h>
//#include <layout/tree_layout.h>

////////////////////////////////////////

namespace gui
{
	typedef layout::box_layout<widget,widget> box_layout;
	typedef layout::form_layout<widget,widget> form_layout;
//	typedef layout::grid_layout<widget,widget> grid_layout;
//	typedef layout::tree_layout<widget,widget> tree_layout;
}

////////////////////////////////////////

namespace layout
{
	extern template class box_layout<gui::widget,gui::widget>;
	extern template class form_layout<gui::widget,gui::widget>;
//	extern template class grid_layout<gui::widget,gui::widget>;
//	extern template class tree_layout<gui::widget,gui::widget>;
}

////////////////////////////////////////

