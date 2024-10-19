/**
 * @author Matthew Guidry (github: mguid65)
 * @date 2024-10-11
 *
 * @cond IGNORE_LICENSE
 *
 * MIT License
 *
 * Copyright (c) 2024 Matthew Guidry
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @endcond
 */

#ifndef MGUID_NAMEDTUPLE_H
#define MGUID_NAMEDTUPLE_H

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace mguid {

/**
 * @brief Compile time string literal container
 * @tparam NSize size of string literal
 */
template <std::size_t NSize>
struct StringLiteral {
  /**
   * @brief Construct a StringLiteral from a string literal
   * @param str a string literal as a const reference to a sized char array
   */
  constexpr explicit(false) StringLiteral(char const (&str)[NSize]) : value{'\0'} {
    std::copy_n(str, NSize, value);
  }

  /**
   * @brief Equality compare against another StringLiteral
   * Since this type is only templated on the size,
   * we need the equality operator to determine sameness for lookup
   *
   * @tparam MSize size of other string literal
   * @param other StringLiteral to compare with
   * @return true if equal; otherwise false
   */
  template <std::size_t MSize>
  [[nodiscard]] constexpr bool operator==(StringLiteral<MSize> other) const {
    constexpr auto equal = [](const auto& lhs, const auto& rhs) {
      for (std::size_t i{0}; i < NSize; i++) {
        if (lhs.value[i] != rhs.value[i]) { return false; }
      }
      return true;
    };
    return MSize == NSize && equal(*this, other);
  }

  /**
   * @brief Equality compare against a string_view
   *
   * subtract one because the string_view doesn't account for the null-terminator
   *
   * @param sv string_view to compare with
   * @return true if equal; otherwise false
   */
  [[nodiscard]] constexpr bool operator==(const std::string_view sv) const {
    return sv == std::string_view{value, size - 1};
  }

  char value[NSize];
  static constexpr std::size_t size{NSize};
};

/**
 * @brief A helper type to associate a StringLiteral tag with a type for use with NamedTuple
 * @tparam Tag the "name" associated with the type
 * @tparam Unused the type to forward to the tuple base in NamedTuple; This type is extracted but
 * not used directly in this helper class
 */
template <StringLiteral Tag, typename Unused>
struct NamedType {
  /**
   * @brief Equality compare against a StringLiteral
   * we need the equality operator to determine sameness for lookup
   *
   * Sameness in this case is only used to determine uniqueness where uniqueness is only determined
   * by differing keys.
   *
   * @tparam MSize size of other string literal
   * @param other StringLiteral to compare with
   * @return true if equal; otherwise false
   */
  template <std::size_t MSize>
  constexpr bool operator==(StringLiteral<MSize> other) const {
    constexpr auto equal = [](const auto& lhs, const auto& rhs) {
      for (std::size_t i{0}; i < Tag.size; i++) {
        if (lhs.value[i] != rhs.value[i]) { return false; }
      }
      return true;
    };
    return MSize == Tag.size && equal(Tag, other);
  }

  /**
   * @brief Equality compare this with another NamedType
   * @tparam OtherTag Tag of other NamedType
   * @tparam OtherType Type of other NamedType
   * @return true if both have the same tag; otherwise false
   */
  template <StringLiteral OtherTag, typename OtherType>
  constexpr bool operator==(NamedType<OtherTag, OtherType>) const {
    return Tag == OtherTag;
  }

  /**
   * @brief Equality compare this with a string_view
   * @param sv string_view to compare with
   * @return true if the string_view is equal to this instances Tag; otherwise false
   */
  constexpr bool operator==(std::string_view sv) const { return Tag == sv; }

  /**
   * @brief Get the tag passed to this NamedType
   * @return the tag passed to this NamedType
   */
  constexpr StringLiteral<Tag.size> tag() const { return Tag; }
};

/**
 * @brief Base template of helper template to extract the type from a NamedType
 * @tparam Type unconstrained type
 */
template <typename Type>
struct ExtractType;

/**
 * @brief Partially specialized helper template
 * @tparam Key key part of NamedType
 * @tparam Type type part of NamedType
 */
template <StringLiteral Key, typename Type>
struct ExtractType<NamedType<Key, Type>> {
  using type = Type;
};

/**
 * @brief Find the index of a NamedType in a pack of NamedType non-types
 *
 * NOTE: This assumes that the NamedType we are looking for exists within the pack
 *
 * @tparam Needle NamedType to search for
 * @tparam Haystack pack of NamedType non-types
 * @return the index equivalent of the location of the type within the pack
 */
template <NamedType Needle, NamedType... Haystack>
constexpr std::size_t key_index() {
  std::size_t index{0};
  ([&index]<NamedType Type>() {
    if (Needle == Type) { return false; }
    ++index;
    return true;
  }.template operator()<Haystack>() &&
   ...);
  return index;
}

/**
 * @brief Find the index of a NamedType with the tag Key in a pack of NamedType non-types
 *
 * NOTE: This assumes that the NamedType with the tag Key exists within the pack
 *
 * @tparam Needle StringLiteral tag to search for
 * @tparam Haystack pack of NamedType non-types
 * @return the index equivalent of the location of the type with the tag within the pack
 */
template <StringLiteral Needle, NamedType... Haystack>
constexpr std::size_t key_index() {
  std::size_t index{0};
  ([&index]<NamedType Type>() {
    if (Needle == Type) { return false; }
    ++index;
    return true;
  }.template operator()<Haystack>() &&
   ...);
  return index;
}

/**
 * @brief Determine if all NamedTypes within the pack are unique
 * @tparam NamedTypes pack of NamedType non-types
 * @return true if all values in the pack are unique; otherwise false
 */
template <NamedType... NamedTypes>
constexpr bool all_unique() {
  if constexpr (sizeof...(NamedTypes) == 0) {
    return true;
  } else {
    bool seen[sizeof...(NamedTypes)] = {false};

    (
        [&seen]<NamedType Type>() {
          seen[key_index<Type, NamedTypes...>()] = true;
        }.template operator()<NamedTypes>(),
        ...);

    for (std::size_t i{0}; i < sizeof...(NamedTypes); i++) {
      if (!seen[i]) { return false; }
    }

    return true;
  }
}

/**
 * @brief Recreation of the exposition only function synth-three-way
 */
constexpr auto SynthThreeWay = []<class T, class U>(const T& t, const U& u)
  requires requires {
    { t < u } -> std::convertible_to<bool>;
    { u < t } -> std::convertible_to<bool>;
  }
{
  if constexpr (std::three_way_comparable_with<T, U>) {
    return t <=> u;
  } else {
    if (t < u) return std::weak_ordering::less;
    if (u < t) return std::weak_ordering::greater;
    return std::weak_ordering::equivalent;
  }
};

template <class T, class U = T>
using SynthThreeWayResultT = decltype(SynthThreeWay(std::declval<T&>(), std::declval<U&>()));

/**
 * @brief Check if Key is one of the tags of a NamedType within the NamedTypes pack
 * @tparam Key key to search for
 * @tparam NamedTypes pack of NamedType to search in
 * @return true if Key exists as the tag of a NamedType in NamedTypes; otherwise false
 */
template <StringLiteral Key, NamedType... NamedTypes>
constexpr bool is_one_of() {
  return (... || (Key == NamedTypes));
}

/**
 * @brief A tuple whose elements can be looked up by name(string literal) along with type and index
 * @tparam NamedTypes pack of NamedType types with unique names
 */
template <typename... NamedTypes>
  requires(all_unique<NamedTypes{}...>())
struct NamedTuple : std::tuple<typename ExtractType<NamedTypes>::type...> {
  using Base = std::tuple<typename ExtractType<NamedTypes>::type...>;
  using ConstBase = const std::tuple<typename ExtractType<NamedTypes>::type...>;

  /**
   * @brief Construct this NamedTuple initializing all elements
   * @tparam InitTypes types of initializer values
   * @param init_values values to initialize each tuple element
   */
  template <typename... InitTypes>
  constexpr explicit(false) NamedTuple(InitTypes&&... init_values)
      : Base{std::forward<InitTypes>(init_values)...} {}

  /**
   * @brief Get the number of elements this NamedTuple holds
   * @return the number of elements this NamedTuple holds
   */
  [[nodiscard]] constexpr std::size_t size() const { return sizeof...(NamedTypes); }

  /**
   * @brief Explicit conversion operator to Base
   * @return Const reference to Base
   */
  [[nodiscard]] constexpr explicit operator const Base&() const {
    return static_cast<const Base&>(*this);
  }

  /**
   * @brief Explicit conversion operator to Base
   * @return Reference to Base
   */
  [[nodiscard]] constexpr explicit operator Base&() { return static_cast<Base&>(*this); }

  /**
   * @brief Set the element of the NamedTuple with the name Tag to value
   * @tparam Tag StringLiteral element name
   * @tparam Value type of value, convertible to the type of the element associated with Tag
   * @param value value to set
   */
  template <StringLiteral Tag, typename Value>
    requires(
        sizeof...(NamedTypes) > 0 && is_one_of<Tag, NamedTypes{}...>() &&
        std::is_convertible_v<Value, std::tuple_element_t<key_index<Tag, NamedTypes{}...>(), Base>>)
  constexpr void set(Value&& value) {
    constexpr std::size_t Index = key_index<Tag, NamedTypes{}...>();
    std::get<Index>(static_cast<Base&>(*this)) = std::forward<Value>(value);
  }

  /**
   * @brief Extracts the element of the NamedTuple whose name is Tag
   * @tparam Tag a StringLiteral to search for
   * @return the element of the NamedTuple whose name is Tag
   */
  template <StringLiteral Tag>
    requires(sizeof...(NamedTypes) > 0 && is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr auto& get() & noexcept {
    constexpr std::size_t Index = key_index<Tag, NamedTypes{}...>();
    return std::get<Index>(static_cast<Base&>(*this));
  }

  /**
   * @brief Extracts the element of the NamedTuple whose name is Tag
   * @tparam Tag a StringLiteral to search for
   * @return the element of the NamedTuple whose name is Tag
   */
  template <StringLiteral Tag>
    requires(sizeof...(NamedTypes) > 0 && is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr const auto& get() const& noexcept {
    constexpr std::size_t Index = key_index<Tag, NamedTypes{}...>();
    return std::get<Index>(static_cast<const Base&>(*this));
  }

  /**
   * @brief Extracts the element of the NamedTuple whose name is Tag
   * @tparam Tag a StringLiteral to search for
   * @return the element of the NamedTuple whose name is Tag
   */
  template <StringLiteral Tag>
    requires(sizeof...(NamedTypes) > 0 && is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr auto&& get() && noexcept {
    constexpr std::size_t Index = key_index<Tag, NamedTypes{}...>();
    return std::get<Index>(static_cast<Base&>(*this));
  }

  /**
   * @brief Extracts the element of the NamedTuple whose name is Tag
   * @tparam Tag a StringLiteral to search for
   * @return the element of the NamedTuple whose name is Tag
   */
  template <StringLiteral Tag>
    requires(sizeof...(NamedTypes) > 0 && is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr const auto&& get() const&& noexcept {
    constexpr std::size_t Index = key_index<Tag, NamedTypes{}...>();
    return std::get<Index>(static_cast<const Base&>(*this));
  }

  /**
   * @brief Element-wise comparison of the elements in this NamedTuple with elements in the other
   * NamedTuple
   *
   * Note: The key is not a part of the value of this type,
   * however it does make the equality behave differently.
   *
   * Do we want to compare as if we were calling get<KeyLhs>() == get<KeyRhs>()
   * or as if we are calling get<IndexLhs>() == get<IndexRhs>()? This matters because looking up by
   * key makes it so the order doesnt matter.
   *
   * @tparam OtherNamedTypes pack of types in other NamedTuple
   * @param other another NamedTuple to compare against
   * @return Returns true if all pairs of corresponding elements are equal; otherwise false
   */
  template <typename... OtherNamedTypes>
  [[nodiscard]] constexpr bool operator==(const NamedTuple<OtherNamedTypes...>& other) const {
    static_assert(sizeof...(NamedTypes) == sizeof...(OtherNamedTypes));
    return ((this->get<NamedTypes{}.tag()>() == other.template get<NamedTypes{}.tag()>()) && ...);
  }

  /**
   * @brief Element-wise comparison of the elements in this NamedTuple with elements in a std::tuple
   *
   * Note: The key is not a part of the value of this type
   *
   * @tparam OtherTypes pack of types in the std::tuple
   * @param other a std::tuple to compare against
   * @return Returns true if all pairs of corresponding elements are equal; otherwise false
   */
  template <typename... OtherTypes>
  [[nodiscard]] constexpr bool operator==(const std::tuple<OtherTypes...>& other) const {
    return static_cast<const Base&>(*this) == other;
  }

  /**
   * @brief Spaceship compare against a std::tuple
   * @tparam OtherTypes pack of types in the std::tuple
   * @param other a std::tuple to compare against
   * @return
   */
  template <typename... OtherTypes>
  [[nodiscard]] constexpr auto operator<=>(const std::tuple<OtherTypes...>& other) const {
    return static_cast<const Base&>(*this) <=> other;
  }

  /**
   * @brief Spaceship compare against a std::tuple
   * @tparam OtherTypes pack of types in the std::tuple
   * @param other a std::tuple to compare against
   * @return
   */
  template <typename... OtherNamedTypes>
  [[nodiscard]] constexpr auto operator<=>(const NamedTuple<OtherNamedTypes...>& other) const {
    static_assert(sizeof...(NamedTypes) == sizeof...(OtherNamedTypes));
    if constexpr (sizeof...(NamedTypes) == 0 && sizeof...(OtherNamedTypes) == 0) {
      return std::strong_ordering::equal;
    }

    std::common_comparison_category_t<SynthThreeWayResultT<
        typename ExtractType<NamedTypes>::type,
        typename ExtractType<OtherNamedTypes>::type>...> result = std::strong_ordering::equivalent;

    ([this, &other, &result]<StringLiteral Tag>() {
      result = SynthThreeWay(this->get<Tag>(), other.template get<Tag>());
      return result != 0;
    }.template operator()<NamedTypes{}.tag()>() || ...);

    return result;
  }
};

}  // namespace mguid

/**
 * @brief Specialization of std::tuple_size for NamedTuple
 * @tparam NamedTypes type list for a NamedTuple
 */
template <typename... NamedTypes>
struct std::tuple_size<mguid::NamedTuple<NamedTypes...>>
    : std::integral_constant<std::size_t, sizeof...(NamedTypes)> {};

/**
 * @brief Specialization of std::tuple_element for NamedTuple
 * @tparam Index index of tuple element in tuple
 * @tparam NamedTypes type list for a NamedTuple
 */
template <std::size_t Index, typename... NamedTypes>
struct std::tuple_element<Index, mguid::NamedTuple<NamedTypes...>> {
  static_assert(Index < sizeof...(NamedTypes), "Index out of range");
  using Base = typename mguid::NamedTuple<NamedTypes...>::Base;
  using type = std::tuple_element_t<Index, Base>;
};

#endif  // MGUID_NAMEDTUPLE_H
