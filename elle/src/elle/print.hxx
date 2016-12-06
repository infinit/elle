#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <boost/format.hpp>

#include <elle/assert.hh>
#include <elle/Printable.hh>
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
    std::enable_if_t<std::is_convertible<T, bool>::value, bool>
    branch_test(T const& t)
    {
      return bool(t);
    }

    template <typename T>
    std::enable_if_t<!std::is_convertible<T, bool>::value, bool>
    branch_test(T const& t)
    {
      elle::err("type is not a truth value: %s", elle::type_info<T>());
    }

    /*----.
    | AST |
    `----*/

    class Expression
      : public elle::Printable
    {};

    class Composite
      : public Expression
    {
    public:
      Composite(std::vector<std::shared_ptr<Expression>> expressions = {})
        : expressions(std::move(expressions))
      {}

      void
      print(std::ostream& s) const override
      {
        bool first = true;
        s << "Composite(";
        for (auto const& e: this->expressions)
        {
          if (first)
            first = false;
          else
            s << ", ";
          s << *e;
        }
        s << ")";
      }

      std::vector<std::shared_ptr<Expression>> expressions;
    };

    class Branch
      : public Expression
    {
    public:
      Branch(std::shared_ptr<Expression> cond,
             std::shared_ptr<Expression> then)
        : cond(cond)
        , then(then)
      {}

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Branch(";
        this->cond->print(s);
        s << ", ";
        this->then->print(s);
        s << ")";
      }

      std::shared_ptr<Expression> cond;
      std::shared_ptr<Expression> then;
    };

    class Index
      : public Expression
    {
    public:
      Index(int n)
        : n(n)
      {}

      static
      std::shared_ptr<Index>
      make(int n)
      {
        return std::make_shared<Index>(n);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Index(" << n << ")";
      }

      int n;
    };

    class Next
      : public Expression
    {
    public:
      Next()
      {}

      static
      std::shared_ptr<Next>
      make()
      {
        return std::make_shared<Next>();
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Next()";
      }
    };

    class Name
      : public Expression
    {
    public:
      Name(std::string n)
        : n(n)
      {}

      static
      std::shared_ptr<Name>
      make(std::string& n)
      {
        return std::make_shared<Name>(n);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Name(" << n << ")";
      }

      std::string n;
    };

    class Literal
      : public Expression
    {
    public:
      Literal(std::string text)
        : text(text)
      {}

      static
      std::shared_ptr<Literal>
      make(std::string& text)
      {
        return std::make_shared<Literal>(text);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Literal(" << text << ")";
      }

      std::string text;
    };

    /*--------------------------.
    | Print individual elements |
    `--------------------------*/

    template <typename T>
    static
    std::enable_if_t<_elle_print_details::is_streamable<T>(), void>
    print(std::ostream& o, T&& value)
    {
      o << std::forward<T>(value);
    }

    template <typename T>
    static
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

    /*----------------.
    | Named arguments |
    `----------------*/

    struct Argument
      : public std::function<void (std::ostream&)>
    {
      template <typename T>
      Argument(T const& value)
        : std::function<void (std::ostream&)>(
          [&] (std::ostream& o) { print(o, value); })
        , _bool([&] { return branch_test(value); })
      {}

      Argument(char const* value)
        : std::function<void (std::ostream&)>(
          [&] (std::ostream& o) { print(o, value); })
        , _bool([&] { return value && strlen(value) > 0; })
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
          this->emplace(e);
      }
    };

    /*------.
    | Print |
    `------*/

    template <typename R = void,
              int I = 0,
              typename F,
              typename ... Tuple,
              typename ... Args>
    typename std::enable_if<I == sizeof ... (Tuple), R>::type
    tuple_call(int n,
               F const& f,
               std::tuple<Tuple...> const& t,
               Args&& ... args);

    template <typename R,
              int I = 0,
              typename F,
              typename ... Tuple,
              typename ... Args>
    typename std::enable_if<I < sizeof ... (Tuple), R>::type
    tuple_call(int n,
               F const& f,
               std::tuple<Tuple...> const& t,
               Args&& ... args)
    {
      if (n == I)
        return f(std::get<I>(t), std::forward<Args>(args) ...);
      else
        return tuple_call<R, I + 1>(n, f, t, std::forward<Args>(args) ...);
    }

    template <typename R = void,
              int I = 0,
              typename F,
              typename ... Tuple,
              typename ... Args>
    ELLE_COMPILER_ATTRIBUTE_NORETURN
    typename std::enable_if<I == sizeof ... (Tuple), R>::type
    tuple_call(int n,
               F const&,
               std::tuple<Tuple...> const&,
               Args&& ...)
    {
      elle::err(
        "too few arguments for format: %s, expected at least %s", I, n);
    }

    template <int I = 0,
              typename F,
              typename Head,
              typename ... Tuple,
              typename ... Args>
    typename std::result_of<F(Head, Args ...)>::type
    tuple_call(int n,
               F const& f,
               std::tuple<Head, Tuple...> const& t,
               Args&& ... args)
    {
      return tuple_call<typename std::result_of<F(Head, Args ...)>::type, I>(
        n, f, t, std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void
    print(std::ostream& s,
          Expression const& ast,
          std::tuple<Args...> const& args,
          int& count,
          bool p,
          NamedArguments const& named = {})
    {
      auto const printer =
        [] (auto const& v, std::ostream& s) { print(s, v); };
      auto* id = &typeid(ast);
      if (id == &typeid(Composite))
        for (auto const& e:static_cast<Composite const&>(ast).expressions)
          print(s, *e, args, count, p, named);
      else if (id == &typeid(Literal))
      {
        if (p)
          s.write(static_cast<Literal const&>(ast).text.c_str(),
                  static_cast<Literal const&>(ast).text.size());
      }
      else if (id == &typeid(Next))
      {
        if (p)
          tuple_call(count, printer, args, s);
        ++count;
      }
      else if (id == &typeid(Index))
      {
        if (p)
          tuple_call(static_cast<Index const&>(ast).n, printer, args, s);
      }
      else if (id == &typeid(Name))
      {
        auto const& name = static_cast<Name const&>(ast).n;
        auto it = named.find(name);
        if (it != named.end())
        {
          if (p)
            it->second(s);
        }
        else
          elle::err("missing named format argument: %s", name);
      }
      else if (id == &typeid(Branch))
      {
        static auto const tester =
          [] (auto const& v) { return branch_test(v); };
        auto const& branch = static_cast<Branch const&>(ast);
        auto* cid = &typeid(*branch.cond);
        if (cid == &typeid(Next))
          p = p && tuple_call<bool>(count++, tester, args);
        else if (cid == &typeid(Index))
          p = p && tuple_call<bool>(
            static_cast<Index const&>(*branch.cond).n, tester, args);
        else if (cid == &typeid(Name))
        {
          auto const& name = static_cast<Name const&>(*branch.cond).n;
          auto it = named.find(name);
          if (it != named.end())
            p = bool(it->second);
          else
            elle::err("missing named format argument: %s", name);
        }
        else
          elle::err("unexpected condition: %s", elle::type_info(*branch.cond));
        print(s, *branch.then, args, count, p, named);
      }
    }

    template <typename ... Args>
    void
    print(std::ostream& s,
          Expression const& ast,
          NamedArguments const& args)
    {
      int count = 0;
      print(s, ast, std::make_tuple(), count, true, args);
    }

    template <typename ... Args>
    void
    print(std::ostream& s,
          Expression const& ast,
          std::tuple<Args...> const& args)
    {
      int count = 0;
      print(s, ast, args, count, true);
    }

    std::shared_ptr<Expression>
    parse(std::string const& input);
  }

  inline
  void
  print(std::ostream& o,
        std::string const& fmt,
        _details::NamedArguments const& args)
  {
    auto const ast = _details::parse(fmt);
    _details::print(o, *ast, args);
  }

  template <typename ... Args>
  void
  print(std::ostream& o, std::string const& fmt, Args&& ... args)
  {
    auto const ast = _details::parse(fmt);
    _details::print(o, *ast, std::make_tuple(std::forward<Args>(args)...));
  }

  template <typename ... Args>
  std::string
  print(std::string const& fmt, Args&& ... args)
  {
    std::stringstream output;
    print(output, fmt, std::forward<Args>(args)...);
    return output.str();
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
