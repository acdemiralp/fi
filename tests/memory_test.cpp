#include "catch.hpp"

#include <fi/memory.hpp>

TEST_CASE("Memory is tested.", "[memory]") 
{
  std::vector<std::int16_t> block {0, 5, 10, 15, 20, 25};
  fi::memory                memory(fi::span<std::int16_t>{block.data(), block.size()});

  REQUIRE(memory.span<std::int16_t>().data == block.data());
  REQUIRE(memory.read<std::int16_t>(block.size()) == block);
  REQUIRE(memory.seek(sizeof std::int16_t));
  REQUIRE(memory.tell() == sizeof std::int16_t);
  REQUIRE(memory.read<std::int16_t>(block.size() - 1) == std::vector<std::int16_t>(block.begin() + 1, block.end()));
}