// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "contract.h"
#include "directory_iterator.h"
#include "fs_watch.h"
#include "stream.h"
#include "uri.h"

#include <functional>
#include <memory>

/// @TODO: should we add our own stat / file_info buffer?
#include <sys/stat.h>
#include <sys/types.h>
#if defined( _WIN32 )
#    ifdef NO_OLDNAMES
struct stat : public struct _stat64
{};
using stat64 = _stat64;
struct statvfs;
#    endif
#else
#    include <sys/statvfs.h>
#    include <unistd.h>
#endif

namespace base
{
////////////////////////////////////////

class file_system
{
public:
    constexpr static std::ios_base::openmode text_read_mode = std::ios_base::in;
    constexpr static std::ios_base::openmode file_read_mode =
        ( std::ios_base::in | std::ios_base::binary );

    constexpr static std::ios_base::openmode text_write_mode =
        ( std::ios_base::out | std::ios_base::trunc );
    constexpr static std::ios_base::openmode file_write_mode =
        ( std::ios_base::out | std::ios_base::trunc | std::ios_base::binary );

    constexpr static mode_t default_dir_mode = mode_t(
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH |
        S_IWOTH | S_IXOTH );

    virtual ~file_system( void );

    /// @brief Retrieves the current directory for the process.
    ///
    /// NB: This should be used carefully, as it is not guaranteed to
    /// be thread safe, so should only be called during initialization
    /// when there is only 1 thread running.
    virtual uri current_path( void ) const = 0;

    /// @brief Perform a stat, retrieving information about path
    ///
    /// if the path is a symlink, returns information about the link, otherwise
    /// this is the same as @sa stat below
    ///
    virtual void lstat( const uri &path, struct stat *buf ) = 0;

    /// @brief Perform a stat, retrieving information about path
    ///
    /// Note that this provides a default implementation which calls
    /// lstat, and if it's link, calls readlink and continues until the
    /// end, which can traverse file_system types (you can make a symlink
    /// to an http address in your posix_file_system for example)
    ///
    /// @return The last uri stat'ed
    virtual uri stat( const uri &path, struct stat *buf );

    /// @brief Perform a stat, retrieving information about the filesystem
    ///
    /// Useful for things like finding out how much free space there
    /// is if saving fails but there isn't a permissions problem and
    /// similar applications.
    virtual void statfs( const uri &path, struct statvfs *s ) = 0;

    /// @brief Reads the location pointed to by a symlink.
    ///
    /// @param path URI to read
    /// @param bufSz Hint to the read that contains the buffer size necessary
    ///              (i.e. if you've just done an lstat and know the value)
    virtual uri readlink( const uri &path, size_t bufSz = 0 ) = 0;

    /// @brief Checks whether the process can access the path with the
    ///        specified mode.
    ///
    /// The mode should be either a bitwise OR of R_OK, W_OK, X_OK, or
    /// the value F_OK
    virtual bool access( const uri &path, int mode ) = 0;
    inline bool  exists( const uri &path ) { return access( path, F_OK ); }

    /// @brief Retrieves an iterator providing access to the entries
    /// in the provided path as a directory
    virtual directory_iterator readdir( const uri &path ) = 0;

    /// @brief Makes a directory with the last name in the path
    ///
    /// Fails if directories above the last entry do not exist
    virtual void mkdir( const uri &path, mode_t mode = default_dir_mode ) = 0;
    /// @brief Makes a directory tree.
    ///
    /// Behaves like the mkdir -p command where it makes all
    /// intervening directories necessary.
    virtual void
    mkdir_all( const uri &path, mode_t mode = default_dir_mode ) = 0;

    /// @brief Removes the last entry in the path as a directory.
    virtual void rmdir( const uri &path ) = 0;
    /// @brief Removes the last entry in the path as a directory,
    ///        including all subentries.
    ///
    /// This is equivalent to rm -rf. Be careful!
    /// A default implementation of this is provided
    virtual void rmdir_all( const uri &path );

    ////////////////////////////////////
    /// @TODO: Add support for xattr? utimens?
    ////////////////////////////////////

    /// @brief Unlinks (removes) a path
    virtual void unlink( const uri &path ) = 0;
    /// @brief Creates a symbolic link from a path to a new location
    virtual void symlink( const uri &curpath, const uri &newpath ) = 0;
    /// @brief Creates a hard link from a path to a new location
    virtual void link( const uri &curpath, const uri &newpath ) = 0;
    /// @brief Renames an existing path to a new path
    virtual void rename( const uri &oldpath, const uri &newpath ) = 0;

    /// @brief Open a stream for read-only access
    virtual istream open_read(
        const base::uri &path, std::ios_base::openmode m = file_read_mode ) = 0;
    /// @brief Open a stream for write-only access
    virtual ostream open_write(
        const base::uri &       path,
        std::ios_base::openmode m = file_write_mode ) = 0;
    /// @brief Open a stream for read and write access
    virtual iostream open(
        const base::uri &       path,
        std::ios_base::openmode m = ( file_read_mode | file_write_mode ) ) = 0;

    virtual fs_watch watch(
        const base::uri &              path,
        const fs_watch::event_handler &evtcb,
        fs_event                       evt_mask,
        bool                           recursive ) = 0;

    /// @brief Finds a reference to a filesystem for the given uri
    static std::shared_ptr<file_system> get( const uri &path )
    {
        return get( path.scheme() );
    }

    /// @brief Finds a reference to a filesystem for the given uri
    static std::shared_ptr<file_system> get( const std::string &scheme );

    /// @brief Registers a file system for a specific scheme
    static void
    add( const std::string &sch, const std::shared_ptr<file_system> &fs );
};

////////////////////////////////////////

} // namespace base
