
#include <signal.h>

int main( void )
{
	raise( SIGSEGV );
}
