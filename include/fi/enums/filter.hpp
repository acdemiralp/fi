#ifndef FI_FILTER_HPP_
#define FI_FILTER_HPP_

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

namespace fi
{
enum class filter
{
  bicubic     = FILTER_BICUBIC   ,
  bilinear    = FILTER_BILINEAR  ,
  box         = FILTER_BOX       ,
  bspline     = FILTER_BSPLINE   ,
  catmull_rom = FILTER_CATMULLROM,
  lanczos_3   = FILTER_LANCZOS3
};
}

#endif
