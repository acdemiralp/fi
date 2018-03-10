#ifndef FI_METADATA_MODEL_HPP_
#define FI_METADATA_MODEL_HPP_

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

namespace fi
{
enum class metadata_model
{
  adobe_xmp       = FIMD_XMP           ,
  animation       = FIMD_ANIMATION     ,
  comments        = FIMD_COMMENTS      ,
  custom          = FIMD_CUSTOM        ,
  exif_exif       = FIMD_EXIF_EXIF     ,
  exif_gps        = FIMD_EXIF_GPS      ,
  exif_interop    = FIMD_EXIF_INTEROP  ,
  exif_main       = FIMD_EXIF_MAIN     ,
  exif_maker_note = FIMD_EXIF_MAKERNOTE,
  exif_raw        = FIMD_EXIF_RAW      ,
  geo_tiff        = FIMD_GEOTIFF       ,
  iptc            = FIMD_IPTC          ,
  unknown         = FIMD_NODATA
};
}

#endif
