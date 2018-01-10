#ifndef FI_IMAGE_HPP_
#define FI_IMAGE_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include <FreeImage.h>
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB

#include <fi/color_type.hpp>
#include <fi/dithering_mode.hpp>
#include <fi/memory.hpp>
#include <fi/quantization_mode.hpp>
#include <fi/span.hpp>
#include <fi/type.hpp>

namespace fi
{
class multi_image;

class image
{
public:
  explicit image  (std::array<std::size_t, 2> dimensions, type type = type::bitmap, const std::size_t bits_per_pixel = 8, const std::array<std::size_t, 3> rgb_mask = {0ull, 0ull, 0ull})
  : native_(FreeImage_AllocateT(
    static_cast<FREE_IMAGE_TYPE>(type), 
    static_cast<std::int32_t> (dimensions     [0]), 
    static_cast<std::int32_t> (dimensions     [1]), 
    static_cast<std::int32_t> (bits_per_pixel    ), 
    static_cast<std::uint32_t>(rgb_mask       [0]),
    static_cast<std::uint32_t>(rgb_mask       [1]), 
    static_cast<std::uint32_t>(rgb_mask       [2])))
  , format_ (FIF_UNKNOWN)
  , managed_(true)
  {
    if (!native_)
      throw std::runtime_error("FreeImage_AllocateT failed.");
  }
  explicit image  (const std::string& filepath, const std::int32_t native_flags = 0) : managed_(true)
  {
    format_ = FreeImage_GetFileType(filepath.c_str(), 0);
    if (format_ == FIF_UNKNOWN)
        format_ = FreeImage_GetFIFFromFilename(filepath.c_str());

    native_ = FreeImage_Load(format_, filepath.c_str(), native_flags);
    if (!native_)
      throw std::runtime_error("FreeImage_Load failed.");
  }
  explicit image  (const memory&      memory  , const std::int32_t native_flags = 0) : managed_(true)
  {
    format_ = FreeImage_GetFileTypeFromMemory(memory.native_, 0);

    native_ = FreeImage_LoadFromMemory(format_, memory.native_, native_flags);
    if (!native_)
      throw std::runtime_error("FreeImage_LoadFromMemory failed.");
  } 
  image           (const image&  that) 
  : native_(FreeImage_Clone(that.native_)), format_(that.format_), managed_(true)
  {
    if (!native_)
      throw std::runtime_error("FreeImage_Clone failed.");
  }
  image           (      image&& temp) noexcept 
  : native_(std::move(temp.native_)), format_(std::move(temp.format_)), managed_(temp.managed_)
  {
    temp.native_ = nullptr;
  }
  virtual ~image  ()
  {
    if (native_ && managed_)
      FreeImage_Unload(native_);
  }
  image& operator=(const image&  that)
  {
    native_  = FreeImage_Clone(that.native_);
    format_  = that.format_;
    managed_ = true;
    if (!native_)
      throw std::runtime_error("FreeImage_Clone failed.");
    return *this;
  }
  image& operator=(      image&& temp) noexcept
  {
    if (this != &temp)
    {
      native_      = std::move(temp.native_ );
      format_      = std::move(temp.format_ );
      managed_     = std::move(temp.managed_);
      temp.native_ = nullptr;
    }
    return *this;
  }

  bool                                     has_pixels                    () const
  {
    return FreeImage_HasPixels(native_) != 0;
  }
                                                                         
  void                                     to_file                       (const std::string& filepath, const std::int32_t native_flags = 0) const
  {
    FreeImage_Save(format_, native_, filepath.c_str(), native_flags);
  }
  void                                     to_memory                     (memory&            memory  , const std::int32_t native_flags = 0) const
  {
    FreeImage_SaveToMemory(format_, native_, memory.native_, native_flags);
  }
                                                                         
  std::string                              format                        () const
  {
    return std::string(FreeImage_GetFormatFromFIF(format_));
  }
                                                                         
  type                                     type                          () const
  {
    return static_cast<fi::type>(FreeImage_GetImageType(native_));
  }
                                                                         
  std::uint8_t*                            pixels                        () const
  {
    return FreeImage_GetBits(native_);
  }
  std::uint8_t*                            scan_line                     (const std::size_t scan_line) const
  {
    return FreeImage_GetScanLine(native_, static_cast<std::int32_t>(scan_line));
  }                                                  
  void                                     set_pixel_palette_index       (const std::array<std::size_t, 2>& position, std::uint8_t index)
  {
    FreeImage_SetPixelIndex(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), &index);
  }
  std::uint8_t                             pixel_palette_index           (const std::array<std::size_t, 2>& position) const
  {
    std::uint8_t index;
    FreeImage_GetPixelIndex(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), &index);
    return index;
  }
  void                                     set_pixel_color               (const std::array<std::size_t, 2>& position, const std::array<std::uint8_t, 4>& color)
  {
    RGBQUAD native_color;
    native_color.rgbRed      = color[0];
    native_color.rgbGreen    = color[1];
    native_color.rgbBlue     = color[2];
    native_color.rgbReserved = color[3];
    FreeImage_SetPixelColor(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), &native_color);
  }
  std::array<std::uint8_t, 4>              pixel_color                   (const std::array<std::size_t, 2>& position) const
  {
    RGBQUAD native_color; 
    FreeImage_GetPixelColor(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), &native_color);
    return {native_color.rgbRed, native_color.rgbGreen, native_color.rgbBlue, native_color.rgbReserved};
  }
                                                                         
  std::size_t                              bits_per_pixel                () const
  {
    return static_cast<std::size_t>(FreeImage_GetBPP(native_));
  }
  std::array<std::size_t , 2>              dimensions                    () const
  {
    return {static_cast<std::size_t>(FreeImage_GetWidth(native_)), static_cast<std::size_t>(FreeImage_GetHeight(native_))};
  }
  std::size_t                              bytes_per_line                () const
  {
    return static_cast<std::size_t>(FreeImage_GetLine(native_));
  }
  std::size_t                              pitch                         () const
  {
    return static_cast<std::size_t>(FreeImage_GetPitch(native_));
  }
  std::size_t                              size                          () const
  {
    return static_cast<std::size_t>(FreeImage_GetMemorySize(native_));
  }
  void                                     set_palette                   (const fi::span<std::array<std::uint8_t, 4>> palette)
  {
    const auto native_palette = FreeImage_GetPalette(native_);
    for(auto i = 0; i < palette.size; ++i)
    {
      native_palette[i].rgbRed      = palette.data[i][0];
      native_palette[i].rgbGreen    = palette.data[i][1];
      native_palette[i].rgbBlue     = palette.data[i][2];
      native_palette[i].rgbReserved = palette.data[i][3];
    }
  }
  std::vector<std::array<std::uint8_t, 4>> palette                       () const
  {
    const auto native_palette = FreeImage_GetPalette(native_);
    std::vector<std::array<std::uint8_t, 4>> palette(palette_size());
    for (auto i = 0; i < palette.size(); ++i)
      palette[i] = {native_palette[i].rgbRed, native_palette[i].rgbGreen, native_palette[i].rgbBlue, native_palette[i].rgbReserved};
    return palette;
  } 
  std::size_t                              palette_size                  () const
  {
    return static_cast<std::size_t>(FreeImage_GetColorsUsed(native_));
  }
  void                                     set_dots_per_meter            (const std::array<std::size_t, 2>& dots_per_meter)
  {
    FreeImage_SetDotsPerMeterX(native_, static_cast<std::uint32_t>(dots_per_meter[0]));
    FreeImage_SetDotsPerMeterY(native_, static_cast<std::uint32_t>(dots_per_meter[1]));
  }
  std::array<std::size_t , 2>              dots_per_meter                () const
  {
    return {static_cast<std::size_t>(FreeImage_GetDotsPerMeterX(native_)), static_cast<std::size_t>(FreeImage_GetDotsPerMeterY(native_))};
  }
  color_type                               color_type                    () const
  {
    return static_cast<fi::color_type>(FreeImage_GetColorType(native_));
  }
  std::array<std::size_t , 3>              color_mask                    () const
  {
    return {
      static_cast<std::size_t>(FreeImage_GetRedMask  (native_)), 
      static_cast<std::size_t>(FreeImage_GetGreenMask(native_)),
      static_cast<std::size_t>(FreeImage_GetBlueMask (native_))};
  }
  void                                     set_transparent               (const bool transparent)
  {
    FreeImage_SetTransparent(native_, transparent);
  }
  bool                                     transparent                   () const
  {
    return FreeImage_IsTransparent(native_) != 0;
  }
  void                                     set_transparency_table        (const fi::span<std::uint8_t> span)
  {
    FreeImage_SetTransparencyTable(native_, span.data, static_cast<std::int32_t>(span.size));
  }
  fi::span<std::uint8_t>                   transparency_table            () const
  {
    return {FreeImage_GetTransparencyTable(native_), static_cast<std::size_t>(FreeImage_GetTransparencyCount(native_))};
  }
  void                                     set_transparent_palette_index (const std::size_t index)
  {
    FreeImage_SetTransparentIndex(native_, static_cast<std::int32_t>(index));
  }
  std::size_t                              transparent_palette_index     () const
  {
    return static_cast<std::size_t>(FreeImage_GetTransparentIndex(native_));
  }
  void                                     set_background_color          (const std::array<std::uint8_t, 4>& color)
  {
    RGBQUAD native_color;
    native_color.rgbRed      = color[0]; 
    native_color.rgbGreen    = color[1]; 
    native_color.rgbBlue     = color[2]; 
    native_color.rgbReserved = color[3];
    FreeImage_SetBackgroundColor(native_, &native_color);
  }
  std::array<std::uint8_t, 4>              background_color              () const
  {
    RGBQUAD native_color;
    FreeImage_GetBackgroundColor(native_, &native_color);
    return {native_color.rgbRed, native_color.rgbGreen, native_color.rgbBlue, native_color.rgbReserved};
  }
  bool                                     has_background_color          () const
  {
    return FreeImage_HasBackgroundColor(native_) != 0;
  }
  void                                     set_thumbnail                 (const std::size_t size, const bool convert = true)
  {
    const auto thumbnail = FreeImage_MakeThumbnail(native_, static_cast<std::int32_t>(size), convert);
    FreeImage_SetThumbnail(native_, thumbnail);
    FreeImage_Unload      (thumbnail);
  }
  void                                     clear_thumbnail               ()
  {
    FreeImage_SetThumbnail(native_, nullptr);
  }
  image                                    thumbnail                     () const
  {
    return image(FreeImage_GetThumbnail(native_));
  }
                                           
  void                                     convert_to_4_bits             ()
  {
    replace(FreeImage_ConvertTo4Bits    (native_));
  }
  void                                     convert_to_8_bits             ()
  {
    replace(FreeImage_ConvertTo8Bits    (native_));
  }
  void                                     convert_to_greyscale          ()
  {
    replace(FreeImage_ConvertToGreyscale(native_));
  }
  void                                     convert_to_16_bits_555        ()
  {
    replace(FreeImage_ConvertTo16Bits555(native_));
  }
  void                                     convert_to_16_bits_565        ()
  {
    replace(FreeImage_ConvertTo16Bits565(native_));
  }
  void                                     convert_to_24_bits            ()
  {
    replace(FreeImage_ConvertTo24Bits   (native_));
  }
  void                                     convert_to_32_bits            ()
  {
    replace(FreeImage_ConvertTo32Bits   (native_));
  }
  
  void                                     convert_to_standard           (               const bool scale_linear = true)
  {
    replace(FreeImage_ConvertToStandardType(native_, scale_linear));
  }
  void                                     convert_to_type               (fi::type type, const bool scale_linear = true)
  {
    replace(FreeImage_ConvertToType(native_, static_cast<FREE_IMAGE_TYPE>(type), scale_linear);
  }

  void                                     color_quantize                (quantization_mode  mode     )
  {
    replace(FreeImage_ColorQuantize(native_, static_cast<FREE_IMAGE_QUANTIZE>(mode)));
  }
  void                                     threshold                     (const std::uint8_t threshold)
  {
    replace(FreeImage_Threshold(native_, threshold));
  }
  void                                     dither                        (dithering_mode     mode     )
  {
    replace(FreeImage_Dither(native_, static_cast<FREE_IMAGE_DITHER>(mode)));
  }
                                           
  void                                     set_tone_mapping_fattal02     (const double saturation = 0.5, const double attenuation = 0.85)
  {
    replace(FreeImage_TmoFattal02  (native_, saturation, attenuation));
  }
  void                                     set_tone_mapping_drago03      (const double gamma      = 2.2, const double exposure    = 0.0 )
  {
    replace(FreeImage_TmoDrago03   (native_, gamma     , exposure));
  }
  void                                     set_tone_mapping_reinhard05   (const double intensity  = 0.0, const double contrast    = 0.0 )
  {
    replace(FreeImage_TmoReinhard05(native_, intensity , contrast));
  }

protected:
  friend multi_image;

  explicit image(FIBITMAP* native) : native_(native), format_(FIF_UNKNOWN), managed_(false)
  {
    if (!native_)
      throw std::runtime_error("Unmanaged FIBITMAP is empty.");
  }
  
  void replace(FIBITMAP* native)
  {
    if (native_)
      FreeImage_Unload(native_);
    native_ = native;
    if (!native_)
      throw std::runtime_error("Failed replace.");
  }

  FIBITMAP*         native_ ;
  FREE_IMAGE_FORMAT format_ ;
  bool              managed_;
};
}

#endif
