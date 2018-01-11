#ifndef FI_FORMAT_INFO_HPP_
#define FI_FORMAT_INFO_HPP_

#include <cstddef>
#include <cstdint>
#include <string>

#include <FreeImage.h>

#include <fi/enums/type.hpp>

namespace fi
{
struct format
{
  explicit format(const FREE_IMAGE_FORMAT format)
  : native                                             (format)
  , name              (FreeImage_GetFormatFromFIF      (native))
  , description       (FreeImage_GetFIFDescription     (native))
  , mime_type         (FreeImage_GetFIFMimeType        (native))
  , extensions        (FreeImage_GetFIFExtensionList   (native))
  , regular_expression(FreeImage_GetFIFRegExpr         (native))
  , supports_read     (FreeImage_FIFSupportsReading    (native) != 0)
  , supports_write    (FreeImage_FIFSupportsWriting    (native) != 0)
  , supports_icc      (FreeImage_FIFSupportsICCProfiles(native) != 0)
  , supports_no_pixels(FreeImage_FIFSupportsNoPixels   (native) != 0)
  {

  }

  bool supports(const std::size_t bits_per_pixel) const
  {
    return FreeImage_FIFSupportsExportBPP(native, static_cast<std::int32_t>(bits_per_pixel)) != 0;
  }
  bool supports(const type        type          ) const
  {
    return FreeImage_FIFSupportsExportType(native, static_cast<FREE_IMAGE_TYPE>(type)) != 0;
  }

  FREE_IMAGE_FORMAT native            ;
  std::string       name              ;
  std::string       description       ;
  std::string       mime_type         ;
  std::string       extensions        ;
  std::string       regular_expression;
  bool              supports_read     ;
  bool              supports_write    ;
  bool              supports_icc      ;
  bool              supports_no_pixels;
};
}

#endif
