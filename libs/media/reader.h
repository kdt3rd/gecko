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

#include <base/const_string.h>
#include <base/uri.h>
#include "metadata.h"
#include "parameter.h"
#include "container.h"


////////////////////////////////////////


namespace media
{

///
/// @brief Class reader provides a creator for readable media containers
///
class reader
{
public:
	reader( base::cstring n );
	virtual ~reader( void );

	const std::string &name( void ) const { return _name; }
	const std::string &description( void ) const { return _description; }

	virtual container create( const base::uri &u, const metadata &params ) = 0;

	/// Lower case extensions
	inline const std::vector<std::string> &extensions( void ) const { return _extensions; }

	inline const std::vector<std::vector<uint8_t>> &magic_numbers( void ) const { return _magics; }

	inline const std::vector<parameter_definition> &parameters( void ) const { return _parms; }

	/// @brief creates a container based on the uri.
	///
	/// This follows the following logic:
	///  1. Look for handler for the media type based on extension
	///  2. if #1 fails to open, and the uri passed is a file and exists,
	///     it will open the file and read in the first N bytes and attempt
	///     to find the reader based on magic numbers.
	///  3. if the above fails, will look if the path is a directory. if so,
	///     will try to open a container based on the contents of the directory.
	///     An example of this would be an IMF package
	///
	/// Here we are overloading the use of metadata to pass parameters
	/// to the readers. They are kind of different, but the same.
	static container open( const base::uri &u,
						   const metadata &openParams = metadata() );

	static void register_reader( const std::shared_ptr<reader> &r );

protected:
	std::string _name;
	std::string _description;
	std::vector<std::string> _extensions;
	std::vector<std::vector<uint8_t>> _magics;
	std::vector<parameter_definition> _parms;
	
private:
	static container scan_header( const base::uri &u, const metadata &openParams );

	reader( const reader & ) = delete;
	reader( reader && ) = delete;
	reader &operator=( const reader & ) = delete;
	reader &operator=( reader && ) = delete;

};

} // namespace media



