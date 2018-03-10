#ifndef FI_COLOR_MASK_HPP_
#define FI_COLOR_MASK_HPP_

#include <cstdint>

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

#include <fi/utility/bitset_enum.hpp>

namespace fi
{
enum class color_mask : std::uint32_t
{
  none      = 0                  ,
  red       = FI_RGBA_RED_MASK   ,
  green     = FI_RGBA_GREEN_MASK ,
  blue      = FI_RGBA_BLUE_MASK  ,
  alpha     = FI_RGBA_ALPHA_MASK ,
  rgb       = FI_RGBA_RGB_MASK   ,
  red_555   = FI16_555_RED_MASK  ,
  green_555 = FI16_555_GREEN_MASK,
  blue_555  = FI16_555_BLUE_MASK ,
  red_565   = FI16_565_RED_MASK  ,
  green_565 = FI16_565_GREEN_MASK,
  blue_565  = FI16_565_BLUE_MASK
};
}

template<>
struct is_bitset_enum<fi::color_mask>
{
  static const bool enable = true;
};

#endif
