
#pragma once

#include <draw/area.h>
#include "container.h"

namespace gui
{

class window;

////////////////////////////////////////

class builder
{
public:
	builder( const std::shared_ptr<window> &c );
	~builder( void );

	template <typename Layout, typename ...Args>
	std::shared_ptr<Layout> new_layout( Args ... args )
	{
		auto ret = std::make_shared<Layout>( _container->area(), std::forward<Args>( args )... );
		_container->add_layout( ret );
		return ret;
	}

	void add_layout( const std::shared_ptr<layout::layout> &l ) { _container->add_layout( l ); }

	void make_label( const std::shared_ptr<draw::area> &a, const std::string &txt );
	void make_button( const std::shared_ptr<draw::area> &a, const std::string &txt );

private:
	std::shared_ptr<container> _container;
};

////////////////////////////////////////

}

