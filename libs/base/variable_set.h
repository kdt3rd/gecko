//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <string>
#include <vector>
#include <map>


////////////////////////////////////////


namespace base
{

///
/// @brief Class variable_set provides a simple wrapper around storing variables
///
/// Variables can be strings, arrays, or hash maps (but only one at a time)
class variable_set
{
public:
	void set( const std::string &v, std::string val );
	void set( const std::string &v, std::vector<std::string> val );
	void set( const std::string &v, std::map<std::string, std::string> val );
	void set( const std::string &v, size_t i, const std::string &val );
	void set( const std::string &v, const std::string &key, const std::string &val );

	void append( const std::string &v, const std::string &val );

	std::string get( const std::string &v ) const;
	std::string get( const std::string &v, size_t i ) const;
	std::string get( const std::string &v, const std::string &hkey ) const;

	/// supports bash style lookups, including recursive lookups
	/// NB: This is not const due to the bash syntax:
	/// ${foo:=bar}
	/// where if foo isn't set, it will set it's value to bar
	std::string lookup( const std::string &v );

	void unset( const std::string &v );

	size_t length( const std::string &v ) const;

	/// returns true if the variable is currently set
	bool defined( const std::string &v ) const;

	/// applies variable substitution using the set of variables.
	/// Unknown variables will be replaced with nothing
	/// NB: This is not const due to the bash syntax:
	/// ${foo:=bar}
	/// where if foo isn't set, it will set it's value to bar
	std::string substitute( const std::string &v );

private:
	void unset_simple( const std::string &v );
	void unset_array( const std::string &v );
	void unset_hash( const std::string &v );

	std::map<std::string, std::string> _simple_vars;
	std::map<std::string, std::vector<std::string> > _array_vars;
	std::map<std::string, std::map<std::string, std::string> > _hash_vars;
};

} // namespace base



