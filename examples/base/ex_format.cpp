/// @name base
/// Formatting class
/// @{

#include <base/format.h>

// To use std::cout
#include <iostream>

// To print numbers as bits (0s and 1s)
#include <bitset>

// To print std::u32string as UTF-8
#include <utf/utf.h>

void example( void )
{
	/// The first format specifiers indicates a width 5, use '0' as the fill character, right aligned.
	/// The second format specifiers indicates a width 20, use '_' as the fill character, and right aligned.
	/// The last format specifiers indicates a precision of 3, with + sign for positive numbers.
	std::cout << base::format( "Some values: {0,w5,f0,ar} {1,w20,f_,ar} {2,+,p3}", 42, "testing", 3.141592 ) << std::endl;

	// Can also be assigned to a string.
	// And (using the utf library), std::u32string also work, along with the usual string, std::string, and characters.
	std::string str = base::format( "{0}{1} {2}{3}", std::u32string( U"¡" ), "Hola", std::string( "Mundo" ), '!' );
	std::cout << str << std::endl;

	// Let show numbers using different bases:
	std::cout << base::format( "{0,b10}", 42 ) << std::endl;
	std::cout << base::format( "0x{0,b16}", 42 ) << std::endl;
	std::cout << base::format( "0x{0,B16}", 42 ) << std::endl;
	std::cout << base::format( "0{0,b8}", 42 ) << std::endl;

	// Print a binary number (e.g. as bits).
	std::cout << base::format( "{0}", std::bitset<8>( 42 ) ) << std::endl;

	// Print pointers
	int x = 1;
	std::cout << base::format( "{0}", &x ) << std::endl;

	// Finally let's try something the will error out
	std::cout << base::format( "This will throw:\n  {0,w}", "xxx" ) << std::endl;
	return 0;
}

