#ifndef FI_MULTI_IMAGE_HPP_
#define FI_MULTI_IMAGE_HPP_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

#include <fi/image.hpp>
#include <fi/memory.hpp>

namespace fi
{
class multi_image
{
public:
  explicit multi_image  (const std::string& filepath, const bool create = false, const bool read_only = false, const bool cache = false, const std::int32_t native_flags = 0) 
  {
    format_ = FreeImage_GetFileType(filepath.c_str(), 0);
    if (format_ == FIF_UNKNOWN)
      format_ = FreeImage_GetFIFFromFilename(filepath.c_str());

    native_ = FreeImage_OpenMultiBitmap(format_, filepath.c_str(), create, read_only, cache, native_flags);
    if (!native_)
      throw std::runtime_error("FreeImage_OpenMultiBitmap failed.");
  }
  explicit multi_image  (const memory& memory, const std::int32_t native_flags = 0)
  {
    format_ = FreeImage_GetFileTypeFromMemory(memory.native_, 0);

    native_ = FreeImage_LoadMultiBitmapFromMemory(format_, memory.native_, native_flags);
    if (!native_)
      throw std::runtime_error("FreeImage_LoadMultiBitmapFromMemory failed.");
  }
  explicit multi_image  (const multi_image&  that) = delete ;
  explicit multi_image  (      multi_image&& temp) noexcept : native_(std::move(temp.native_)), format_(std::move(temp.format_))
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
      format_      = std::move(temp.format_);
      temp.native_ = nullptr;
    }
    return *this;
  }

  void        to_memory(memory& memory, const std::int32_t native_flags = 0) const
  {
    FreeImage_SaveMultiBitmapToMemory(format_, native_, memory.native_, native_flags);
  }

  std::size_t size     () const
  {
    return static_cast<std::size_t>(FreeImage_GetPageCount(native_));
  }
  void        push_back(const image& image)
  {
    FreeImage_AppendPage(native_, image.native_);
  }
  void        insert   (const std::size_t index, const image& image)
  {
    FreeImage_InsertPage(native_, static_cast<std::int32_t>(index), image.native_);
  }
  void        erase    (const std::size_t index)
  {
    FreeImage_DeletePage(native_, static_cast<std::int32_t>(index));
  }
  void        swap     (const std::size_t source, const std::size_t target)
  {
    FreeImage_MovePage(native_, static_cast<std::int32_t>(target), static_cast<std::int32_t>(source));
  }

  image       lock     (const std::size_t index)
  {
    return image(FreeImage_LockPage(native_, static_cast<std::int32_t>(index)));
  }
  void        unlock   (const image& image, const bool changed = true)
  {
    FreeImage_UnlockPage(native_, image.native_, changed);
  }

protected:
  FIMULTIBITMAP*    native_;
  FREE_IMAGE_FORMAT format_;
};
}

#endif
