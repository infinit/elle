namespace elle
{
  template<typename T, typename... Args>
  std::unique_ptr<T>
  make_unique(Args&&... args)
  {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }

  /*-------------.
  | Construction |
  `-------------*/

  template <typename T>
  constexpr
  generic_unique_ptr<T>::generic_unique_ptr():
    Super(nullptr, [] (T* p) { std::default_delete<T>()(p); })
  {}

  template <typename T>
  constexpr
  generic_unique_ptr<T>::generic_unique_ptr(std::nullptr_t p):
    Super(p, [] (T* p) { std::default_delete<T>()(p); })
  {}

  template <typename T>
  generic_unique_ptr<T>::generic_unique_ptr(typename Self::pointer p):
    Super(p, [] (T* p) { std::default_delete<T>()(p); })
  {}

  template <typename T>
  generic_unique_ptr<T>::generic_unique_ptr(
    typename Self::pointer p,
    std::function<void (T*)> const& deleter):
    Super(p, deleter)
  {}

  template <typename T>
  template <typename D>
  generic_unique_ptr<T>::generic_unique_ptr(typename Self::pointer p,
                                            D&& deleter):
    Super(p, [deleter] (T* p) { deleter(p); })
  {}

  template <typename T>
  generic_unique_ptr<T>::generic_unique_ptr(Self&& p):
    Super(p.release(), p.get_deleter())
  {}

  template <typename P, typename D, typename T>
  static
  std::function<void(T*)>
  extract_deleter(std::unique_ptr<P, D> const& source)
  {
    auto d = source.get_deleter();
    return [=] (T* p) { d(p); };
  }

  template <typename T>
  template <typename P, typename D>
  generic_unique_ptr<T>::generic_unique_ptr(std::unique_ptr<P, D>&& source):
    Super(source.release(), extract_deleter<P, D, T>(source))
  {}

  /*-----------.
  | Assignment |
  `-----------*/

  template <typename T>
  template <typename P, typename D>
  generic_unique_ptr<T>&
  generic_unique_ptr<T>::operator = (std::unique_ptr<P, D>&& source)
  {
    this->~Self();
    new (this) Self(std::move(source));
    return *this;
  }

  template <typename T>
  template <typename P>
  generic_unique_ptr<T>&
  generic_unique_ptr<T>::operator = (elle::generic_unique_ptr<P>&& source)
  {
    this->~Self();
    new (this) Self(std::move(source));
    return *this;
  }
}
