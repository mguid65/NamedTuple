#include "NamedTuple.hpp"

#include <catch2/catch_all.hpp>

#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>

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
    [[maybe_unused]] mguid::NamedTuple<mguid::NamedType<"key1", int>, mguid::NamedType<"key2", int>,
                                       mguid::NamedType<"key3", int>, mguid::NamedType<"key4", int>,
                                       mguid::NamedType<"key5", int>, mguid::NamedType<"key6", int>,
                                       mguid::NamedType<"key7", int>, mguid::NamedType<"key8", int>>
        nt{};
    REQUIRE(std::tuple_size_v<decltype(nt)> == std::size_t{8});
  }
  SECTION("Aggregate Initialization") {
    [[maybe_unused]] mguid::NamedTuple<
        mguid::NamedType<"key1", int>, mguid::NamedType<"key2", char>,
        mguid::NamedType<"key3", int>, mguid::NamedType<"key4", char>,
        mguid::NamedType<"key5", int>, mguid::NamedType<"key6", char>,
        mguid::NamedType<"key7", int>, mguid::NamedType<"key8", char>>
        nt{1, 'a', 3, 'b', 5, 'c', 7, 'd'};

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
  SECTION("Nested") {
    [[maybe_unused]] mguid::NamedTuple<mguid::NamedType<
        "nested",
        mguid::NamedTuple<mguid::NamedType<
            "nested", mguid::NamedTuple<mguid::NamedType<
                          "nested", mguid::NamedTuple<mguid::NamedType<"inner", int>>>>>>>>
        nt;

    REQUIRE(std::is_same_v<
            std::remove_cvref_t<decltype(nt.get<"nested">())>,
            mguid::NamedTuple<mguid::NamedType<
                "nested", mguid::NamedTuple<mguid::NamedType<
                              "nested", mguid::NamedTuple<mguid::NamedType<"inner", int>>>>>>>);

    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"nested">().get<"nested">())>,
                           mguid::NamedTuple<mguid::NamedType<
                               "nested", mguid::NamedTuple<mguid::NamedType<"inner", int>>>>>);
  }
  SECTION("Multiple Types") {
    [[maybe_unused]] mguid::NamedTuple<
        mguid::NamedType<"bool", bool>, mguid::NamedType<"int8_t", std::int8_t>,
        mguid::NamedType<"int16_t", std::int16_t>, mguid::NamedType<"int32_t", std::int32_t>,
        mguid::NamedType<"int64_t", std::int64_t>, mguid::NamedType<"uint8_t", std::uint8_t>,
        mguid::NamedType<"uint16_t", std::uint16_t>, mguid::NamedType<"uint32_t", std::uint32_t>,
        mguid::NamedType<"uint64_t", std::uint64_t>, mguid::NamedType<"float", float>,
        mguid::NamedType<"double", double>, mguid::NamedType<"string", std::string>>
        nt;

    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"bool">())>, bool>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"int8_t">())>, std::int8_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"int16_t">())>, std::int16_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"int32_t">())>, std::int32_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"int64_t">())>, std::int64_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"uint8_t">())>, std::uint8_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"uint16_t">())>, std::uint16_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"uint32_t">())>, std::uint32_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"uint64_t">())>, std::uint64_t>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"float">())>, float>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"double">())>, double>);
    REQUIRE(std::is_same_v<std::remove_cvref_t<decltype(nt.get<"string">())>, std::string>);
  }
}

TEST_CASE("NamedTuple Setter") {
  SECTION("Set") {
    [[maybe_unused]] mguid::NamedTuple<mguid::NamedType<"key", int>> nt{2};
    REQUIRE(nt.get<"key">() == 2);
    nt.set<"key">(42);
    REQUIRE(nt.get<"key">() == 42);
  }
}

TEST_CASE("Comparison") {
  SECTION("Empty Equal") {
    mguid::NamedTuple<> nt1;
    mguid::NamedTuple<> nt2;
    REQUIRE(nt1 == nt2);
  }
}

TEST_CASE("Specialization of std::tuple_element") {
  SECTION("Single Element") {
    REQUIRE(std::is_same_v<std::tuple_element_t<0, mguid::NamedTuple<mguid::NamedType<"test", float>>>, float>);
    REQUIRE(std::is_same_v<std::tuple_element_t<0, mguid::NamedTuple<mguid::NamedType<"test", int>>>, int>);
  }
  SECTION("Multiple Elements") {
    REQUIRE(std::is_same_v<std::tuple_element_t<0,
                mguid::NamedTuple<mguid::NamedType<"test1", float>,
                                  mguid::NamedType<"test2", char>,
                                  mguid::NamedType<"test3", int>>>, float>);
    REQUIRE(std::is_same_v<std::tuple_element_t<1,
                mguid::NamedTuple<mguid::NamedType<"test1", float>,
                                  mguid::NamedType<"test2", char>,
                                  mguid::NamedType<"test3", int>>>, char>);
    REQUIRE(std::is_same_v<std::tuple_element_t<2,
                mguid::NamedTuple<mguid::NamedType<"test1", float>,
                                  mguid::NamedType<"test2", char>,
                                  mguid::NamedType<"test3", int>>>, int>);
  }
}

TEST_CASE("Specialization of std::tuple_size") {
  SECTION("Empty") { REQUIRE(std::tuple_size_v<mguid::NamedTuple<>> == 0); }
  SECTION("Single Element") {
    REQUIRE(std::tuple_size_v<mguid::NamedTuple<mguid::NamedType<"test", int>>> == 1);
  }
  SECTION("Multiple Elements") {
    REQUIRE(std::tuple_size_v<
                mguid::NamedTuple<mguid::NamedType<"test1", int>, mguid::NamedType<"test2", int>,
                                  mguid::NamedType<"test3", int>>> == 3);
  }
}