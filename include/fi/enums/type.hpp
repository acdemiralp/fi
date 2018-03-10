#ifndef FI_TYPE_HPP_
#define FI_TYPE_HPP_

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

namespace fi
{
enum class type
{
  unknown = FIT_UNKNOWN,
  bitmap  = FIT_BITMAP ,
  uint16  = FIT_UINT16 ,
  int16   = FIT_INT16  ,
  uint32  = FIT_UINT32 ,
  int32   = FIT_INT32  ,
  float_  = FIT_FLOAT  ,
  double_ = FIT_DOUBLE ,
  complex = FIT_COMPLEX,
  rgb16   = FIT_RGB16  ,
  rgba16  = FIT_RGBA16 ,
  rgbf    = FIT_RGBF   ,
  rgbaf   = FIT_RGBAF
};
}

#endif
