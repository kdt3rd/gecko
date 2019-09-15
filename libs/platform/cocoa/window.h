// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "platform.h"

#include <memory>
#include <platform/window.h>

namespace platform
{
namespace cocoa
{
class context;

////////////////////////////////////////

/// @brief Cocoa implementation of window.
class window : public ::platform::window
{
public:
    window(
        window_type                                wt,
        const std::shared_ptr<::platform::screen> &s,
        const rect &                               p );
    ~window( void ) override;

    ::platform::context &hw_context( void ) override;

    void raise( void ) override;
    void lower( void ) override;

    void show( void ) override;
    void hide( void ) override;
    bool is_visible( void ) override;

    void fullscreen( bool fs ) override;

    void set_title( const std::string &t ) override;

    double scale_factor( void );

    void set_ns( void *nsview );

protected:
    void apply_minimum_size( coord_type w, coord_type h ) override;

    void make_current( const std::shared_ptr<cursor> & ) override;

    rect query_geometry( void ) override;
    bool update_geometry( rect &r ) override;

    void submit_delayed_expose( const rect &r ) override;

private:
    struct objcwrapper;
    std::unique_ptr<objcwrapper> _impl;
    std::shared_ptr<context>     _context;
};

////////////////////////////////////////

} // namespace cocoa
} // namespace platform
