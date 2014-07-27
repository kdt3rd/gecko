
#pragma once

#include <functional>

namespace base
{

////////////////////////////////////////

/// @brief Undefined generic action
///
/// A callback class that must be specialized.
template<typename> class action;

////////////////////////////////////////

/// @brief A callback that stores a function.
template<typename Result, typename ... Args>
class action<Result(Args...)>
{
public:
	/// @brief Type fo the function
	typedef std::function<Result(Args...)> Function;

	/// @brief Set the function to be called
	///
	/// The Functor must be assignable to the Function type.
	template<typename Functor>
	void callback( const Functor &c )
	{
		_action = c;
	}

	/// @brief Call the stored function
	void operator()( Args ...args )
	{
		if ( _action )
			_action( std::forward<Args>( args )... );
	}

private:
	Function _action;
};

////////////////////////////////////////

}

