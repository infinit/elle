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
    VSignal<V>::Emit(V const& val)
    {
      _value = val;
      return _signal();
    }

    template <typename V>
    bool
    VSignal<V>::EmitOne(V const& val)
    {
      _value = val;
      return _signal_one();
    }

    template <typename V>
    V const&
    VSignal<V>::Value() const
    {
      return _value;
    }
  }
}
