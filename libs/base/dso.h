// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////

namespace base
{
///
/// @brief Class dso provides...
///
class dso
{
public:
    dso( void ) = default;

    /// @brief causes provided file name to be made available as a dynamic
    ///        object for lookup.
    ///
    /// if fn is nullptr, the main application is loaded, enabling symbol
    /// lookup in the application and loaded (global) objects
    ///
    /// Not all O.S. support this concept, but if the namespace is not null,
    /// the constructor attempts to load the dso and place it in the
    /// specified namespace (which basically puts it in a different link
    /// map). This is usually a very limited resource, meaning only a small
    /// number of namespaces are allowed (i.e. glibc allows a maximum of 16).
    ///
    /// So while the intent is to provide some isolation such that a loaded
    /// dso does not pollute the main app namespace, one can not load each
    /// plugin in a large application into a unique namespace. However, it
    /// is probably best to avoid globalsymbol pollution by putting all
    /// plugins into a separate (single) namespace, or into a small subset
    /// of namespaces.
    ///
    /// Another potential use of a namespace may be to enable multiple
    /// loads of the same dso, so to avoid the compression of a dlopen to
    /// just incrementing the ref count on a specified dso.
    ///
    /// Or to be particularly clever, and application could enable multiple
    /// versions of a library such as python 2 and python 3 to coexist at
    /// the same time with a use of namespaces.
    ///
    explicit dso(
        const char *fn, bool makeGlobal = false, const char *ns = nullptr );
    ~dso( void );
    dso( const dso & ) = delete;
    dso &operator=( const dso & ) = delete;
    dso( dso && );
    dso &operator=( dso && );

    bool valid( void ) const { return _handle != nullptr; }

    /// Clear and unload this instance of the dso (remember the DSO may
    /// continue to be resident if there are other references to it)
    void reset( void );

    /// @sa dso comments in the constructor about the optional namespace
    void
    load( const char *fn, bool makeGlobal = false, const char *ns = nullptr );

    // todo: provide this to support finding c++ signatures in a
    // generic C-friendly way?
    //
    //void *find_first_by_signature( const char *signature );

    void *find( const char *symn, const char *symver = nullptr );

    const std::string &last_error() const { return _last_err; }

private:
    std::string _fn;
    std::string _last_err;
    void *      _handle = nullptr;
};

namespace dl_extra
{
/// @brief abstraction around an active shared object.
///
/// This can be used to query against for things like backtrace. This
/// is distinct from dladdr in that it will attempt to resolve symbols
/// from the non-dynamic symbols as well as the dynamic symbol table
/// by loading the binary file and trolling through the binary.
class active_shared_object
{
public:
    active_shared_object( const char *name, uintptr_t base_addr );
    active_shared_object( void *phdr );

    const std::string &path( void ) const;
    const std::string &name( void ) const;
    uintptr_t          base( void ) const;

    bool is_closest( void *addr, const active_shared_object *curdso );

    //void *find_symbol( const char *name );
    std::string find_symbol( void *addr, bool include_offset = true );

private:
    class impl;
    std::unique_ptr<impl> _impl;
};

/// @brief constructs a list of active dynamic objects (shared libraries and plugins).
///
/// This can be used to query against for things like backtrace.
///
/// NB: Care should be taken in multi-threaded environments that the
/// other threads are not actively loading and unloading objects. This
/// should provide a consistent view and won't crash, but may be
/// (partially) out of date view of loaded objects
///
class active_objects
{
public:
    active_objects();

    //void *find_first( const char *name );
    //void *find_next( const std::shared_ptr<active_shared_object> &x, const char *name );

    std::shared_ptr<active_shared_object> find_dso( void *addr ) const;

private:
    std::vector<std::shared_ptr<active_shared_object>> _dsos;
};

/// @brief equivalent to dlsym( RTLD_NEXT, sig ), but provided here
/// for cross platform convenience
void *find_next( const char *sig, const char *vers = nullptr );

} // namespace dl_extra

} // namespace base
