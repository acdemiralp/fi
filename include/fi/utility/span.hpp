#ifndef FI_SPAN_HPP_
#define FI_SPAN_HPP_

#include <cstddef>

namespace fi
{
template<typename type>
struct span
{
  type*       data;
  std::size_t size;
};
}

#endif