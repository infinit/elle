#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <typeinfo>

#include <elle/compiler.hh>

// Work around Clang 3.5.0 bug where having this helper in the elle namespace
// will find a << overload for elle::serialization::Serializer::SerializerIn
// (and only that type). If such an overload existed, ADL would find it anyway
// no matter where it's called from.
namespace _elle_print_details
{
  template <typename T>
  constexpr
  std::enable_if_exists_t<
    decltype(std::declval<std::ostream&>() << std::declval<T>()), bool>
  _is_streamable(int)
  {
    return true;
  };

  template <typename T>
  constexpr
  bool
  _is_streamable(unsigned int)
  {
    return false;
  };

  template <typename T>
  constexpr
  bool
  is_streamable()
  {
    return _is_streamable<T>(42);
  };
}

namespace elle
{
  namespace _details
  {
    /// Default print for non-streamable objects.
    void
    default_print(std::ostream& o, std::type_info const& info, void const* p);

    /// Fail because a type has no truth value.
    ELLE_COMPILER_ATTRIBUTE_NORETURN
    void
    err_nonbool(std::type_info const& info);

    /*--------.
    | Helpers |
    `--------*/

    // GCC 4.9, when instantiating this piece of code, generates tons
    // of warnings about our `-> decltype(bool(t))`:
    //
    // print.hxx:57:19: warning: the compiler can assume that the address
    // of 't' will always evaluate to 'true' [-Waddress]
    //
    // The pragma cannot be restristed to that line.
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Waddress"
#endif

    /// Boolean value of types featuring to bool() operator.
    template <typename T>
    auto
    branch_test(T const& t, int)
      -> decltype(bool(t))
    {
      return bool(t);
    }

#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif

    /// A string is "false" iff it's empty.
    ///
    /// We also support dubious derived classes.  See das's
    /// serialization test with NopeString.
    template <typename T>
    auto
    branch_test(T const& t, int)
      -> std::enable_if_t<std::is_base_of<std::string, T>{},
                          bool>
    {
      return !t.empty();
    }

    /// A C-string is "false" iff it's empty.
    inline
    auto
    branch_test(char const* t, int)
    {
      return t && *t;
    }

    /// Cannot not use this type as a Boolean value.
    template <typename T>
    ELLE_COMPILER_ATTRIBUTE_NORETURN
    bool
    branch_test(T const& t, ...)
    {
      _details::err_nonbool(typeid(T));
    }

    /*--------------------------.
    | Print individual elements |
    `--------------------------*/

    template <typename T>
    std::enable_if_t<_elle_print_details::is_streamable<T>(), void>
    print(std::ostream& o, T&& value)
    {
      o << std::forward<T>(value);
    }

    template <typename T>
    std::enable_if_t<!_elle_print_details::is_streamable<T>(), void>
    print(std::ostream& o, T&& value)
    {
      _details::default_print(
        o,
        typeid(T),
        reinterpret_cast<void const*>(&value));
    }

    inline
    void
    print(std::ostream& o, bool value)
    {
      o << (value ? "true" : "false");
    }

    inline
    void
    print(std::ostream& o, char const* value)
    {
      o << (value ? value : "");
    }

    inline
    void
    print(std::ostream& o, char* value)
    {
      o << (value ? value : "");
    }

    template <typename T>
    void
    print(std::ostream& o, std::unique_ptr<T> const& value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    template <typename T>
    void
    print(std::ostream& o, std::unique_ptr<T>& value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    template <typename T>
    void
    print(std::ostream& o, std::shared_ptr<T> const& value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    template <typename T>
    void
    print(std::ostream& o, std::shared_ptr<T>& value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    template <typename T>
    void
    print(std::ostream& o, std::weak_ptr<T> const& value)
    {
      print(o, value.lock());
    }

    inline
    void
    print(std::ostream& o, void* value)
    {
      if (value)
        o << value;
      else
        o << "nullptr";
    }

    template <typename T>
    void
    print(std::ostream& o, T* value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    /*------------.
    | Arguments.  |
    `------------*/

    struct Argument
      : public std::function<auto (std::ostream&) -> void>
    {
      using Super = std::function<auto (std::ostream&) -> void>;
      template <typename T>
      Argument(T const& value)
        : Super{[&value] (std::ostream& o) { print(o, value); }}
        , _bool{[&value] { return branch_test(value, 0); }}
      {}

      operator bool() const
      {
        return this->_bool();
      }

      std::function<bool()> _bool;
    };

    struct NamedArgument
      : public std::pair<std::string, Argument>
    {
      template <class T>
      NamedArgument(std::string const& name, T const& value)
        : std::pair<std::string, Argument>(name, value)
      {}
    };

    struct NamedArguments
      : public std::unordered_map<std::string, Argument>
    {
      NamedArguments(std::initializer_list<NamedArgument> l)
      {
        for (auto& e: l)
          this->emplace(std::move(e));
      }
    };

    void
    print(std::ostream& s,
          std::string const& fmt,
          std::vector<Argument> const& args,
          NamedArguments const& named);

    template <typename ... Args>
    std::vector <Argument>
    erasure(Args const& ... args)
    {
      std::vector<Argument> res;
      res.reserve(sizeof ... (args));
      int unused[] __attribute__((unused)) = {(res.emplace_back(args), 0)...};
      return res;
    }
  }

  /*-----------.
  | Positional |
  `-----------*/

  template <typename ... Args>
  std::ostream&
  print(std::ostream& o, std::string const& fmt, Args&& ... args)
  {
    _details::print(o, fmt, _details::erasure(args...), {});
    return o;
  }

  template <typename ... Args>
  std::string
  print(std::string const& fmt, Args&& ... args)
  {
    std::stringstream output;
    print(output, fmt, std::forward<Args>(args)...);
    return output.str();
  }

  /*------.
  | Named |
  `------*/

  inline
  std::ostream&
  print(std::ostream& o,
        std::string const& fmt,
        _details::NamedArguments const& args)
  {
    _details::print(o, fmt, {}, args);
    return o;
  }

  inline
  std::string
  print(std::string const& fmt,
        _details::NamedArguments const& args)
  {
    std::stringstream output;
    print(output, fmt, args);
    return output.str();
  }
}
