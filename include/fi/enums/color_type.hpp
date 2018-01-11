#ifndef FI_COLOR_TYPE_HPP_
#define FI_COLOR_TYPE_HPP_

#include <FreeImage.h>

namespace fi
{
enum class color_type
{
  white_is_0 = FIC_MINISWHITE,
  black_is_0 = FIC_MINISBLACK,
  rgb        = FIC_RGB       ,
  palette    = FIC_PALETTE   ,
  rgba       = FIC_RGBALPHA  ,
  cmyk       = FIC_CMYK
};
}

#endif
