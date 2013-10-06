
#pragma once

////////////////////////////////////////

class rectangle
{
public:
	rectangle( float x, float y, float w, float h )
		: m_x( x ), m_y( y ), m_w( w ), m_h( h )
	{
	}

	inline float x( void ) const { return m_x; }
	inline float y( void ) const { return m_y; }
	inline float w( void ) const { return m_w; }
	inline float h( void ) const { return m_h; }

	inline rectangle operator*( float m ) const
	{
		return rectangle( x() * m, y() * m, w() * m, h() * m );
	}

private:
	float m_x, m_y;
	float m_w, m_h;
};

////////////////////////////////////////

// vim:ft=cpp:
