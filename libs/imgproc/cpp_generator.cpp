
#include "cpp_generator.h"
#include <utf/utf.h>

namespace imgproc
{

////////////////////////////////////////

cpp_generator::cpp_generator( std::ostream &cpp )
	: _cpp( cpp )
{
}

////////////////////////////////////////

void cpp_generator::add_functions( const std::vector<std::shared_ptr<func>> &funcs )
{
	for ( auto f: funcs )
		_funcs[f->name()] = f;
}

////////////////////////////////////////

type cpp_generator::generate( const std::u32string &name, const std::vector<type> &types )
{
	auto f = get_function( name );
	const auto &args = f->args();

	precondition( args.size() == types.size(), "incorrect number of argument types" );

	auto sc = std::make_shared<scope>();
	for ( size_t a = 0; a < args.size(); ++a )
		sc->add( args[a], types[a] );

	std::stringstream code;
	compile( f, sc, code );

	return type( data_type::UINT8, 2 );
}

////////////////////////////////////////

void cpp_generator::compile( const std::shared_ptr<func> &f, const std::shared_ptr<scope> &sc, std::stringstream &code )
{
}

////////////////////////////////////////

std::shared_ptr<func> cpp_generator::get_function( const std::u32string &name )
{
	auto f = _funcs.find( name );
	if ( f == _funcs.end() )
		throw_runtime( "function {0} not found", name );
	return f->second;
}

////////////////////////////////////////

}

