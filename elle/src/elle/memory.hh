#ifndef ELLE_MEMORY_HH
# define ELLE_MEMORY_HH

# include <cstddef>
# include <memory>

namespace elle
{
  template<typename T, typename... Args>
  std::unique_ptr<T>
  make_unique(Args&&... args);

  template <typename T>
  class generic_unique_ptr:
    public std::unique_ptr<T, std::function<void (int*)>>
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef generic_unique_ptr<T> Self;
    typedef std::unique_ptr<T, std::function<void (int*)>> Super;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Construct a null pointer.
    constexpr
    generic_unique_ptr();
    /// Construct a null pointer.
    constexpr
    generic_unique_ptr(nullptr_t);
    /// Construct a pointer to \a p.
    explicit
    generic_unique_ptr(typename Self::pointer p);
    /// Construct a pointer to \a p with specific deleter.
    explicit
    generic_unique_ptr(typename Self::pointer p,
                       std::function<void (int*)> const& deleter);
    /// Construct a pointer to \a p with specific deleter.
    template <typename D>
    explicit
    generic_unique_ptr(typename Self::pointer p, D&& deleter);
    /// Construct a pointer that steals ownership from \a p.
    /* implicit */
    generic_unique_ptr(Self&& p);
    /// Construct a pointer that steals ownership from \a p.
    template <typename P, typename D>
    /* implicit */
    generic_unique_ptr(std::unique_ptr<P, D>&& source);
    /// Copy is disabled.
    generic_unique_ptr(Self const& p) = delete;

  /*-----------.
  | Assignment |
  `-----------*/
  public:
    /// Construct a pointer that steals ownership from \a p.
    template <typename P, typename D>
    Self&
    operator = (std::unique_ptr<P, D>&& source);
  };
}

# include <elle/memory.hxx>

#endif
