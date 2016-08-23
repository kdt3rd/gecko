
#pragma once

namespace layout
{

////////////////////////////////////////

class symbol
{
public:
	enum class type
	{
		INVALID,
		EXTERNAL,
		SLACK,
		ERROR,
		DUMMY
	};

	symbol( void )
	{
	}

	symbol( type t, size_t id )
		: _type( t ), _id( id )
	{
	}

	type kind( void ) const
	{
		return _type;
	}

	size_t id( void ) const
	{
		return _id;
	}

	explicit operator bool() const
	{
		return _type != type::INVALID;
	}

	bool operator<( const symbol &s ) const
	{
		return _id < s._id;
	}

private:
	type _type = type::INVALID;
	size_t _id = 0;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const symbol &s )
{
	switch ( s.kind() )
	{
		case symbol::type::INVALID: out << "i" << s.id(); break;
		case symbol::type::EXTERNAL: out << "v" << s.id(); break;
		case symbol::type::SLACK: out << "s" << s.id(); break;
		case symbol::type::ERROR: out << "e" << s.id(); break;
		case symbol::type::DUMMY: out << "d" << s.id(); break;
	}
	return out;
}

////////////////////////////////////////

}

