
#include "cpp_generator.h"
#include "variable.h"
#include <utf/utf.h>

namespace imgproc
{

////////////////////////////////////////

cpp_generator::cpp_generator( std::ostream &cpp )
	: _cpp( cpp )
{
	auto table = std::make_shared<function_table>( [&](const std::u32string &f ,const std::vector<type> &args )
	{
		return this->generate( f, args );
	} );

	_globals->set_functions( table );

	// Add built-in functions
	table->add( U"floor", std::make_shared<func>( U"floor", U"x" ) );
	table->add( U"ceil", std::make_shared<func>( U"ceil", U"x" ) );
	table->add( U"abs", std::make_shared<func>( U"abs", U"x" ) );
}

////////////////////////////////////////

void cpp_generator::add_functions( const std::vector<std::shared_ptr<func>> &funcs )
{
	auto table = _globals->functions();
	for ( auto f: funcs )
		table->add( f->name(), f );
}

////////////////////////////////////////

type cpp_generator::generate( const std::u32string &name, const std::vector<type> &types )
{
	std::cout << std::flush;
	auto f = get_function( name );
	const auto &args = f->args();

	precondition( args.size() == types.size(), "expected {0} argument types, but got {1}", args.size(), types.size() );

	if ( !f->result() )
	{
		// Built-in function
		return types.front();
	}

	auto sc = std::make_shared<scope>( _globals );
	for ( size_t a = 0; a < args.size(); ++a )
		sc->add( args[a], types[a] );

	auto t = f->result()->result_type( sc );

	std::cout << "Generating for: " << name << '\n';
	for ( size_t i = 0 ; i < types.size(); ++i )
		std::cout << "  " << types[i] << '\n';
	std::cout << " => " << t << std::endl;

	return t;
}

////////////////////////////////////////

void cpp_generator::compile( const std::shared_ptr<func> &f, std::shared_ptr<scope> &sc )
{
	std::ostringstream code;
}

////////////////////////////////////////

type result_type( const std::shared_ptr<expr> &exp, const std::shared_ptr<scope> &scope )
{
	return type();
}

////////////////////////////////////////

std::shared_ptr<func> cpp_generator::get_function( const std::u32string &name )
{
	auto f = _globals->functions()->get( name );
	if ( !f )
		throw_runtime( "function \"{0}\" not found", name );
	return f;
}

////////////////////////////////////////

}

