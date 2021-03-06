// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "blob.h"

#include <memory>
#include <string>

extern "C"
{
    struct sqlite3_stmt;
}

namespace sqlite
{
class statement;

////////////////////////////////////////

/// @brief Field from a query.
class field
{
public:
    /// @brief Destructor.
    ~field( void );
    field( const field & ) = default;
    field &operator=( const field & ) = default;
    field( field && )                 = default;
    field &operator=( field && ) = default;

    /// @brief Name of the field.
    std::string name( void ) const;

    /// @brief Size of the field.
    size_t size( void ) const;

    /// @brief Check if the field is null.
    bool is_null( void ) const;

    /// @brief Convert to an in64_t.
    operator int64_t() const;

    /// @brief Convert to a double.
    operator double() const;

    /// @brief Convert to a string.
    operator std::string() const;

    /// @brief Convert to a blob.
    operator blob() const;

private:
    friend class statement;
    field( sqlite3_stmt *stmt, int idx );

    sqlite3_stmt *_stmt;
    int           _index;
};

////////////////////////////////////////

} // namespace sqlite
