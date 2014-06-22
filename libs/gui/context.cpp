
#include "context.h"
#include <base/contract.h>

namespace gui
{

std::vector<context *> context::_contexts;

////////////////////////////////////////

void context::push_context( void )
{
	_contexts.push_back( this );
}

////////////////////////////////////////

void context::pop_context( void )
{
	precondition( !_contexts.empty(), "context not available" );
	precondition( _contexts.back() == this, "incorrect context" );
	_contexts.pop_back();
}

////////////////////////////////////////

context &context::current( void )
{
	precondition( !_contexts.empty(), "context not available" );
	return *_contexts.back();
}

////////////////////////////////////////

}

