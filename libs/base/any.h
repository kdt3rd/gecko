// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>
#include <utility>
#include <algorithm>
#include <typeinfo>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include "contract.h"
#include "compiler_abi.h"

namespace base
{

class bad_any_cast : public std::bad_cast
{
public:
	bad_any_cast( void ) = default;
	~bad_any_cast( void ) override;
	bad_any_cast( const bad_any_cast & ) = default;
	bad_any_cast( bad_any_cast && ) = default;
	bad_any_cast &operator=( const bad_any_cast & ) = default;
	bad_any_cast &operator=( bad_any_cast && ) = default;

	const char *what() const noexcept override;
};
#define throw_bad_any_cast() \
	throw_location( base::bad_any_cast() )

template <typename T>
struct in_place_type {};
// need c++17 to have inline variables...
//template <typename T>
//inline constexpr in_place_type_t<T> in_place_type {};

////////////////////////////////////////

/// @brief Holds a value of any type.
///
/// This is similar to a void pointer (void *), but is type-safe.
///
/// This has been re-written to mimic the C++17 standard version as
/// much as possible, such that when c++17 is a reality for most
/// people, this can be easily swapped and replaced.
///
/// Unfortunately, both gcc and libcxx code is fully using the new std
/// type_traits types, so can't be copied more directly
class any final
{
private:
	// rather than require a virtual base class and always requiring a
	// new / delete, we burn a function pointer here, which should
	// replace the vtable pointer and enable us to have small value
	// optimization for free
	
	union val_storage
	{
		constexpr inline val_storage() : ptr(nullptr) {}
		val_storage(const val_storage &) = delete;
		val_storage(val_storage &&) = delete;

		inline void *buffer( void ) noexcept { return &buf; }
		inline const void *buffer( void ) const noexcept { return &buf; }

		void *ptr;
		// using 3 here to match libcxx, which enables this to cover shared_ptr,
		// etc. which seems like a good idea, but then is a total of 4 pointers size
		// for this object, so 32 bytes instead of just 24 bytes, so a vector of any
		// will align onto 64 byte L1
		std::aligned_storage<3 * sizeof(void *), alignof(void *)>::type buf;
	};

	template <typename T, typename safeT = std::is_nothrow_move_constructible<T>,
			  bool fits = (sizeof(T) <= sizeof(val_storage)) && (alignof(T) <= alignof(val_storage))>
		using internal_storage = std::integral_constant<bool, safeT::value && fits>;

	enum class val_op
	{
		tinfo, get, copy, destroy, move, size
	};

	using proc_func = void * (*) ( val_op, const any *, any *, const std::type_info *tinfo );

	template <typename T> struct local_store
	{
		template <typename ...Args>
		static inline T &create( any &dest, Args &&... args )
		{
			T *r = ::new (dest._store.buffer()) T( std::forward<Args>(args)... );
			dest._processor = &local_store::apply;
			return *r;
		}

		static inline void destroy( any &t )
		{
			T *v = static_cast<T *>( t._store.buffer() );
			v->~T();
			t._processor = nullptr;
		}

		static inline void *apply( val_op o, const any *v, any *a, const std::type_info *tinfo )
		{
			switch ( o )
			{
				case val_op::tinfo:
					return const_cast<void *>( static_cast<const void *>( &typeid( T ) ) );

				case val_op::size:
					return reinterpret_cast<void *>( uintptr_t( sizeof(T) ) );

				case val_op::get:
					if ( tinfo && *tinfo == typeid(T) )
					{
						auto p = static_cast<const T *>( v->_store.buffer() );
						return const_cast<T *>( p );
					}
					return nullptr;

				case val_op::copy:
				{
					auto p = static_cast<const T *>( v->_store.buffer() );
					create( *a, *p );
					break;
				}

				case val_op::destroy:
					destroy( *const_cast<any *>( v ) );
					break;

				case val_op::move:
				{
					auto p = static_cast<const T *>( v->_store.buffer() );
					create( *a, std::move( *const_cast<T *>( p ) ) );
					destroy( *const_cast<any *>( v ) );
					break;
				}
			}
			return nullptr;
		}
	};

	template <typename T> struct heap_store
	{
		template <typename ...Args>
		static inline T &create( any &dest, Args &&... args )
		{
			// TODO: Understand why libcxx uses a std::allocator, puts
			// memory in a unique_ptr and does in-place construction.
			// gcc does not (yet, anyway)
			// std::allocators seem only partly implemented in many c++11
			T *r = new T( std::forward<Args>(args)... );
			dest._store.ptr = r;
			dest._processor = &heap_store::apply;
			return *r;
		}

		static inline void destroy( any &t )
		{
			delete static_cast<T *>( t._store.ptr );
			t._processor = nullptr;
		}

		static inline void *apply( val_op o, const any *v, any *a, const std::type_info *tinfo )
		{
			switch ( o )
			{
				case val_op::tinfo:
					return const_cast<void *>( static_cast<const void *>( &typeid( T ) ) );

				case val_op::size:
					return reinterpret_cast<void *>( uintptr_t( sizeof(T) ) );

				case val_op::get:
					if ( tinfo && *tinfo == typeid(T) )
					{
						auto p = static_cast<const T *>( v->_store.ptr );
						return const_cast<T *>( p );
					}
					return nullptr;

				case val_op::copy:
				{
					auto p = static_cast<const T *>( v->_store.ptr );
					create( *a, *p );
					break;
				}

				case val_op::destroy:
					destroy( *const_cast<any *>( v ) );
					break;

				case val_op::move:
					// this is only called on empty / being-constructed objects
					// so don't have to do all the steps expected
					a->_store.ptr = const_cast<any *>( v )->_store.ptr;
					a->_processor = &heap_store::apply;
					const_cast<any *>( v )->_processor = nullptr;
					break;
			}
			return nullptr;
		}
	};

	proc_func _processor = nullptr;
	val_storage _store;

	inline void *call_proc( val_op op, any *o = nullptr, const std::type_info *tinfo = nullptr ) const
	{
		return _processor( op, this, o, tinfo );
	}

public:
	/// @brief Decay type alias
	template <typename T> using decay = typename std::decay<T>::type;
	/// @brief processor table type alias
	template <typename T> using processor = typename std::conditional< internal_storage<T>::value, local_store<T>, heap_store<T> >::type;

	/// @brief Default constructor
	/// It contains no value until one is assigned.
	///
	/// This is defined as constexpr such that it is safe to use in
	/// static non-local initialized objects (see cppreference.com or
	/// other reference for discussion)
	inline constexpr any( void ) noexcept : _processor( nullptr ) {}
	inline ~any( void ) { reset(); }

	inline any( const any &o )
		 : _processor( nullptr )
	{
		if ( o.has_value() )
			o.call_proc( val_op::copy, this, nullptr );
	}
	inline any( any &&o ) noexcept
		: _processor( nullptr )
	{
		if ( o.has_value() )
			o.call_proc( val_op::move, this );
	}

	// allow auto construction of any other type except any (to prevent recursion)
	template <typename T, typename Tp = decay<T>, typename = typename std::enable_if<std::is_copy_constructible<Tp>::value && !std::is_same<Tp, any>::value>::type>
		inline any( T &&val ) // NOLINT
	{
		processor<Tp>::create( *this, std::forward<T>( val ) );
	}

	///NB: we are not providing the exact in_place_t mechanism since
	/// that involves manipulating the std namespace which would be
	/// dangerous, but provide a similar mechanism in case anyone
	/// needs it, should allow future search and replace quite easily.
	template <typename T, typename ... Args, typename Tp = decay<T>, typename = typename std::enable_if<std::is_copy_constructible<Tp>::value && !std::is_same<Tp, any>::value>::type>
	explicit inline any( in_place_type<T>, Args &&... args )
	{
		processor<Tp>::create( *this, std::forward<Args>( args )... );
	}
	
	template <typename T, typename U, typename ... Args, typename Tp = decay<T>, typename = typename std::enable_if<std::is_copy_constructible<Tp>::value && !std::is_same<Tp, any>::value>::type>
	explicit inline any( in_place_type<T>, std::initializer_list<U> il, Args &&... args )
	{
		processor<Tp>::create( *this, il, std::forward<Args>( args )... );
	}

	/// no side effects on exception
	inline any &operator=( const any &o )
	{
		any( o ).swap( *this );
		return *this;
	}

	inline any &operator=( any &&o ) noexcept
	{
		any( std::move( o ) ).swap( *this );
		return *this;
	}

	template <typename T, typename Tp = decay<T>, typename = typename std::enable_if<std::is_copy_constructible<Tp>::value && !std::is_same<Tp, any>::value>::type>
	inline any &operator=( T &&rhs )
	{
		any( std::forward<T>( rhs ) ).swap( *this );
		return *this;
	}

	template <typename T, typename ... Args, typename Tp = decay<T>, typename = typename std::enable_if<std::is_constructible<Tp, Args...>::value && std::is_copy_constructible<Tp>::value >::type>
	inline Tp &emplace( Args &&... args )
	{
		reset();
		return processor<Tp>::create( *this, std::forward<Args>( args )... );
	}

	template <typename T, typename U, typename ... Args, typename Tp = decay<T>, typename = typename std::enable_if<std::is_constructible<Tp, Args...>::value && std::is_copy_constructible<Tp>::value >::type>
	inline Tp &emplace( std::initializer_list<T> il, Args &&... args )
	{
		reset();
		return processor<Tp>::create( *this, il, std::forward<Args>( args )... );
	}
	
	/// checks if the any object holds a value
	inline bool has_value( void ) const noexcept { return _processor != nullptr; }

	/// returns the c++ typeid of the contained object (or void if empty)
	inline const std::type_info& type() const noexcept
	{
		if ( has_value() )
			return *static_cast<const std::type_info *>( this->_processor( val_op::tinfo, nullptr, nullptr, nullptr ) );
		return typeid(void);
	}

	void reset( void ) noexcept
	{
		if ( has_value() )
			this->call_proc( val_op::destroy );
	}

	void swap( any &o ) noexcept
	{
		if ( this == &o )
			return;

		if ( has_value() && o.has_value() )
		{
			any tmp;
			o.call_proc( val_op::move, &tmp );
			this->call_proc( val_op::move, &o );
			tmp.call_proc( val_op::move, this );
		}
		else if ( has_value() )
			this->call_proc( val_op::move, &o );
		else if ( o.has_value() )
			o.call_proc( val_op::move, this );
	}

	/// NB: NON STANDARD INTERFACE
	///
	/// we currently use this to stream raw bytes to the hash function
	/// in engine.
	size_t size( void ) const noexcept
	{
		if ( has_value() )
			return reinterpret_cast<size_t>( this->call_proc( val_op::size ) );
		return 0;
	}

protected:
	template <typename T> friend const T *any_cast( const any * ) noexcept;
	template <typename T> friend T *any_cast( any * ) noexcept;

	inline bool is_typed( const std::type_info &t ) const
	{
		// TODO: safe to use pointer compare? probably not - could
		// have any things from plugins or something in the future
		return this->type() == t;
	}

	template <typename T>
	const T *as( void ) const noexcept
	{
		return static_cast<const T *>( this->call_proc( val_op::get, nullptr, &typeid(T) ) );
	}

	template <typename T>
	T *as( void ) noexcept
	{
		return static_cast<T *>( this->call_proc( val_op::get, nullptr, &typeid(T) ) );
	}
};

template <typename T, typename ... Args>
inline any make_any( Args &&... args )
{
	return any( in_place_type<T>{}, std::forward<Args>( args )... );
}

template <typename T, typename U, typename ... Args>
inline any make_any( std::initializer_list<U> il, Args &&... args )
{
	return any( in_place_type<T>{}, il, std::forward<Args>( args )... );
}

template <class T>
inline T any_cast( const any &a )
{
	auto p = any_cast<typename std::remove_cv<typename std::remove_reference<T>::type>::type>( &a );
	if ( p )
		return static_cast<T>( *p );
	throw_bad_any_cast();
}

template <class T>
inline T any_cast( any &a )
{
	auto p = any_cast<typename std::remove_cv<typename std::remove_reference<T>::type>::type>( &a );
	if ( p )
		return static_cast<T>( *p );
	throw_bad_any_cast();
}

namespace detail
{

template <typename T>
inline T any_cast_move_true( typename std::remove_reference<T>::type *p, std::true_type )
{
	return std::move( *p );
}

template <typename T>
inline T any_cast_move_true( typename std::remove_reference<T>::type *p, std::false_type )
{
	return *p;
}

} // namespace detail

template <class T>
inline T any_cast( any &&a )
{
	using can_move = std::integral_constant<
		bool,
		std::is_move_constructible<T>::value
		&& !std::is_lvalue_reference<T>::value>;

	auto p = any_cast<typename std::remove_reference<T>::type>( &a );
	if ( p == nullptr )
		throw_bad_any_cast();
	return detail::any_cast_move_true<T>( p, can_move() );
}

template <class T>
inline const T *any_cast( const any *a ) noexcept
{
	if ( a == nullptr )
		return nullptr;
	return a->as<T>();
}

template <class T>
inline T *any_cast( any *a ) noexcept
{
	if ( a == nullptr )
		return nullptr;
	return a->as<T>();
}

inline void swap( any &a, any &b )
{
	a.swap( b );
}

////////////////////////////////////////

} // namespace base

// in case we call std::swap
namespace std
{

inline void swap( base::any &a, base::any &b )
{
	a.swap( b );
}

} // namespace std


