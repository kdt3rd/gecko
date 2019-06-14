// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace base
{
/// @brief Retrieves a backtrace of 10 functions and outputs to stderr
///
/// This involves no calls to malloc, so can be used in scenarios
/// where that is necessary (i.e. signal handlers)
void backtrace( void );

/// @brief Outputs the backtrace to the provided stream
///
/// NB: This will call malloc and similar functions - be careful
/// where it is called from
void backtrace( std::ostream &os, size_t nLevels = 10 );

/// @brief Fills a vector of strings with the backtrace
///
/// NB: This will call malloc and similar functions - be careful where
/// it is called from
void backtrace( std::vector<std::string> &l, size_t nLevels = 10 );

} // namespace base
