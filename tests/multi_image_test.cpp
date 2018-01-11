#include "catch.hpp"

#include <fi/multi_image.hpp>

TEST_CASE("Multi image is tested.", "[multi_image]")
{
  fi::multi_image multi_image("test.tiff", true);
  REQUIRE(multi_image.size() == 0ull);
  multi_image.push_back(fi::image("test.png"));
  REQUIRE(multi_image.size() == 1ull);
  multi_image.erase(0);
  REQUIRE(multi_image.size() == 0ull);
}