// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

// TODO: we could forward declare, but subclasses probably want most of these?
#include "gradient.h"
#include "paint.h"

#include <gl/mesh.h>
#include <gl/program.h>
#include <gl/texture.h>
#include <map>
#include <platform/context.h>
#include <string>

namespace platform
{
class context;
}

namespace draw
{
////////////////////////////////////////

class drawable
{
public:
    virtual ~drawable( void );

    virtual void rebuild( platform::context &ctxt ) = 0;
    virtual void draw( platform::context &ctxt )    = 0;

protected:
    std::shared_ptr<gl::texture>
    new_gradient( gl::api &ogl, const gradient &g, size_t n = 128 );

    std::shared_ptr<gl::program> new_program(
        gl::api &          ogl,
        const std::string &vert,
        const std::string &frag,
        bool               cached = true );

    std::shared_ptr<gl::texture>
    get_fill_texture( gl::api &ogl, const paint &p );

    gl::program::uniform fill_mesh(
        gl::api &          ogl,
        gl::mesh &         m,
        const paint &      p,
        const std::string &vert = "position_uv.vert" );

    gl::program::uniform stroke_mesh(
        gl::api &          ogl,
        gl::mesh &         m,
        const paint &      p,
        const std::string &vert = "simple.vert" );

private:
    static std::map<std::string, std::weak_ptr<gl::program>> _program_cache;
};

////////////////////////////////////////

} // namespace draw
