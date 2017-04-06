//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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



