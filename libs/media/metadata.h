//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/variant.h>
#include <base/const_string.h>
//#include <experimental/any>
#include <base/any.h>
#include <map>


////////////////////////////////////////


namespace media
{

///
/// exif types:
/// val bytes type
/// 1   1     unsigned byte
/// 2   1     ascii string
/// 3   2     unsigned short
/// 4   4     unsigned long
/// 5   8     unsigned rational (2x long)
/// 6   1     signed byte
/// 7   1     undefined
/// 8   2     signed short
/// 9   4     signed long
/// 10  8     signed rational
/// 11  4     single float
/// 12  8     double float
///
/// any of the above can be an array (size in file is n elements, not bytes)
///
/// tiff type       bytes
/// byte            1
/// ascii           1
/// unsigned short  2
/// unsigned long   4
/// rational        8 (2xunsigned long)
/// signed byte     1
/// signed short    2
/// signed long     4
/// signed rational 8 (2x long)
/// float           4
/// double          8
/// ifd             4 (%32-bit unsigned integer)
/// long8           8
/// signed long8    8
/// ifd8            8 (%64-bit unsigned integer)
/// 
/// exr attributes
///   basic types   bytes
/// ---------------------
/// unsigned char   1
/// short           2
/// unsigned short  2
/// int             4
/// unsigned int    4
/// unsigned long   8
/// half            2
/// float           4
/// double          8
///
///   predefined attr types
/// -----------------------
///   box2i          4 int
///   box2f          4 float
///   chlist         sequence of channels followed by a null byte (0x00
///                  channel
///                    name       : 0-terminated string, from 1 - 255 bytes
///                    pixel_type : int (0 uint, 1 half, 2 float)
///                    pLinear    : unsigned char (0 or 1)
///                    reserved   : 3 char, should be 0
///                    xSampling  : int
///                    ySampling  : int
///   chromaticities 8 float (red.x, red.y, green.x, green.y, blue.x, blue.y, white.x, white.y)
///   compression    unsigned char (enum)
///   double         double
///   envmap         unsigned char (enum)
///   float          float
///   int            int
///   keycode        7 int : filmMfcCode, filmType, prefix, count, perfOffset, perfsPerFrame, perfsPerCnt
///   lineOrder      unsigned char (enum)
///   m33f           9 float
///   m44f           16 float
///   preview        2 unsigned int (w,h), followed by 4*w*h unsigned char
///   rational       int followed by unsigned int
///   string         int followed by sequence of char
///   stringvector   0 or more string
///   tiledesc       2 unsigned ints (x,y size) followed by mode (unsigned char)
///   timecode       2 unsigned int
///   v2i            2 int
///   v2f            2 float
///   v3i            3 int
///   v3f            3 float
///
///
///
///   both exif and tiff use a uint16 to denote the entry (which others have mapped to a string table)
///   exr has a string name for each metadata attribute
///
///
///
///   
/// @brief metadata provides a container of metadata items.
///
/// The basic premise of this class is to be able to store metadata
/// such as random attributes from an OpenEXR file, or XMP data, or
/// similar. This data takes the form of being largely a string to
/// value map, where the value is of some set of types (including the
/// possiblity of having a metadata chunk itself to support having
/// structures).
///
/// XMP has the following types:
/// property
///   - have qualifiers (another property)
/// structure (name -> property bag)
/// arrays
///   - unordered (bag)
///   - ordered (sequence)
///   - alternative (supposed to choose one of, i.e. multiple languages)
/// 
/// qualifiers (attached to simple property value, further describe simple property value)
///
/// NB: for now, let's just typedef metadata to a map to avoid
/// implementing anything until we see what we end up with
//using metadata_value = std::experimental::any;
using metadata_value = base::any;
typedef std::map<std::string, metadata_value> metadata;

#define media_ImageDescription "img_desc"
#define media_ImageDisplayWin "disp_win"
#define media_ImagePixelAspectRatio "img_pixAR"
} // namespace media



