#ifndef ELLE_OPTION_HH
# define ELLE_OPTION_HH

# include <cstddef>
# include <type_traits>

namespace elle
{
  namespace _details
  {
    template <std::size_t Size, int Index, typename ... Types>
    class OptionHelper;
  }

  template <typename ... Types>
  class Option
    : public _details::OptionHelper<0u, 0, Types ...>
  {
  public:
    typedef Option<Types ...> Self;
    typedef _details::OptionHelper<0u, 0, Types ...> Super;
    using Super::Super;
    template <typename T>
    Option(T&& value);
    Option(Self const& source);
    Option(Self& source);
    Option(Self&& source);
    Self&
    operator =(Self&& option);
    Self&
    operator =(Self const& option);

    template <typename T>
    T&
    get();
    template <typename T>
    bool
    is() const;
    template <typename T>
    void
    emplace(T&& value);
    template <typename T, typename ... Args>
    void
    emplace(Args&& ... value);
  };
}

# include <elle/Option.hxx>

#endif
