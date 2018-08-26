//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <map>
#include "any.h"

////////////////////////////////////////

namespace base
{

///
/// @brief Class context provides a super low level container to store data.
///
/// The idea is that each class can store data and it's received by
/// the type of the object.
///
/// This should be used to subclass from for things like opengl
/// contexts and such that entities can store data as part of the
/// context and retrieve it. This eliminates the need for static data,
/// and enables different instances of the same type to share data.
///
/// TODO: sharing 
///
/// TODO: thread safety
///
class context
{
protected:
    using stash_type = std::map<std::type_index, any>;
    using stash_ptr = std::shared_ptr<stash_type>;

public:
    context( void );
	virtual ~context( void );

    /// @brief Copy construction will create an implicitly shared context
    context( const context & ) = default;
    /// @brief Assignment will create an implicitly shared context
    ///
    /// NB: be careful of object slicing
    context &operator=( const context & ) = default;

    /// @brief Move construction will do the expected
    context( context && ) = default;
    /// @brief Move assignment will do the expected
    ///
    /// NB: be careful of object slicing
    context &operator=( context && ) = default;

    /// @brief routine meant to be decorated to share context information
    ///
    /// The base class version of this triggers the common stash of items to be shared.
    ///
    /// This is best used when the same objects may appear in multiple
    /// contexts, such as icons or something, where there are system
    /// resources attached to those objects.
    virtual void share( const context &c );

    /// @brief returns true if it is a new object, false if it was
    /// previously stashed in the context.
    template <typename Storer, typename StoreType>
    bool retrieve_common( Storer &&, std::shared_ptr<StoreType> &v )
    {
        using fundamental_type = typename std::remove_pointer<typename std::decay<Storer>::type>::type;
        any &sv = stash()[std::type_index( typeid(fundamental_type) )];
        bool isnew = false;
        if ( !sv.has_value() )
        {
            static_assert( std::is_default_constructible<StoreType>::value, "non-default constructible objects must use alternate function w provided factory function" );
            v = std::make_shared<StoreType>();
            sv = v;
            isnew = true;
        }
        else
            v = any_cast< std::shared_ptr<StoreType> >( sv );

        return isnew;
    }

    /// @brief returns true if it is a new object, false if it was
    /// previously stashed in the context.
    ///
    /// same as @sa retrieve_common, just allows a factory method to
    /// be used to create the stashed object if it doesn't exist (but
    /// otherwise don't require construction or passing of argument
    /// like a standard make_shared or similar).
    template <typename Storer, typename StoreType, typename Factory>
    bool retrieve_common( Storer &&, std::shared_ptr<StoreType> &v, Factory &&factory )
    {
        using fundamental_type = typename std::remove_pointer<typename std::decay<Storer>::type>::type;
        any &sv = stash()[std::type_index( typeid(fundamental_type) )];
        bool isnew = false;
        if ( !sv.has_value() )
        {
            static_assert( std::is_same<std::shared_ptr<StoreType>, decltype(std::forward<Factory>(factory)())>::value ||
                           std::is_convertible<decltype(std::forward<Factory>(factory)()), std::shared_ptr<StoreType> >::value,
                           "factory not returning a shared_ptr of the requested storage type" );
            v = std::forward<Factory>(factory)();
            sv = v;
            isnew = true;
        }
        else
            v = any_cast< std::shared_ptr<StoreType> >( sv );

        return isnew;
    }

protected:
    stash_type &stash( void );

private:
    stash_ptr _stash;
};

} // namespace base



