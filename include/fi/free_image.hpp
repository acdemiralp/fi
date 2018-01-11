#ifndef FI_FREEIMAGE_HPP_
#define FI_FREEIMAGE_HPP_

#include <iostream>
#include <string>

#include <FreeImage.h>

namespace fi
{
inline void        initialize       ()
{
  FreeImage_Initialise(false);
}
inline void        uninitialize     ()
{
  FreeImage_DeInitialise();
}

inline std::string version          ()
{
  return FreeImage_GetVersion();
}
inline std::string copyright        ()
{
  return FreeImage_GetCopyrightMessage();
}

inline void        set_debug_enabled(const bool enabled)
{
  if (enabled)
    FreeImage_SetOutputMessage([ ] (FREE_IMAGE_FORMAT format, const char* message) {std::cout << message << std::endl;});
  else
    FreeImage_SetOutputMessage(nullptr);
}
}

#endif
