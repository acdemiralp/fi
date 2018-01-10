#ifndef FI_MULTI_IMAGE_HPP_
#define FI_MULTI_IMAGE_HPP_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <FreeImage.h>
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB

#include <fi/image.hpp>
#include <fi/memory.hpp>

namespace fi
{
class multi_image
{
public:
  multi_image           (const std::string& filepath, format format, const bool create = false, const bool read_only = false, const bool cache = false, const std::int32_t native_flags = 0) 
  : native_(FreeImage_OpenMultiBitmap(static_cast<FREE_IMAGE_FORMAT>(format), filepath.c_str(), create, read_only, cache, native_flags))
  {
    if (!native_)
      throw std::runtime_error("FreeImage_OpenMultiBitmap failed. Filepath: " + filepath + ". Format: " + std::to_string(static_cast<std::int32_t>(format)));
  }
  multi_image           (const multi_image&  that) = delete ;
  multi_image           (      multi_image&& temp) noexcept : native_(std::move(temp.native_))
  {
    temp.native_ = nullptr;
  }
  virtual ~multi_image  ()
  {
    if (native_)
      FreeImage_CloseMultiBitmap(native_, 0);
  }
  multi_image& operator=(const multi_image&  that) = delete ;
  multi_image& operator=(      multi_image&& temp) noexcept
  {
    if (this != &temp)
    {
      native_      = std::move(temp.native_);
      temp.native_ = nullptr;
    }
    return *this;
  }

  void test()
  {
    //FreeImage_GetPageCount(native_);
    //FreeImage_AppendPage(native_, )
    //  auto bitmap = FreeImage_LockPage(native_, 0);
    //FreeImage_UnlockPage(native_, nullptr, true);
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
