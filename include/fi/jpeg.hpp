#ifndef FI_JPEG_TRANSFORM_HPP_
#define FI_JPEG_TRANSFORM_HPP_

#include <cstddef>
#include <cstdint>
#include <string>

#define FREEIMAGE_COLORORDER 1
#include <FreeImage.h>

#include <fi/utility/rectangle.hpp>

namespace fi
{
namespace jpeg
{
inline void flip_horizontal(const std::string& source_filepath, const std::string& target_filepath, const bool perfect = true)
{
  FreeImage_JPEGTransform(source_filepath.c_str(), target_filepath.c_str(), FIJPEG_OP_FLIP_H, perfect);
}
inline void flip_vertical  (const std::string& source_filepath, const std::string& target_filepath, const bool perfect = true)
{
  FreeImage_JPEGTransform(source_filepath.c_str(), target_filepath.c_str(), FIJPEG_OP_FLIP_V, perfect);
}
inline void transpose      (const std::string& source_filepath, const std::string& target_filepath, const bool perfect = true)
{
  FreeImage_JPEGTransform(source_filepath.c_str(), target_filepath.c_str(), FIJPEG_OP_TRANSPOSE, perfect);
}
inline void transverse     (const std::string& source_filepath, const std::string& target_filepath, const bool perfect = true)
{
  FreeImage_JPEGTransform(source_filepath.c_str(), target_filepath.c_str(), FIJPEG_OP_TRANSVERSE, perfect);
}
inline void rotate_90      (const std::string& source_filepath, const std::string& target_filepath, const bool perfect = true)
{
  FreeImage_JPEGTransform(source_filepath.c_str(), target_filepath.c_str(), FIJPEG_OP_ROTATE_90, perfect);
}
inline void rotate_180     (const std::string& source_filepath, const std::string& target_filepath, const bool perfect = true)
{
  FreeImage_JPEGTransform(source_filepath.c_str(), target_filepath.c_str(), FIJPEG_OP_ROTATE_180, perfect);
}
inline void rotate_270     (const std::string& source_filepath, const std::string& target_filepath, const bool perfect = true)
{
  FreeImage_JPEGTransform(source_filepath.c_str(), target_filepath.c_str(), FIJPEG_OP_ROTATE_270, perfect);
}
inline void crop           (const std::string& source_filepath, const std::string& target_filepath, const rectangle<std::size_t>& rectangle)
{
  FreeImage_JPEGCrop(source_filepath.c_str(), target_filepath.c_str(), static_cast<std::int32_t>(rectangle.left), static_cast<std::int32_t>(rectangle.top), static_cast<std::int32_t>(rectangle.right), static_cast<std::int32_t>(rectangle.bottom));
}
}
}

#endif
