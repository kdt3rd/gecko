
#pragma once

#include <iostream>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <base/contract.h>
#include <string>
#include <typeindex>

#include "meta.h"
#include "tuple_util.h"

namespace base
{

////////////////////////////////////////

template<typename... Ts>
class variant_helper;

template<typename F, typename... Ts>
class variant_helper<F, Ts...>
{
public:
	static void destroy( std::type_index id, void *data )
	{
		if ( id == typeid(F) )
			reinterpret_cast<F*>(data)->~F();
		else
			variant_helper<Ts...>::destroy( id, data );
	}

	static void move( std::type_index old_t, void *old_v, void *new_v )
	{
		if ( old_t == typeid(F) )
			new (new_v) F( std::move( *reinterpret_cast<F*>( old_v ) ) );
		else
			variant_helper<Ts...>::move( old_t, old_v, new_v );
	}

	static void copy( std::type_index old_t, const void *old_v, void *new_v )
	{
		if ( old_t == typeid(F) )
			new (new_v) F( *reinterpret_cast<const F*>( old_v ) );
		else
			variant_helper<Ts...>::copy( old_t, old_v, new_v );
	}

	template<typename T>
	static constexpr bool is_valid( void )
	{
		return std::is_base_of<F,T>::value || std::is_same<F,T>::value || variant_helper<Ts...>::template is_valid<T>();
	}
};

template<>
class variant_helper<>
{
public:
	static void destroy( std::type_index id, void *data )
	{
	}

	static void move( std::type_index old_t, void *old_v, void *new_v )
	{
	}

	static void copy( std::type_index old_t, const void *old_v, void *new_v )
	{
	}

	template<typename T>
	static constexpr bool is_valid( void )
	{
		return false;
	}
};

////////////////////////////////////////

template<typename ...Ts>
class visitor_helper;

template<typename T1, typename T2, typename ...Ts>
class visitor_helper<T1, T2, Ts...>
{
public:
	template<typename Result, typename Visitor, typename Variant>
	static Result visit( Visitor &v, const Variant &x )
	{
		if ( x.template is<T1>() )
			return v( x.template get<T1>() );
		else
			return visitor_helper<T2, Ts...>::template visit<Result,Visitor,Variant>( v, x );
	}

	template<typename Result, typename Visitor, typename Variant>
	static Result visit( Visitor &v, Variant &x )
	{
		if ( x.template is<T1>() )
			return v( x.template get<T1>() );
		else
			return visitor_helper<T2, Ts...>::template visit<Result,Visitor,Variant>( v, x );
	}
};

template<typename T>
class visitor_helper<T>
{
public:
	template<typename Result, typename Visitor, typename Variant>
	static Result visit( Visitor &v, const Variant &x )
	{
		if ( x.template is<T>() )
			return v( x.template get<T>() );
		else
			throw_runtime( "variant not visited ({0})", x.type_name() );
	}

	template<typename Result, typename Visitor, typename Variant>
	static Result visit( Visitor &v, Variant &x )
	{
		if ( x.template is<T>() )
			return v( x.template get<T>() );
		else
			throw_runtime( "variant not visited" );
	}
};

////////////////////////////////////////
//
template<typename Result, typename Visitor, typename Tuple, typename ...Variants>
class visitor_multi;

template<typename Result, typename Visitor, typename Tuple, typename Variant, typename V2, typename ...Variants>
class visitor_multi<Result,Visitor,Tuple,Variant,V2, Variants...>
{
public:
	visitor_multi( Visitor &v, Tuple &&t, const V2 &v2, const Variants &...variants )
		: _realv( v ), _values( std::move( t ) ), _rest( v2, variants... )
	{
	}

	template<typename Value>
	Result operator()( const Value &v )
	{
		auto t = std::tuple_cat( _values, std::tie( v ) );
		visitor_multi<Result, Visitor, decltype(t), V2, Variants...> newv( _realv, t, tuple_tail( _rest ) );
		return visit( newv, std::get<0>( _rest ) );
	}

private:
	Visitor &_realv;
	Tuple _values;
	std::tuple<V2, Variants...> _rest;
};

template<typename Result, typename Visitor, typename Tuple, typename Variant>
class visitor_multi<Result,Visitor,Tuple,Variant>
{
public:
	visitor_multi( Visitor &v, const Tuple &t, std::tuple<> nothing )
		: _realv( v ), _values( t )
	{
	}

	template<typename Value>
	Result operator()( const Value &v )
	{
		auto t = std::tuple_cat( _values, std::tie( v ) );
		apply( _realv, t );
	}

private:
	Visitor &_realv;
	Tuple _values;
};

////////////////////////////////////////

template<typename ...Ts>
class variant
{
public: 
	variant( void )
	{
	}

	variant( const variant<Ts...> &old )
		: _type_id( old._type_id )
	{
		Helper::copy( old._type_id, &old._data, &_data );
	}

	variant( variant<Ts...> &&old )
		: _type_id( old._type_id )
	{
		Helper::move( old._type_id, &old._data, &_data );
	}

	template<typename T, typename base::enable_if_any<int,std::is_base_of<Ts,typename std::remove_const<T>::type>::value...>::type * = nullptr>
	variant( T &&t )
	{
		set<T>( std::move( t ) );
	}

	template<typename T, typename base::enable_if_any<int,std::is_base_of<Ts,typename std::remove_const<T>::type>::value...>::type * = nullptr>
	variant( const T &t )
	{
		set<T>( t );
	}

	~variant( void )
	{
		Helper::destroy( _type_id, &_data );
	}

	// Serves as both the move and the copy asignment operator.
	variant<Ts...>& operator=( variant<Ts...> old )
	{
		std::swap( _type_id, old._type_id );
		std::swap( _data, old._data );
		return *this;
	}

	template<typename T>
	bool is( void ) const
	{
		return _type_id == typeid(T);
	}

	bool valid( void ) const
	{
		return _type_id != invalid_type();
	}

	template<typename T, typename... Args>
	void set( Args &&...args )
	{
		using TT = typename std::remove_reference<T>::type;

		static_assert( Helper::template is_valid<TT>(), "invalid invariant type" );

		// First we destroy the current contents
		Helper::destroy( _type_id, &_data );
		new (&_data) TT( std::forward<Args>( args )... );
		_type_id = typeid(TT);
	}

	template<typename T>
	T& get( void )
	{
		// It is a dynamic_cast-like behaviour
		if ( _type_id == typeid(T) )
			return *reinterpret_cast<T*>( &_data );
		else
			throw std::bad_cast();
	}

	template<typename T>
	const T& get( void ) const
	{
		// It is a dynamic_cast-like behaviour
		if ( _type_id == typeid(T) )
			return *reinterpret_cast<const T*>( &_data );
		else
			throw std::bad_cast();
	}

	void clear( void )
	{
		Helper::destroy( _type_id, &_data );
		_type_id = invalid_type();
	}

	const char *type_name( void ) const
	{
		return _type_id.name();
	}

private:
	static const size_t _data_size = static_max<sizeof(Ts)...>::value;
	static const size_t _data_align = static_max<alignof(Ts)...>::value;

	using DataType = typename std::aligned_storage<_data_size, _data_align>::type;

	using Helper = variant_helper<Ts...>;

	static inline std::type_index invalid_type( void )
	{
		return typeid(void);
	}

	std::type_index _type_id = invalid_type();
	DataType _data;
};

////////////////////////////////////////

template<typename Result = void, typename Visitor, typename ...Ts>
Result visit( Visitor &v, const variant<Ts...> &x )
{
	return visitor_helper<Ts...>::template visit<Result,Visitor,base::variant<Ts...>>( v, x );
}

////////////////////////////////////////

template<typename Result = void, typename Visitor, typename Variant1, typename Variant2, typename ...Variants>
static Result visit( Visitor &v, const Variant1 &x, const Variant2 &y, const Variants &...vs )
{
	visitor_multi<Result, Visitor, std::tuple<>, Variant1, Variant2, Variants...> newv( v, std::tuple<>(), y, vs... );
	visit( newv, x );
}

////////////////////////////////////////

}

