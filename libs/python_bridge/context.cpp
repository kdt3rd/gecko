//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#ifdef HAVE_PYTHON
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <Python.h>
#endif
#if PY_MAJOR_VERSION <= 1
#error python 2 or higher is required
#else
# if PY_MAJOR_VERSION < 3
#  define IS_PYTHON2 1
# elif PY_MAJOR_VERSION < 4
#  define IS_PYTHON3 1
# endif
#endif


#include "context.h"
#include <stdexcept>
#include <atomic>
#include <map>

////////////////////////////////////////

namespace python_bridge
{

#ifdef HAVE_PYTHON
static std::atomic_flag _active_context = ATOMIC_FLAG_INIT;
class context::priv_Impl
{
public:
	priv_Impl( const std::string &argv0 )
	{
		if ( _active_context.test_and_set( std::memory_order_acquire ) )
			throw std::runtime_error( "Current version of python does not allow multiple active contexts at once" );

#ifdef IS_PYTHON2
		_progname = strdup( argv0.c_str() );
#else
		_progname = Py_DecodeLocale( argv0.c_str(), nullptr );
		if ( ! _progname )
			throw std::runtime_error( "Unable to decode argv[0]" );
#endif
		Py_SetProgramName( _progname );
		Py_Initialize();
	}

	~priv_Impl( void )
	{
#ifdef IS_PYTHON2
		Py_Finalize();
		if ( _progname )
			::free( _progname );
#else
		if ( Py_FinalizeEx() < 0 )
			std::cerr << "ERROR shutting down python interpreter" << std::endl;
		if ( _progname )
			PyMem_RawFree( _progname );
#endif
		_active_context.clear( std::memory_order_release );
	}

	bool load_module( const char *fn, const std::string &tag )
	{
		auto i = _glob_modules.find( tag );
		if ( i != _glob_modules.end() )
			Py_DECREF( i->second );

#ifdef IS_PYTHON2
		PyObject *pName = PyString_FromString( fn );
#else
		PyObject *pName = PyUnicode_DecodeFSDefault( fn );
#endif
		// TODO: add error checks
		PyObject *pModule = PyImport_Import( pName );
		Py_DECREF( pName );
		if ( pModule )
		{
			_glob_modules[tag] = pModule;
			return true;
		}
		return false;
	}

private:
#ifdef IS_PYTHON2
	char *_progname = nullptr;
#else
	wchar_t *_progname = nullptr;
#endif
	std::map<std::string, PyObject *> _glob_modules;
};
#endif

////////////////////////////////////////

context::context( const std::string &argv0 )
#ifdef HAVE_PYTHON
		: _impl( new priv_Impl( argv0 ) )
#endif
{
}

////////////////////////////////////////

context::~context( void )
{
}

////////////////////////////////////////

bool context::run( const std::string &fn )
{
#ifdef HAVE_PYTHON
	return _impl->load_module( fn.c_str(), fn );
#else
	return false;
#endif
}

////////////////////////////////////////

bool context::run( const base::uri &u )
{
#ifdef HAVE_PYTHON
	throw std::logic_error( "Not Yet Implemented" );
#else
	return false;
#endif
}

////////////////////////////////////////

void context::push_eval_lookup( lookup &l )
{
#ifdef HAVE_PYTHON
#endif
}

////////////////////////////////////////

void context::pop_eval_lookup( void )
{
#ifdef HAVE_PYTHON
#endif
}

////////////////////////////////////////

bool context::evaluate( bool &result, const char *s )
{
#ifdef HAVE_PYTHON
#else
	return false;
#endif
}

////////////////////////////////////////

bool context::evaluate( int &result, const char *s )
{
#ifdef HAVE_PYTHON
#else
	return false;
#endif
}

////////////////////////////////////////

bool context::evaluate( float &result, const char *s )
{
#ifdef HAVE_PYTHON
#else
	return false;
#endif
}

////////////////////////////////////////

bool context::evaluate( double &result, const char *s )
{
#ifdef HAVE_PYTHON
#else
	return false;
#endif
}

////////////////////////////////////////

bool context::evaluate( int64_t &result, const char *s )
{
#ifdef HAVE_PYTHON
#else
	return false;
#endif
}

////////////////////////////////////////

bool context::evaluate( std::string &result, const char *s )
{
#ifdef HAVE_PYTHON
#else
	return false;
#endif
}

////////////////////////////////////////

bool context::evaluate( std::wstring &result, const char *s )
{
#ifdef HAVE_PYTHON
#else
	return false;
#endif
}

////////////////////////////////////////

} // namespace python_bridge

