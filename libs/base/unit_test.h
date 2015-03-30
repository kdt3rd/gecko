
#pragma once

#include <string>
#include <functional>
#include <map>

namespace base
{

////////////////////////////////////////

class unit_test
{
public:
	unit_test( const std::string &n, const std::function<void(void)> &t );
	~unit_test( void );

	void test( void ) const;

	static void run( void );
	static void run( const std::string &n );

private:
	static std::map<std::string,unit_test*> _tests;

	std::string _name;
	std::function<void(void)> _test;
};

////////////////////////////////////////

}

