//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "layout.h"
#include <vector>

namespace layout
{

////////////////////////////////////////

class grid_layout : public layout
{
public:
	size_t add_columns( size_t n, double flex = 1.0, int32_t pri = 0 );
	size_t add_rows( size_t n, double flex = 1.0, int32_t pri = 0 );

	void add( const std::shared_ptr<area> &a, size_t x, size_t y, size_t w, size_t h );

	void add( const std::shared_ptr<area> &a, size_t x, size_t y )
	{
		add( a, x, y, 1, 1 );
	}

	void compute_bounds( void ) override;

	void compute_layout( void ) override;

private:
	struct cell
	{
		cell( const std::shared_ptr<area> &a, size_t x, size_t y, size_t w, size_t h )
			: _area( a ), _x( x ), _y( y ), _w( w ), _h( h )
		{
		}

		std::weak_ptr<area> _area;
		size_t _x, _y, _w, _h;
	};

	std::list<cell> _areas;
	std::vector<std::shared_ptr<area>> _cols;
	std::vector<std::shared_ptr<area>> _rows;
};

////////////////////////////////////////

}

