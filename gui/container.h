
#pragma once

#include <view/view.h>
#include <layout/layout.h>
#include <react/reactor.h>
#include <map>

namespace gui
{

////////////////////////////////////////

class container : public view::view, public layout::layout, public react::reactor
{
public:
	container( const std::shared_ptr<draw::area> &a );
	~container( void );

	const std::shared_ptr<draw::area> &area( void ) { return _area; }

	void add_layout( const std::shared_ptr<layout::layout> &l ) { _layouts.push_back( l ); }
	void add_view( const std::shared_ptr<view::view> &v ) { _views.push_back( v ); }

	virtual void layout( const std::shared_ptr<draw::canvas> &c );
	virtual void paint( const std::shared_ptr<draw::canvas> &c );

	virtual void recompute_minimum( void );
	virtual void recompute_layout( void );

	virtual bool mousePress( const draw::point &p, int button );
	virtual bool mouseRelease( const draw::point &p, int button );
	virtual bool mouseMove( const draw::point &p );

private:
	std::shared_ptr<draw::area> _area;
	std::vector<std::shared_ptr<view::view>> _views;
	std::vector<std::shared_ptr<layout::layout>> _layouts;
	std::vector<std::shared_ptr<reactor>> _reactors;
};

////////////////////////////////////////

}

