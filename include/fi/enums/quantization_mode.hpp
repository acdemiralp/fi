#ifndef FI_QUANTIZATION_MODE_HPP_
#define FI_QUANTIZATION_MODE_HPP_

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

namespace fi
{
enum class quantization_mode
{
  xiaolin_wu           = FIQ_WUQUANT ,
  neural_network       = FIQ_NNQUANT ,
  lossless_fast_pseudo = FIQ_LFPQUANT
};
}

#endif
