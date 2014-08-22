

template<typename Type, size_t N>
struct buffer
{
	buffer( )
	{
	}

	template<typename ...Args>
	set_size( Args ...args )
	{
		static_assert( N == sizeof...( args ), "invalid number of size" );
		doit<N-1>( args... );
	}

	template<size_t M, typename ...Args>
	void doit( int64_t s, Args ...args )
	{
		size[M] = s;
		doit<M-1>( args... );
	}

	template<>
	void doit<0>( int64_t s )
	{
		size[0] = s;
	}

	Type *data;

	int64_t offset[N], stride[N];

	Type *get( int64_t x[N] );
	{
		int64_t offset = x[0] - offset[0];
		for ( size_t i = 1; i < N; ++i )
			offset = offset * size[i] + ( x[i] - offset[i] );
	}
};


size[] = { 2, 3, 5 }

x[] = { 0, 1, 3 }


offset = 0 - 0;
offset = ( 0 * 3 ) + 1
offset = ( ( 0 * 3 ) + 1 ) * 5 + 3;

stride[] = { 5*3, 3, 1 }

offset = 0 * 15 + 1 * 3 + 3 * 1

Array = 2 x 3 x 5

// aaaaa
// bbbBb
// ccccc
//
// 00000
// 11111
// 22222

0 1 3

( 0 * 3 + 1 ) * 5 + 3

offset = x[0] * 3 * 5 + x[1] * 3 + x[2]

size of 2 doesn't really matter -> only when checking bounds.
