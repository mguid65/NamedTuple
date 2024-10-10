/**
 * @author Matthew Guidry (github: mguid65)
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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <tuple>
#include <type_traits>

namespace mguid {

template <std::size_t N>
struct StringLiteral {
  constexpr StringLiteral(char const (&str)[N]) { std::copy_n(str, N, value); }
  template <std::size_t M>
  constexpr bool operator==(StringLiteral<M> other) const {
    constexpr auto equal = [](const auto& lhs, const auto& rhs) {
      std::size_t i{0};
      for (; i < N; i++) {
        if (lhs.value[i] != rhs.value[i]) {
          return false;
        }
      }
      return true;
    };
    return M == N && equal(*this, other);
  }
  char value[N];
  static constexpr std::size_t size = N;
};

template <StringLiteral Key, StringLiteral... Keys>
constexpr bool is_one_of() {
  return (... || (Key == Keys));
}

template <StringLiteral Tag, typename Type>
struct NamedType {
  template <std::size_t M>
  constexpr bool operator==(StringLiteral<M> other) const {
    constexpr auto equal = [](const auto& lhs, const auto& rhs) {
      std::size_t i{0};
      for (; i < Tag.size; i++) {
        if (lhs.value[i] != rhs.value[i]) {
          return false;
        }
      }
      return true;
    };
    return M == Tag.size && equal(Tag, other);
  }

  template <StringLiteral OtherTag, typename OtherType>
  constexpr bool operator==(NamedType<OtherTag, OtherType>) const {
    return Tag == OtherTag;
  }
};

template <typename Type>
struct ExtractType;

template <StringLiteral Key, typename Type>
struct ExtractType<NamedType<Key, Type>> {
  using type = Type;
};

template <NamedType Type, NamedType... NamedTypes>
constexpr std::size_t key_index() {
  std::size_t index{0};
  ([&index]() {
    if (Type == NamedTypes) {
      return false;
    }
    ++index;
    return true;
  }() &&
   ...);
  return index;
}

template <StringLiteral Key, NamedType... NamedTypes>
constexpr std::size_t key_index() {
  std::size_t index{0};
  ([&index]() {
    if (Key == NamedTypes) {
      return false;
    }
    ++index;
    return true;
  }() &&
   ...);
  return index;
}

template <NamedType... NamedTypes>
constexpr bool all_unique() {
  bool seen[sizeof...(NamedTypes)] = {0};

  ([&seen]() { seen[key_index<NamedTypes, NamedTypes...>()] = true; }(), ...);

  for (std::size_t i{0}; i < sizeof...(NamedTypes); i++) {
    if (!seen[i]) {
      return false;
    }
  }

  return true;
}

template <StringLiteral Key, NamedType... NamedTypes>
constexpr bool is_one_of() {
  return (... || (Key == NamedTypes));
}

template <typename... NamedTypes>
  requires(all_unique<NamedTypes{}...>())
struct NamedTuple
    : std::tuple<typename ExtractType<NamedTypes>::type...> {
  
  using Base = std::tuple<typename ExtractType<NamedTypes>::type...>;
  
  template <StringLiteral Tag>
    requires(is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr auto& get() & noexcept {
    return std::get<key_index<Tag, NamedTypes{}...>()>(static_cast<Base&>(*this));
  }

  template <StringLiteral Tag>
    requires(is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr const auto& get() const& noexcept {
    return std::get<key_index<Tag, NamedTypes...>()>(static_cast<Base&>(*this));
  }

  template <StringLiteral Tag>
    requires(is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr auto&& get() && noexcept {
    return std::get<key_index<Tag, NamedTypes{}...>()>(static_cast<Base&>(*this));
  }

  template <StringLiteral Tag>
    requires(is_one_of<Tag, NamedTypes{}...>())
  [[nodiscard]] constexpr const auto&& get() const&& noexcept{
    return std::get<key_index<Tag, NamedTypes...>()>(static_cast<Base&>(*this));
  }
};

} // namespace mguid
