#ifndef FI_COLOR_CHANNEL_HPP_
#define FI_COLOR_CHANNEL_HPP_

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

namespace fi
{
enum class color_channel
{
  rgb       = FICC_RGB  ,
  red       = FICC_RED  ,
  green     = FICC_GREEN,
  blue      = FICC_BLUE ,
  alpha     = FICC_ALPHA,
  black     = FICC_BLACK,
  real      = FICC_REAL ,
  imaginary = FICC_IMAG ,
  magnitude = FICC_MAG  ,
  phase     = FICC_PHASE
};
}

#endif
