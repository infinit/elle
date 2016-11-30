#ifndef DAS_SYMBOL_HXX
# define DAS_SYMBOL_HXX

# include <type_traits>

# define DAS_SYMBOL(Name) DAS_SYMBOL_NAMED(Name, Name)

# define DAS_SYMBOL_NAMED(Name, CName)                          \
  __attribute__((unused))                                       \
  constexpr static                                              \
  class Symbol_##Name                                           \
    : public ::das::SpecificSymbol<Symbol_##Name>               \
  {                                                             \
  public:                                                       \
    using ::das::SpecificSymbol<Symbol_##Name>::operator=;      \
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
      using type = decltype(std::declval<T>().CName);           \
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
      -> decltype(o.CName) const&                               \
    {                                                           \
      return o.CName;                                           \
    }                                                           \
                                                                \
    template <typename T>                                       \
    static                                                      \
    auto                                                        \
    attr_get(T& o)                                              \
      -> decltype(o.CName)&                                     \
    {                                                           \
      return o.CName;                                           \
    }                                                           \
                                                                \
  private:                                                      \
    template <typename T>                                       \
    static constexpr std::enable_if_exists_t<                   \
      decltype(std::declval<T&>().CName), bool>                 \
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
      using type = decltype(std::declval<T&>().CName(           \
                              std::declval<Args>()...));        \
    };                                                          \
                                                                \
    template <typename T, typename ... Args>                    \
    static                                                      \
    typename method_type<T, Args...>::type                      \
    method_call(T&& o, Args&& ... args)                         \
    {                                                           \
      return o.CName(std::forward<Args>(args)...);              \
    }                                                           \
                                                                \
  private:                                                      \
    template <typename T, typename ... Args>                    \
    static constexpr                                            \
      std::enable_if_exists_t<                                  \
        decltype(std::declval<T&>().CName(                      \
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
  } CName = {};                                                 \

#endif
