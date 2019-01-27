// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace scene
{

namespace detail
{
struct one_value
{
	double v;
};
struct three_value
{
	double x, y, z;
};

}

class transform_list
{
	small_vector<op_code, 8> _ops;
	small_vector<one_value
};

class transform_stack
{
public:
	void add_x_translate( const time &t, double tx );
	void add_y_translate( const time &t, double ty );
	void add_z_translate( const time &t, double tz );
	void add_translate( const time &t, double tx, double ty, double tz );

	void add_x_scale( const time &t, double sx );
	void add_y_scale( const time &t, double sy );
	void add_z_scale( const time &t, double sz );
	void add_scale( const time &t, double sx, double sy, double sz );

	void add_x_rotate( const time &t, double theta );
	void add_y_rotate( const time &t, double theta );
	void add_z_rotate( const time &t, double theta );
	void add_rotate( const time &t, double xtheta, double ytheta, double ztheta );

	void add_matrix( const time &t, const matrix &m );

	matrix collapse( const time &t );
	transform_stack invert( void ) const;
};

} // namespace scene

