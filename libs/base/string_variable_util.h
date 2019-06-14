// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

////////////////////////////////////////

namespace base
{
////////////////////////////////////////

/// Finds variables of the form $foo and ${foo}
/// also supported are most of the bash variable
/// references $foo[1]
bool find_unix_vars(
    std::vector<std::string> &pieces,
    std::vector<std::string> &vars,
    const std::string &       str );

/// applies a bash-like variable substitution
std::string replace_unix_vars(
    const std::string &                                      str,
    const std::function<std::string( const std::string & )> &varLookup );

////////////////////////////////////////

/// Finds variables of the form %foo%
bool find_dos_vars(
    std::vector<std::string> &pieces,
    std::vector<std::string> &vars,
    const std::string &       str );

/// applies dos-like variable substitution (%foo% for value).
/// no support for delayed evaluation has been provided
std::string replace_dos_vars(
    const std::string &                                str,
    std::function<std::string( const std::string & )> &varLookup );

} // namespace base
