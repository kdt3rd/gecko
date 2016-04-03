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

#include <base/variant.h>
#include <base/const_string.h>
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
typedef std::map<std::string, base::any> metadata;

} // namespace media



