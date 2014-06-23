
// From Andrei Alexandrescu (2012)

#pragma once

////////////////////////////////////////

namespace base
{

/// @brief Contains a value or an exception
template <class T>
class expected
{
public:
	/// @brief Construct with a value
	expected( const T &rhs )
		: _ham( rhs ), _got_ham( true )
	{
	}

	/// @brief Construct with an rvalue
	expected( T &&rhs )
		: _ham( std::move( rhs ) ), _got_ham( true )
	{
	}

	/// @brief Copy constructor
	expected( const expected &rhs )
		: _got_ham( rhs._got_ham )
	{
		if ( _got_ham )
			new(&_ham) T( rhs._ham );
		else
			new(&_spam) std::exception_ptr( rhs._spam );
	}

	/// @brief Move constructor
	expected( expected &&rhs )
		: _got_ham( rhs._got_ham )
	{
		if ( _got_ham )
			new(&_ham) T( std::move( rhs._ham ) );
		else
			new(&_spam) std::exception_ptr( std::move( rhs._spam ) );
	}

	/// @brief Swap
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

	/// @brief Construct from an exception
	template <class E>
	static expected<T> fromException( const E &exception )
	{
		if ( typeid(exception) != typeid(E) )
		{
			throw std::invalid_argument( "slicing detected" );
		}
		return fromException( std::make_exception_ptr( exception ) );
	}

	/// @brief Constructor from an exception pointer
	static expected<T> fromException( std::exception_ptr p )
	{
		expected<T> result;
		result._got_ham = false;
		new(&result._spam) std::exception_ptr( std::move( p ) );
		return result;
	}

	/// @brief Constructor from the current exception
	static expected<T> fromException( void )
	{
		return fromException( std::current_exception() );
	}

	/// @brief Do we have a value?
	bool valid( void ) const
	{
		return _got_ham;
	}

	/// @brief Get the value (or throw the exception)
	T &get( void )
	{
		if ( !_got_ham )
			std::rethrow_exception( _spam );
		return _ham;
	}

	/// @brief Get the const value (or throw the exception)
	const T &get( void ) const
	{
		if ( !_got_ham )
			std::rethrow_exception( _spam );
		return _ham;
	}

	/// @brief Check if we have an exception of type E
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

	/// @brief Run a function and return an expected value
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
