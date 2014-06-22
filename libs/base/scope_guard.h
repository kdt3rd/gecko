
#pragma once

#include <utility>

////////////////////////////////////////

namespace base
{

template <class function>
class scope_guard
{
public:
	scope_guard( function f )
		: _f( std::move( f ) ), _active(true)
	{
	}

	~scope_guard( void )
	{
		if ( _active )
			_f();
	}

	void dismiss( void )
	{
		_active = false;
	}

	scope_guard( void ) = delete;

	scope_guard( const scope_guard & ) = delete;

	scope_guard &operator=( const scope_guard & ) = delete;

	scope_guard( scope_guard && rhs )
		: _f( std::move( rhs._f ) ), _active( rhs._active )
	{
		rhs.dismiss();
	}

private:
	function _f;
	bool _active;
};

namespace detail
{
	enum class scope_guardOnExit {};
	template <typename function>
	scope_guard<function> operator+=( scope_guardOnExit, function &&fn )
	{
		return scope_guard<function>( std::forward<function>( fn ) );
	}
}

}

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)

#define on_scope_exit \
	auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = base::detail::scope_guardOnExit()

