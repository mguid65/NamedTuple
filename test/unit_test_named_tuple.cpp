#include "NamedTuple.hpp"

#include <catch2/catch_all.hpp>

#include <tuple>

TEST_CASE("NamedTuple Constructor") {
  SECTION("Empty") {
    [[maybe_unused]] mguid::NamedTuple<> nt{};
    REQUIRE(std::tuple_size_v<decltype(nt)> == std::size_t{0});
  }
  SECTION("Single Type") {
    [[maybe_unused]] mguid::NamedTuple<mguid::NamedType<"key", int>> nt{};
    REQUIRE(std::tuple_size_v<decltype(nt)> == std::size_t{1});
  }
  SECTION("Aggregate Initialization") {}
}

