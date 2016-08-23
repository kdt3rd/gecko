
#pragma once

#include "expression.h"

namespace layout
{

////////////////////////////////////////

class area
{
public:
	area( const std::string &n );
	virtual ~area( void );

	const variable &left( void ) const { return _left; }
	const variable &right( void ) const { return _right; }
	const variable &top( void ) const { return _top; }
	const variable &bottom( void ) const { return _bottom; }

	expression width( void ) const { return _right - _left; }
	expression height( void ) const { return _bottom - _top; }

	const variable &minimum_width( void ) const { return _min_width; }
	const variable &minimum_height( void ) const { return _min_height; }

protected:
	variable _left, _right;
	variable _top, _bottom;

	variable _min_width;
	variable _min_height;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const area &a )
{
	out << a.left().value() << "-" << a.right().value() << "#" << a.top().value() << "-" << a.bottom().value();
	return out;
}

////////////////////////////////////////

}

