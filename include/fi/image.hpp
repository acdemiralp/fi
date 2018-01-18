#ifndef FI_IMAGE_HPP_
#define FI_IMAGE_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

#include <fi/enums/color_channel.hpp>
#include <fi/enums/color_mask.hpp>
#include <fi/enums/color_type.hpp>
#include <fi/enums/dithering_mode.hpp>
#include <fi/enums/filter.hpp>
#include <fi/enums/quantization_mode.hpp>
#include <fi/enums/metadata_model.hpp>
#include <fi/enums/type.hpp>
#include <fi/utility/rectangle.hpp>
#include <fi/utility/span.hpp>
#include <fi/format.hpp>
#include <fi/memory.hpp>
#include <fi/metadata.hpp>

namespace fi
{
class image
{
public: 
  explicit image  (                 
    const std::array<std::size_t, 2>& dimensions     ,
    type                              type           = type::bitmap, 
    const std::size_t                 bits_per_pixel = 8, 
    const std::array<color_mask, 3>   color_mask     = {color_mask::none, color_mask::none, color_mask::none})
  : native_(FreeImage_AllocateT(
    static_cast<FREE_IMAGE_TYPE>(type),
    static_cast<std::int32_t> (dimensions[0]),
    static_cast<std::int32_t> (dimensions[1]),
    static_cast<std::int32_t> (bits_per_pixel),
    static_cast<std::uint32_t>(color_mask[0]),
    static_cast<std::uint32_t>(color_mask[1]),
    static_cast<std::uint32_t>(color_mask[2])))
  {
    if (!native_)
      throw std::runtime_error("FreeImage_AllocateT failed.");
  }
  template<typename color_type = std::array<std::uint8_t, 4>>
  explicit image  (                 
    const color_type&                 color          , 
    const std::array<std::size_t, 2>& dimensions     , 
    type                              type           = type::bitmap, 
    const std::size_t                 bits_per_pixel = 8, 
    const std::array<color_mask, 3>   color_mask     = {color_mask::none, color_mask::none, color_mask::none})
  : native_(FreeImage_AllocateExT(
    static_cast<FREE_IMAGE_TYPE>(type),
    static_cast<std::int32_t>   (dimensions[0]),
    static_cast<std::int32_t>   (dimensions[1]),
    static_cast<std::int32_t>   (bits_per_pixel),
    &color[0],
    0,
    nullptr,
    static_cast<std::uint32_t>  (color_mask[0]),
    static_cast<std::uint32_t>  (color_mask[1]),
    static_cast<std::uint32_t>  (color_mask[2])
    ))
  {
    if (!native_)
      throw std::runtime_error("FreeImage_AllocateExT failed.");
  }
  template<typename data_type = std::uint8_t>
  explicit image  (
    data_type*                        data           , 
    const std::array<std::size_t, 2>& dimensions     , 
    type                              type           = type::bitmap, 
    const std::size_t                 bits_per_pixel = 8, 
    const std::array<color_mask, 3>   color_mask     = {color_mask::none, color_mask::none, color_mask::none}, 
    const bool                        shallow        = false, 
    const bool                        top_down       = false)
  : native_(FreeImage_ConvertFromRawBitsEx(
    !shallow,
    reinterpret_cast<std::uint8_t*>(data),
    static_cast<FREE_IMAGE_TYPE>   (type),
    static_cast<std::int32_t>      (dimensions[0]),
    static_cast<std::int32_t>      (dimensions[1]),
    static_cast<std::int32_t>      ((bits_per_pixel * dimensions[0] + 31) / 32 * 4),
    static_cast<std::int32_t>      (bits_per_pixel),
    static_cast<std::uint32_t>     (color_mask[0]),
    static_cast<std::uint32_t>     (color_mask[1]),
    static_cast<std::uint32_t>     (color_mask[2]),
    top_down))
  , managed_(!shallow)
  {
    if (!native_)
      throw std::runtime_error("FreeImage_ConvertFromRawBitsEx failed.");
  }

  explicit image  (const std::string& filepath, const std::int32_t native_flags = 0)
  {
    format_ = FreeImage_GetFileType(filepath.c_str(), 0);
    if (format_ == FIF_UNKNOWN)
        format_ = FreeImage_GetFIFFromFilename(filepath.c_str());

    native_ = FreeImage_Load(format_, filepath.c_str(), native_flags);
    if (!native_)
      throw std::runtime_error("FreeImage_Load failed.");
  } 
  explicit image  (const memory&      memory  , const std::int32_t native_flags = 0)
  {
    format_ = FreeImage_GetFileTypeFromMemory(memory.native(), 0);

    native_ = FreeImage_LoadFromMemory(format_, memory.native(), native_flags);
    if (!native_)
      throw std::runtime_error("FreeImage_LoadFromMemory failed.");
  } 
             
  explicit image  (const image& that, const rectangle<std::size_t>& rectangle, const bool shallow = false) 
  : format_(that.format_)
  {
    if (shallow)
    {
      native_ = FreeImage_CreateView(that.native_, static_cast<std::int32_t>(rectangle.left), static_cast<std::int32_t>(rectangle.top), static_cast<std::int32_t>(rectangle.right), static_cast<std::int32_t>(rectangle.bottom));
      if (!native_)
        throw std::runtime_error("FreeImage_CreateView failed.");
    }
    else
    {
      native_ = FreeImage_Copy(that.native_, static_cast<std::int32_t>(rectangle.left), static_cast<std::int32_t>(rectangle.top), static_cast<std::int32_t>(rectangle.right), static_cast<std::int32_t>(rectangle.bottom));
      if (!native_)
        throw std::runtime_error("FreeImage_Copy failed.");
    }
    FreeImage_CloneMetadata(native_, that.native_);
  }
  explicit image  (const image& that, const color_channel channel)
  : native_ (that.type() == type::complex ? FreeImage_GetComplexChannel(that.native_, static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel)) : FreeImage_GetChannel(that.native_, static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel)))
  , format_ (that.format_)
  {
    if (!native_)
      throw std::runtime_error(that.type() == type::complex ? "FreeImage_GetComplexChannel failed." : "FreeImage_GetChannel failed.");
    FreeImage_CloneMetadata(native_, that.native_);
  }

  explicit image  (FIBITMAP* native) : native_(native), managed_(false)
  {
    if (!native_)
      throw std::runtime_error("FIBITMAP is nullptr.");
  }
  
  image           (const image&  that) : native_(FreeImage_Clone(that.native_)), format_(that.format_)
  {
    if (!native_)
      throw std::runtime_error("FreeImage_Clone failed.");

    FreeImage_CloneMetadata(native_, that.native_);
  }
  image           (      image&& temp) noexcept : native_(std::move(temp.native_)), format_(std::move(temp.format_)), managed_(std::move(temp.managed_))
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

    FreeImage_CloneMetadata(native_, that.native_);

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
  
  // Save functionality.
  void                                     save                          (const std::string& filepath, const std::int32_t native_flags = 0)                                                                        const
  {
    FreeImage_Save(FreeImage_GetFIFFromFilename(filepath.c_str()), native_, filepath.c_str(), native_flags);
  }
  
  // Property access / mutate functionality.
  std::array<std::size_t, 2>               dimensions                    ()                                                                                                                                        const
  {
    return {static_cast<std::size_t>(FreeImage_GetWidth(native_)), static_cast<std::size_t>(FreeImage_GetHeight(native_))};
  }
  type                                     type                          ()                                                                                                                                        const
  {
    return static_cast<fi::type>(FreeImage_GetImageType(native_));
  }
  std::size_t                              bits_per_pixel                ()                                                                                                                                        const
  {
    return static_cast<std::size_t>(FreeImage_GetBPP(native_));
  }
  std::array<color_mask, 3>                color_mask                    ()                                                                                                                                        const
  {
    return 
    {
      static_cast<fi::color_mask>(FreeImage_GetRedMask  (native_)), 
      static_cast<fi::color_mask>(FreeImage_GetGreenMask(native_)),
      static_cast<fi::color_mask>(FreeImage_GetBlueMask (native_))
    };
  }
  std::size_t                              size                          ()                                                                                                                                        const
  {
    return static_cast<std::size_t>(FreeImage_GetMemorySize(native_));
  }
  std::size_t                              line_size                     ()                                                                                                                                        const
  {
    return static_cast<std::size_t>(FreeImage_GetLine(native_));
  }
  std::size_t                              bitmap_size                   ()                                                                                                                                        const
  {
    return static_cast<std::size_t>(FreeImage_GetDIBSize(native_));
  }
  std::size_t                              pitch                         ()                                                                                                                                        const
  {
    return static_cast<std::size_t>(FreeImage_GetPitch(native_));
  }
  color_type                               color_type                    ()                                                                                                                                        const
  {
    return static_cast<fi::color_type>(FreeImage_GetColorType(native_));
  }
  
  void                                     set_dots_per_meter            (const std::array<std::size_t, 2>& dots_per_meter)
  {
    FreeImage_SetDotsPerMeterX(native_, static_cast<std::uint32_t>(dots_per_meter[0]));
    FreeImage_SetDotsPerMeterY(native_, static_cast<std::uint32_t>(dots_per_meter[1]));
  }
  std::array<std::size_t, 2>               dots_per_meter                ()                                                                                                                                        const
  {
    return {static_cast<std::size_t>(FreeImage_GetDotsPerMeterX(native_)), static_cast<std::size_t>(FreeImage_GetDotsPerMeterY(native_))};
  }

  // Pixel access functionality.
  bool                                     empty                         ()                                                                                                                                        const
  {
    return FreeImage_HasPixels(native_) == 0;
  }
  template<typename type = std::uint8_t>                                                                                                                                                                      
  span<type>                               pixels                        ()                                                                                                                                        const
  {
    return {reinterpret_cast<type*>(FreeImage_GetBits(native_)), dimensions()[1] * pitch() / sizeof type};
  }
  template<typename type = std::uint8_t>                                                                                                                                                                      
  span<type>                               line                          (const std::size_t index)                                                                                                                 const
  {
    return {reinterpret_cast<type*>(FreeImage_GetScanLine(native_, static_cast<std::int32_t>(index))), pitch() / sizeof type};
  }
  template<typename type = std::uint8_t>                                                                                                                                                                      
  std::vector<type>                        to_vector                     (const bool bottom_up = true)                                                                                                             const 
  {
    std::vector<type> buffer(dimensions()[1] * pitch() / sizeof type);
    auto mask = color_mask();
    FreeImage_ConvertToRawBits(
      reinterpret_cast<std::uint8_t*>(buffer.data()),
      native_,
      static_cast<std::int32_t> (pitch()),
      static_cast<std::uint32_t>(bits_per_pixel()),
      static_cast<std::uint32_t>(mask[0]),
      static_cast<std::uint32_t>(mask[1]),
      static_cast<std::uint32_t>(mask[2]),
      !bottom_up);
    return buffer;
  }
  
  void                                     set_pixel_color               (const std::array<std::size_t, 2>& position, const std::array<std::uint8_t, 4>& color) // Underlying call enforces RGBQUAD use.
  {
    FreeImage_SetPixelColor(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), reinterpret_cast<RGBQUAD*>(const_cast<std::uint8_t*>(color.data())));
  }
  std::array<std::uint8_t, 4>              pixel_color                   (const std::array<std::size_t, 2>& position)                                                                                              const // Underlying call enforces RGBQUAD use.
  {
    std::array<std::uint8_t, 4> color;
    FreeImage_GetPixelColor(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), reinterpret_cast<RGBQUAD*>(color.data()));
    return color;
  }                                                                                                                                                                                                                  
  void                                     set_pixel_palette_index       (const std::array<std::size_t, 2>& position, std::uint8_t index)
  {
    FreeImage_SetPixelIndex(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), &index);
  }                                                                 
  std::uint8_t                             pixel_palette_index           (const std::array<std::size_t, 2>& position)                                                                                              const
  {
    std::uint8_t index;
    FreeImage_GetPixelIndex(native_, static_cast<std::uint32_t>(position[0]), static_cast<std::uint32_t>(position[1]), &index);
    return       index;
  }    

  // Color / transparency functionality.
  span<std::array<std::uint8_t, 4>>        palette                       () // Underlying call enforces RGBQUAD use.
  {
    return {reinterpret_cast<std::array<std::uint8_t, 4>*>(FreeImage_GetPalette(native_)), static_cast<std::size_t>(FreeImage_GetColorsUsed(native_))};
  }
  
  void                                     set_transparent               (const bool transparent)
  {
    FreeImage_SetTransparent(native_, transparent);
  }
  bool                                     transparent                   ()                                                                                                                                        const
  {
    return FreeImage_IsTransparent(native_) != 0;
  }
  void                                     set_transparent_palette_index (const std::uint8_t index)
  {
    FreeImage_SetTransparentIndex(native_, static_cast<std::int32_t>(index));
  }                                                                                                       
  std::uint8_t                             transparent_palette_index     ()                                                                                                                                        const
  {
    return static_cast<std::uint8_t>(FreeImage_GetTransparentIndex(native_));
  }
  void                                     set_transparency_table        (const span<std::uint8_t>& table)
  {
    FreeImage_SetTransparencyTable(native_, table.data, static_cast<std::int32_t>(table.size));
  }                                                                                                
  span<std::uint8_t>                       transparency_table            ()                                                                                                                                        const
  {
    return {FreeImage_GetTransparencyTable(native_), static_cast<std::size_t>(FreeImage_GetTransparencyCount(native_))};
  }
  
  void                                     set_background_color          (const std::array<std::uint8_t, 4>& color) // Underlying call enforces RGBQUAD use.
  {
    FreeImage_SetBackgroundColor(native_, reinterpret_cast<RGBQUAD*>(const_cast<std::uint8_t*>(color.data())));
  }                                                                          
  std::array<std::uint8_t, 4>              background_color              ()                                                                                                                                        const // Underlying call enforces RGBQUAD use.
  {
    std::array<std::uint8_t, 4> color;
    FreeImage_GetBackgroundColor(native_, reinterpret_cast<RGBQUAD*>(color.data()));
    return color;
  }
  void                                     reset_background_color        ()
  {
    FreeImage_SetBackgroundColor(native_, nullptr);
  }                                                     
  bool                                     has_background_color          ()                                                                                                                                        const
  {
    return FreeImage_HasBackgroundColor(native_) != 0;
  }
  
  // Adjustment functionality.    
  void                                     adjust                        (                       const double brightness, const double contrast, const double gamma, const bool invert = false)                    const
  {
    FreeImage_AdjustColors(native_, brightness, contrast, gamma, invert);
  }
  void                                     adjust_channel                (color_channel channel, const double brightness, const double contrast, const double gamma, const bool invert = false)                    const
  {
    std::array<std::uint8_t, 256> lut;
    FreeImage_GetAdjustColorsLookupTable(lut.data(), brightness, contrast, gamma, invert);
    FreeImage_AdjustCurve               (native_, lut.data(), static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel));
  }
  void                                     adjust_brightness             (const double percentage)                                                                                                                 const
  {
    FreeImage_AdjustBrightness(native_, percentage);
  }
  void                                     adjust_contrast               (const double percentage)                                                                                                                 const
  {
    FreeImage_AdjustContrast  (native_, percentage);
  }
  void                                     adjust_gamma                  (const double gamma     )                                                                                                                 const
  {
    FreeImage_AdjustGamma     (native_, gamma);
  }
  void                                     invert                        ()                                                                                                                                        const
  {
    FreeImage_Invert(native_);
  }

  // Post processing functionality.
  std::array<unsigned long, 256>           histogram                     (color_channel channel)                                                                                                                   const
  {
    std::array<unsigned long, 256> histogram_native;
    FreeImage_GetHistogram(native_, histogram_native.data(), static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel));
    return histogram_native;
  }
  void                                     flip_horizontal               ()                                                                                                                                        const
  {
    FreeImage_FlipHorizontal(native_);
  }
  void                                     flip_vertical                 ()                                                                                                                                        const
  {
    FreeImage_FlipVertical(native_);
  }
  void                                     pre_multiply_alpha            ()                                                                                                                                        const
  {
    FreeImage_PreMultiplyWithAlpha(native_);
  }
  template<typename color_type = std::array<std::uint8_t, 4>>
  void                                     fill                          (const color_type& color, const std::int32_t native_options = 0)
  {
    FreeImage_FillBackground(native_, &color, native_options);
  }
  template<typename color_type = std::array<std::uint8_t, 4>>
  void                                     apply_color_mapping           (const span<color_type>&   source, const span<color_type>&   target, const bool ignore_alpha = false, const bool two_way = false) // Underlying call enforces RGBQUAD use. TODO: Make span of array of 4 bytes.
  {
    FreeImage_ApplyColorMapping(native_, reinterpret_cast<RGBQUAD*>(source.data), reinterpret_cast<RGBQUAD*>(target.data), static_cast<std::uint32_t>(source.size), ignore_alpha, two_way);
  }
  void                                     apply_palette_index_mapping   (const span<std::uint8_t>& source, const span<std::uint8_t>& target,                                  const bool two_way = false)
  {
    FreeImage_ApplyPaletteIndexMapping(native_, source.data, target.data, static_cast<std::uint32_t>(source.size), two_way);
  }
  void                                     apply_overlay                 (const image& image, const std::array<std::size_t, 2>& position, const std::size_t alpha = 255)
  {
    FreeImage_Paste(native_, image.native_, static_cast<std::int32_t>(position[0]), static_cast<std::int32_t>(position[1]), static_cast<std::int32_t>(alpha));
  }
  void                                     apply_channel                 (const image& image, color_channel channel)
  {
    type() == type::complex 
      ? FreeImage_SetComplexChannel(native_, image.native_, static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel)) 
      : FreeImage_SetChannel       (native_, image.native_, static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel));
  }
  
  // Post processing functionality (replacing).
  void                                     apply_checkerboard_background ()
  {
    replace(FreeImage_Composite(native_));
  }
  void                                     apply_background              (const std::array<std::uint8_t, 4>& color) // Underlying call enforces RGBQUAD use.
  {
    replace(FreeImage_Composite(native_, false, reinterpret_cast<RGBQUAD*>(const_cast<std::uint8_t*>(color.data())), nullptr));
  }
  void                                     apply_background              (const image&                       image)
  {
    replace(FreeImage_Composite(native_, false, nullptr, image.native_));
  }
  void                                     threshold                     (const std::uint8_t threshold)
  {
    replace(FreeImage_Threshold(native_, threshold));
  }
  void                                     quantize                      (quantization_mode  mode     )
  {
    replace(FreeImage_ColorQuantize  (native_, static_cast<FREE_IMAGE_QUANTIZE>(mode)));
  }
  void                                     dither                        (dithering_mode     mode     )
  {
    replace(FreeImage_Dither(native_, static_cast<FREE_IMAGE_DITHER>(mode)));
  }                        
  void                                     tone_map_fattal02             (const double saturation = 0.5, const double attenuation = 0.85)
  {
    replace(FreeImage_TmoFattal02  (native_, saturation, attenuation));
  }
  void                                     tone_map_drago03              (const double gamma      = 2.2, const double exposure    = 0.0 )
  {
    replace(FreeImage_TmoDrago03   (native_, gamma     , exposure));
  }
  void                                     tone_map_reinhard05           (const double intensity  = 0.0, const double contrast    = 0.0 )
  {
    replace(FreeImage_TmoReinhard05(native_, intensity , contrast));
  }
  void                                     tone_map_reinhard05_ex        (const double intensity  = 0.0, const double contrast    = 0.0 , const double adaptation = 1.0, const double color_correction = 0.0)
  {
    replace(FreeImage_TmoReinhard05Ex(native_, intensity, contrast, adaptation, color_correction));
  } 
  void                                     multigrid_poisson             (const std::size_t iterations)
  {
    replace(FreeImage_MultigridPoissonSolver(native_, static_cast<std::int32_t>(iterations)));
  }
  
  // Transformation functionality (replacing).
  void                                     translate                     (const std::array<double, 2>& translation, const bool use_mask = false)
  {
    replace(FreeImage_RotateEx(native_, 0.0, translation[0], translation[1], 0.0, 0.0, use_mask));
  }
  void                                     rotate                        (const double angle)
  {
    replace(FreeImage_Rotate(native_, angle, nullptr));
  } 
  template<typename color_type = std::array<std::uint8_t, 4>>
  void                                     rotate                        (const double angle, const color_type& background_color)
  {
    replace(FreeImage_Rotate(native_, angle, &background_color));
  } 
  void                                     rotate                        (const double angle, const std::array<double, 2>& origin, const bool use_mask = false)
  {
    replace(FreeImage_RotateEx(native_, angle, 0.0, 0.0, origin[0], origin[1], use_mask));
  }                                                                 
  
  // Resize functionality (replacing).
  void                                     resize                        (const std::array<std::size_t, 2>& target_dimensions,                                          filter filter = filter::catmull_rom)
  {
    replace(FreeImage_Rescale(
      native_                                             , 
      static_cast<std::int32_t>     (target_dimensions[0]), 
      static_cast<std::int32_t>     (target_dimensions[1]), 
      static_cast<FREE_IMAGE_FILTER>(filter              )));
  }
  void                                     resize_rectangle              (const std::array<std::size_t, 2>& target_dimensions, const rectangle<std::size_t>& rectangle, filter filter = filter::catmull_rom)
  {
    replace(FreeImage_RescaleRect(
      native_                                             , 
      static_cast<std::int32_t>     (target_dimensions[0]), 
      static_cast<std::int32_t>     (target_dimensions[1]), 
      static_cast<std::int32_t>     (rectangle.left      ), 
      static_cast<std::int32_t>     (rectangle.top       ), 
      static_cast<std::int32_t>     (rectangle.right     ), 
      static_cast<std::int32_t>     (rectangle.bottom    ), 
      static_cast<FREE_IMAGE_FILTER>(filter              )));
  }
  template<typename color_type = std::array<std::uint8_t, 4>>
  void                                     resize_canvas                 (const rectangle<std::int32_t>& delta, const color_type& background_color, const std::int32_t native_options = 0)
  {
    replace(FreeImage_EnlargeCanvas(
      native_          , 
      delta.left       ,
      delta.top        , 
      delta.right      , 
      delta.bottom     ,
      &background_color,
      native_options   ));
  }

  // Conversion functionality (replacing).
  void                                     to_type                       (fi::type type, const bool scale_linear = true)
  {
    replace(FreeImage_ConvertToType(native_, static_cast<FREE_IMAGE_TYPE>(type), scale_linear));
  }
  void                                     to_standard                   (               const bool scale_linear = true)
  {
    replace(FreeImage_ConvertToStandardType(native_, scale_linear));
  }
  void                                     to_greyscale                  ()
  {
    replace(FreeImage_ConvertToGreyscale(native_));
  }
  void                                     to_4_bits                     ()
  {
    replace(FreeImage_ConvertTo4Bits    (native_));
  }
  void                                     to_8_bits                     ()
  {
    replace(FreeImage_ConvertTo8Bits    (native_));
  }
  void                                     to_16_bits_555                ()
  {
    replace(FreeImage_ConvertTo16Bits555(native_));
  }
  void                                     to_16_bits_565                ()
  {
    replace(FreeImage_ConvertTo16Bits565(native_));
  }
  void                                     to_24_bits                    ()
  {
    replace(FreeImage_ConvertTo24Bits   (native_));
  }
  void                                     to_32_bits                    ()
  {
    replace(FreeImage_ConvertTo32Bits   (native_));
  }
  
  // Thumbnail functionality.
  void                                     create_thumbnail              (const std::size_t size, const bool convert = true)
  {
    const auto thumbnail = FreeImage_MakeThumbnail(native_, static_cast<std::int32_t>(size), convert);
    FreeImage_SetThumbnail(native_, thumbnail);
    FreeImage_Unload(thumbnail);
  }
  void                                     delete_thumbnail              ()
  {
    FreeImage_SetThumbnail(native_, nullptr);
  }
  image                                    thumbnail                     ()                                                                                                                                        const
  {
    // Note: This image is not managed (its native representation will exist past destruction).
    return image(FreeImage_GetThumbnail(native_));
  }
  
  // ICC functionality.
  template<typename type>
  void                                     create_icc_profile            (const span<type>& span)
  {
    FreeImage_CreateICCProfile(native_, span.data, static_cast<long>(span.size));
  }
  void                                     delete_icc_profile            ()
  {
    FreeImage_DestroyICCProfile(native_);
  }
  template<typename type>                                                
  span<type>                               icc_profile                   ()                                                                                                                                        const
  {
    const auto profile = FreeImage_GetICCProfile(native_);
    return {profile->data, static_cast<std::size_t>(profile->size)};
  }
  
  // Metadata functionality.
  void                                     add_metadata                  (metadata_model model, const fi::metadata& metadata)
  {
    FreeImage_SetMetadata        (static_cast<FREE_IMAGE_MDMODEL>(model), native_, metadata.key().c_str(), metadata.native());
  }
  void                                     add_metadata                  (metadata_model model, const std::string& key, const std::string& value)
  {
    FreeImage_SetMetadataKeyValue(static_cast<FREE_IMAGE_MDMODEL>(model), native_, key.c_str(), value.c_str());
  }
  void                                     remove_metadata               (metadata_model model, const std::string& key)
  {
    FreeImage_SetMetadata(static_cast<FREE_IMAGE_MDMODEL>(model), native_, key.c_str(), nullptr);
  }
  fi::metadata                             metadata                      (metadata_model model, const std::string& key)                                                                                            const
  {
    FITAG* tag = nullptr;
    FreeImage_GetMetadata(static_cast<FREE_IMAGE_MDMODEL>(model), native_, key.c_str(), &tag);
    // Note: This metadata is not managed (its native representation will exist past destruction).
    return fi::metadata(tag);
  }
  std::size_t                              metadata_count                (metadata_model model)                                                                                                                    const
  {
    return static_cast<std::size_t>(FreeImage_GetMetadataCount(static_cast<FREE_IMAGE_MDMODEL>(model), native_));
  }
  void                                     iterate_metadata              (metadata_model model, const std::function<void(const fi::metadata&)>& callback)                                                          const
  {
    FITAG* tag      = nullptr;
    auto   iterator = FreeImage_FindFirstMetadata(static_cast<FREE_IMAGE_MDMODEL>(model), native_, &tag);
    do 
    {
      // Note: This metadata is not managed (its native representation will exist past destruction).
      const fi::metadata metadata(tag);
      callback(metadata);
    }
    while (FreeImage_FindNextMetadata(iterator, &tag));
    FreeImage_FindCloseMetadata(iterator);
  }
  BITMAPINFOHEADER*                        bitmap_info_header            ()                                                                                                                                        const
  {
    return FreeImage_GetInfoHeader(native_);
  }
  BITMAPINFO*                              bitmap_info                   ()                                                                                                                                        const
  {
    return FreeImage_GetInfo(native_);
  }
  
  // Accessors.                                                                                                                                                                                                    
  FIBITMAP*                                native                        ()                                                                                                                                        const
  {
    return native_;
  }
  format                                   format                        ()                                                                                                                                        const
  {
    return fi::format(format_);
  }

protected:
  void replace(FIBITMAP* native)
  {
    if (native_)
      FreeImage_Unload(native_);
    native_ = native;
    if (!native_)
      throw std::runtime_error("Replace failed.");
  }

  FIBITMAP*         native_  ;
  FREE_IMAGE_FORMAT format_  = FIF_UNKNOWN;
  bool              managed_ = true       ;
};

inline memory::memory(const image& image, const std::int32_t native_flags) : memory()
{
  FreeImage_SaveToMemory(image.format().native, image.native(), native_, native_flags);
}
}

#endif
