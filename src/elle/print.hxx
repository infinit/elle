#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <boost/format.hpp>

#include <elle/TypeInfo.hh>
#include <elle/err.hh>

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
    /*--------.
    | Helpers |
    `--------*/

    template <typename T>
    std::enable_if_exists_t<decltype(bool(std::declval<T>())), bool>
    branch_test(T const& t, int)
    {
      return bool(t);
    }

    template <typename T>
    ELLE_COMPILER_ATTRIBUTE_NORETURN
    bool
    branch_test(T const& t, ...)
    {
      elle::err(elle::print("type is not a truth value: {}",
                            elle::type_info<T>()));
    }

    /*--------------------------.
    | Print individual elements |
    `--------------------------*/

    template <typename T>
    inline
    std::enable_if_t<_elle_print_details::is_streamable<T>(), void>
    print(std::ostream& o, T&& value)
    {
      o << std::forward<T>(value);
    }

    template <typename T>
    inline
    std::enable_if_t<!_elle_print_details::is_streamable<T>(), void>
    print(std::ostream& o, T&& value)
    {
      static boost::format parsed("%f(%x)");
      boost::format format(parsed);
      format % elle::type_info(value);
      format % reinterpret_cast<const void*>(&value);
      o << format;
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
    inline
    void
    print(std::ostream& o, std::unique_ptr<T> const& value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    template <typename T>
    inline
    void
    print(std::ostream& o, std::unique_ptr<T>& value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    template <typename T>
    inline
    void
    print(std::ostream& o, std::shared_ptr<T> const& value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    template <typename T>
    inline
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
    inline
    void
    print(std::ostream& o, T* value)
    {
      if (value)
        print(o, *value);
      else
        o << "nullptr";
    }

    /*----------------.
    | Named arguments |
    `----------------*/

    struct Argument
      : public std::function<void (std::ostream&)>
    {
      template <typename T>
      Argument(T const& value)
        : std::function<void (std::ostream&)>(
          [&value] (std::ostream& o) { print(o, value); })
        , _bool([&value] { return branch_test(value, 0); })
      {}

      Argument(char const* value)
        : std::function<void (std::ostream&)>(
          [value] (std::ostream& o) { print(o, value); })
        , _bool([value] { return value && strlen(value) > 0; })
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
  void
  print(std::ostream& o, std::string const& fmt, Args&& ... args)
  {
    _details::print(o, fmt, _details::erasure(args...), {});
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
  void
  print(std::ostream& o,
        std::string const& fmt,
        _details::NamedArguments const& args)
  {
    _details::print(o, fmt, {}, args);
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
