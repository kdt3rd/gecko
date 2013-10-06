
#pragma once

////////////////////////////////////////

class form_layout
{
public:
	form_layout( const std::shared_ptr<area> &a );

	size_t size( void ) const { return _areas.size(); }

	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> new_line( void );

	std::pair<std::shared_ptr<area>,std::shared_ptr<area>> get_line( size_t i ) { return _areas.at( i ); }

private:
	tight_constraint _left, _right;

	std::vector<std::pair<std::shared_ptr<area>,std::shared_ptr<area>>> _areas;
};

////////////////////////////////////////

