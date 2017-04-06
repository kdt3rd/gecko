//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "reader.h"
#include <base/file_system.h>
#include "file_sequence.h"

////////////////////////////////////////

namespace media
{

///
/// @brief Class file_per_sample_reader provides...
///
class file_per_sample_reader : public reader
{
public:
	file_per_sample_reader( base::cstring n );
	virtual ~file_per_sample_reader( void );

protected:
	std::shared_ptr<base::file_system> scan_samples( int64_t &first, int64_t &last, const file_sequence &fseq );

private:

};

} // namespace media



