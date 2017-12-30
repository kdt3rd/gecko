//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>

////////////////////////////////////////

namespace gui
{

class widget;

///
/// @brief Class widget_ptr provides...
///
template <typename T>
class widget_ptr
{
public:
	using widget_type = T;
	using pointer_type = std::shared_ptr<widget_type>;

	template <typename ... Args>
	widget_ptr( Args && ... args )
		: _instance( std::make_shared<widget_type>( std::forward<Args>( args )... ) )
	{
	}

	explicit widget_ptr( const pointer_type &x ) : _instance( x ) {}
	explicit widget_ptr( pointer_type &&x ) : _instance( std::move( x ) ) {}
	widget_ptr( widget_ptr &o ) : _instance( o._instance ) {}
	widget_ptr( const widget_ptr & ) = default;
	constexpr widget_ptr( widget_ptr && ) noexcept = default;
	widget_ptr &operator=( const widget_ptr & ) = default;
	widget_ptr &operator=( widget_ptr && ) noexcept = default;
	~widget_ptr( void ) = default;

	T &operator *() const noexcept { return *_instance; }
	T *operator ->() const noexcept { return get(); }
	T *get() const noexcept { return _instance.get(); }

	void swap( widget_ptr &o ) noexcept { _instance.swap( o._instance ); }

	explicit operator bool() const noexcept { return static_cast<bool>( _instance ); }

	explicit operator pointer_type() const noexcept { return _instance; }

private:
	pointer_type _instance;
};

template <typename X, typename Y>
inline bool operator==( const widget_ptr<X> &x, const widget_ptr<Y> &y ) noexcept
{ return x.get() == y.get(); }

template <typename X>
inline bool operator==( const widget_ptr<X> &x, std::nullptr_t ) noexcept
{ return !x; }

template <typename X>
inline bool operator==( std::nullptr_t, const widget_ptr<X> &x ) noexcept
{ return !x; }

template <typename X, typename Y>
inline bool operator!=( const widget_ptr<X> &x, const widget_ptr<Y> &y ) noexcept
{ return x.get() != y.get(); }

template <typename X>
inline bool operator!=( const widget_ptr<X> &x, std::nullptr_t ) noexcept
{ return static_cast<bool>( x ); }

template <typename X>
inline bool operator!=( std::nullptr_t, const widget_ptr<X> &x ) noexcept
{ return static_cast<bool>( x ); }

template <typename X, typename Y>
inline bool operator<( const widget_ptr<X> &x, const widget_ptr<Y> &y ) noexcept
{
	using eltX = typename widget_ptr<X>::widget_type;
	using eltY = typename widget_ptr<Y>::widget_type;
	using commP = typename std::common_type<eltX *, eltY *>::type;
	return std::less<commP>()( x.get(), y.get() );
}

template <typename X>
inline bool operator<( const widget_ptr<X> &x, std::nullptr_t ) noexcept
{
	using elt = typename widget_ptr<X>::widget_type;
	return std::less<elt *>()( x.get(), nullptr );
}

template <typename X>
inline bool operator<( std::nullptr_t, const widget_ptr<X> &x ) noexcept
{
	using elt = typename widget_ptr<X>::widget_type;
	return std::less<elt *>()( nullptr, x.get() );
}

template <typename X, typename Y>
inline bool operator<=( const widget_ptr<X> &x, const widget_ptr<Y> &y ) noexcept
{ return !(y < x); }

template <typename X>
inline bool operator<=( const widget_ptr<X> &x, std::nullptr_t ) noexcept
{ return !(nullptr < x); }

template <typename X>
inline bool operator<=( std::nullptr_t, const widget_ptr<X> &x ) noexcept
{ return !(x < nullptr); }

template <typename X, typename Y>
inline bool operator>( const widget_ptr<X> &x, const widget_ptr<Y> &y ) noexcept
{ return y < x; }

template <typename X>
inline bool operator>( const widget_ptr<X> &x, std::nullptr_t ) noexcept
{ return nullptr < x; }

template <typename X>
inline bool operator>( std::nullptr_t, const widget_ptr<X> &x ) noexcept
{ return x < nullptr; }

template <typename X, typename Y>
inline bool operator>=( const widget_ptr<X> &x, const widget_ptr<Y> &y ) noexcept
{ return !(x < y); }

template <typename X>
inline bool operator>=( const widget_ptr<X> &x, std::nullptr_t ) noexcept
{ return !(x < nullptr); }

template <typename X>
inline bool operator>=( std::nullptr_t, const widget_ptr<X> &x ) noexcept
{ return !(nullptr < x); }

template <typename X> inline void
swap( widget_ptr<X> &a, widget_ptr<X> &b ) noexcept
{ a.swap( b ); }

} // namespace gui

namespace std
{

template <typename X>
struct less<gui::widget_ptr<X>> : public binary_function<X, X, bool>
{
	inline bool operator()( const gui::widget_ptr<X> &a,
							const gui::widget_ptr<X> &b ) const noexcept
	{
		using elt = typename gui::widget_ptr<X>::widget_type;
		return std::less<elt *>()( a.get(), b.get() );
	}
};

template <typename X> inline void
swap( gui::widget_ptr<X> &a, gui::widget_ptr<X> &b ) noexcept
{ a.swap( b ); }

// TODO: provide std::hash specialization?

template<typename X, typename Y>
inline gui::widget_ptr<X>
static_pointer_cast( const gui::widget_ptr<Y>& r ) noexcept
{
	shared_ptr<X> t = static_pointer_cast<X>( static_cast<std::shared_ptr<Y>>( r ) );
	return gui::widget_ptr<X>( std::move( t ) );
}

template<typename X, typename Y>
inline gui::widget_ptr<X>
const_pointer_cast( const gui::widget_ptr<Y>& r ) noexcept
{
	shared_ptr<X> t = const_pointer_cast<X>( static_cast<std::shared_ptr<Y>>( r ) );
	return gui::widget_ptr<X>( std::move( t ) );
}

template<typename X, typename Y>
inline gui::widget_ptr<X>
dynamic_pointer_cast( const gui::widget_ptr<Y>& r ) noexcept
{
	shared_ptr<X> t = dynamic_pointer_cast<X>( static_cast<std::shared_ptr<Y>>( r ) );
	return gui::widget_ptr<X>( std::move( t ) );
}

#if __cplusplus > 201402L
template<typename X, typename Y>
inline gui::widget_ptr<X>
reinterpret_pointer_cast( const gui::widget_ptr<Y>& r ) noexcept
{
	shared_ptr<X> t = reinterpret_pointer_cast<X>( static_cast<std::shared_ptr<Y>>( r ) );
	return gui::widget_ptr<X>( std::move( t ) );
}
#endif

}


