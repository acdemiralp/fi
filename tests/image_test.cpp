#include "catch.hpp"

#include <fi/image.hpp>

TEST_CASE("Image is tested.", "[image]") 
{
  fi::image image(std::array<std::size_t, 2>{100, 100});
}