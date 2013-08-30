#ifndef ELLE_MEMORY_HXX
# define ELLE_MEMORY_HXX

namespace elle
{
  template<typename T, typename... Args>
  std::unique_ptr<T>
  make_unique(Args&&... args)
  {
    return std::unique_ptr<T>{new T{std::forward<Args>(args)...}};
  }

  /*-------------.
  | Construction |
  `-------------*/

  template <typename T>
  constexpr
  generic_unique_ptr<T>::generic_unique_ptr():
    Super()
  {}

  template <typename T>
  constexpr
  generic_unique_ptr<T>::generic_unique_ptr(nullptr_t p):
    Super(p)
  {}

  template <typename T>
  generic_unique_ptr<T>::generic_unique_ptr(typename Self::pointer p):
    Super(p, [] (T* p) { std::default_delete<T>()(p); })
  {}

  template <typename T>
  generic_unique_ptr<T>::generic_unique_ptr(
    typename Self::pointer p,
    std::function<void (int*)> const& deleter):
    Super(p, deleter)
  {}

  template <typename T>
  template <typename D>
  generic_unique_ptr<T>::generic_unique_ptr(typename Self::pointer p,
                                            D&& deleter):
    Super(p, [d = std::forward<D>(deleter)] (T* p) { d(p); })
  {}

  template <typename T>
  generic_unique_ptr<T>::generic_unique_ptr(Self&& p):
    Super(p.release(), p.get_deleter())
  {}

  template <typename T>
  template <typename P, typename D>
  generic_unique_ptr<T>::generic_unique_ptr(std::unique_ptr<P, D>&& source):
    Super(source.release(), [d = source.get_deleter()] (T* p) { d(p); })
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
}

#endif
