#ifndef FI_MULTI_IMAGE_HPP_
#define FI_MULTI_IMAGE_HPP_

#include <cstdint>
#include <stdexcept>
#include <string>

#include <FreeImage.h>
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB

#include <fi/format.hpp>
#include <fi/memory.hpp>

namespace fi
{
class multi_image
{
public:
  multi_image           (const std::string& filepath, format format) : native_(FreeImage_OpenMultiBitmap(static_cast<FREE_IMAGE_FORMAT>(format), filepath.c_str(), true, true, false, 0))
  {
    if (!native_)
      throw std::runtime_error("Unsuccessful FreeImage_OpenMultiBitmap. Filepath: " + filepath + ". Format: " + std::to_string(static_cast<std::int32_t>(format)));
  }
  multi_image           (const multi_image&  that) = default;
  multi_image           (      multi_image&& temp) = default;
  virtual ~multi_image  ()
  {
    if (native_)
      FreeImage_CloseMultiBitmap(native_);
  }
  multi_image& operator=(const multi_image&  that) = default;
  multi_image& operator=(      multi_image&& temp) = default;
  
  void from_handle()
  {
    FreeImageIO io;
    fi_handle handle;
    native_ = FreeImage_OpenMultiBitmapFromHandle()
  }

  void from_memory(const memory& memory, format format, const std::int32_t native_flags = 0)
  {
    native_ = FreeImage_LoadMultiBitmapFromMemory(static_cast<FREE_IMAGE_FORMAT>(format), memory.native_, native_flags);
  }
  void to_memory  (      memory& memory, format format, const std::int32_t native_flags = 0) const
  {
    FreeImage_SaveMultiBitmapToMemory(static_cast<FREE_IMAGE_FORMAT>(format), native_, memory.native_, native_flags);
  }

protected:
  FIMULTIBITMAP* native_;
};
}

#endif
