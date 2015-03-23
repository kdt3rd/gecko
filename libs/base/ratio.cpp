
#include "ratio.h"
#include "math_functions.h"

namespace base
{

////////////////////////////////////////

void ratio::simplify( void )
{
	int64_t d = math::gcd( _num, _den );
	if ( d > 1 )
	{
		_num /= d;
		_den /= d;
	}
}

////////////////////////////////////////

}

