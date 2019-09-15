// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "container.h"
#include "parameter.h"

#include <base/const_string.h>
#include <base/uri.h>

////////////////////////////////////////

namespace media
{
///
/// @brief Class reader provides a creator for readable media containers
///
class reader
{
public:
    reader( base::cstring n );
    virtual ~reader( void );

    const std::string &name( void ) const { return _name; }
    const std::string &description( void ) const { return _description; }

    virtual container
    create( const base::uri &u, const parameter_set &params ) = 0;

    /// Lower case extensions
    inline const std::vector<std::string> &extensions( void ) const
    {
        return _extensions;
    }

    inline const std::vector<std::vector<uint8_t>> &magic_numbers( void ) const
    {
        return _magics;
    }

    inline const parameter_definitions &parameters( void ) const
    {
        return _parms;
    }

    parameter_set default_parameters( void ) const;

    /// @brief creates a container based on the uri.
    ///
    /// This follows the following logic:
    ///  1. Look for handler for the media type based on extension
    ///  2. if #1 fails to open, and the uri passed is a file and exists,
    ///     it will open the file and read in the first N bytes and attempt
    ///     to find the reader based on magic numbers.
    ///  3. if the above fails, will look if the path is a directory. if so,
    ///     will try to open a container based on the contents of the directory.
    ///     An example of this would be an IMF package
    static container open(
        const base::uri &u, const parameter_set &openParams = parameter_set() );

    /// @brief attempts to determine the reader that might be used.
    ///
    /// if @param temp_open is true, the container will be temporarily opened and the
    /// exact parameters, if known, will be set. if false, only the extension of the
    /// uri will be used, and the default values of the parameters will be used to
    /// initialize the parameter_set
    static parameter_set
    query_parameters( const base::uri &u, bool temp_open = true );

    /// @brief used to register a reader.
    ///
    /// TODO: define plugin layer
    static void register_reader( const std::shared_ptr<reader> &r );

protected:
    std::string                       _name;
    std::string                       _description;
    std::vector<std::string>          _extensions;
    std::vector<std::vector<uint8_t>> _magics;
    std::vector<parameter_definition> _parms;

private:
    static container
    scan_header( const base::uri &u, const parameter_set &openParams );

    reader( const reader & ) = delete;
    reader( reader && )      = delete;
    reader &operator=( const reader & ) = delete;
    reader &operator=( reader && ) = delete;
};

} // namespace media
