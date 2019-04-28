#pragma once

#include <type_traits>

namespace elle
{
  template <typename T>
  struct optimal_argument_type
  {
    constexpr static auto plain
    = (std::is_fundamental<T>{}
           || std::is_enum<T>{}
           || std::is_pointer<T>{});

    using type = std::conditional_t<plain, T, T const&>;
  };

  template <typename T>
  using optimal_argument_type_t = typename optimal_argument_type<T>::type;
}
