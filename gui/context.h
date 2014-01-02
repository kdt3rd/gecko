
#pragma once

#include <core/scope_guard.h>
#include <draw/rect.h>

namespace gui
{

////////////////////////////////////////

class context
{
public:
	virtual ~context( void ) {}

	virtual void invalidate( const draw::rect &r ) = 0;

	static context &current( void );

protected:
	void push_context( void );
	void pop_context( void );

	template<typename func>
	void in_context( const func &f )
	{
		push_context();
		SCOPE_EXIT { pop_context(); };
		f();
	}

private:
	static std::vector<context *> _contexts;
};

////////////////////////////////////////

}

