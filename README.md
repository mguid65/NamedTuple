# NamedTuple
C++20 single header named tuple

Try it out on [Compiler Explorer](https://godbolt.org/z/x3hPYjzGd)

## Build Sample and Tests

```shell
conan install . -b missing
cmake --preset conan-release
cmake --build --preset conan-release
```

## Usage

```c++
#include "NamedTuple.hpp"

int main() {
    mguid::NamedTuple<mguid::NamedType<"first", int>, 
                      mguid::NamedType<"second", int>> nt1{0, 0};
    
    const auto& first = nt1.get<"first">();
    const auto& second = nt1.get<"second">();
    
    nt1.set<"first">(42);
    
    mguid::NamedTuple<mguid::NamedType<"first", int>, 
                      mguid::NamedType<"second", int>> nt2{13, 37};
    mguid::NamedTuple<mguid::NamedType<"first", int>, 
                      mguid::NamedType<"second", int>> nt2{13, 37};
}
```

## A Note on Comparisons

A normal `std::tuple` has an ordering imposed on its data in the order that template parameter are specified,
and equality is defined as follows:

`sizeof...(TTypes) == sizeof...(UTypes)` and for all indices `I` in `[0, sizeof...(TTypes))`,
`std::get<I>(lhs) == std::get<I>(rhs)`

but the NamedTuple doesn't have this same restriction. The equality operator of the NamedTuple can be thought like this:

`sizeof...(TTypes) == sizeof...(UTypes)` and for all keys `Key` in `[Keys...]`, `lhs.get<Key>() == rhs.get<Key>()`

With this definition, it doesn't matter whether members are in the same order, but only that all the keys exist in `lhs` and `rhs`.

In order to get the behavior of `std::tuple`, first cast both sides to their respective `Base`s.

When comparing a `NamedTuple` with a `std::tuple` the std::tuple equality will be used.

All of these considerations are also true for `<=>`.