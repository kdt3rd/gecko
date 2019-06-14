// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#include "colors.h"

#include <base/contract.h>

namespace draw
{
////////////////////////////////////////

const std::vector<std::string> &palette_names( void )
{
    static std::vector<std::string> names = {
        "Red",         "Pink",       "Purple",    "Deep Purple", "Indigo",
        "Blue",        "Light Blue", "Cyan",      "Teal",        "Green",
        "Light Green", "Lime",       "Yellow",    "Amber",       "Orange",
        "Deep Orange", "Brown",      "Blue Grey", "Grey"
    };

    return names;
}

////////////////////////////////////////

const palette &get_palette( const std::string &name )
{
    if ( name == "Red" )
        return red;
    if ( name == "Pink" )
        return pink;
    if ( name == "Purple" )
        return purple;
    if ( name == "Deep Purple" )
        return deep_purple;
    if ( name == "Indigo" )
        return indigo;
    if ( name == "Blue" )
        return blue;
    if ( name == "Light Blue" )
        return light_blue;
    if ( name == "Cyan" )
        return cyan;
    if ( name == "Teal" )
        return teal;
    if ( name == "Green" )
        return green;
    if ( name == "Light Green" )
        return light_green;
    if ( name == "Lime" )
        return lime;
    if ( name == "Yellow" )
        return yellow;
    if ( name == "Amber" )
        return amber;
    if ( name == "Orange" )
        return orange;
    if ( name == "Deep Orange" )
        return deep_orange;
    if ( name == "Brown" )
        return brown;
    if ( name == "Blue Grey" )
        return blue_grey;
    if ( name == "Grey" )
        return grey;
    throw_runtime( "palette {0} not found", name );
}

////////////////////////////////////////

const palette red = { { gl::color::make8( 0xFF, 0xEB, 0xEE ),
                        gl::color::make8( 0xFF, 0xCD, 0xD2 ),
                        gl::color::make8( 0xEF, 0x9A, 0x9A ),
                        gl::color::make8( 0xE5, 0x73, 0x73 ),
                        gl::color::make8( 0xEF, 0x53, 0x50 ),
                        gl::color::make8( 0xF4, 0x43, 0x36 ),
                        gl::color::make8( 0xE5, 0x39, 0x35 ),
                        gl::color::make8( 0xD3, 0x2F, 0x2F ),
                        gl::color::make8( 0xC6, 0x28, 0x28 ),
                        gl::color::make8( 0xB7, 0x1C, 0x1C ) } };

////////////////////////////////////////

const palette pink = { { gl::color::make8( 0xFC, 0xE4, 0xEC ),
                         gl::color::make8( 0xF8, 0xBB, 0xD0 ),
                         gl::color::make8( 0xF4, 0x8F, 0xB1 ),
                         gl::color::make8( 0xF0, 0x62, 0x92 ),
                         gl::color::make8( 0xEC, 0x40, 0x7A ),
                         gl::color::make8( 0xE9, 0x1E, 0x63 ),
                         gl::color::make8( 0xD8, 0x1B, 0x60 ),
                         gl::color::make8( 0xC2, 0x18, 0x5B ),
                         gl::color::make8( 0xAD, 0x14, 0x57 ),
                         gl::color::make8( 0x88, 0x0E, 0x4F ) } };

////////////////////////////////////////

const palette purple = { { gl::color::make8( 0xF3, 0xE5, 0xF5 ),
                           gl::color::make8( 0xE1, 0xBE, 0xE7 ),
                           gl::color::make8( 0xCE, 0x93, 0xD8 ),
                           gl::color::make8( 0xBA, 0x68, 0xC8 ),
                           gl::color::make8( 0xAB, 0x47, 0xBC ),
                           gl::color::make8( 0x9C, 0x27, 0xB0 ),
                           gl::color::make8( 0x8E, 0x24, 0xAA ),
                           gl::color::make8( 0x7B, 0x1F, 0xA2 ),
                           gl::color::make8( 0x6A, 0x1B, 0x9A ),
                           gl::color::make8( 0x4A, 0x14, 0x8C ) } };

////////////////////////////////////////

const palette deep_purple = { { gl::color::make8( 0xED, 0xE7, 0xF6 ),
                                gl::color::make8( 0xD1, 0xC4, 0xE9 ),
                                gl::color::make8( 0xB3, 0x9D, 0xDB ),
                                gl::color::make8( 0x95, 0x75, 0xCD ),
                                gl::color::make8( 0x7E, 0x57, 0xC2 ),
                                gl::color::make8( 0x67, 0x3A, 0xB7 ),
                                gl::color::make8( 0x5E, 0x35, 0xB1 ),
                                gl::color::make8( 0x51, 0x2D, 0xA8 ),
                                gl::color::make8( 0x45, 0x27, 0xA0 ),
                                gl::color::make8( 0x31, 0x1B, 0x92 ) } };

////////////////////////////////////////

const palette indigo = { { gl::color::make8( 0xE8, 0xEA, 0xF6 ),
                           gl::color::make8( 0xC5, 0xCA, 0xE9 ),
                           gl::color::make8( 0x9F, 0xA8, 0xDA ),
                           gl::color::make8( 0x79, 0x86, 0xCB ),
                           gl::color::make8( 0x5C, 0x6B, 0xC0 ),
                           gl::color::make8( 0x3F, 0x51, 0xB5 ),
                           gl::color::make8( 0x39, 0x49, 0xAB ),
                           gl::color::make8( 0x30, 0x3F, 0x9F ),
                           gl::color::make8( 0x28, 0x35, 0x93 ),
                           gl::color::make8( 0x1A, 0x23, 0x7E ) } };

////////////////////////////////////////

const palette blue = { { gl::color::make8( 0xE3, 0xF2, 0xFD ),
                         gl::color::make8( 0xBB, 0xDE, 0xFB ),
                         gl::color::make8( 0x90, 0xCA, 0xF9 ),
                         gl::color::make8( 0x64, 0xB5, 0xF6 ),
                         gl::color::make8( 0x42, 0xA5, 0xF5 ),
                         gl::color::make8( 0x21, 0x96, 0xF3 ),
                         gl::color::make8( 0x1E, 0x88, 0xE5 ),
                         gl::color::make8( 0x19, 0x76, 0xD2 ),
                         gl::color::make8( 0x15, 0x65, 0xC0 ),
                         gl::color::make8( 0x0D, 0x47, 0xA1 ) } };

////////////////////////////////////////

const palette light_blue = { { gl::color::make8( 0xE1, 0xF5, 0xFE ),
                               gl::color::make8( 0xB3, 0xE5, 0xFC ),
                               gl::color::make8( 0x81, 0xD4, 0xFA ),
                               gl::color::make8( 0x4F, 0xC3, 0xF7 ),
                               gl::color::make8( 0x29, 0xB6, 0xF6 ),
                               gl::color::make8( 0x03, 0xA9, 0xF4 ),
                               gl::color::make8( 0x03, 0x9B, 0xE5 ),
                               gl::color::make8( 0x02, 0x88, 0xD1 ),
                               gl::color::make8( 0x02, 0x77, 0xBD ),
                               gl::color::make8( 0x01, 0x57, 0x9B ) } };

////////////////////////////////////////

const palette cyan = { { gl::color::make8( 0xE0, 0xF7, 0xFA ),
                         gl::color::make8( 0xB2, 0xEB, 0xF2 ),
                         gl::color::make8( 0x80, 0xDE, 0xEA ),
                         gl::color::make8( 0x4D, 0xD0, 0xE1 ),
                         gl::color::make8( 0x26, 0xC6, 0xDA ),
                         gl::color::make8( 0x00, 0xBC, 0xD4 ),
                         gl::color::make8( 0x00, 0xAC, 0xC1 ),
                         gl::color::make8( 0x00, 0x97, 0xA7 ),
                         gl::color::make8( 0x00, 0x83, 0x8F ),
                         gl::color::make8( 0x00, 0x60, 0x64 ) } };

////////////////////////////////////////

const palette teal = { { gl::color::make8( 0xE0, 0xF2, 0xF1 ),
                         gl::color::make8( 0xB2, 0xDF, 0xDB ),
                         gl::color::make8( 0x80, 0xCB, 0xC4 ),
                         gl::color::make8( 0x4D, 0xB6, 0xAC ),
                         gl::color::make8( 0x26, 0xA6, 0x9A ),
                         gl::color::make8( 0x00, 0x96, 0x88 ),
                         gl::color::make8( 0x00, 0x89, 0x7B ),
                         gl::color::make8( 0x00, 0x79, 0x6B ),
                         gl::color::make8( 0x00, 0x69, 0x5C ),
                         gl::color::make8( 0x00, 0x4D, 0x40 ) } };

////////////////////////////////////////

const palette green = { { gl::color::make8( 0xE8, 0xF5, 0xE9 ),
                          gl::color::make8( 0xC8, 0xE6, 0xC9 ),
                          gl::color::make8( 0xA5, 0xD6, 0xA7 ),
                          gl::color::make8( 0x81, 0xC7, 0x84 ),
                          gl::color::make8( 0x66, 0xBB, 0x6A ),
                          gl::color::make8( 0x4C, 0xAF, 0x50 ),
                          gl::color::make8( 0x43, 0xA0, 0x47 ),
                          gl::color::make8( 0x38, 0x8E, 0x3C ),
                          gl::color::make8( 0x2E, 0x7D, 0x32 ),
                          gl::color::make8( 0x1B, 0x5E, 0x20 ) } };

////////////////////////////////////////

const palette light_green = { { gl::color::make8( 0xF1, 0xF8, 0xE9 ),
                                gl::color::make8( 0xDC, 0xED, 0xC8 ),
                                gl::color::make8( 0xC5, 0xE1, 0xA5 ),
                                gl::color::make8( 0xAE, 0xD5, 0x81 ),
                                gl::color::make8( 0x9C, 0xCC, 0x65 ),
                                gl::color::make8( 0x8B, 0xC3, 0x4A ),
                                gl::color::make8( 0x7C, 0xB3, 0x42 ),
                                gl::color::make8( 0x68, 0x9F, 0x38 ),
                                gl::color::make8( 0x55, 0x8B, 0x2F ),
                                gl::color::make8( 0x33, 0x69, 0x1E ) } };

////////////////////////////////////////

const palette lime = { { gl::color::make8( 0xF9, 0xFB, 0xE7 ),
                         gl::color::make8( 0xF0, 0xF4, 0xC3 ),
                         gl::color::make8( 0xE6, 0xEE, 0x9C ),
                         gl::color::make8( 0xDC, 0xE7, 0x75 ),
                         gl::color::make8( 0xD4, 0xE1, 0x57 ),
                         gl::color::make8( 0xCD, 0xDC, 0x39 ),
                         gl::color::make8( 0xC0, 0xCA, 0x33 ),
                         gl::color::make8( 0xAF, 0xB4, 0x2B ),
                         gl::color::make8( 0x9E, 0x9D, 0x24 ),
                         gl::color::make8( 0x82, 0x77, 0x17 ) } };

////////////////////////////////////////

const palette yellow = { { gl::color::make8( 0xFF, 0xFD, 0xE7 ),
                           gl::color::make8( 0xFF, 0xF9, 0xC4 ),
                           gl::color::make8( 0xFF, 0xF5, 0x9D ),
                           gl::color::make8( 0xFF, 0xF1, 0x76 ),
                           gl::color::make8( 0xFF, 0xEE, 0x58 ),
                           gl::color::make8( 0xFF, 0xEB, 0x3B ),
                           gl::color::make8( 0xFD, 0xD8, 0x35 ),
                           gl::color::make8( 0xFB, 0xC0, 0x2D ),
                           gl::color::make8( 0xF9, 0xA8, 0x25 ),
                           gl::color::make8( 0xF5, 0x7F, 0x17 ) } };

////////////////////////////////////////

const palette amber = { { gl::color::make8( 0xFF, 0xF8, 0xE1 ),
                          gl::color::make8( 0xFF, 0xEC, 0xB3 ),
                          gl::color::make8( 0xFF, 0xE0, 0x82 ),
                          gl::color::make8( 0xFF, 0xD5, 0x4F ),
                          gl::color::make8( 0xFF, 0xCA, 0x28 ),
                          gl::color::make8( 0xFF, 0xC1, 0x07 ),
                          gl::color::make8( 0xFF, 0xB3, 0x00 ),
                          gl::color::make8( 0xFF, 0xA0, 0x00 ),
                          gl::color::make8( 0xFF, 0x8F, 0x00 ),
                          gl::color::make8( 0xFF, 0x6F, 0x00 ) } };

////////////////////////////////////////

const palette orange = { { gl::color::make8( 0xFF, 0xF3, 0xE0 ),
                           gl::color::make8( 0xFF, 0xE0, 0xB2 ),
                           gl::color::make8( 0xFF, 0xCC, 0x80 ),
                           gl::color::make8( 0xFF, 0xB7, 0x4D ),
                           gl::color::make8( 0xFF, 0xA7, 0x26 ),
                           gl::color::make8( 0xFF, 0x98, 0x00 ),
                           gl::color::make8( 0xFB, 0x8C, 0x00 ),
                           gl::color::make8( 0xF5, 0x7C, 0x00 ),
                           gl::color::make8( 0xEF, 0x6C, 0x00 ),
                           gl::color::make8( 0xE6, 0x51, 0x00 ) } };

////////////////////////////////////////

const palette deep_orange = { { gl::color::make8( 0xFB, 0xE9, 0xE7 ),
                                gl::color::make8( 0xFF, 0xCC, 0xBC ),
                                gl::color::make8( 0xFF, 0xAB, 0x91 ),
                                gl::color::make8( 0xFF, 0x8A, 0x65 ),
                                gl::color::make8( 0xFF, 0x70, 0x43 ),
                                gl::color::make8( 0xFF, 0x57, 0x22 ),
                                gl::color::make8( 0xF4, 0x51, 0x1E ),
                                gl::color::make8( 0xE6, 0x4A, 0x19 ),
                                gl::color::make8( 0xD8, 0x43, 0x15 ),
                                gl::color::make8( 0xBF, 0x36, 0x0C ) } };

////////////////////////////////////////

const palette brown = { { gl::color::make8( 0xEF, 0xEB, 0xE9 ),
                          gl::color::make8( 0xD7, 0xCC, 0xC8 ),
                          gl::color::make8( 0xBC, 0xAA, 0xA4 ),
                          gl::color::make8( 0xA1, 0x88, 0x7F ),
                          gl::color::make8( 0x8D, 0x6E, 0x63 ),
                          gl::color::make8( 0x79, 0x55, 0x48 ),
                          gl::color::make8( 0x6D, 0x4C, 0x41 ),
                          gl::color::make8( 0x5D, 0x40, 0x37 ),
                          gl::color::make8( 0x4E, 0x34, 0x2E ),
                          gl::color::make8( 0x3E, 0x27, 0x23 ) } };

////////////////////////////////////////

const palette blue_grey = { { gl::color::make8( 0xEC, 0xEF, 0xF1 ),
                              gl::color::make8( 0xCF, 0xD8, 0xDC ),
                              gl::color::make8( 0xB0, 0xBE, 0xC5 ),
                              gl::color::make8( 0x90, 0xA4, 0xAE ),
                              gl::color::make8( 0x78, 0x90, 0x9C ),
                              gl::color::make8( 0x60, 0x7D, 0x8B ),
                              gl::color::make8( 0x54, 0x6E, 0x7A ),
                              gl::color::make8( 0x45, 0x5A, 0x64 ),
                              gl::color::make8( 0x37, 0x47, 0x4F ),
                              gl::color::make8( 0x26, 0x32, 0x38 ) } };

////////////////////////////////////////

const palette grey = { { gl::color::make8( 0xFA, 0xFA, 0xFA ),
                         gl::color::make8( 0xF5, 0xF5, 0xF5 ),
                         gl::color::make8( 0xEE, 0xEE, 0xEE ),
                         gl::color::make8( 0xE0, 0xE0, 0xE0 ),
                         gl::color::make8( 0xBD, 0xBD, 0xBD ),
                         gl::color::make8( 0x9E, 0x9E, 0x9E ),
                         gl::color::make8( 0x75, 0x75, 0x75 ),
                         gl::color::make8( 0x61, 0x61, 0x61 ),
                         gl::color::make8( 0x42, 0x42, 0x42 ),
                         gl::color::make8( 0x21, 0x21, 0x21 ) } };

////////////////////////////////////////

} // namespace draw
