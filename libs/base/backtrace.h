//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <vector>
#include <iostream>
#include <string>

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

}

