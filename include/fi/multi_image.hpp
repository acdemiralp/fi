#ifndef FI_MULTI_IMAGE_HPP_
#define FI_MULTI_IMAGE_HPP_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

#include <fi/format.hpp>
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
  explicit multi_image  (const memory&      memory                                                                                     , const std::int32_t native_flags = 0)
  {
    format_ = FreeImage_GetFileTypeFromMemory(memory.native(), 0);

    native_ = FreeImage_LoadMultiBitmapFromMemory(format_, memory.native(), native_flags);
    if (!native_) 
      throw std::runtime_error("FreeImage_LoadMultiBitmapFromMemory failed.");
  }
  explicit multi_image  (const multi_image&  that) = delete;
  explicit multi_image  (      multi_image&& temp) noexcept : native_(std::move(temp.native_)), format_(std::move(temp.format_))
  {
    temp.native_ = nullptr;
  }
  virtual ~multi_image  ()
  {
    if (native_)
      FreeImage_CloseMultiBitmap(native_, 0);
  }
  multi_image& operator=(const multi_image&  that) = delete;
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
  
  std::size_t       size       ()                                                    const
  {
    return static_cast<std::size_t>(FreeImage_GetPageCount(native_));
  }
  void              push_back  (const image& image)
  {
    FreeImage_AppendPage(native_, image.native());
  }
  void              insert     (const std::size_t index , const image& image)
  {
    if (index >= size()) throw std::out_of_range("Index out of range.");
    FreeImage_InsertPage(native_, static_cast<std::int32_t>(index), image.native());
  }
  void              erase      (const std::size_t index )
  {
    if (index >= size()) throw std::out_of_range("Index out of range.");
    FreeImage_DeletePage(native_, static_cast<std::int32_t>(index));
  }
  void              swap       (const std::size_t source, const std::size_t target)  const
  {
    if (source >= size()) throw std::out_of_range("Source index out of range.");
    if (target >= size()) throw std::out_of_range("Target index out of range.");
    FreeImage_MovePage(native_, static_cast<std::int32_t>(target), static_cast<std::int32_t>(source));
  }
                               
  std::vector<bool> lock_state ()                                                    const
  {
    int locked_page_count;
    FreeImage_GetLockedPageNumbers(native_, nullptr, &locked_page_count);

    std::vector<int> locked_pages(static_cast<std::size_t>(locked_page_count));
    FreeImage_GetLockedPageNumbers(native_, locked_pages.data(), &locked_page_count);

    std::vector<bool> pages(size(), false);
    std::for_each(locked_pages.begin(), locked_pages.end(), [&pages] (int page_index) { pages[page_index] = true; });
    return pages;
  }
  image             lock       (const std::size_t index)                             const
  {
    if (index >= size()) throw std::out_of_range ("Index out of range.");
    // Note: This image is not managed (its native representation will exist past destruction).
    return image(FreeImage_LockPage(native_, static_cast<std::int32_t>(index)));
  }
  void              unlock     (const image& image, const bool changed = true)       const
  {
    FreeImage_UnlockPage(native_, image.native(), changed);
  }
        
  FIMULTIBITMAP*    native     ()                                                    const
  {
    return native_;
  }
  format            format     ()                                                    const
  {
    return fi::format(format_);
  }

protected:
  FIMULTIBITMAP*    native_;
  FREE_IMAGE_FORMAT format_;
};

inline memory::memory(const multi_image& multi_image, const std::int32_t native_flags) : memory()
{
  FreeImage_SaveMultiBitmapToMemory(multi_image.format().native, multi_image.native(), native_, native_flags);
}
}

#endif
