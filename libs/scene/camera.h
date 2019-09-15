// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

////////////////////////////////////////

namespace scene
{
enum class lens_type
{
    rectilinear,     // gnomonic, perspective, conventional r = f tan(theta)
    stereographic,   // panoramic r = 2 f tan(theta / 2)
    equidistant,     // linear, linear scaled r = f * theta
    equisolid_angle, // equal area r = 2 * f * sin( theta / 2 )
    orthographic,    // r = f * sin( theta )
};

class lens
{
public:
    lens( lens_type ideallens );
    // lensfun lens name
    lens( const char *lensname );

    lens_type type( void ) const;

    void   focal_length( double l );
    double focal_length( void ) const;
};

class camera
{
public:
    camera( const char *name );

    void sensor_size( double x_mm, double y_mm );
    void set_lens( const lens &l );

    void position( const time &t, double x, double y, double z );
    void orientation( const time &t, double x, double y, double z );

    /// takes 1.4, 2.0 corresponding to f/1.4, f/2.0
    /// this is an ideal number, so there is no difference between
    /// f-number and t-stop
    void aperture( const time &t, double fnumber );
    void iso( const time &t, double iso );
    // in seconds (or more likely, fractions of a second)
    void shutter_speed( const time &t, double ss );

    /// computes and updates a scale to the iso to compensate such
    /// that the light accumulated corresponds to the provided
    /// fnumber, allowing one to have a "fake" fnumber controlling
    /// depth of field, but have the lighting be what it is
    void compensate_iso_for_fnumber( const time &t, double fnumber );
    void reset_iso_compensation( const time &t );
};

} // namespace scene
