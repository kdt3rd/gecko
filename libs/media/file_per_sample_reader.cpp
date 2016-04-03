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

#include "file_per_sample_reader.h"
#include "file_sequence.h"
#include <base/string_util.h>
#include <limits>

////////////////////////////////////////

namespace media
{

////////////////////////////////////////

file_per_sample_reader::file_per_sample_reader( base::cstring n )
	: reader( std::move( n ) )
{
}

////////////////////////////////////////

file_per_sample_reader::~file_per_sample_reader( void )
{
}


////////////////////////////////////////


std::shared_ptr<base::file_system>
file_per_sample_reader::scan_samples( int64_t &first, int64_t &last, const file_sequence &fseq )
{
	first = std::numeric_limits<int64_t>::max();
	last = std::numeric_limits<int64_t>::min();

	
	auto fs = base::file_system::get( fseq.uri() );
	{
		auto dir = fs->readdir( fseq.parent() );
		while ( ++dir )
		{
			int64_t f = 0;
			if ( fseq.extract_frame( *dir, f ) )
			{
				first = std::min( first, f );
				last = std::max( last, f );
			}
		}
	}
	return fs;
}

////////////////////////////////////////

} // media



