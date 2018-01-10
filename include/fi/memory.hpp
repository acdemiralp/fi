#ifndef FI_MEMORY_HPP_
#define FI_MEMORY_HPP_

#include <stdexcept>

#include <FreeImage.h>
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB

namespace fi
{
class image;
class multi_image;

class memory
{
public:
  memory           () : native_(FreeImage_OpenMemory())
  {
    if (!native_)
      throw std::runtime_error("Unsuccessful FreeImage_OpenMemory.");
  }
  memory           (const memory&  that) = default;
  memory           (      memory&& temp) = default;
  virtual ~memory  ()
  {
    if (native_)
      FreeImage_CloseMemory(native_);
  }
  memory& operator=(const memory&  that) = default;
  memory& operator=(      memory&& temp) = default;

protected:
  friend image;
  friend multi_image;

  FIMEMORY* native_;
};
}

#endif
