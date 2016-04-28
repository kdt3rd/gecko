//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
int core_count( void );

/// @brief Override core count returned by above core_count
///
/// All items that use @sa core_count to initialize a number of
/// threads for a thread pool or similar object will instead return
/// the count provided here. Any value less than 0 will reset this,
/// and the core_count will return to returning the system count.
void override_core_count( int cnt = -1 );

} // namespace thread

} // namespace base



