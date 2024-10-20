#include <NamedTuple.hpp>

#include <iostream>

using mguid::NamedTuple;
using mguid::NamedType;
using mguid::NamedTypeV;

int main() {
  int i = 5;
  std::reference_wrapper<int> i_ref{i};
  auto nt = mguid::make_tuple(NamedTypeV<"int_key">(i_ref),
                                             NamedTypeV<"float_key">(1.0f),
                                             NamedTypeV<"char_key">('c'));

  nt.get<"int_key">() = 42;
  nt.get<"float_key">() = 0.3f;
  nt.get<"char_key">() = 'g';

  std::cout << nt.get<"int_key">() << '\n';
  std::cout << nt.get<"float_key">() << '\n';
  std::cout << nt.get<"char_key">() << '\n';
}
