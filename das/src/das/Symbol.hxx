#ifndef DAS_SYMBOL_HXX
# define DAS_SYMBOL_HXX

# include <type_traits>

# define DAS_SYMBOL(Name)                                       \
  constexpr                                                     \
  class Symbol_##Name                                           \
    : public ::das::Symbol                                      \
  {                                                             \
  public:                                                       \
    /* Clang requires a user-defined   */                       \
    /* default constructor for default */                       \
    /* initialization. */                                       \
    constexpr                                                   \
    Symbol_##Name()                                             \
    {}                                                          \
                                                                \
    /* Name */                                                  \
                                                                \
    static std::string name()                                   \
    {                                                           \
      return #Name;                                             \
    }                                                           \
                                                                \
    /* Attribute */                                             \
                                                                \
  public:                                                       \
    template <typename T>                                       \
    struct                                                      \
    attr_type                                                   \
    {                                                           \
      using type = decltype(std::declval<T>().Name);            \
    };                                                          \
                                                                \
    template <typename T>                                       \
    static                                                      \
    bool                                                        \
    attr_has()                                                  \
    {                                                           \
      return Symbol_##Name::_attr_has<T>(0);                    \
    }                                                           \
                                                                \
    template <typename T>                                       \
    static                                                      \
    auto                                                        \
    attr_get(T const& o)                                        \
      -> decltype(o.Name) const&                                \
    {                                                           \
      return o.Name;                                            \
    }                                                           \
                                                                \
    template <typename T>                                       \
    static                                                      \
    auto                                                        \
    attr_get(T& o)                                              \
      -> decltype(o.Name)&                                      \
    {                                                           \
      return o.Name;                                            \
    }                                                           \
                                                                \
  private:                                                      \
    template <typename T>                                       \
    static constexpr std::enable_if_exists_t<                   \
      decltype(std::declval<T&>().Name), bool>                  \
    _attr_has(int)                                              \
    {                                                           \
      return true;                                              \
    }                                                           \
                                                                \
    template <typename T>                                       \
    static constexpr                                            \
    bool                                                        \
    _attr_has(...)                                              \
    {                                                           \
      return false;                                             \
    }                                                           \
  } Name;                                                       \

#endif
