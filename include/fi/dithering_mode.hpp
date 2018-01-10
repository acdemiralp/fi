#ifndef FI_DITHERING_MODE_HPP_
#define FI_DITHERING_MODE_HPP_

#include <FreeImage.h>

namespace fi
{
enum class dithering_mode
{
  floyd_steinberg = FID_FS          ,
  bayer_order_2   = FID_BAYER4x4    ,
  bayer_order_3   = FID_BAYER8x8    ,
  bayer_order_4   = FID_BAYER16x16  ,
  cluster_order_3 = FID_CLUSTER6x6  ,
  cluster_order_4 = FID_CLUSTER8x8  ,
  cluster_order_8 = FID_CLUSTER16x16
};
}

#endif
