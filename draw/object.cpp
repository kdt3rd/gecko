
#include "object.h"

namespace draw
{

////////////////////////////////////////

object::object( void )
{
}

////////////////////////////////////////

void object::draw( gl::context &ctxt )
{
	if ( _program )
	{
		ctxt.use_program( _program );
		_program->set_uniform( "mvpMatrix", ctxt.current_matrix() );
	}

	auto va = _vao->bind();
	for ( auto part: _parts )
		va.draw( std::get<0>( part ), std::get<1>( part ), std::get<2>( part ) );

	if ( _program )
		ctxt.use_program();
}

////////////////////////////////////////

}

