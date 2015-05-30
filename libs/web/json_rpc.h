
#pragma once

#include <base/json.h>
#include <memory>
#include <typeinfo>

namespace web
{

////////////////////////////////////////

inline void from_json( const base::json &j, std::string &v )
{
	v = j.get<base::json_string>();
}

inline void from_json( const base::json &j, int64_t &v )
{
	v = j.get<base::json_number>();
}

inline void from_json( const base::json &j, bool &v )
{
	v = j.get<base::json_bool>();
}

////////////////////////////////////////

inline void to_json( const std::string &v, base::json &j )
{
	j.set<base::json_string>( v );
}

inline void to_json( const char *v, base::json &j )
{
	j.set<base::json_string>( v );
}

inline void to_json( int64_t v, base::json &j )
{
	j.set<base::json_number>( v );
}

inline void to_json( bool v, base::json &j )
{
	j.set<base::json_bool>( v );
}

////////////////////////////////////////

namespace detail
{

template<typename T>
T get( const base::json &params, size_t p )
{
	precondition( params.is<base::json_array>(), "expected parameters to be json array" );
	using web::from_json;
	T result;
	try
	{
		from_json( params.at( p ), result );
	}
	catch( ... )
	{
		throw_add( "invalid json RPC type parameter {0}, expected {1} ({2})", p, typeid(T).name(), params[p] );
	}

	return std::move( result );
}

template<typename T>
void put( base::json &params, const T &v, size_t p )
{
	using web::to_json;
	to_json( v, params[p] );
}

class base_function
{
public:
	virtual ~base_function( void );
	virtual void call( base::json &result, const base::json &in ) = 0;
};

template<typename F> class function;

template<typename R,typename ...Args>
class function<R(Args...)> : public base_function
{
public:
	function( const std::function<R(Args...)> &f )
		: _func( f )
	{
	}

	function( void )
	{
	}

	std::function<R(Args...)> &f( void )
	{
		return _func;
	}

	void call( base::json &result, const base::json &in ) override
	{
		typename base::gen_sequence<sizeof...(Args)>::type seq;
		call_helper( result, in, seq );
	}

private:
	typedef std::tuple<Args...> tuple_type;

	template<size_t ...S>
	void call_helper( base::json &result, const base::json &in, base::sequence<S...> )
	{
		using web::to_json;
		auto tmp = _func( get<typename std::remove_cv<typename std::remove_reference<typename std::tuple_element<S,tuple_type>::type>::type>::type>( in, S )... );
		to_json( tmp, result );
	}

	std::function<R(Args...)> _func;
};

template<typename ...Args>
class function<void(Args...)> : public base_function
{
public:
	function( const std::function<void(Args...)> &f )
		: _func( f )
	{
	}

	function( void )
	{
	}

	std::function<void(Args...)> &f( void )
	{
		return _func;
	}


	void call( base::json &result, const base::json &in ) override
	{
		result.set<base::json_null>();
		typename base::gen_sequence<sizeof...(Args)>::type seq;
		call_helper( in, seq );
	}

private:
	typedef std::tuple<Args...> tuple_type;

	template<size_t ...S>
	void call_helper( const base::json &in, base::sequence<S...> )
	{
		_func( get<typename std::remove_cv<typename std::remove_reference<typename std::tuple_element<S,tuple_type>::type>::type>::type>( in, S )... );
	}

	std::function<void(Args...)> _func;
};

}

////////////////////////////////////////

class json_rpc
{
public:
	json_rpc( void );

	template<typename F>
	std::function<F> &method( const char *name )
	{
		auto f = std::make_shared<detail::function<F>>();
		_function[name] = f;
		return f->f();
	}

	base::json local_call( const std::string &rpc );
	base::json local_call( const base::json &rpc );

	template<typename T, typename ...Args>
	base::json remote_call( const std::string &method, Args ...args )
	{
		base::json result;
		result["jsonrpc"] = "2.0";
		result["method"] = method;
		result["id"] = 1;

		base::json params;
		push_helper( params, args... );
		result["params"] = std::move( params );
		std::cout << result << std::endl;
		return std::move( result );
	}

private:
	template<typename Arg, typename ...Args>
	void push_helper( base::json &params, Arg a, Args ...args )
	{
		using web::to_json;
		to_json( a, params.push_back() );
		push_helper( params, args... );
	}

	base::json call( const char *name, const base::json &param );

	std::map<std::string,std::shared_ptr<detail::base_function>> _function;
};

////////////////////////////////////////

}

