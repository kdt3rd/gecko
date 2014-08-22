
#include <imgproc/buffer.h>

using namespace imgproc;

float lerp( float lo, float hi, float m )
{
	return hi * m + lo * ( 1.F - m );
}

buffer<float,1> uniform_buckets( int64_t nbuckets, float lo, float hi )
{
	buffer<float,1> result( nbuckets + 1 );

	for ( int64_t b = 0; b < nbuckets + 1; ++b )
	{
		result( b ) = lerp( lo, hi, float( b ) / float( nbuckets ) );
	}

	return result;
}

buffer<int64_t,1> histogram( const buffer<float,2> &img, const buffer<float,1> &buckets )
{
	buffer<int64_t,1> result( buckets.upper( 0 ) - buckets.lower( 0 ) - 1 );

	result.set_offset( buckets.lower( 0 ) );
	for ( int64_t b = result.lower( 0 ); b < result.upper( 0 ); ++b )
	{
		int64_t count = 0;
		for ( int64_t y = img.lower( 1 ); y < img.upper( 1 ); ++y )
		{
			for ( int64_t x = img.lower( 0 ); x < img.upper( 0 ); ++x )
			{
				if ( img( x, y ) > buckets( b ) && img( x, y ) < buckets( b + 1 ) )
					++count;
			}
		}
		result( b ) = count;
	}

	return result;
}

buffer<int64_t,1> test( buffer<float,2> &input )
{
	buffer<int64_t,1> result( 256 );
	result = histogram( input, uniform_buckets( 256, 0, 1 ) );
	return result;
}


