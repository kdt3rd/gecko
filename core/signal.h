
#include <functional>
#include <vector>

namespace core {

namespace detail {

/// proto_signal is the template implementation for callback list.
template<typename, typename> class proto_signal;   // undefined

/// collector_invocation invokes signal handlers differently depending on return type.
template<typename collector, typename callback> class collector_invocation;

/// collector_last returns the result of the last signal handler from a signal emission.
template<typename ret_type>
class collector_last
{
public:
	typedef ret_type collector_result;

	explicit collector_last( void )
		: _last()
	{
	}

	inline bool operator()( collector_result r )
	{
		_last = r;
		return true;
	}

	collector_result result( void )
	{
		return _last;
	}

private:
	collector_result _last;
};

/// collector_default implements the default signal handler collection behaviour.
template<typename result>
class collector_default : collector_last<result>
{
};

/// collector_default specialisation for signals with void return type.
template<>
class collector_default<void>
{
public:
	typedef void collector_result;
	void result( void )
	{
	}

	inline bool operator()(void)
	{
		return true;
	}
};

/// collector_invocation specialisation for regular signals.
template<class collector, typename result, typename ...args>
class collector_invocation<collector, result( args... )>
{
public:
	inline bool invoke( collector &c, const std::function<result( args... )> &cbf, args... a )
	{
		return c( cbf( a... ) );
	}
};

/// collector_invocation specialisation for signals with void return type.
template<class collector, typename ...args>
class collector_invocation<collector, void( args... )>
{
public:
	inline bool invoke( collector &c, const std::function<void( args... )> &cbf, args... a )
	{
		cbf( a... );
		return c();
	}
};

typedef std::uintptr_t connection;

/// proto_signal template specialised for the callback signature and collector.
template<class collector, typename ret_type, typename ...args>
class proto_signal<ret_type( args... ), collector> : private collector_invocation<collector, ret_type( args... )>
{
protected:
	typedef std::function<ret_type( args... )> callback;
	typedef typename callback::result_type result;
	typedef typename collector::collector_result collector_result;

private:
	/// signal_link implements a doubly-linked ring with ref-counted nodes containing the signal handlers.
	class signal_link
	{
	public:
		signal_link *next, *prev;
		callback function;
		int ref_count;

		explicit signal_link( const callback &cbf )
			: next( nullptr ), prev( nullptr ), function( cbf ), ref_count( 1 )
		{
		}

		~signal_link( void )
		{
			assert( ref_count == 0 );
		}

		void incref( void )
		{
			ref_count += 1;
			assert( ref_count > 0 );
		}

		void decref( void )
		{
			ref_count -= 1;
			if ( !ref_count )
				delete this;
			else
				assert( ref_count > 0 );
		}

		void unlink( void )
		{
			function = nullptr;
			if ( next )
				next->prev = prev;
			if ( prev )
				prev->next = next;
			decref();
			// leave intact ->next, ->prev for stale iterators
		}

		connection add_before( const callback &cb )
		{
			signal_link *link = new signal_link( cb );
			link->prev = prev; // link to last
			link->next = this;
			prev->next = link; // link from last
			prev = link;
			return connection( link );
		}

		bool deactivate( const callback &cbf )
		{
			if ( cbf == function )
			{
				function = nullptr;      // deactivate static head
				return true;
			}
			for ( auto *link = this->next ? this->next : this; link != this; link = link->next )
			{
				if ( cbf == link->function )
				{
					link->unlink();     // deactivate and unlink sibling
					return true;
				}
			}
			return false;
		}

		bool remove_sibling( connection id )
		{
			for ( auto *link = this->next ? this->next : this; link != this; link = link->next )
			{
				if ( id == connection( link ) )
				{
					link->unlink();     // deactivate and unlink sibling
					return true;
				}
			}
			return false;
		}
	};

	signal_link   *_callback_ring; // linked ring of callback nodes

	proto_signal( const proto_signal & ) = delete;
	proto_signal &operator=( const proto_signal & ) = delete;

	void ensure_ring( void )
	{
		if (!_callback_ring)
		{
			_callback_ring = new signal_link( callback() ); // ref_count = 1
			_callback_ring->incref(); // ref_count = 2, head of ring, can be deactivated but not removed
			_callback_ring->next = _callback_ring; // ring head initialization
			_callback_ring->prev = _callback_ring; // ring tail initialization
		}
	}

public:
	/// proto_signal constructor, connects default callback if non-NULL.
	proto_signal( const callback &method )
		: _callback_ring( nullptr )
	{
		if ( method != nullptr )
		{
			ensure_ring();
			_callback_ring->function = method;
		}
	}

	/// proto_signal destructor releases all resources associated with this signal.
	~proto_signal( void )
	{
		if ( _callback_ring )
		{
			while ( _callback_ring->next != _callback_ring )
				_callback_ring->next->unlink();
			assert( _callback_ring->ref_count >= 2 );
			_callback_ring->decref();
			_callback_ring->decref();
		}
	}

	/// Operator to add a new function or lambda as signal handler, returns a handler connection ID.
	connection operator+=( const callback &cb )
	{
		ensure_ring();
		return _callback_ring->add_before( cb );
	}

	/// Operator to remove a signal handler through it connection ID, returns if a handler was removed.
	bool operator-=( connection conn )
	{
		return _callback_ring ? _callback_ring->remove_sibling( conn ) : false;
	}

	/// Emit a signal, i.e. invoke all its callbacks and collect return types with the Collector.
	collector_result operator()( args ...a )
	{
		collector col;
		if ( !_callback_ring )
			return col.result();

		signal_link *link = _callback_ring;
		link->incref();
		do
		{
			if ( link->function != nullptr )
			{
				const bool continue_emission = this->invoke( col, link->function, a... );
				if ( !continue_emission )
					break;
			}
			signal_link *old = link;
			link = old->next;
			link->incref();
			old->decref();
		} while ( link != _callback_ring );
		link->decref();
		return col.result();
	}
};

}

/**
 * Signal is a template type providing an interface for arbitrary callback lists.
 * A signal type needs to be declared with the function signature of its callbacks,
 * and optionally a return result collector class type.
 * Signal callbacks can be added with operator+= to a signal and removed with operator-=, using
 * a callback connection ID return by operator+= as argument.
 * The callbacks of a signal are invoked with the emit() method and arguments according to the signature.
 * The result returned by emit() depends on the signal collector class. By default, the result of
 * the last callback is returned from emit(). Collectors can be implemented to accumulate callback
 * results or to halt a running emissions in correspondance to callback results.
 * The signal implementation is safe against recursion, so callbacks may be removed and
 * added during a signal emission and recursive emit() calls are also safe.
 * The overhead of an unused signal is intentionally kept very low, around the size of a single pointer.
 * Note that the Signal template types is non-copyable.
 */
template <typename signature, class collector = detail::collector_default<typename std::function<signature>::result_type> >
class signal: public detail::proto_signal<signature, collector>
{
public:
	typedef detail::proto_signal<signature, collector> proto_signal;
	typedef typename proto_signal::callback callback;

	/// Signal constructor, supports a default callback as argument.
	signal( const callback &method = callback() )
		: proto_signal( method )
	{
	}
};

/// This function creates a std::function by binding @a object to the member function pointer @a method.
template<class instance, class object_type, typename ret_type, typename ...args>
std::function<ret_type( args... )> slot( instance &o, ret_type(object_type::*m)( args... ) )
{
	return [&o, m]( args... a ) { return (o.*m)( a... ); };
}

/// This function creates a std::function by binding @a object to the member function pointer @a method.
template<class object_type, typename ret_type, typename ...args>
std::function<ret_type( args... )> slot( object_type *o, ret_type(object_type::*m) ( args... ) )
{
	return [o, m]( args... a ) { return (o->*m)( a... ); };
}

/// Keep signal emissions going while all handlers return !0 (true).
template<typename ret_type>
class collector_while
{
public:
	typedef ret_type collector_result;

	explicit collector_while( void )
	{
	}

	const collector_result &result( void )
	{
		return _result;
	}

	inline bool operator()( collector_result r )
	{
		_result = r;
		return _result ? true : false;
	}

private:
	collector_result _result;
};

/// Keep signal emissions going while all handlers return 0 (false).
template<typename ret_type>
class collector_until
{
public:
	typedef ret_type collector_result;

	explicit collector_until( void )
	{
	}

	const collector_result &result( void )
	{
		return _result;
	}

	inline bool operator()( collector_result r )
	{
		_result = r;
		return _result ? false : true;
	}

private:
	collector_result _result;
};

/// collector_vector returns the result of the all signal handlers from a signal emission in a std::vector.
template<typename ret_type>
class collector_vector
{
public:
	typedef std::vector<ret_type> collector_result;

	const collector_result &result( void )
	{
		return _result;
	}

	inline bool operator()( ret_type r )
	{
		_result.push_back( r );
		return true;
	}

private:
	collector_result _result;
};

}

