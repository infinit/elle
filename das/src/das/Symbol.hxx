#ifndef DAS_SYMBOL_HXX
# define DAS_SYMBOL_HXX

# include <type_traits>

# define DAS_SYMBOL(Name)                       \
  constexpr                                     \
  class Symbol_##Name                           \
    : public ::das::Symbol                      \
  {                                             \
  public:                                       \
    template <typename T>                       \
    static                                      \
    auto                                        \
    attr_get(T const& o)                        \
      -> decltype(o.Name) const&                \
    {                                           \
      return o.Name;                            \
    }                                           \
                                                \
    template <typename T>                       \
    static                                      \
    auto                                        \
    attr_get(T& o)                              \
      -> decltype(o.Name)&                      \
    {                                           \
      return o.Name;                            \
    }                                           \
                                                \
    static std::string name()                   \
    {                                           \
      return #Name;                             \
    }                                           \
  } Name;                                       \

#endif
