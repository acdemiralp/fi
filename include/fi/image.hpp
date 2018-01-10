#ifndef FI_IMAGE_HPP_
#define FI_IMAGE_HPP_

#include <cstdint>

#include <FreeImage.h>
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB

#include <fi/format.hpp>
#include <fi/memory.hpp>

namespace fi
{
class image
{
public:
  image           ()                   = default;
  image           (const image&  that) = default;
  image           (      image&& temp) = default;
  virtual ~image  ()
  {
    if(native_)
      FreeImage_Unload(native_);
  }
  image& operator=(const image&  that) = default;
  image& operator=(      image&& temp) = default;

  bool has_pixels() const
  {
    return FreeImage_HasPixels(native_);
  }
  
  void from_memory(const memory& memory, format format, const std::int32_t native_flags = 0)
  {
    native_ = FreeImage_LoadFromMemory(static_cast<FREE_IMAGE_FORMAT>(format), memory.native_, native_flags);
  }
  void to_memory  (      memory& memory, format format, const std::int32_t native_flags = 0) const
  {
    FreeImage_SaveToMemory(static_cast<FREE_IMAGE_FORMAT>(format), native_, memory.native_, native_flags);
  }

  void convert_to_4_bits()
  {
    const auto native = FreeImage_ConvertTo4Bits(native_);
    FreeImage_Unload(native_);
    native_ = native;
  }

  void set_tone_mapping_fattal02  (const double saturation = 0.5, const double attenuation = 0.85)
  {
    native_ = FreeImage_TmoFattal02  (native_, saturation, attenuation);
  }
  void set_tone_mapping_drago03   (const double gamma      = 2.2, const double exposure    = 0.0 )
  {
    native_ = FreeImage_TmoDrago03   (native_, gamma     , exposure);
  }
  void set_tone_mapping_reinhard05(const double intensity  = 0.0, const double contrast    = 0.0 )
  {
    native_ = FreeImage_TmoReinhard05(native_, intensity , contrast);
  }

protected:
  FIBITMAP* native_;
};
}

#endif
