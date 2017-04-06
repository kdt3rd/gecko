//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "layouts.h"

namespace layout
{

template class box_layout<gui::widget,gui::widget>;
template class form_layout<gui::widget,gui::widget>;
template class grid_layout<gui::widget,gui::widget>;
template class tree_layout<gui::widget,gui::widget>;

}
