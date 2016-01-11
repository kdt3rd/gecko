
#pragma once

namespace layout
{

////////////////////////////////////////

/// @brief A layout
///
/// Layouts work in 2 passes.
/// The first pass computes the minimum size from areas.
/// The second pass computes the actual position and size of each area.
template<typename container, typename area>
class layout
{
public:
	virtual ~layout( void )
	{
	}

	virtual void set_pad( double left, double right, double top, double bottom ) = 0;
	virtual void set_spacing( double horiz, double vert ) = 0;

	virtual void recompute_minimum( container &a ) = 0;
	virtual void recompute_layout( container &a ) = 0;

	virtual void added( const std::shared_ptr<area> & )
	{
	}
};

////////////////////////////////////////

}

