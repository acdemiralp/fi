#ifndef FI_MEMORY_HPP_
#define FI_MEMORY_HPP_

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

#include <fi/utility/span.hpp>

namespace fi
{
class image;
class multi_image;

class memory
{
public:
  explicit memory  () : native_(FreeImage_OpenMemory())
  {
    if (!native_)
      throw std::runtime_error("FreeImage_OpenMemory failed.");
  }
  template<typename type = std::uint8_t>
  explicit memory  (span<type> span) : native_(FreeImage_OpenMemory(reinterpret_cast<std::uint8_t*>(span.data), static_cast<unsigned long>(sizeof type * span.size)))
  {
    if (!native_)
      throw std::runtime_error("FreeImage_OpenMemory failed.");
  }
  memory           (const memory&  that) = delete;
  memory           (      memory&& temp) noexcept : native_(std::move(temp.native_))
  {
    temp.native_ = nullptr;
  }
  virtual ~memory  ()
  {
    if (native_)
      FreeImage_CloseMemory(native_);
  }
  memory& operator=(const memory&  that) = delete;
  memory& operator=(      memory&& temp) noexcept
  {
    if (this != &temp)
    {
      native_      = std::move(temp.native_);
      temp.native_ = nullptr;
    }
    return *this;
  }
  
  template<typename type = std::uint8_t>
  span<type>        data ()                                                               const
  {
    std::uint8_t* bytes;
    unsigned long size ;
    FreeImage_AcquireMemory(native_, &bytes, &size);
    return {reinterpret_cast<type*>(bytes), static_cast<std::size_t>(size / sizeof(type))};
  }

  template<typename type = std::uint8_t>
  std::vector<type> read (const std::size_t size)                                         const
  {
    std::vector<type> buffer(size);
    FreeImage_ReadMemory(buffer.data(), static_cast<unsigned>(sizeof type), static_cast<unsigned>(buffer.size()), native_);
    return buffer;
  }
  template<typename type = std::uint8_t>
  void              write(span<type> span)
  {
    FreeImage_WriteMemory(span.data, static_cast<unsigned>(sizeof type), static_cast<unsigned>(span.size), native_);
  }
  
  bool              seek (const std::size_t offset, const std::int32_t origin = SEEK_SET) const
  {
    return FreeImage_SeekMemory(native_, static_cast<long>(offset), origin) != 0;
  }
  std::size_t       tell ()                                                               const
  {
    return static_cast<std::size_t>(FreeImage_TellMemory(native_));
  }

protected:
  friend image;
  friend multi_image;

  FIMEMORY* native_;
};
}

#endif
