
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
	using target = gl::texture::target;

	if ( _program )
	{
		ctxt.use_program( _program );
		_program->set_uniform( "mvp_matrix", ctxt.current_matrix() );
	}

	std::vector<gl::bound_texture> bindings;
	for ( auto &t: _textures )
		bindings.push_back( std::move( t->bind( target::TEXTURE_RECTANGLE, bindings.size() ) ) );

	auto va = _vao->bind();
	for ( auto part: _parts )
		va.draw( std::get<0>( part ), std::get<1>( part ), std::get<2>( part ) );

	if ( _program )
		ctxt.use_program();
}

////////////////////////////////////////

}

