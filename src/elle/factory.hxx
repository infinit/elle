#pragma once

namespace elle
{
  template <typename T>
  int
  Factory<T>::register_(std::string const& name, Builder builder)
  {
    Factory<T>::_items().insert(std::make_pair(name, builder));
    return 0;
  }

  template <typename T>
  std::unique_ptr<T>
  Factory<T>::instantiate(std::string const& name, Arguments const& args)
  {
    auto it = Factory<T>::_items().find(name);
    if (it == Factory<T>::_items().end())
      throw KeyError(name);
    return it->second(args);
  }

  template <typename T>
  typename Factory<T>::Items&
  Factory<T>::_items()
  {
    static Items items;
    return items;
  }
}
