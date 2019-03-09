// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include "format.h"

////////////////////////////////////////

namespace base
{

namespace detail
{

template <typename SV,
		  bool = std::enable_if< std::is_copy_constructible<typename SV::value_type>::value ||
								 std::is_nothrow_move_constructible<typename SV::value_type>::value,
								 std::true_type >::type::value >
struct shrink_helper
{
	static void do_it( SV & ) {}
};

template <typename SV>
struct shrink_helper<SV, true>
{
	static void do_it( SV &a )
	{
		try
		{
			SV( std::make_move_iterator( a.begin() ),
				std::make_move_iterator( a.end() ),
				a.get_allocator() ).swap( a );
		}
		catch ( ... )
		{
		}
	}
};

template <typename InIter, typename FwdIter, typename Alloc>
inline FwdIter uninit_copy_a( InIter f, InIter l, FwdIter dest, Alloc &a )
{
	using value_type = typename std::iterator_traits<FwdIter>::value_type;
	using allocator_traits = typename std::allocator_traits<Alloc>;
	FwdIter cur = dest;
	try
	{
		for ( ; f != l; ++f, (void) ++cur )
			allocator_traits::construct( a, std::addressof( *cur ), *f );
		return cur;
	}
	catch ( ... )
	{
		for ( ; dest != cur; ++dest )
			dest->~value_type();
		throw;
	}
}

}

///
/// @brief Class small_vector provides a simple analog to std::vector
/// but with in-class storage for a small number of items.
///
/// This is to be used in performance sensitive areas when vectors are
/// normally small / fixed sizes without preventing the occasional
/// large array from working.
///
template <typename T, size_t N, class Alloc = std::allocator<T> >
class small_vector
{
private:
	using alloc_type = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using apointer = typename std::allocator_traits<alloc_type>::pointer;
	using capointer = typename std::allocator_traits<alloc_type>::const_pointer;
	using allocator_traits = std::allocator_traits<Alloc>;

	struct sv_impl : public alloc_type
	{
		apointer _start = nullptr;
		apointer _end = nullptr;
		apointer _storage_end = nullptr;
		sv_impl( void ) noexcept(std::is_nothrow_default_constructible<alloc_type>::value) = default;
		sv_impl( apointer ptr, size_t n ) noexcept(std::is_nothrow_default_constructible<alloc_type>::value)
			: _start( ptr ), _end( ptr ), _storage_end( ptr + n )
		{}
		sv_impl( apointer ptr, size_t n, const Alloc &a )
			: alloc_type( a ), _start( ptr ), _end( ptr ), _storage_end( ptr + n )
		{}
		sv_impl( size_t n, apointer ptr, size_t ln, const Alloc &a )
			: alloc_type( a ), _start( ptr ), _end( ptr + n ), _storage_end( ptr + ln )
		{
			if ( _end > _storage_end )
			{
				_start = this->allocate( n );
				_end = _start + n;
				_storage_end = _end;
			}
		}

		void swap( sv_impl &o )
		{
			std::swap( _start, o._start );
			std::swap( _end, o._end );
			std::swap( _storage_end, o._storage_end );
		}

		void reset_to_small( apointer ptr, size_t n, size_t ls, bool dodealloc = true )
		{
			if ( dodealloc )
				this->deallocate( _start, static_cast<size_type>( _storage_end - _start ) );
			_start = ptr;
			_end = ptr + n;
			_storage_end = ptr + ls;
		}
	};

	sv_impl _impl;
	alloc_type &get_allocator_ref( void ) { return _impl; }
	static constexpr bool allocator_is_always_equal( void )
	{
#if __cplusplus > 201402L
		return allocator_traits::is_always_equal::value;
#else
		return false;
#endif
	}
	static constexpr size_t value_size = sizeof(T);
	static constexpr size_t store_count = (N == 0) ? size_t(1) : N;
	using storage = typename std::aligned_storage<store_count * value_size, alignof(T)>::type;
	storage _store;

	inline apointer small_store( void ) { return reinterpret_cast<apointer>( &_store ); }
	inline capointer small_store( void ) const { return reinterpret_cast<capointer>( &_store ); }

public:
	using value_type = T;
	using reference = T &;
	using const_reference = const T &;
	using pointer = T *;
	using const_pointer = const T *;

	using size_type = size_t;
	using difference_type = std::ptrdiff_t;

	using iterator = T *;
	using const_iterator = const T *;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using allocator_type = Alloc;
	
	small_vector( void ) noexcept(std::is_nothrow_default_constructible<allocator_type>::value) : _impl( small_store(), store_count ) {}
	small_vector( const allocator_type &a ) : _impl( small_store(), store_count, a ) {}

	small_vector( const small_vector &o )
		: _impl( o.size(), small_store(), store_count, allocator_traits::select_on_container_copy_construction( _impl ) )
	{ _impl._end = std::uninitialized_copy( o.begin(), o.end(), begin() ); }
	
	small_vector( size_type n, const allocator_type &a = allocator_type() )
		: _impl( n, small_store(), store_count, a )
	{
		for ( auto i = begin(); i != end(); ++i )
			allocator_traits::construct( _impl, i );
	}
	small_vector( size_type n, const value_type &v, const allocator_type &a = allocator_type() )
		: _impl( n, small_store(), store_count, a )
	{ std::uninitialized_fill( begin(), end(), v ); }

	small_vector( small_vector &&o ) noexcept
	{
		if ( o.is_small() )
		{
			_impl._end = detail::uninit_copy_a( std::make_move_iterator( o.begin() ),
												std::make_move_iterator( o.end() ), begin(), _impl );
			o.clear();
		}
		else
		{
			_impl._start = o._impl._start;
			_impl._end = o._impl._end;
			_impl._storage_end = o._impl._storage_end;
			o._impl.reset_to_small( o.small_store(), 0, o.store_count, false );
		}
	}
	small_vector( const small_vector &o, const allocator_type &a )
		: _impl( o.size(), small_store(), store_count, a )
	{ _impl._end = std::uninitialized_copy( o.begin(), o.end(), begin() ); }

	small_vector( small_vector &&o, const allocator_type &a ) noexcept(small_vector::allocator_is_always_equal()) : _impl( small_store(), store_count, a )
	{
		if ( o.is_small() )
		{
			_impl._end = detail::uninit_copy_a( std::make_move_iterator( o.begin() ),
												std::make_move_iterator( o.end() ),
												begin(), _impl );
			o.clear();
		}
		else
		{
			_impl._start = o._impl._start;
			_impl._end = o._impl._end;
			_impl._storage_end = o._impl._storage_end;
			o._impl.reset_to_small( o.small_store(), 0, o.store_count, false );
		}
	}
	small_vector( std::initializer_list<value_type> l, const allocator_type &a = allocator_type() ) : _impl( small_store(), store_count, a )
	{
		range_init( l.begin(), l.end(), std::random_access_iterator_tag() );
	}
	template <typename InputIter, typename = typename std::enable_if< std::is_convertible< typename std::iterator_traits<InputIter>::iterator_category, std::input_iterator_tag >::value >::type>
	small_vector( InputIter b, InputIter e, const allocator_type &a = allocator_type() )
		: _impl( small_store(), store_count, a )
	{
		using iter_cat = typename std::iterator_traits<InputIter>::iterator_category;
		range_init( b, e, iter_cat() );
	}
	~small_vector( void ) noexcept
	{
		clear();
		if ( ! is_small() )
			_impl.deallocate( _impl._start, capacity() );
	}

	small_vector &operator=( const small_vector &o )
	{
		if ( &o != this )
		{
			if ( allocator_traits::propagate_on_container_copy_assignment::value )
			{
				if ( ! allocator_is_always_equal() && get_allocator_ref() != o.get_allocator_ref() )
				{
					// replacement allocator can't free existing storage
					clear();
					_impl.reset_to_small( small_store(), 0, store_count );
				}
				get_allocator_ref() = o.get_allocator_ref();
			}

			assign_helper( o.begin(), o.end(), std::random_access_iterator_tag() );
		}
		return *this;
	}

	small_vector &operator=( small_vector &&o ) noexcept(small_vector::allocator_is_always_equal() ||
														 allocator_traits::propagate_on_container_move_assignment::value)
	{
		if ( &o != this )
		{
			constexpr bool has_move_storage = allocator_traits::propagate_on_container_move_assignment::value || allocator_is_always_equal();
			if ( ( has_move_storage || get_allocator_ref() == o.get_allocator_ref() ) &&
				 ! is_small() && ! o.is_small() )
			{
				// can just swap pointers
				small_vector tmp(get_allocator());
				_impl.swap( tmp._impl );
				_impl.swap( o._impl );
				if ( allocator_traits::propagate_on_container_move_assignment::value )
					get_allocator_ref() = std::move( o.get_allocator_ref() );
				o._impl.reset_to_small( o.small_store(), 0, o.store_count, false );
			}
			else
			{
				// can't do full move because of internal storage or allocator constraint, just
				// do element-wise move assignment
				assign( std::make_move_iterator( o.begin() ), std::make_move_iterator( o.end() ) );
				o.clear();
			}
		}
		return *this;
	}
	small_vector &operator=( std::initializer_list<value_type> l ) { assign_helper( l.begin(), l.end(), std::random_access_iterator_tag() ); return *this; }

	void assign( size_type n, const value_type &v )
	{
		if ( n > capacity() )
		{
			small_vector tmp( n, v, get_allocator() );
			tmp.swap( *this );
		}
		else if ( n > size() )
		{
			std::fill( begin(), end(), v );
			// TODO: handle exceptions
			for ( size_t x = n - size(); x > 0; --x )
			{
				allocator_traits::construct( _impl, std::addressof( _impl._end ), v );
				++_impl._end;
			}
		}
		else
			erase_at_end( std::fill_n( _impl._start, n, v ) );
	}
	template <typename InputIter, typename = typename std::enable_if< std::is_convertible< typename std::iterator_traits<InputIter>::iterator_category, std::input_iterator_tag >::value >::type>
	void assign( InputIter b, InputIter e )
	{
		using iter_cat = typename std::iterator_traits<InputIter>::iterator_category;
		assign_helper( b, e, iter_cat() );
	}
	void assign( std::initializer_list<value_type> l ) { assign_helper( l.begin(), l.end(), std::random_access_iterator_tag() ); }

	allocator_type get_allocator( void ) const noexcept { return allocator_type( _impl ); }

	inline iterator begin( void ) { return iterator( _impl._start ); }
	inline const_iterator begin( void ) const { return const_iterator( _impl._start ); }

	inline iterator end( void ) { return iterator( _impl._end ); }
	inline const_iterator end( void ) const { return const_iterator( _impl._end ); }

	inline reverse_iterator rbegin( void ) { return iterator( _impl._end ); }
	inline const_reverse_iterator rbegin( void ) const { return const_reverse_iterator( _impl._end ); }

	inline reverse_iterator rend( void ) { return iterator( _impl._start ); }
	inline const_reverse_iterator rend( void ) const { return const_iterator( _impl._start ); }

	inline const_iterator cbegin( void ) const { return const_iterator( _impl._start ); }
	inline const_iterator cend( void ) const { return const_iterator( _impl._end ); }
	inline const_reverse_iterator crbegin( void ) const { return const_reverse_iterator( _impl._end ); }
	inline const_reverse_iterator crend( void ) const { return const_iterator( _impl._start ); }

	inline size_type size( void ) const noexcept { return size_type( _impl._end - _impl._start ); }
	inline size_type max_size( void ) const noexcept { return allocator_traits::max_size( _impl ); }

	void resize( size_type n )
	{
		if ( n > size() )
			default_append( n - size() );
		else if ( n < size() )
			erase_at_end( begin() + n );
	}
	void resize( size_type n, const value_type &v )
	{
		if ( n > size() )
			fill_insert( end(), n - size(), v );
		else if ( n < size() )
			erase_at_end( begin() + n );
	}

	void shrink_to_fit( void )
	{
		if ( ! is_small() )
		{
			if ( empty() )
				_impl.reset_to_small( small_store(), 0, store_count );
			else
			{
				size_t n = size();
				if ( n <= store_count )
				{
					iterator f = begin();
					std::uninitialized_copy( f, end(), small_store() );
					for ( size_t i = 0; i < n; (void) ++f, ++i )
						allocator_traits::destroy( _impl, f );
					_impl.reset_to_small( small_store(), n, store_count );
				}
				else if ( capacity() != n )
				{
					detail::shrink_helper<small_vector>::do_it( *this );
				}
			}
		}
	}

	inline size_type capacity( void ) const noexcept { return size_type( _impl._storage_end - _impl._start ); }

	inline bool empty( void ) const noexcept { return _impl._start == _impl._end; }

	// non-standard (i.e. not part of std::vector)
	inline bool is_small( void ) const { return _impl._start == small_store(); }

	void reserve( size_type n )
	{
		if ( n > max_size() )
			throw std::length_error( format( "small_vector reserve {0} beyond max_size {1}", n, max_size() ) );
		if ( capacity() < n )
		{
			const size_type old = size();
			pointer news = allocate_and_copy( n,
											  std::make_move_iterator( begin() ),
											  std::make_move_iterator( end() ) );
			for ( pointer cur = _impl._start; cur != _impl._end; (void) ++cur )
				allocator_traits::destroy( _impl, cur );
			if ( ! is_small() )
				_impl.deallocate( _impl._start, static_cast<size_t>( _impl._storage_end - _impl._start ) );
			_impl._start = news;
			_impl._end = news + old;
			_impl._storage_end = news + n;
		}
	}

	reference operator[]( size_type i ) noexcept { return *(_impl._start + i); }
	const_reference operator[]( size_type i ) const noexcept { return *(_impl._start + i); }

protected:
	void range_check( size_type i ) const { if ( i > size() ) throw std::out_of_range( format( "small_vector::range_check index {0} >= this->size() {1}", i, size() ) ); }
#ifndef NDEBUG
	void require_nonempty( void ) const { if ( empty() ) throw std::out_of_range( "small_vector::require_nonempty this is empty" ); }
#else
	inline void require_nonempty( void ) const {}
#endif

public:
	reference at( size_type i ) { range_check( i ); return (*this)[i]; }
	const_reference at( size_type i ) const { range_check( i ); return (*this)[i]; }

	reference front( void ) { require_nonempty(); return *begin(); }
	const_reference front( void ) const { require_nonempty(); return *begin(); }
	reference back( void ) { require_nonempty(); return *( end() - 1 ); }
	const_reference back( void ) const { require_nonempty(); return *( end() - 1 ); }

	pointer data( void ) noexcept { return empty() ? nullptr : _impl._start; }
	const_pointer data( void ) const noexcept { return empty() ? nullptr : _impl._start; }

	void push_back( const value_type &v )
	{
		if ( _impl._end != _impl._storage_end )
		{
			allocator_traits::construct( _impl, _impl._end, v );
			++_impl._end;
		}
		else
			realloc_insert( end(), v );
	}
	void push_back( value_type &&v ) { emplace_back( std::move( v ) ); }

	template <typename... Args>
#if __cplusplus > 201402L
	reference
#else
	void
#endif
	emplace_back( Args &&... args )
	{
		if ( _impl._end != _impl._storage_end )
		{
			allocator_traits::construct( _impl, _impl._end, std::forward<Args>( args )... );
			++_impl._end;
		}
		else
			realloc_insert( end(), std::forward<Args>( args )... );

#if __cplusplus > 201402L
		return back();
#endif
	}

	void pop_back( void )
	{
		require_nonempty();
		--_impl._end;
		allocator_traits::destroy( _impl, _impl._end );
	}

	template <typename... Args>
	iterator emplace( const_iterator p, Args &&... args )
	{
		return emplace_helper( p, std::forward<Args>( args )... );
	}

	iterator insert( const_iterator p, const value_type &v )
	{
		const size_type off = p - cbegin();
		if ( _impl._end != _impl._storage_end )
		{
			if ( p == cend() )
			{
				allocator_traits::construct( _impl, _impl._end, v );
				++_impl._end;
			}
			else
			{
				// v could be an existing copy...
				TempValue tmp( this, v );
				insert_helper( begin() + off, std::move( tmp.val() ) );
			}
		}
		else
			realloc_insert( begin() + off, v );

		return begin() + off;
	}

	iterator insert( const_iterator p, value_type &&v ) { return insert_rval( p, std::move( v ) ); }
	iterator insert( const_iterator p, std::initializer_list<value_type> l )
	{
		size_t off = p - cbegin();
		range_insert( begin() + off, l.begin(), l.end(), std::random_access_iterator_tag() );
		return begin() + off;
	}

	iterator insert( const_iterator p, size_type n, const value_type &v )
	{
		size_t off = p - cbegin();
		fill_insert( begin() + off, n, v );
		return begin() + off;
	}
	template <typename InputIter, typename = typename std::enable_if< std::is_convertible< typename std::iterator_traits<InputIter>::iterator_category, std::input_iterator_tag >::value >::type>
	iterator insert( const_iterator p, InputIter b, InputIter e )
	{
		using iter_cat = typename std::iterator_traits<InputIter>::iterator_category;
		size_t off = p - cbegin();
		range_insert( begin() + off, b, e, iter_cat() );
		return begin() + off;
	}

	iterator erase( const_iterator p ) { return do_erase( begin() + ( p - cbegin() ) ); }
	iterator erase( const_iterator first, const_iterator last )
	{
		const auto b = begin();
		const auto cb = cbegin();
		return do_erase( b + ( first - cb ), b + ( last - cb ) );
	}

	void swap( small_vector &o )
	{
		// TODO: do we have to do anything w/ the allocators?
		if ( o.is_small() )
		{
			small_vector tmp( std::move( o ) );

			if ( is_small() )
				o = std::move( *this );
			else
			{
				o._impl._start = _impl._start;
				o._impl._end = _impl._end;
				o._impl._storage_end = _impl._storage_end;
				_impl.reset_to_small( small_store(), 0, o.store_count, false );
			}
			*this = std::move( tmp );
		}
		else if ( is_small() )
		{
			small_vector tmp( std::move( *this ) );
			_impl._start = o._impl._start;
			_impl._end = o._impl._end;
			_impl._storage_end = o._impl._storage_end;
			o._impl.reset_to_small( o.small_store(), 0, o.store_count, false );
			o = std::move( tmp );
		}
		else
			_impl.swap( o._impl );
	}

	void clear( void ) noexcept { erase_at_end( _impl._start ); }

private:
	void erase_at_end( pointer p ) noexcept
	{
		size_type n = static_cast<size_type>( _impl._end - p );
		pointer f = p;
		for ( ; n > 0; (void) ++f, --n )
			allocator_traits::destroy( _impl, f );
		_impl._end = p;
	}

	void do_erase( iterator p )
	{
		if ( ( p + 1 ) != end() )
			std::move( p + 1, end(), p );
		--_impl._end;
		allocator_traits::destroy( _impl, _impl._end );
	}

	void do_erase( iterator f, iterator l )
	{
		if ( f != l )
		{
			if ( l != end() )
				std::move( l, end(), f );
			erase_at_end( f + ( end() - l ) );
		}
	}

	size_type check_len( size_type n ) const
	{
		if ( ( max_size() - size() ) < n )
			throw std::length_error( "small_vector request to allocate past max_size" );
		const size_type l = size() + std::max( n, size() );
		return ( l < size() || l > max_size() ) ? max_size() : l;
	}

	template <typename Iter>
	pointer allocate_and_copy( size_t n, Iter f, Iter l )
	{
		pointer ret = _impl.allocate( n );
		try
		{
			detail::uninit_copy_a( f, l, ret, _impl );
			return ret;
		}
		catch ( ... )
		{
			_impl.deallocate( ret, n );
			throw;
		}
	}		
	template <typename ... Args>
	void realloc_insert( iterator p, Args && ... args )
	{
		const size_type l = check_len( size_type( 1 ) );
		const size_type nbefore = static_cast<size_type>( p - begin() );
		pointer start = _impl.allocate( l );
		pointer e = start;
		try
		{
			allocator_traits::construct( _impl, start + nbefore, std::forward<Args>( args )... );
			e = pointer();
			e = detail::uninit_copy_a( std::make_move_iterator( _impl._start ),
									   std::make_move_iterator( p ), start, _impl );
			++e;
			e = detail::uninit_copy_a( std::make_move_iterator( p ),
									   std::make_move_iterator( _impl._end ), e, _impl );
		}
		catch ( ... )
		{
			if ( ! e )
				allocator_traits::destroy( _impl, start + nbefore );
			else
			{
				for ( pointer cur = start; cur != e; (void) ++cur )
					allocator_traits::destroy( _impl, cur );
			}
			_impl.deallocate( start, l );
			throw;
		}
		for ( pointer cur = _impl._start; cur != _impl._end; (void) ++cur )
			allocator_traits::destroy( _impl, cur );
		if ( ! is_small() )
			_impl.deallocate( _impl._start, static_cast<size_type>( _impl._storage_end - _impl._start ) );
		_impl._start = start;
		_impl._end = e;
		_impl._storage_end = start + l;
	}

	struct TempValue
	{
		template <typename... Args>
		explicit TempValue( small_vector *v, Args &&... args ) : _vec( v )
		{
			allocator_traits::construct( v->_impl, ptr(), std::forward<Args>( args )... );
		}

		~TempValue( void )
		{
			allocator_traits::destroy( _vec->_impl, ptr() );
		}

		value_type &val( void ) { return *reinterpret_cast<value_type *>( &_buf ); }
	private:
		pointer ptr( void ) { return std::pointer_traits<pointer>::pointer_to( val() ); }

		small_vector *_vec;
		using buf_type = typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;
		buf_type _buf;
	};

	template <typename Arg>
	void insert_helper( iterator p, Arg &&v )
	{
		allocator_traits::construct( _impl, _impl._end, std::move( *(_impl._end - 1) ) );
		++_impl._end;
		std::move_backward( p, _impl._end - 2, _impl._end - 1 );
		*p = std::forward<Arg>( v );
	}

	iterator insert_rval( const_iterator p, value_type &&v )
	{
		size_t off = p - cbegin();
		if ( _impl._end != _impl._storage_end )
		{
			if ( p == cend() )
			{
				allocator_traits::construct( _impl, _impl._end, std::move( v ) );
				++_impl._end;
			}
			else
				insert_helper( begin() + off, std::move( v ) );
		}
		else
			realloc_insert( begin() + off, std::move( v ) );

		return begin() + off;
	}

	template <typename... Args>
	iterator emplace_helper( const_iterator p, Args &&... args )
	{
		size_t off = p - cbegin();
		if ( _impl._end != _impl._storage_end )
		{
			if ( p == cend() )
			{
				allocator_traits::construct( _impl, _impl._end, std::forward<Args>( args )... );
				++_impl._end;
			}
			else
			{
				// what if args... aliases something in this, make a temporary value
				TempValue tmp( this, std::forward<Args>( args )... );
				insert_helper( begin() + off, std::move( tmp.val() ) );
			}
		}
		else
			realloc_insert( begin() + off, std::forward<Args>( args )... );

		return begin() + off;
	}

	iterator emplace_helper( const_iterator p, value_type &&v )
	{
		return insert_rval( p, std::move( v ) );
	}

	template <typename InpIter>
	void
	range_init( InpIter f, InpIter l, std::input_iterator_tag )
	{
		// can't know how many we'll get :(
		for ( ; f != l; ++f )
			emplace_back( *f );
	}

	template <typename FwdIter>
	void
	range_init( FwdIter f, FwdIter l, std::forward_iterator_tag )
	{
		const size_type n = std::distance( f, l );
		if ( n > store_count )
		{
			_impl._start = _impl.allocate( n );
			_impl._storage_end = _impl._end;
		}
		_impl._end = detail::uninit_copy_a( f, l, begin(), _impl );
	}

	template <typename InpIter>
	void assign_helper( InpIter f, InpIter l, std::input_iterator_tag )
	{
		pointer cur = _impl._start;
		for ( ; f != l && cur != _impl._end; ++cur, ++f )
			*cur = *f;
		if ( f == l )
			erase_at_end( cur );
		else
		{
			range_insert( end(), f, l, std::input_iterator_tag() );
		}
	}

	template <typename FwdIter>
	void assign_helper( FwdIter f, FwdIter l, std::forward_iterator_tag )
	{
		const size_type n = std::distance( f, l );
		if ( n > capacity() )
		{
			pointer tmp = allocate_and_copy( n, f, l );
			for ( pointer cur = _impl._start; cur != _impl._end; (void) ++cur )
				allocator_traits::destroy( _impl, cur );
			if ( ! is_small() )
				_impl.deallocate( _impl._start, _impl._storage_end - _impl._start );
			_impl._start = tmp;
			_impl._end = tmp + n;
			_impl._storage_end = _impl._end;
		}
		else if ( size() > n )
		{
			erase_at_end( std::copy( f, l, begin() ) );
		}
		else
		{
			FwdIter m = f;
			std::advance( m, size() );
			std::copy( f, m, begin() );
			_impl._end = detail::uninit_copy_a( m, l, _impl._end, _impl );
		}
	}

	template <typename InpIter>
	void range_insert( iterator p, InpIter f, InpIter l, std::input_iterator_tag )
	{
		for ( ; f != l; ++f )
		{
			p = insert( p, *f );
			++p;
		}
	}

	template <typename FwdIter>
	void range_insert( iterator p, FwdIter f, FwdIter l, std::forward_iterator_tag )
	{
		if ( f == l )
			return;

		const size_type n = std::distance( f, l );
		const size_type nleft = _impl._storage_end - _impl._end;
		if ( nleft >= n )
		{
			const size_type nafter = end() - p;
			pointer old = _impl._end;
			if ( nafter > n )
			{
				detail::uninit_copy_a( std::make_move_iterator( _impl._storage_end - n ),
									   std::make_move_iterator( _impl._end ),
									   _impl._end,
									   _impl );
				_impl._end += n;
				std::move_backward( p, old - n, old );
				std::copy( f, l, p );
			}
			else
			{
				FwdIter m = f;
				std::advance( m, nafter );
				detail::uninit_copy_a( m, l, _impl._end, _impl );
				_impl._end += n - nafter;
				detail::uninit_copy_a( std::make_move_iterator( p ),
									   std::make_move_iterator( old ),
									   _impl._end, _impl );
				_impl._end += nafter;
				std::copy( f, m, p );
			}
		}
		else
		{
			const size_type len = check_len( n );
			pointer start = _impl.allocate( len );
			pointer e = start;
			try
			{
				// TODO: need to handle exceptions in move? or assume they are noexcept?
				e = detail::uninit_copy_a( std::make_move_iterator( _impl._start ),
										   std::make_move_iterator( p ),
										   start, _impl );
				e = detail::uninit_copy_a( f, l, e, _impl );
				e = detail::uninit_copy_a( std::make_move_iterator( p ),
										   std::make_move_iterator( _impl._end ),
										   e, _impl );
			}
			catch ( ... )
			{
				for ( pointer i = start; i != e; ++i )
					allocator_traits::destroy( _impl, i );
				_impl.deallocate( start, len );
				throw;
			}
			
			for ( pointer i = _impl._start; i != _impl._end; ++i )
				allocator_traits::destroy( _impl, i );
			if ( ! is_small() )
				_impl.deallocate( _impl._start, capacity() );
			_impl._start = start;
			_impl._end = e;
			_impl._storage_end = start + len;
		}
	}
};

template <typename T, size_t N1, size_t N2, typename Alloc>
inline bool operator==( const small_vector<T, N1, Alloc> &a, const small_vector<T, N2, Alloc> &b )
{
	return a.size() == b.size() && std::equal( a.begin(), a.end(), b.begin() );
}

template <typename T, size_t N1, size_t N2, typename Alloc>
inline bool operator!=( const small_vector<T, N1, Alloc> &a, const small_vector<T, N2, Alloc> &b )
{
	return !(a == b);
}

template <typename T, size_t N1, size_t N2, typename Alloc>
inline bool operator<( const small_vector<T, N1, Alloc> &a, const small_vector<T, N2, Alloc> &b )
{
	return std::lexicographical_compare( a.begin(), a.end(), b.begin(), b.end() );
}

template <typename T, size_t N1, size_t N2, typename Alloc>
inline bool operator>( const small_vector<T, N1, Alloc> &a, const small_vector<T, N2, Alloc> &b )
{
	return b < a;
}

template <typename T, size_t N1, size_t N2, typename Alloc>
inline bool operator<=( const small_vector<T, N1, Alloc> &a, const small_vector<T, N2, Alloc> &b )
{
	return !(b < a);
}

template <typename T, size_t N1, size_t N2, typename Alloc>
inline bool operator>=( const small_vector<T, N1, Alloc> &a, const small_vector<T, N2, Alloc> &b )
{
	return !(a < b);
}

} // namespace base

namespace std
{

template <typename T, size_t N, typename Alloc>
inline void swap( base::small_vector<T, N, Alloc> &a, base::small_vector<T, N, Alloc> &b )
{
	a.swap( b );
}

}



