/// @cond UTF

#include <vector>
#include <cassert>

////////////////////////////////////////

namespace
{
	template<typename T>
	struct Equal
	{
		static bool test( const T &a, const T &b )
		{
			return a == b;
		}
	};

	template<>
	struct Equal<double>
	{
		static bool test( const double &a, const double &b )
		{
			const uint64_t *x = reinterpret_cast<const uint64_t*>( &a );
			const uint64_t *y = reinterpret_cast<const uint64_t*>( &b );
			return *x == *y;
		}
	};
}

////////////////////////////////////////

template<typename T>
class compressed_table
{
public:
	compressed_table( const std::vector<T> &table, size_t bs )
		: block_size( bs )
	{
		assert( table.size() % block_size == 0 );

		for ( size_t i = 0; i < table.size(); i += block_size )
		{
			size_t found = data.size();

			// Search the data for this block.
			for ( size_t j = 0; j < data.size(); ++j )
			{
				for ( size_t k = 0; k < block_size; ++k )
				{
					if ( j+k < data.size() )
					{
						if ( ! Equal<T>::test( table[i+k], data[j+k] ) )
							break;
					}
					else
					{
						found = j;
						break;
					}
					if ( k == block_size-1 )
						found = j;
				}
				if ( found < data.size() )
					break;
			}

			if ( found + block_size >= data.size() )
			{
				data.resize( found + block_size );
				for ( size_t k = 0; k < block_size; ++k )
					data[found+k] = table[i+k];
			}
			blocks.push_back( found );
		}
	}

	T operator[]( size_t idx )
	{
		return at( idx );
	}

	T at( size_t idx )
	{
		size_t block = idx / block_size;
		size_t off = idx % block_size;
		return data[blocks[block]+off];
	}

	size_t size( void )
	{
		return blocks.size() * block_size;
	}

	size_t data_size( void )
	{
		return data.size();
	}

	size_t num_blocks( void )
	{
		return blocks.size();
	}

	void save( std::ostream &out, const std::string &dtype, const std::string &name )
	{
		assert( data.size() < UINT16_MAX );
		out << "const " << dtype << ' ' << name << "_data[" << data.size() << "] =\n{";
		for ( size_t i = 0; i < data.size(); ++i )
		{
			if ( i % 16 == 0 )
				out << "\n\t";
			out << std::setw( 5 ) << (uint32_t)data[i];
			if ( i < data.size()-1 )
				out << ",";
		}
		out << "\n};\n";

		out << "\nconst uint16_t " << name << "_table[" << blocks.size() << "] =\n{";
		for ( size_t i = 0; i < blocks.size(); ++i )
		{
			if ( i % 16 == 0 )
				out << "\n\t";
			out << std::setw( 5 ) << (uint32_t)blocks[i];
			if ( i < blocks.size()-1 )
				out << ",";
		}
		out << "\n};\n";

		out << "\n" << dtype << " get_" << name << "( char32_t cp )\n";
		out << "{\n";
		out << "\tchar32_t block = cp / " << block_size << ";\n";
		out << "\tchar32_t offset = cp % " << block_size << ";\n";
		out << "\treturn " << name << "_data[" << name << "_table[block]+offset];\n";
		out << "}\n";
	}

private:
	std::vector<T> data;
	std::vector<size_t> blocks;
	size_t block_size;
};

////////////////////////////////////////

/// @endcond
