#ifndef FI_IMAGE_HPP_
#define FI_IMAGE_HPP_

#include <FreeImage.h>
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB

#include <fi/filter.hpp>
#include <fi/format.hpp>

namespace fi
{
class image
{
public:
  image           ()                   = default;
  image           (const image&  that) = default;
  image           (      image&& temp) = default;
  virtual ~image  ()                   = default;
  image& operator=(const image&  that) = default;
  image& operator=(      image&& temp) = default;

protected:
  FIBITMAP* native_;
};
}

#endif
