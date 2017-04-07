#pragma once

namespace elle
{
  template <typename I>
  struct BoolIterator
    : public I
  {
    BoolIterator(I it, bool v = true)
      : I(it)
      , value(v)
    {}

    operator bool() const
    {
      return this->value;
    }

    bool value;
  };

  template <typename C, typename E>
  auto
  find(C& c, E const& e)
  {
    auto it = c.find(e);
    return BoolIterator<decltype(it)>(it, it != c.end());
  }
}
