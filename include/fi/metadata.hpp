#ifndef FI_METADATA_HPP_
#define FI_METADATA_HPP_

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

#include <fi/enums/metadata_model.hpp>
#include <fi/enums/metadata_type.hpp>

namespace fi
{
class image;

class metadata
{
public:
  explicit metadata  () : native_(FreeImage_CreateTag())
  {
    if (!native_)
      throw std::runtime_error("FreeImage_CreateTag failed.");
  }
  explicit metadata  (FITAG* native) : native_(native), managed_(false)
  {
    if (!native_)
      throw std::runtime_error("FITAG is nullptr.");
  }
  metadata           (const metadata&  that) : native_(FreeImage_CloneTag(that.native_))
  {
    if (!native_)
      throw std::runtime_error("FreeImage_CloneTag failed.");
  }                                                                                
  metadata           (      metadata&& temp) noexcept : native_(std::move(temp.native_)), managed_(std::move(temp.managed_))
  {
    temp.native_ = nullptr;
  }
  virtual ~metadata  ()
  {
    if (native_ && managed_)
      FreeImage_DeleteTag(native_);
  }
  metadata& operator=(const metadata&  that)
  {
    native_ = FreeImage_CloneTag(that.native_);
    if (!native_)
      throw std::runtime_error("FreeImage_CloneTag failed.");
    return *this;
  }
  metadata& operator=(      metadata&& temp) noexcept
  {
    if (this != &temp)
    {
      native_      = std::move(temp.native_ );
      managed_     = std::move(temp.managed_);
      temp.native_ = nullptr;
    }
    return *this;
  }

  void          set_key          (const std::string&   key          )
  {
    FreeImage_SetTagKey(native_, key.c_str());
  }
  void          set_description  (const std::string&   description  )
  {
    FreeImage_SetTagDescription(native_, description.c_str());
  }
  void          set_id           (const std::uint16_t& id           )
  {
    FreeImage_SetTagID(native_, id);
  }
  void          set_type         (const metadata_type  type         )
  {
    FreeImage_SetTagType(native_, static_cast<FREE_IMAGE_MDTYPE>(type));
  }
  void          set_size         (const std::size_t    size         )
  {
    FreeImage_SetTagCount(native_, static_cast<unsigned long>(size));
  }
  void          set_size_in_bytes(const std::size_t    size_in_bytes)
  {
    FreeImage_SetTagLength(native_, static_cast<unsigned long>(size_in_bytes));
  }
  template<typename type>
  void          set_value        (const type*          value        )
  {
    FreeImage_SetTagValue(native_, value);
  }

  std::string   key              () const
  {
    return FreeImage_GetTagKey(native_);
  }
  std::string   description      () const
  {
    return FreeImage_GetTagDescription(native_);
  }
  std::uint16_t id               () const
  {
    return FreeImage_GetTagID(native_);
  }
  metadata_type type             () const
  {
    return static_cast<metadata_type>(FreeImage_GetTagType(native_));
  }
  std::size_t   size             () const
  {
    return static_cast<std::size_t>(FreeImage_GetTagCount(native_));
  }
  std::size_t   size_in_bytes    () const
  {
    return static_cast<std::size_t>(FreeImage_GetTagLength(native_));
  }
  template<typename type>        
  const type*   value            () const
  {
    return reinterpret_cast<const type*>(FreeImage_GetTagValue(native_));
  }

  std::string   to_string        (metadata_model model) const
  {
    return FreeImage_TagToString(static_cast<FREE_IMAGE_MDMODEL>(model), native_, nullptr);
  }

  FITAG* native() const
  {
    return native_;
  }

protected:
  FITAG* native_  ;
  bool   managed_ = true;
};
}

#endif
