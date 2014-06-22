
#pragma once

#include <vector>
#include <core/scope_guard.h>
#include <core/rect.h>

namespace gui
{

////////////////////////////////////////

class context
{
public:
	virtual ~context( void ) {}

	virtual void invalidate( const core::rect &r ) = 0;

	static context &current( void );

protected:
	void push_context( void );
	void pop_context( void );

	template<typename func>
	void in_context( const func &f )
	{
		push_context();
		on_scope_exit += [&]() { pop_context(); };
		f();
	}

private:
	static std::vector<context *> _contexts;
};

////////////////////////////////////////

}

