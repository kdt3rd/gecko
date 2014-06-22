
#pragma once

////////////////////////////////////////

namespace core
{

template <class T>
class expected
{
public:
	expected( const T &rhs )
		: _ham( rhs ), _got_ham( true )
	{
	}

	expected( T &&rhs )
		: _ham( std::move( rhs ) ), _got_ham( true )
	{
	}

	expected( const expected &rhs )
		: _got_ham( rhs._got_ham )
	{
		if ( _got_ham )
			new(&_ham) T( rhs._ham );
		else
			new(&_spam) std::exception_ptr( rhs._spam );
	}

	expected( expected &&rhs )
		: _got_ham( rhs._got_ham )
	{
		if ( _got_ham )
			new(&_ham) T( std::move( rhs._ham ) );
		else
			new(&_spam) std::exception_ptr( std::move( rhs._spam ) );
	}

	void swap( expected& rhs )
	{
		if ( _got_ham )
		{
			if ( rhs._got_ham )
			{
				using std::swap;
				swap( _ham, rhs._ham );
			}
			else
			{
				auto t = std::move( rhs._spam );
				new(&rhs._ham) T( std::move( _ham ) );
				new(&_spam) std::exception_ptr( t );
			}
		}
		else
	   	{
			if ( rhs._got_ham )
			{
				rhs.swap( *this );
			}
			else
		   	{
				_spam.swap( rhs._spam );
				std::swap( _got_ham, rhs._got_ham );
			}
		}
	}

	template <class E>
	static expected<T> fromException( const E &exception )
	{
		if ( typeid(exception) != typeid(E) )
		{
			throw std::invalid_argument( "slicing detected" );
		}
		return fromException( std::make_exception_ptr( exception ) );
	}

	static expected<T> fromException( std::exception_ptr p )
	{
		expected<T> result;
		result._got_ham = false;
		new(&result._spam) std::exception_ptr( std::move( p ) );
		return result;
	}

	static expected<T> fromException( void )
	{
		return fromException( std::current_exception() );
	}

	bool valid( void ) const
	{
		return _got_ham;
	}

	T &get( void )
	{
		if ( !_got_ham )
			std::rethrow_exception( _spam );
		return _ham;
	}

	const T &get( void ) const
	{
		if ( !_got_ham )
			std::rethrow_exception( _spam );
		return _ham;
	}

	template <class E>
	bool hasException( void ) const
	{
		try
		{
			if ( !_got_ham )
				std::rethrow_exception(_spam);
		}
	   	catch ( const E& object )
		{
			return true;
		}
	   	catch (...)
		{
		}
		return false;
	}

	template <class F>
	static expected fromCode( F fun )
	{
		try
		{
			return expected( fun() );
		}
		catch (...)
		{
			return fromException();
		}
	}

private:
	union
	{
		T _ham;
		std::exception_ptr _spam;
	};

	bool _got_ham;

	expected(void )
	{
	}
};

}
