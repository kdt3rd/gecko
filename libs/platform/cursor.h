// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////

namespace platform
{
enum standard_cursor : uint8_t
{
    DEFAULT = 0,
    TEXT,
    URL_LINK_POINTER,
    HELP,
    CONTEXT_MENU,
    PROGRESS, // background activity?
    WAIT,
    DND_COPY,
    DND_ALIAS,
    DND_NO_DROP,
    NOT_ALLOWED,
    ALL_SCROLL,
    RESIZE_ROW,
    RESIZE_COL,
    RESIZE_EAST,
    RESIZE_NORTH_EAST,
    RESIZE_NORTH,
    RESIZE_NORTH_WEST,
    RESIZE_WEST,
    RESIZE_SOUTH_WEST,
    RESIZE_SOUTH,
    RESIZE_SOUTH_EAST,
    RESIZE_EAST_WEST,
    RESIZE_NORTH_SOUTH,
    RESIZE_NORTH_EAST_SOUTH_WEST,
    RESIZE_NORTH_WEST_SOUTH_EAST,
    VERTICAL_TEXT, // horizontal i-beam
    CROSSHAIR,
    CELL,

    //	EYEDROPPER,
    //	XFORM_ROTATE,
    //	XFORM_SCALE,
    //	XFORM_PIVOT
};

class system;

///
/// @brief Class cursor provides...
///
class cursor
{
public:
    cursor( void );
    cursor( const cursor &c ) = delete;
    cursor( cursor &&c )      = delete;
    cursor &operator=( const cursor &c ) = delete;
    cursor &operator=( cursor &&c ) = delete;
    virtual ~cursor( void );

    virtual bool supports_color( void ) const     = 0;
    virtual bool supports_animation( void ) const = 0;

    virtual bool is_animated( void ) const = 0;

    // TODO: add ability to load / create custom cursor...

    static std::shared_ptr<cursor>
    load( standard_cursor sc, const std::shared_ptr<system> &s );

private:
};

} // namespace platform
