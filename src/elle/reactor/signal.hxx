#pragma once

namespace elle
{
  namespace reactor
  {
    template <typename V>
    VSignal<V>::VSignal(std::string const& name)
      : Super(name)
    {}

    template <typename V>
    bool
    VSignal<V>::emit(V const& val)
    {
      this->_value = val;
      return this->_signal();
    }

    template <typename V>
    bool
    VSignal<V>::emit_one(V const& val)
    {
      this->_value = val;
      return this->_signal_one();
    }
  }
}
