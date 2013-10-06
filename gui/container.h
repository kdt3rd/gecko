
#pragma once

#include <memory>
#include <vector>
#include "area.h"

////////////////////////////////////////

class container
{
public:
	container( void );

	const std::shared_ptr<area> &bounds( void ) const { return _areas.front(); }

	const std::shared_ptr<area> &new_area( void ) { _areas.push_back( std::make_shared<area>() ); return _areas.back(); }

	std::vector<std::shared_ptr<area>>::iterator begin( void ) { return _areas.begin(); }
	std::vector<std::shared_ptr<area>>::iterator end( void ) { return _areas.end(); }


private:
	std::vector<std::shared_ptr<area>> _areas;
};

////////////////////////////////////////

