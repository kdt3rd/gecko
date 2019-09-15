// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <cstddef>
#include <cstdint>
#ifdef _WIN32
#    include <malloc.h>
#endif
#include "contract.h"
#include "pointer.h"

#include <cstdlib>

////////////////////////////////////////

namespace base
{
class allocator
{
public:
    static constexpr size_t max_align = alignof( std::max_align_t );

    allocator( void );
    virtual ~allocator( void );

    // not copy-able
    allocator( const allocator & ) = delete;
    allocator &operator=( const allocator & ) = delete;
    allocator( allocator && )                 = delete;
    allocator &operator=( allocator && ) = delete;

    virtual std::shared_ptr<void>
    allocate( size_t bytes, size_t alignment = max_align ) = 0;
};

} // namespace base
