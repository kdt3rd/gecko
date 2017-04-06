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
} // namespace media



