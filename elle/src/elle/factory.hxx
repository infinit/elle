#ifndef ELLE_FACTORY_HXX
# define ELLE_FACTORY_HXX


namespace elle
{
  template<typename T>
  int
  Factory<T>::register_(std::string const& name, Builder builder)
  {
    _items().insert(std::make_pair(name, builder));
    return 0;
  }

  template<typename T>
  std::unique_ptr<T>
  Factory<T>::instantiate(std::string const& name, Arguments const& args)
  {
    auto it = _items().find(name);
    if (it == _items().end())
      throw KeyError(name);
    return it->second(args);
  }
  template<typename T>
  typename Factory<T>::Items& Factory<T>::_items()
  {
    static Items _items;
    return _items;
  }
}

#endif