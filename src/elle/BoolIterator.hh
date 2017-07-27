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
}
