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
    REQUIRE(std::same_as<std::tuple_element_t<0, decltype(nt)>, int>);
  }
  SECTION("Multiple Types") {
    [[maybe_unused]] mguid::NamedTuple<
      mguid::NamedType<"key1", int>,
      mguid::NamedType<"key2", int>,
      mguid::NamedType<"key3", int>,
      mguid::NamedType<"key4", int>,
      mguid::NamedType<"key5", int>,
      mguid::NamedType<"key6", int>,
      mguid::NamedType<"key7", int>,
      mguid::NamedType<"key8", int>> nt{};
    REQUIRE(std::tuple_size_v<decltype(nt)> == std::size_t{8});
  }
  SECTION("Aggregate Initialization") {
    [[maybe_unused]] mguid::NamedTuple<
      mguid::NamedType<"key1", int>,
      mguid::NamedType<"key2", char>,
      mguid::NamedType<"key3", int>,
      mguid::NamedType<"key4", char>,
      mguid::NamedType<"key5", int>,
      mguid::NamedType<"key6", char>,
      mguid::NamedType<"key7", int>,
      mguid::NamedType<"key8", char>> nt{ 1, 'a', 3, 'b', 5, 'c', 7, 'd' };

    REQUIRE(std::get<0>(nt) == 1);
    REQUIRE(std::get<1>(nt) == 'a');
    REQUIRE(std::get<2>(nt) == 3);
    REQUIRE(std::get<3>(nt) == 'b');
    REQUIRE(std::get<4>(nt) == 5);
    REQUIRE(std::get<5>(nt) == 'c');
    REQUIRE(std::get<6>(nt) == 7);
    REQUIRE(std::get<7>(nt) == 'd');

    REQUIRE(nt.get<"key1">() == 1);
    REQUIRE(nt.get<"key2">() == 'a');
    REQUIRE(nt.get<"key3">() == 3);
    REQUIRE(nt.get<"key4">() == 'b');
    REQUIRE(nt.get<"key5">() == 5);
    REQUIRE(nt.get<"key6">() == 'c');
    REQUIRE(nt.get<"key7">() == 7);
    REQUIRE(nt.get<"key8">() == 'd');
  }
}

