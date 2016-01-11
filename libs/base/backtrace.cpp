
#include "backtrace.h"
#include <execinfo.h>
#include <memory>
#include "scope_guard.h"
#include <unistd.h>

namespace base
{

void backtrace( void )
{
	void *array[10];
	int size = ::backtrace( array, 10 );
	::backtrace_symbols_fd( array, size, STDERR_FILENO );
}


////////////////////////////////////////


void backtrace( std::ostream &os, size_t nLevels )
{
	std::unique_ptr<void *[]> array( new void *[nLevels] );
	int size = ::backtrace( array.get(), static_cast<int>( nLevels ) );
	char **syms = ::backtrace_symbols( array.get(), size );
	on_scope_exit{ free( syms ); };

	for ( int i = 0; i < size; ++i )
		os << syms[i] << '\n';
}


////////////////////////////////////////


void backtrace( std::vector<std::string> &l, size_t nLevels )
{
	l.clear();

	std::unique_ptr<void *[]> array( new void *[nLevels] );
	int size = ::backtrace( array.get(), static_cast<int>( nLevels ) );
	char **syms = ::backtrace_symbols( array.get(), size );
	on_scope_exit{ free( syms ); };

	for ( int i = 0; i < size; ++i )
		l.emplace_back( std::string( syms[i] ) );
}


////////////////////////////////////////


} // namespace base
