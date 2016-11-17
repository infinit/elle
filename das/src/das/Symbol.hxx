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
                                                                \
  /* Method */                                                  \
                                                                \
  public:                                                       \
    template <typename T, typename ... Args>                    \
    static constexpr                                            \
    bool                                                        \
    method_has()                                                \
    {                                                           \
      return Symbol_##Name::_method_has<T, Args...>(0);         \
    }                                                           \
                                                                \
    template <typename T, typename ... Args>                    \
    struct method_type                                          \
    {                                                           \
      static_assert(method_has<T, Args...>(),                   \
                    "no such method");                          \
      using type = decltype(std::declval<T&>().Name(            \
                              std::declval<Args>()...));        \
    };                                                          \
                                                                \
    template <typename T, typename ... Args>                    \
    static                                                      \
    typename method_type<T, Args...>::type                      \
    method_call(T&& o, Args&& ... args)                         \
    {                                                           \
      return o.Name(std::forward<Args>(args)...);               \
    }                                                           \
                                                                \
  private:                                                      \
    template <typename T, typename ... Args>                    \
    static constexpr                                            \
      std::enable_if_exists_t<                                  \
        decltype(std::declval<T&>().Name(                       \
                 std::declval<Args>()...)), bool>               \
    _method_has(int)                                            \
    {                                                           \
      return true;                                              \
    }                                                           \
                                                                \
    template <typename T, typename ... Args>                    \
    static constexpr                                            \
    bool                                                        \
    _method_has(...)                                            \
    {                                                           \
      return false;                                             \
    }                                                           \
  } Name;                                                       \

#endif
