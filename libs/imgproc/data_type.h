
#pragma once

namespace imgproc
{

////////////////////////////////////////

enum class data_type
{
	UINT8,
	UINT16,
	UINT32,
	FLOAT32,
	FLOAT64
};

////////////////////////////////////////

typedef std::pair<data_type,size_t> type;

////////////////////////////////////////

inline std::string type_name( data_type t )
{
	switch ( t )
	{
		case data_type::UINT8: return "uint8_t";
		case data_type::UINT16: return "uint16_t";
		case data_type::UINT32: return "uint32_t";
		case data_type::FLOAT32: return "float";
		case data_type::FLOAT64: return "double";
	}
}

////////////////////////////////////////

}

