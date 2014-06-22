
#pragma once

#include <base/signal.h>

namespace model
{

template<typename T> class field;

////////////////////////////////////////

class record
{
public:
	template<typename T> using field = model::field<T>;
	template<typename T> using shared_field = field<std::shared_ptr<T>>;

	base::signal<void(void)> changes;

	inline void changed( void )
	{
		if ( _delay <= 0 )
		{
			changes();
			_changed = false;
		}
		else
			_changed = true;
	}

	inline void delay_changes( void )
	{
		++_delay;
	}

	inline void resume( void )
	{
		--_delay;
		if ( _delay <= 0 && _changed)
			changes();
	}

private:
	int _delay = 0;
	bool _changed = false;
};

////////////////////////////////////////

}

