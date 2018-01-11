#ifndef FI_FORMAT_INFO_HPP_
#define FI_FORMAT_INFO_HPP_

#include <cstddef>
#include <cstdint>
#include <string>

#include <FreeImage.h>

#include <fi/type.hpp>

namespace fi
{
struct format_info
{
  explicit format_info(const FREE_IMAGE_FORMAT format)
  : native                                             (format)
  , name              (FreeImage_GetFormatFromFIF      (native))
  , description       (FreeImage_GetFIFDescription     (native))
  , mime_type         (FreeImage_GetFIFMimeType        (native))
  , extensions        (FreeImage_GetFIFExtensionList   (native))
  , regular_expression(FreeImage_GetFIFRegExpr         (native))
  , read_support      (FreeImage_FIFSupportsReading    (native) != 0)
  , write_support     (FreeImage_FIFSupportsWriting    (native) != 0)
  , icc_support       (FreeImage_FIFSupportsICCProfiles(native) != 0)
  , no_pixel_support  (FreeImage_FIFSupportsNoPixels   (native) != 0)
  {

  }

  bool check_support(const std::size_t bits_per_pixel) const
  {
    return FreeImage_FIFSupportsExportBPP(native, static_cast<std::int32_t>(bits_per_pixel)) != 0;
  }
  bool check_support(const type        type          ) const
  {
    return FreeImage_FIFSupportsExportType(native, static_cast<FREE_IMAGE_TYPE>(type)) != 0;
  }

  FREE_IMAGE_FORMAT native            ;
  std::string       name              ;
  std::string       description       ;
  std::string       mime_type         ;
  std::string       extensions        ;
  std::string       regular_expression;
  bool              read_support      ;
  bool              write_support     ;
  bool              icc_support       ;
  bool              no_pixel_support  ;
};
}

#endif
