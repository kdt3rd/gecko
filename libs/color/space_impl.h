//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

////////////////////////////////////////

namespace color
{

template <typename T>
inline void remove_opponency( T &a, T &b, T &c, const T ina, const T inb, const T inc, space s )
{
	switch ( s )
	{
		case space::RGB:
		case space::XYZ: a = ina; b = inb; c = inc; break;
		case space::YCBCR_BT601: break;
		case space::YCBCR_BT709: break;
		case space::YCBCR_BT2020: break;
		case space::YCBCR_BT2100: break;
		case space::YCBCR_SYCC: break;
		case space::YCBCR_CUSTOM: break;
		case space::HSV_HEX: break;
		case space::HSV_CYL: break;
		case space::HSI_HEX: break;
		case space::HSI_CYL: break;
		case space::HSL_HEX: break;
		case space::HSL_CYL: break;
		case space::ICTCP: break;
		case space::IPT: break;
		case space::xyY: break;
		case space::LMS_HPE: break;
		case space::LMS_CAM02: break;
		case space::LMS_ICTCP: break;
		case space::LMS_OPPONENT: break;
		case space::CHONG: break;
		case space::HUNTER_LAB: break;
		case space::CIE_LAB_76:
            b = ( ina + T(16) ) / T(116);
            a = ( inb / T(500) ) + b;
            a = b - ( inc / T(200) );
            break;
		case space::CIE_LUV_76: break;
		case space::CIE_LCH_76: break;
		case space::CIE_UVW_64: break;
		case space::UNKNOWN:
            a = ina; b = inb; c = inc;
            break;
	}
}

template <typename T>
inline void add_opponency( T &a, T &b, T &c, const T ina, const T inb, const T inc, space s )
{
	switch ( s )
	{
		case space::RGB:
		case space::XYZ: a = ina; b = inb; c = inc; break;
		case space::YCBCR_BT601: break;
		case space::YCBCR_BT709: break;
		case space::YCBCR_BT2020: break;
		case space::YCBCR_BT2100: break;
		case space::YCBCR_SYCC: break;
		case space::YCBCR_CUSTOM: break;
		case space::HSV_HEX: break;
		case space::HSV_CYL: break;
		case space::HSI_HEX: break;
		case space::HSI_CYL: break;
		case space::HSL_HEX: break;
		case space::HSL_CYL: break;
		case space::ICTCP: break;
		case space::IPT: break;
		case space::xyY: break;
		case space::LMS_HPE: break;
		case space::LMS_CAM02: break;
		case space::LMS_ICTCP: break;
		case space::LMS_OPPONENT: break;
		case space::CHONG: break;
		case space::HUNTER_LAB: break;
		case space::CIE_LAB_76:
            a = T(116) * inb - T(16);
            b = T(500)*(ina - inb);
            c = T(200)*(inb - inc);
            break;
		case space::CIE_LUV_76: break;
		case space::CIE_LCH_76: break;
		case space::CIE_UVW_64: break;
		case space::UNKNOWN:
            a = ina; b = inb; c = inc;
            break;
	}
}

} // namespace color



