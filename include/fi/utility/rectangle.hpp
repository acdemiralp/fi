#ifndef FI_RECTANGLE_HPP_
#define FI_RECTANGLE_HPP_

namespace fi
{
// I resisted creating this struct to use standard library primitives
// instead, but it has proven itself necessary for clarity.
template <typename type>
struct rectangle
{
  type left, right, top, bottom;
};
}

#endif