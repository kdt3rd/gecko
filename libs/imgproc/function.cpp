
#include <iterator>
#include "function.h"

namespace imgproc
{

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const function &f )
{
	if ( f.name().empty() )
		out << "lambda ( ";
	else
		out << "function " << f.name() << "( ";

	if ( !f.args().empty() )
	{
		std::copy( f.args().begin(), f.args().end()-1, std::ostream_iterator<std::u32string>( out, ", " ) );
		out << f.args().back();
	}
	out << " )\n{\n    ";
	out << f.result();
	out << ";\n}\n";

	return out;
}

////////////////////////////////////////

}

