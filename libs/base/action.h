
#pragma once

#include <functional>

template<typename> class action; // undefined

template<typename Result, typename ... Args>
class action<Result(Args...)>
{
public:
	typedef std::function<Result(Args...)> Function;

	template<typename Functor>
	void callback( const Functor &c )
	{
		_action = c;
	}

	void operator()( Args ...args )
	{
		if ( _action )
			_action( std::forward<Args>( args )... );
	}

private:
	Function _action;
};

