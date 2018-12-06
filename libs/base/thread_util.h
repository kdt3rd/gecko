//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once


////////////////////////////////////////


namespace base
{

namespace thread
{

/// @brief Returns the best guess as to the number of cores existing
/// on the system.
///
/// std::thread::hardware_concurrency() does not always return a
/// useable value on all systems. This attempts to remedy that by
/// trying that first, then using other older API to guess.
long core_count( void );

/// @brief Override core count returned by above core_count
///
/// All items that use @sa core_count to initialize a number of
/// threads for a thread pool or similar object will instead return
/// the count provided here. Any value less than 0 will reset this,
/// and the core_count will return to returning the system count.
void override_core_count( long cnt = -1 );

} // namespace thread

} // namespace base



