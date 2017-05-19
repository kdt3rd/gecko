//
// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <utility>

////////////////////////////////////////

namespace base
{

/// @brief Run a function, unless dismiss is called
///
/// Store a function and call it upon destruction, unless dismiss is called first.
template <class function>
class scope_guard
{
public:
	/// @brief Constructor.
	explicit scope_guard( function f )
		: _f( std::move( f ) ), _active(true)
	{
	}

	/// @brief Destructor.
	~scope_guard( void )
	{
		if ( _active )
			_f();
	}

	/// @brief Dismiss the guard.
	void dismiss( void )
	{
		_active = false;
	}

	scope_guard( void ) = delete;

	scope_guard( const scope_guard & ) = delete;
	scope_guard &operator=( const scope_guard & ) = delete;

	/// @brief Move constructor.
	scope_guard( scope_guard && rhs ) noexcept
		: _f( std::move( rhs._f ) ), _active( rhs._active )
	{
		rhs.dismiss();
	}

	scope_guard &operator=( scope_guard &&rhs ) noexcept
	{
		_f = std::move( rhs._f );
		_active = rhs._active;
		rhs.dismiss();
		return *this;
	}

private:
	function _f;
	bool _active;
};

/// @brief Create a scope guard from a function.
template<class function>
scope_guard<function> make_guard( function f )
{
	return scope_guard<function>( f );
}

namespace detail
{
	enum class scope_guardOnExit {};
	template <typename function>
	scope_guard<function> operator+=( scope_guardOnExit, function &&fn )
	{
		return scope_guard<function>( std::forward<function>( fn ) );
	}
} // namespace detail

} // namespace base

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)

/// @brief Execute a code block on scope exit
#define on_scope_exit \
	auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = base::detail::scope_guardOnExit() += [&](void)

