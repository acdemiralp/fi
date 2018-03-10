#ifndef FI_COLOR_TYPE_HPP_
#define FI_COLOR_TYPE_HPP_

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

namespace fi
{
enum class color_type
{
  white_is_min = FIC_MINISWHITE,
  black_is_min = FIC_MINISBLACK,
  rgb          = FIC_RGB       ,
  rgba         = FIC_RGBALPHA  ,
  cmyk         = FIC_CMYK      ,
  palette      = FIC_PALETTE
};
}

#endif
