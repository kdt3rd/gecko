//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
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

//	virtual bool supports_concept() const = 0;

	virtual container create( const base::uri &u, const std::vector<track_description> &td, const parameter_set &params ) = 0;

	/// Lower case extensions
	inline const std::vector<std::string> &extensions( void ) const { return _extensions; }

	inline const std::vector<parameter_definition> &parameters( void ) const { return _parms; }
	parameter_set default_parameters( void ) const;

	/// @brief try to find a writer based on the extension
	///
	/// NB: this may return an empty pointer (indicating no known extension)
	static std::shared_ptr<writer> find_by_ext( const base::uri &u );
	static std::shared_ptr<writer> find_by_name( const std::string &n );

	/// @brief based on extension, picks a writer, initializes a parameter set
	///
	/// if options is provided, treats it as a comma separated, name=value set
	/// of updates to the defaults, such as:
	///
	/// compression=jpeg,quality=95
	static parameter_set parameters_by_ext( const base::uri &u, const std::string &opts = std::string() );

	static std::vector< std::shared_ptr<writer> > available( void );

	/// @brief create a simple writer
	///
	/// This will create a writer with a default track description.
	static container open( const base::uri &u,
						   const parameter_set &params = parameter_set(),
						   const std::string &forcewriter = std::string() );

	static container open( const base::uri &u,
						   const std::vector<track_description> &td,
						   const parameter_set &params = parameter_set(),
						   const std::string &forcewriter = std::string() );

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



