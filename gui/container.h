
#pragma once

#include <view/view.h>
#include <layout/layout.h>
#include <reaction/reaction.h>
#include <map>

namespace gui
{

////////////////////////////////////////

class container : public view::view, public layout::layout, public reaction::reaction
{
public:
	container( const std::shared_ptr<::layout::area> &a );
	~container( void );

	const std::shared_ptr<::layout::area> &area( void ) { return _area; }

	void add_layout( const std::shared_ptr<::layout::layout> &l ) { _layouts.push_back( l ); }
	void add_view( const std::shared_ptr<::view::view> &v );
	void add_reaction( const std::shared_ptr<::reaction::reaction> &r ) { _reactions.push_back( r ); }

	virtual void layout( const std::shared_ptr<draw::canvas> &c );
	virtual void paint( const std::shared_ptr<draw::canvas> &c );

	virtual void recompute_minimum( void );
	virtual void recompute_layout( void );

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_release( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );

private:
	std::shared_ptr<::layout::area> _area;
	std::vector<std::shared_ptr<::view::view>> _views;
	std::vector<std::shared_ptr<::layout::layout>> _layouts;
	std::vector<std::shared_ptr<reaction>> _reactions;
};

////////////////////////////////////////

}

