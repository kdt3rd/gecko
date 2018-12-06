//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once
#include <base/contract.h>
#include "chromaticities.h"
#include "illuminant.h"

////////////////////////////////////////

namespace color
{

namespace two_deg
{

template <typename T>
inline constexpr chromaticities<T> ACES_AP0( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.73470), T(0.26530) ),
		chroma_coord<T>( T(0.00000), T(1.00000) ),
		chroma_coord<T>( T(0.00010), T(-0.07700) ),
		chroma_coord<T>( T(0.32168), T(0.33767) )
	);
}

template <typename T>
inline constexpr chromaticities<T> ACES_AP1( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.713), T(0.293) ),
		chroma_coord<T>( T(0.165), T(0.830) ),
		chroma_coord<T>( T(0.128), T(0.044) ),
		chroma_coord<T>( T(0.32168), T(0.33767) )
	);
}

template <typename T>
inline constexpr chromaticities<T> Rec_601_NTSC( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.630), T(0.340) ),
		chroma_coord<T>( T(0.310), T(0.595) ),
		chroma_coord<T>( T(0.155), T(0.070) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

template <typename T>
inline constexpr chromaticities<T> Rec_601_PAL( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.640), T(0.330) ),
		chroma_coord<T>( T(0.290), T(0.600) ),
		chroma_coord<T>( T(0.150), T(0.060) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

template <typename T>
inline constexpr chromaticities<T> Rec_709( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.64000), T(0.33000) ),
		chroma_coord<T>( T(0.30000), T(0.60000) ),
		chroma_coord<T>( T(0.15000), T(0.06000) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

template <typename T>
inline constexpr chromaticities<T> Rec_2020( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.70800), T(0.29200) ),
		chroma_coord<T>( T(0.17000), T(0.79700) ),
		chroma_coord<T>( T(0.13100), T(0.04600) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

template <typename T>
inline constexpr chromaticities<T> P3_D60( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.6800), T(0.32000) ),
		chroma_coord<T>( T(0.26500), T(0.69000) ),
		chroma_coord<T>( T(0.15000), T(0.06000) ),
		chroma_coord<T>( T(0.32168), T(0.33767) )
	);
}

template <typename T>
inline constexpr chromaticities<T> P3_D65( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.6800), T(0.32000) ),
		chroma_coord<T>( T(0.26500), T(0.69000) ),
		chroma_coord<T>( T(0.15000), T(0.06000) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

template <typename T>
inline constexpr chromaticities<T> P3_DCI( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.6800), T(0.32000) ),
		chroma_coord<T>( T(0.26500), T(0.69000) ),
		chroma_coord<T>( T(0.15000), T(0.06000) ),
		chroma_coord<T>( T(0.314), T(0.351) )
	);
}

template <typename T>
inline constexpr chromaticities<T> ARRI_Alexa_WCG( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.68400), T(0.31300) ),
		chroma_coord<T>( T(0.22100), T(0.84800) ),
		chroma_coord<T>( T(0.08610), T(-0.10200) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

template <typename T>
inline constexpr chromaticities<T> SONY_SGamut( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.7300), T(0.2800) ),
		chroma_coord<T>( T(0.1400), T(0.8550) ),
		chroma_coord<T>( T(0.1000), T(-0.050) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

template <typename T>
inline constexpr chromaticities<T> SONY_SGamut3Cine( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.76600), T(0.27500) ),
		chroma_coord<T>( T(0.22500), T(0.80000) ),
		chroma_coord<T>( T(0.08900), T(-.08700) ),
		chroma_coord<T>( T(0.3127), T(0.3290) )
	);
}

/// ProPhoto RGB
template <typename T>
inline constexpr chromaticities<T> ROMM( void ) 
{
	return chromaticities<T>( 
		chroma_coord<T>( T(0.7347), T(0.2653) ),
		chroma_coord<T>( T(0.1596), T(0.8404) ),
		chroma_coord<T>( T(0.0366), T(0.0001) ),
		chroma_coord<T>( T(0.3457), T(0.3585) )
	);
}

} // namespace two_deg

namespace ten_deg
{

} // namespace ten_deg

} // namespace color



