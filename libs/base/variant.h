
#pragma once

#include <iostream>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <string>
#include <typeindex>

#include "contract.h"
#include "meta.h"
#include "tuple_util.h"

namespace base
{

namespace detail
{

////////////////////////////////////////

template<typename... Ts>
class variant_helper;

template<typename F, typename... Ts>
class variant_helper<F, Ts...>
{
public:
	static void destroy( std::size_t id, void *data )
	{
		if ( id == typeid(F).hash_code() )
			reinterpret_cast<F*>(data)->~F();
		else
			variant_helper<Ts...>::destroy( id, data );
	}

	static void move( std::size_t old_t, void *old_v, void *new_v )
	{
		if ( old_t == typeid(F).hash_code() )
			new (new_v) F( std::move( *reinterpret_cast<F*>( old_v ) ) );
		else
			variant_helper<Ts...>::move( old_t, old_v, new_v );
	}

	static void copy( std::size_t old_t, const void *old_v, void *new_v )
	{
		if ( old_t == typeid(F).hash_code() )
			new (new_v) F( *reinterpret_cast<const F*>( old_v ) );
		else
			variant_helper<Ts...>::copy( old_t, old_v, new_v );
	}

	template<typename T>
	static constexpr bool is_valid( void )
	{
		return std::is_base_of<F,T>::value || std::is_same<F,T>::value || variant_helper<Ts...>::template is_valid<T>();
	}

	static const char *type_name( std::size_t id )
	{
		if ( id == typeid(F).hash_code() )
			return typeid(F).name();
		return variant_helper<Ts...>::type_name( id );
	}
};

template<>
class variant_helper<>
{
public:
	static void destroy( std::size_t, void * )
	{
	}

	static void move( std::size_t, void *, void * )
	{
	}

	static void copy( std::size_t, const void *, void * )
	{
	}

	template<typename T>
	static constexpr bool is_valid( void )
	{
		return false;
	}

	static const char *type_name( std::size_t )
	{
		return typeid(void).name();
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
	visitor_multi( Visitor &v, const Tuple &t, std::tuple<> )
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

}

////////////////////////////////////////

/// @brief Variant type (aka tagged union).
/// The variant can take on a value from any of the given types (or have no value at all).
/// The visit(...) function can be used to apply any visitor class to the variant.
/// @sa base::visit
template<typename ...Ts>
class variant
{
public:
	/// @brief Constructor
	variant( void )
	{
	}

	/// @brief Copy constructor
	variant( const variant<Ts...> &old )
		: _type_id( old._type_id )
	{
		Helper::copy( old._type_id, &old._data, &_data );
	}

	/// @brief Move constructor
	variant( variant<Ts...> &&old )
		: _type_id( old._type_id )
	{
		Helper::move( old._type_id, &old._data, &_data );
	}

	/// @brief Constructor with move value.
	template<typename T, typename base::enable_if_any<int,std::is_base_of<Ts,typename std::remove_const<T>::type>::value...>::type * = nullptr>
	variant( T &&t )
	{
		set<T>( std::move( t ) );
	}

	/// @brief Constructor with value.
	template<typename T, typename base::enable_if_any<int,std::is_base_of<Ts,typename std::remove_const<T>::type>::value...>::type * = nullptr>
	variant( const T &t )
	{
		set<T>( t );
	}

	/// @brief Destructor
	~variant( void )
	{
		clear();
	}

	/// @brief Assignment operator.
	variant<Ts...>& operator=( const variant<Ts...> &old )
	{
		if ( &old != this )
		{
			clear();
			_type_id = old._type_id;
			Helper::copy( old._type_id, &old._data, &_data );
		}

		return *this;
	}

	/// @brief Assignment operator.
	variant<Ts...>& operator=( variant<Ts...> &&old )
	{
		clear();
		_type_id = old._type_id;
		Helper::move( old._type_id, &old._data, &_data );

		return *this;
	}

	/// @brief Check is we are holding the given type.
	template<typename T>
	bool is( void ) const
	{
		return _type_id == typeid(T).hash_code();
	}

	/// @brief Check is we have a value.
	bool valid( void ) const
	{
		return _type_id != invalid_type();
	}

	/// @brief Set value to the given type.
	template<typename T, typename... Args>
	void set( Args &&...args )
	{
		using TT = typename std::remove_reference<T>::type;

		static_assert( Helper::template is_valid<TT>(), "invalid variant type" );

		Helper::destroy( _type_id, &_data );
		new (&_data) TT( std::forward<Args>( args )... );
		_type_id = typeid(TT).hash_code();
	}

	/// @brief Get the value as the given type.
	template<typename T>
	T &get( void )
	{
		// It is a dynamic_cast-like behaviour
		if ( _type_id == typeid(T).hash_code() )
			return *reinterpret_cast<T*>( &_data );

		throw std::bad_cast();
	}

	/// @brief Get the value as the given type.
	template<typename T>
	const T &get( void ) const
	{
		// It is a dynamic_cast-like behaviour
		if ( _type_id == typeid(T).hash_code() )
			return *reinterpret_cast<const T*>( &_data );

		throw std::bad_cast();
	}

	/// @brief Clear the variant to empty.
	void clear( void )
	{
		Helper::destroy( _type_id, &_data );
		_type_id = invalid_type();
	}

	/// @brief Name of the current type.
	const char *type_name( void ) const
	{
		return Helper::type_name( _type_id );
	}

private:
	using Helper = detail::variant_helper<Ts...>;

	static inline std::size_t invalid_type( void )
	{
		return std::size_t(-1);//typeid(void).hash_code();
	}

	typedef typename aligned_union<0, Ts...>::type DataType;
	std::size_t _type_id = invalid_type();
	DataType _data;
};

////////////////////////////////////////

/// @brief Apply the visitor to the variant.
template<typename Result = void, typename Visitor, typename ...Ts>
Result visit( Visitor &v, const variant<Ts...> &x )
{
	return detail::visitor_helper<Ts...>::template visit<Result,Visitor,base::variant<Ts...>>( v, x );
}

////////////////////////////////////////

/// @brief Apply the visitor with multiple variants.
template<typename Result = void, typename Visitor, typename Variant1, typename Variant2, typename ...Variants>
static Result visit( Visitor &v, const Variant1 &x, const Variant2 &y, const Variants &...vs )
{
	detail::visitor_multi<Result, Visitor, std::tuple<>, Variant1, Variant2, Variants...> newv( v, std::tuple<>(), y, vs... );
	visit( newv, x );
}

////////////////////////////////////////

}

