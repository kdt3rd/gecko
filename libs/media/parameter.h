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

#include <base/variant.h>
#include <base/const_string.h>
#include <vector>


////////////////////////////////////////


namespace media
{

enum parameter_type
{
	PARAM_INT,
	PARAM_STRING,
	PARAM_FLOAT
};

class parameter_definition
{
public:
	typedef std::pair<int64_t,int64_t> int_range;
	typedef std::pair<double,double> float_range;
	typedef std::vector<std::string> string_list;

	parameter_definition( base::cstring name, parameter_type pt );
	parameter_definition( base::cstring name, int64_t minVal, int64_t maxVal );
	parameter_definition( base::cstring name, double minVal, double maxVal );
	parameter_definition( base::cstring name, std::vector<std::string> vals );

	~parameter_definition( void ) = default;

	const std::string &name( void ) const { return _name; }

	parameter_type type( void ) const { return _type; }

	void help( std::string h ) { _help = std::move( h ); }
	const std::string &help( void ) const { return _help; }

	bool has_constraint( void ) const { return _allowed_vals.valid(); }

	const int_range &range_int( void ) const;
	const std::pair<double, double> &range_float( void ) const;
	const std::vector<std::string> &string_vals( void ) const;
	
private:
	std::string _name;
	parameter_type _type;
	base::variant<int_range, float_range, string_list> _allowed_vals;
	std::string _help;
};

} // namespace media



