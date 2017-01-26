#include <type_traits>

#define DAS_SYMBOL(Name) DAS_SYMBOL_NAMED(Name, Name)
#define DAS_SYMBOL_TYPE(Name) DAS_SYMBOL_TYPE_NAMED(Name, Name)

#define DAS_SYMBOL_TYPE_NAMED_(Name, CName)                     \
  template <typename S>                                         \
  class _Symbol_##Name                                          \
    : public ::das::SpecificSymbol<S>                           \
  {                                                             \
  public:                                                       \
    using Super = ::das::SpecificSymbol<S>;                     \
    using Super::operator=;                                     \
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
    using attr_type = decltype(std::declval<T>().CName);        \
                                                                \
    template <typename T>                                       \
    static constexpr                                            \
    bool                                                        \
    attr_has()                                                  \
    {                                                           \
      return S::template _attr_has<T>(0);                       \
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
    static constexpr                                            \
    std::enable_if_exists_t<                                    \
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
      return S::template _method_has<T, Args...>(0);            \
    }                                                           \
                                                                \
    template <typename T, typename ... Args>                    \
    using method_type                                           \
      = decltype(std::declval<T&>()                             \
                 .CName(std::declval<Args>()...));              \
                                                                \
    template <typename T, typename ... Args>                    \
    static                                                      \
    auto                                                        \
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
  };                                                            \

#define DAS_SYMBOL_TYPE_NAMED(Name, CName)                      \
  DAS_SYMBOL_TYPE_NAMED_(Name, CName);                          \
  class Symbol_##Name                                           \
    : public _Symbol_##Name<Symbol_##Name>                      \
  {                                                             \
  public:                                                       \
    using Super = _Symbol_##Name<Symbol_##Name>;                \
    using Super::operator =;                                    \
  };                                                            \

#define DAS_SYMBOL_NAMED(Name, CName)                           \
  DAS_SYMBOL_TYPE_NAMED(Name, CName);                           \
  __attribute__((unused))                                       \
  constexpr static Symbol_##Name CName = {};
