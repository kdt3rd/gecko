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

#include "exr_writer.h"
#if defined(HAVE_OPENEXR)
# include "reader.h"
# include "image_frame.h"
# include "video_track.h"
# include "file_sequence.h"
# include <base/string_util.h>
# include <base/file_system.h>
# include <thread>

# pragma GCC diagnostic push
# if defined(__clang__)
#  pragma GCC diagnostic ignored "-Wreserved-id-macro"
#  pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#  pragma GCC diagnostic ignored "-Wdeprecated-register"
#  pragma GCC diagnostic ignored "-Wweak-vtables"
# endif
# pragma GCC diagnostic ignored "-Wdeprecated"
# pragma GCC diagnostic ignored "-Wconversion"
# pragma GCC diagnostic ignored "-Wfloat-equal"
# pragma GCC diagnostic ignored "-Wshadow"
# pragma GCC diagnostic ignored "-Wsign-conversion"
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wunused-parameter"

# include <ImfMultiPartInputFile.h>
# include <ImfInputPart.h>
# include <ImfVersion.h>
# include <ImfStringAttribute.h>
# include <ImfStringVectorAttribute.h>
# include <ImfChannelList.h>
# include <ImfMultiView.h>
# include <ImfStandardAttributes.h>
# include <ImfThreading.h>
# pragma GCC diagnostic pop

# include <algorithm>
# include <iostream>
#endif


////////////////////////////////////////


namespace media
{

void
register_exr_writer( void )
{
#ifdef HAVE_OPENEXR
#endif
}


////////////////////////////////////////


exr_writer::exr_writer( void )
{
}


////////////////////////////////////////


exr_writer::~exr_writer( void )
{
}


////////////////////////////////////////


} // media



