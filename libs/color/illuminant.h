//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

////////////////////////////////////////

namespace color
{

enum class standard_illuminant
{
	A,
	B,
	C,
	D50,
	D55,
	D60,
	D65,
	D75,
	DCI_PROJ,
	E,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
};

/// CIE 1931 two degree observer numbers
namespace two_deg
{

template <typename T>
inline chroma_coord<T> illuminant( standard_illuminant si )
{
	switch ( si )
	{
		case standard_illuminant::A: return chroma_coord<T>( T(0.44757), T(0.40745) );
		case standard_illuminant::B: return chroma_coord<T>( T(0.34842), T(0.35161) );
		case standard_illuminant::C: return chroma_coord<T>( T(0.31006), T(0.31616) );
		case standard_illuminant::D50: return chroma_coord<T>( T(0.34567), T(0.35850) );
		case standard_illuminant::D55: return chroma_coord<T>( T(0.33242), T(0.34743) );
		case standard_illuminant::D60: return chroma_coord<T>( T(0.32168), T(0.33767) );
//		case standard_illuminant::D65: return chroma_coord<T>( T(0.31271), T(0.32902) );
		case standard_illuminant::D65: return chroma_coord<T>( T(0.31270), T(0.32900) );
		case standard_illuminant::D75: return chroma_coord<T>( T(0.29902), T(0.31485) );
		case standard_illuminant::DCI_PROJ: return chroma_coord<T>( T(0.314), T(0.351) );
		case standard_illuminant::E: return chroma_coord<T>( T(1.0/3.0), T(1.0/3.0) );
		case standard_illuminant::F1: return chroma_coord<T>( T(0.31310), T(0.33727) );
		case standard_illuminant::F2: return chroma_coord<T>( T(0.37208), T(0.37529) );
		case standard_illuminant::F3: return chroma_coord<T>( T(0.40910), T(0.39430) );
		case standard_illuminant::F4: return chroma_coord<T>( T(0.44018), T(0.40329) );
		case standard_illuminant::F5: return chroma_coord<T>( T(0.31379), T(0.34531) );
		case standard_illuminant::F6: return chroma_coord<T>( T(0.37790), T(0.38835) );
		case standard_illuminant::F7: return chroma_coord<T>( T(0.31292), T(0.32933) );
		case standard_illuminant::F8: return chroma_coord<T>( T(0.34588), T(0.35875) );
		case standard_illuminant::F9: return chroma_coord<T>( T(0.37417), T(0.37281) );
		case standard_illuminant::F10: return chroma_coord<T>( T(0.34609), T(0.35986) );
		case standard_illuminant::F11: return chroma_coord<T>( T(0.38052), T(0.37713) );
		case standard_illuminant::F12: return chroma_coord<T>( (0.43695), T(0.40441) );
	}
	throw_runtime( "Unhandled standard illuminant" );
}

} // namespace two_deg

/// CIE 1964 10 degree observer
namespace ten_deg
{

template <typename T>
inline chroma_coord<T> illuminant( standard_illuminant si )
{
	switch ( si )
	{
		case standard_illuminant::A: return chroma_coord<T>( T(0.45117), T(0.40594) );
		case standard_illuminant::B: return chroma_coord<T>( T(0.34980), T(0.35270) );
		case standard_illuminant::C: return chroma_coord<T>( T(0.31039), T(0.31905) );
		case standard_illuminant::D50: return chroma_coord<T>( T(0.34773), T(0.35952) );
		case standard_illuminant::D55: return chroma_coord<T>( T(0.33411), T(0.34877) );
		case standard_illuminant::D60: return chroma_coord<T>( T(0.322957407931312), T(0.339135835524579) );
		case standard_illuminant::D65: return chroma_coord<T>( T(0.31382), T(0.33100) );
		case standard_illuminant::D75: return chroma_coord<T>( T(0.29968), T(0.31740) );
			// what do we put here?
//		case standard_illuminant::DCI_PROJ: return chroma_coord<T>( T(0.314), T(0.351) );
		case standard_illuminant::DCI_PROJ: break;
		case standard_illuminant::E: return chroma_coord<T>( T(1.0/3.0), T(1.0/3.0) );
		case standard_illuminant::F1: return chroma_coord<T>( T(0.31811), T(0.33559) );
		case standard_illuminant::F2: return chroma_coord<T>( T(0.37925), T(0.36733) );
		case standard_illuminant::F3: return chroma_coord<T>( T(0.41761), T(0.38324) );
		case standard_illuminant::F4: return chroma_coord<T>( T(0.44920), T(0.39074) );
		case standard_illuminant::F5: return chroma_coord<T>( T(0.31975), T(0.34246) );
		case standard_illuminant::F6: return chroma_coord<T>( T(0.38660), T(0.37847) );
		case standard_illuminant::F7: return chroma_coord<T>( T(0.31569), T(0.32960) );
		case standard_illuminant::F8: return chroma_coord<T>( T(0.34902), T(0.35939) );
		case standard_illuminant::F9: return chroma_coord<T>( T(0.37829), T(0.37045) );
		case standard_illuminant::F10: return chroma_coord<T>( T(0.35090), T(0.35444) );
		case standard_illuminant::F11: return chroma_coord<T>( T(0.38541), T(0.37123) );
		case standard_illuminant::F12: return chroma_coord<T>( (0.44256), T(0.39717) );
	}
	throw_runtime( "Unhandled standard illuminant" );
}

} // namespace ten_deg

} // namespace color



