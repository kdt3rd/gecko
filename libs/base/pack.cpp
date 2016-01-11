//
// Copyright (c) 2012 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "pack.h"
#include <limits>


////////////////////////////////////////


namespace base
{


////////////////////////////////////////


pack::pack( void )
{
}


////////////////////////////////////////


pack::pack( int w, int h, bool allowFlipped )
{
	reset( w, h, allowFlipped );
}


////////////////////////////////////////


pack::~pack( void )
{
}


////////////////////////////////////////


void
pack::reset( int w, int h, bool allowFlipped )
{
	_width = w;
	_height = h;
	_allowFlipped = allowFlipped;

	_usedSurfaceArea = 0;
	_skyLine.clear();
	_skyLine.emplace_back( 0, 0, w );

	_usedRects.clear();
	_freeRects.clear();
}


////////////////////////////////////////


pack::area
pack::insert( int aw, int ah )
{
	area node = waste_insert( aw, ah );

	if ( node.height != 0 )
	{
		_usedSurfaceArea += aw * ah;
		return node;
	}

	int bH, bWA;
	size_t bIdx;
	node = find_min_waste( aw, ah, bH, bWA, bIdx );
	if ( bIdx != size_t(-1) )
	{
		add( bIdx, node );
		_usedSurfaceArea += aw * ah;
	}
	else
		node.clear();

	return node;
}


////////////////////////////////////////


bool
pack::fits( size_t idx, int w, int h, int &y, int &wastedArea ) const
{
	int x = _skyLine[idx].x;

	if ( x + w > _width )
		return false;

	int widthLeft = w;
	size_t i = idx;
	y = _skyLine[idx].y;

	while ( widthLeft > 0 )
	{
		y = std::max( y, _skyLine[i].y );
		if ( y + h > _height )
			return false;

		widthLeft -= _skyLine[i].width;
		++i;
	}

	wastedArea = 0;
	const int rectLeft = _skyLine[idx].x;
	const int rectRight = rectLeft + w;

	for ( ; idx < _skyLine.size() && _skyLine[idx].x < rectRight; ++idx )
	{
		const node &cur = _skyLine[idx];
		if ( cur.x >= rectRight || cur.x + cur.width <= rectLeft )
			break;

		int leftSide = cur.x;
		int rightSide = std::min( rectRight, leftSide + cur.width );

		wastedArea += ( rightSide - leftSide ) * ( y - cur.y );
	}

	return true;
}


////////////////////////////////////////


pack::area
pack::find_min_waste( int aw, int ah, int &bestH, int &bestWA, size_t &idx )
{
	bestH = std::numeric_limits<int>::max();
	bestWA = bestH;
	idx = size_t(-1);

	area retval;

	for ( size_t i = 0; i < _skyLine.size(); ++i )
	{
		int y, wastedArea;

		if ( fits( i, aw, ah, y, wastedArea ) )
		{
			if ( wastedArea < bestWA || ( wastedArea == bestWA && ( y + ah ) < bestH ) )
			{
				bestH = y + ah;
				bestWA = wastedArea;
				idx = i;
				retval = { _skyLine[i].x, y, aw, ah };
			}
		}

		if ( _allowFlipped && fits( i, ah, aw, y, wastedArea ) )
		{
			if ( wastedArea < bestWA || ( wastedArea == bestWA && ( y + aw ) < bestH ) )
			{
				bestH = y + aw;
				bestWA = wastedArea;
				idx = i;
				retval = { _skyLine[i].x, y, ah, aw };
			}
		}
	}

	return retval;
}


////////////////////////////////////////


void
pack::add( size_t idx, const area &a )
{
	add_waste( idx, a.width, a.height, a.y );

	_skyLine.emplace( _skyLine.begin() + long(idx), a.x, a.y + a.height, a.width );

	for ( size_t i = idx + 1; i < _skyLine.size(); ++i )
	{
		node &cur = _skyLine[i];
		const node &prev = _skyLine[i - 1];
		if ( cur.x < prev.x + prev.width )
		{
			int shrink = prev.x + prev.width - cur.x;
			cur.x += shrink;
			cur.width -= shrink;

			if ( cur.width <= 0 )
			{
				_skyLine.erase( _skyLine.begin() + long(i) );
				--i;
				continue;
			}
		}

		break;
	}
	merge();
}


////////////////////////////////////////


void
pack::merge( void )
{
	for ( size_t i = 0; i < _skyLine.size() - 1; ++i )
	{
		node &cur = _skyLine[i];
		const node &next = _skyLine[i + 1];
		if ( cur.y == next.y )
		{
			cur.width += next.width;
			_skyLine.erase( _skyLine.begin() + long(i + 1) );
			--i;
		}
	}
}


////////////////////////////////////////


pack::area
pack::waste_insert( int aw, int ah )
{
	size_t idx = size_t(-1);
	area retval;
	int bestScore = std::numeric_limits<int>::max();

	for ( size_t i = 0; i < _freeRects.size(); ++i )
	{
		const area &cur = _freeRects[i];
		if ( cur.matches( aw, ah ) )
		{
			if ( _allowFlipped || !cur.flipped( aw, ah ) )
			{
				// exact match;
				retval = cur;
				idx = i;
				bestScore = std::numeric_limits<int>::min();
				break;
			}
		}

		if ( cur.fits_normal( aw, ah ) )
		{
			int score = cur.score( aw, ah );
			if ( score < bestScore )
			{
				retval = cur;
				retval.width = aw;
				retval.height = ah;
				bestScore = score;
				idx = i;
			}
		}
		else if ( _allowFlipped && cur.fits_flipped( aw, ah ) )
		{
			int score = cur.score( ah, aw );
			if ( score < bestScore )
			{
				retval = cur;
				retval.width = ah;
				retval.height = aw;
				bestScore = score;
				idx = i;
			}
		}
	}

	if ( retval.height == 0 || idx == size_t(-1) )
		return retval;

	area &leftover = _freeRects[idx];
	const int leftoverW = leftover.width - retval.width;
	const int leftoverH = leftover.height - retval.height;

	area bottom = leftover, right = leftover;

	bottom.y += retval.height;
	bottom.height -= retval.height;
	right.x += retval.width;
	right.width -= retval.width;

	// splithorizontal
	if ( retval.width * leftoverH <= leftoverW * retval.height )
	{
		bottom.width = leftover.width;
		right.height = retval.height;
	}
	else
	{
		bottom.width = retval.width;
		right.height = leftover.height;
	}

	_freeRects.erase( _freeRects.begin() + long(idx) );
	if ( bottom.width > 0 && bottom.height > 0 )
		_freeRects.emplace_back( bottom );
	if ( right.width > 0 && right.height > 0 )
		_freeRects.emplace_back( right );

	for ( size_t i = 0; i < _freeRects.size(); ++i )
	{
		for ( size_t j = i+1; j < _freeRects.size(); ++j )
		{
			area &curI = _freeRects[i];
			const area &curJ = _freeRects[j];
			if ( curI.width == curJ.width && curI.x == curJ.x )
			{
				if ( curI.y == curJ.y + curJ.height )
				{
					curI.y -= curJ.height;
					curI.height += curJ.height;
					_freeRects.erase( _freeRects.begin() + long(j) );
					--j;
				}
				else if ( curI.y + curI.height == curJ.y )
				{
					curI.height += curJ.height;
					_freeRects.erase( _freeRects.begin() + long(j) );
					--j;
				}
			}
			else if ( curI.height == curJ.height && curI.y == curJ.y )
			{
				if ( curI.x == curJ.x + curJ.width )
				{
					curI.x -= curJ.width;
					curI.width += curJ.width;
					_freeRects.erase( _freeRects.begin() + long(j) );
					--j;
				}
				else if ( curI.x + curI.width == curJ.x )
				{
					curI.width += curJ.width;
					_freeRects.erase( _freeRects.begin() + long(j) );
					--j;
				}
			}
		}
	}

	_usedRects.emplace_back( retval );

	return retval;
}


////////////////////////////////////////


void
pack::add_waste( size_t idx, int w, int /*h*/, int y )
{
	const int rectLeft = _skyLine[idx].x;
	const int rectRight = rectLeft + w;

	for ( ; idx < _skyLine.size() && _skyLine[idx].x < rectRight; ++idx )
	{
		const node &cur = _skyLine[idx];
		if ( cur.x >= rectRight || cur.x + cur.width <= rectLeft )
			break;

		int leftSide = cur.x;
		int rightSide = std::min( rectRight, leftSide + cur.width);

		_freeRects.emplace_back( leftSide, cur.y, rightSide - leftSide, y - cur.y );
	}
}


////////////////////////////////////////


}



