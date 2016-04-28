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
#include "track_description.h"


////////////////////////////////////////


namespace media
{

///
/// @brief Class writer provides...
///
class writer
{
public:
	writer( base::cstring n );
	virtual ~writer( void );

	const std::string &name( void ) const { return _name; }
	const std::string &description( void ) const { return _description; }

	virtual container create( const base::uri &u, const std::vector<track_description> &td, const metadata &params ) = 0;

	/// Lower case extensions
	inline const std::vector<std::string> &extensions( void ) const { return _extensions; }

	inline const std::vector<parameter_definition> &parameters( void ) const { return _parms; }

	static const std::string ForceWriterMetadataName;
	static container open( const base::uri &u,
						   const std::vector<track_description> &td,
						   const metadata &openParams = metadata() );

	static void register_writer( const std::shared_ptr<writer> &w );

protected:
	std::string _name;
	std::string _description;
	std::vector<std::string> _extensions;
	std::vector<parameter_definition> _parms;

private:
	writer( const writer & ) = delete;
	writer( writer && ) = delete;
	writer &operator=( const writer & ) = delete;
	writer &operator=( writer && ) = delete;

};

} // namespace media



