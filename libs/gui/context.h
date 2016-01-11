
#pragma once

#include <base/scope_guard.h>
#include <base/rect.h>

namespace gui
{

////////////////////////////////////////

/// @brief Context for gui.
class context
{
public:
	context( void ) = default;
	virtual ~context( void );
	context( const context & ) = delete;
	context( context && ) = delete;
	context &operator=( const context & ) = delete;
	context &operator=( context && ) = delete;

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
};

////////////////////////////////////////

}

