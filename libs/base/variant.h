
#pragma once

#include <iostream>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <string>

#include "meta.h"

namespace base
{

////////////////////////////////////////

template<typename... Ts>
class variant_helper;

template<typename F, typename... Ts>
class variant_helper<F, Ts...>
{
public:
	inline static void destroy( size_t id, void *data )
	{
		if ( id == typeid(F).hash_code() )
			reinterpret_cast<F*>(data)->~F();
		else
			variant_helper<Ts...>::destroy( id, data );
	}

	inline static void move( size_t old_t, void *old_v, void *new_v )
	{
		if ( old_t == typeid(F).hash_code() )
			new (new_v) F( std::move( *reinterpret_cast<F*>( old_v ) ) );
		else
			variant_helper<Ts...>::move( old_t, old_v, new_v );
	}

	inline static void copy(size_t old_t, const void *old_v, void *new_v)
	{
		if ( old_t == typeid(F).hash_code() )
			new (new_v) F( *reinterpret_cast<const F*>( old_v ) );
		else
			variant_helper<Ts...>::copy( old_t, old_v, new_v );
	}
};

template<>
class variant_helper<>
{
public:
	inline static void destroy( size_t id, void *data )
	{
	}

	inline static void move( size_t old_t, void *old_v, void *new_v )
	{
	}

	inline static void copy( size_t old_t, const void *old_v, void *new_v )
	{
	}
};

////////////////////////////////////////

template<typename ...Ts>
class visitor_helper;

template<typename T, typename ...Ts>
class visitor_helper<T, Ts...>
{
public:
	template<typename Variant, typename Visitor>
	static void visit( const Variant &x, Visitor &v )
	{
		if ( x.template is<T>() )
			v( x.template get<T>() );
		else
			visitor_helper<Ts...>::visit( x, v );
	}

	template<typename Variant, typename Visitor>
	static void visit( Variant &x, Visitor &v )
	{
		if ( x.template is<T>() )
			v( x.template get<T>() );
		else
			visitor_helper<Ts...>::visit( x, v );
	}
};

template<>
class visitor_helper<>
{
public:
	template<typename Variant, typename Visitor>
	static void visit( const Variant &x, Visitor &v )
	{
	}

	template<typename Variant, typename Visitor>
	static void visit( Variant &x, Visitor &v )
	{
	}
};

////////////////////////////////////////

template<typename ...Ts>
class variant
{
public: 
	variant( void )
		: _type_id( invalid_type() )
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
		return _type_id == typeid(T).hash_code();
	}

	bool valid( void ) const
	{
		return _type_id != invalid_type();
	}

	template<typename T, typename... Args>
	void set( Args &&...args )
	{
		// First we destroy the current contents	
		Helper::destroy( _type_id, &_data );
		new (&_data) T( std::forward<Args>( args )... );
		_type_id = typeid(T).hash_code();
	}

	template<typename T>
	T& get( void )
	{
		// It is a dynamic_cast-like behaviour
		if ( _type_id == typeid(T).hash_code() )
			return *reinterpret_cast<T*>( &_data );
		else
			throw std::bad_cast();
	}

	template<typename T>
	const T& get( void ) const
	{
		// It is a dynamic_cast-like behaviour
		if ( _type_id == typeid(T).hash_code() )
			return *reinterpret_cast<const T*>( &_data );
		else
			throw std::bad_cast();
	}

	template<typename Visitor>
	static void visit( Visitor &v, const base::variant<Ts...> &x )
	{
		visitor_helper<Ts...>::visit( x, v );
	}

private: 
	static const size_t _data_size = static_max<sizeof(Ts)...>::value;
	static const size_t _data_align = static_max<alignof(Ts)...>::value;

	using DataType = typename std::aligned_storage<_data_size, _data_align>::type;

	using Helper = variant_helper<Ts...>;
	
	static inline size_t invalid_type( void )
	{
		return typeid(void).hash_code();
	}

	size_t _type_id;
	DataType _data;
};

////////////////////////////////////////

}

