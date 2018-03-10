#ifndef FI_QUANTIZATION_MODE_HPP_
#define FI_QUANTIZATION_MODE_HPP_

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

namespace fi
{
enum class quantization_mode
{
  lossless_fast_pseudo = FIQ_LFPQUANT,
  neural_network       = FIQ_NNQUANT ,
  xiaolin_wu           = FIQ_WUQUANT
};
}

#endif
