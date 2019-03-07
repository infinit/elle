#pragma once

namespace elle
{
  template <typename ... T>
  auto
  product(T&& ... elements)
  {
    return (elements * ... * 1);
  }

  template <typename ... T>
  auto
  sum(T&& ... elements)
  {
    return (elements + ... + 0);
  }
}
