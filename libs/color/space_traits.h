//
// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

////////////////////////////////////////

namespace color
{

namespace detail
{

struct st_generic_XYZ_center
{
	static constexpr bool need_rgb_input = false;
	static constexpr bool might_change_linearity = true;
};

struct st_enables_adaptation : public st_generic_XYZ_center
{
	static constexpr bool supports_adaptation = true;
	static constexpr bool has_middle_function = true;
	static constexpr bool force_middle_function = false;
	static constexpr bool provides_color_opponents = false;
};

struct st_middle_func : public st_generic_XYZ_center
{
	static constexpr bool supports_adaptation = false;
	static constexpr bool has_middle_function = true;
	static constexpr bool force_middle_function = true;
	static constexpr bool provides_color_opponents = false;
};

struct st_opponent_func : public st_generic_XYZ_center
{
	static constexpr bool supports_adaptation = false;
	static constexpr bool has_middle_function = true;
	static constexpr bool force_middle_function = true;
	static constexpr bool provides_color_opponents = true;
};

struct st_ycbcr_func : public st_generic_XYZ_center
{
	static constexpr bool supports_adaptation = false;
	static constexpr bool has_middle_function = false;
	static constexpr bool force_middle_function = false;
	static constexpr bool provides_color_opponents = true;
};

struct st_hue_shift
{
	static constexpr bool need_rgb_input = true;
	static constexpr bool might_change_linearity = false;
	static constexpr bool supports_adaptation = false;
	static constexpr bool has_middle_function = false;
	static constexpr bool force_middle_matrix = false;
	static constexpr bool provides_color_opponents = true;
};

}

template <space> struct space_traits {};

template <>
struct space_traits<space::RGB> : public detail::st_enables_adaptation {};

template <>
struct space_traits<space::XYZ> : public detail::st_enables_adaptation {};

template <>
struct space_traits<space::xyY> : public detail::st_enables_adaptation {};

template <>
struct space_traits<space::LMS_HPE> : public detail::st_middle_func {};

template <>
struct space_traits<space::LMS_CAM02> : public detail::st_middle_func {};

template <>
struct space_traits<space::LMS_ICTCP> : public detail::st_middle_func {};

template <>
struct space_traits<space::CHONG> : public detail::st_opponent_func {};

template <>
struct space_traits<space::CIE_LAB_76> : public detail::st_opponent_func {};

template <>
struct space_traits<space::HUNTER_LAB> : public detail::st_opponent_func {};

template <>
struct space_traits<space::CIE_LUV_76> : public detail::st_opponent_func {};

template <>
struct space_traits<space::CIE_LCH_76> : public detail::st_opponent_func {};

template <>
struct space_traits<space::CIE_UVW_64> : public detail::st_opponent_func {};

template <>
struct space_traits<space::LMS_OPPONENT> : public detail::st_opponent_func {};

template <>
struct space_traits<space::YCBCR_BT601> : public detail::st_ycbcr_func {};

template <>
struct space_traits<space::YCBCR_BT709> : public detail::st_ycbcr_func {};

template <>
struct space_traits<space::YCBCR_BT2020> : public detail::st_ycbcr_func {};

template <>
struct space_traits<space::YCBCR_BT2100> : public detail::st_ycbcr_func {};

template <>
struct space_traits<space::YCBCR_SYCC> : public detail::st_ycbcr_func {};

template <>
struct space_traits<space::YCBCR_CUSTOM> : public detail::st_ycbcr_func {};

template <>
struct space_traits<space::ICTCP> : public detail::st_opponent_func {};

template <>
struct space_traits<space::IPT> : public detail::st_opponent_func {};

template <>
struct space_traits<space::HSV_HEX> : public detail::st_hue_shift {};

template <>
struct space_traits<space::HSV_CYL> : public detail::st_hue_shift {};

template <>
struct space_traits<space::HSI_HEX> : public detail::st_hue_shift {};

template <>
struct space_traits<space::HSI_CYL> : public detail::st_hue_shift {};

template <>
struct space_traits<space::HSL_HEX> : public detail::st_hue_shift {};

template <>
struct space_traits<space::HSL_CYL> : public detail::st_hue_shift {};

inline constexpr bool has_opponency( const space s )
{
#if __cplusplus >= 201402L
	switch ( s )
	{
		case space::RGB: return space_traits<space::RGB>::provides_color_opponents;
		case space::XYZ: return space_traits<space::XYZ>::provides_color_opponents;
		case space::xyY: return space_traits<space::xyY>::provides_color_opponents;
		case space::LMS_HPE: return space_traits<space::LMS_HPE>::provides_color_opponents;
		case space::LMS_CAM02: return space_traits<space::LMS_CAM02>::provides_color_opponents;
		case space::LMS_ICTCP: return space_traits<space::LMS_ICTCP>::provides_color_opponents;
		case space::CHONG: return space_traits<space::CHONG>::provides_color_opponents;
		case space::CIE_LAB_76: return space_traits<space::CIE_LAB_76>::provides_color_opponents;
		case space::HUNTER_LAB: return space_traits<space::HUNTER_LAB>::provides_color_opponents;
		case space::CIE_LUV_76: return space_traits<space::CIE_LUV_76>::provides_color_opponents;
		case space::CIE_LCH_76: return space_traits<space::CIE_LCH_76>::provides_color_opponents;
		case space::CIE_UVW_64: return space_traits<space::CIE_UVW_64>::provides_color_opponents;
		case space::LMS_OPPONENT: return space_traits<space::LMS_OPPONENT>::provides_color_opponents;
		case space::YCBCR_BT601: return space_traits<space::YCBCR_BT601>::provides_color_opponents;
		case space::YCBCR_BT709: return space_traits<space::YCBCR_BT709>::provides_color_opponents;
		case space::YCBCR_BT2020: return space_traits<space::YCBCR_BT2020>::provides_color_opponents;
		case space::YCBCR_BT2100: return space_traits<space::YCBCR_BT2100>::provides_color_opponents;
		case space::YCBCR_SYCC: return space_traits<space::YCBCR_SYCC>::provides_color_opponents;
		case space::YCBCR_CUSTOM: return space_traits<space::YCBCR_CUSTOM>::provides_color_opponents;
		case space::ICTCP: return space_traits<space::ICTCP>::provides_color_opponents;
		case space::IPT: return space_traits<space::IPT>::provides_color_opponents;
		case space::HSV_HEX: return space_traits<space::HSV_HEX>::provides_color_opponents;
		case space::HSV_CYL: return space_traits<space::HSV_CYL>::provides_color_opponents;
		case space::HSI_HEX: return space_traits<space::HSI_HEX>::provides_color_opponents;
		case space::HSI_CYL: return space_traits<space::HSI_CYL>::provides_color_opponents;
		case space::HSL_HEX: return space_traits<space::HSL_HEX>::provides_color_opponents;
		case space::HSL_CYL: return space_traits<space::HSL_CYL>::provides_color_opponents;

		case space::UNKNOWN:
		//default:
			break;
	}
	return false;
#else
	return (s == space::RGB) ? space_traits<space::RGB>::provides_color_opponents :
		(s == space::XYZ) ? space_traits<space::XYZ>::provides_color_opponents :
		(s == space::xyY) ? space_traits<space::xyY>::provides_color_opponents :
		(s == space::LMS_HPE) ? space_traits<space::LMS_HPE>::provides_color_opponents :
		(s == space::LMS_CAM02) ? space_traits<space::LMS_CAM02>::provides_color_opponents :
		(s == space::CHONG) ? space_traits<space::CHONG>::provides_color_opponents :
		(s == space::CIE_LAB_76) ? space_traits<space::CIE_LAB_76>::provides_color_opponents :
		(s == space::HUNTER_LAB) ? space_traits<space::HUNTER_LAB>::provides_color_opponents :
		(s == space::CIE_LUV_76) ? space_traits<space::CIE_LUV_76>::provides_color_opponents :
		(s == space::CIE_LCH_76) ? space_traits<space::CIE_LCH_76>::provides_color_opponents :
		(s == space::CIE_UVW_64) ? space_traits<space::CIE_UVW_64>::provides_color_opponents :
		(s == space::LMS_OPPONENT) ? space_traits<space::LMS_OPPONENT>::provides_color_opponents :
		(s == space::YCBCR_BT601) ? space_traits<space::YCBCR_BT601>::provides_color_opponents :
		(s == space::YCBCR_BT709) ? space_traits<space::YCBCR_BT709>::provides_color_opponents :
		(s == space::YCBCR_BT2020) ? space_traits<space::YCBCR_BT2020>::provides_color_opponents :
		(s == space::YCBCR_BT2100) ? space_traits<space::YCBCR_BT2100>::provides_color_opponents :
		(s == space::YCBCR_SYCC) ? space_traits<space::YCBCR_SYCC>::provides_color_opponents :
		(s == space::YCBCR_CUSTOM) ? space_traits<space::YCBCR_CUSTOM>::provides_color_opponents :
		(s == space::ICTCP) ? space_traits<space::ICTCP>::provides_color_opponents :
		(s == space::IPT) ? space_traits<space::IPT>::provides_color_opponents :
		(s == space::HSV_HEX) ? space_traits<space::HSV_HEX>::provides_color_opponents :
		(s == space::HSV_CYL) ? space_traits<space::HSV_CYL>::provides_color_opponents :
		(s == space::HSI_HEX) ? space_traits<space::HSI_HEX>::provides_color_opponents :
		(s == space::HSI_CYL) ? space_traits<space::HSI_CYL>::provides_color_opponents :
		(s == space::HSL_HEX) ? space_traits<space::HSL_HEX>::provides_color_opponents :
		(s == space::HSL_CYL) ? space_traits<space::HSL_CYL>::provides_color_opponents :
		false;
#endif
}

} // namespace color
