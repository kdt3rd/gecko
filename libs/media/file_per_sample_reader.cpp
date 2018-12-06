//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
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



