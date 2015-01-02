
#pragma once

#include <vector>
#include <base/scope_guard.h>
#include <base/rect.h>

namespace gui
{

////////////////////////////////////////

/// @brief Context for gui.
class context
{
public:
	virtual ~context( void ) {}

	virtual void invalidate( const base::rect &r ) = 0;

	static context &current( void );

protected:
	void push_context( void );
	void pop_context( void );

	template<typename func>
	void in_context( const func &f )
	{
		push_context();
		on_scope_exit { pop_context(); };
		f();
	}

private:
	static std::vector<context *> _contexts;
};

////////////////////////////////////////

}

