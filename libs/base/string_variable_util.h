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

#include <functional>
#include <string>
#include <vector>
#include <map>


////////////////////////////////////////


namespace base
{


////////////////////////////////////////


/// Finds variables of the form $foo and ${foo}
/// also supported are most of the bash variable
/// references $foo[1]
bool
find_unix_vars( std::vector<std::string> &pieces,
				std::vector<std::string> &vars,
				const std::string &str );

/// applies a bash-like variable substitution
std::string
replace_unix_vars( const std::string &str,
				   const std::function<std::string(const std::string &)> &varLookup );


////////////////////////////////////////


/// Finds variables of the form %foo%
bool
find_dos_vars( std::vector<std::string> &pieces,
			   std::vector<std::string> &vars,
			   const std::string &str );

/// applies dos-like variable substitution (%foo% for value).
/// no support for delayed evaluation has been provided
std::string
replace_dos_vars( const std::string &str,
				  std::function<std::string(const std::string &)> &varLookup );

} // namespace base



