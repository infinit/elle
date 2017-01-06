#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>

#include <elle/Error.hh>
#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/serialization/json.hh>

#include <das/named.hh>

namespace das
{
  namespace cli
  {
    /// Command line error
    class Error
      : public elle::Error
    {
    public:
      using elle::Error::Error;
    };

    /// Error on a specific option
    class OptionError
      : public virtual Error
    {
    public:
      OptionError(std::string option)
        : Error("") // Never called (virtual)
        , _option(std::move(option))
      {}

      /// The erroneous option
      ELLE_ATTRIBUTE_R(std::string, option);
    };

    /// Error on a value
    class ValueError
      : public virtual Error
    {
    public:
      ValueError(std::string value)
        : Error("") // Never called (virtual)
        , _value(std::move(value))
      {}

      /// The erroneous option
      ELLE_ATTRIBUTE_R(std::string, value);
    };

    /// Error on an option value
    class OptionValueError
      : public OptionError
      , public ValueError
    {
    public:
      OptionValueError(std::string option,
                       std::string value,
                       std::string message)
        : Error(elle::sprintf("invalid value \"%s\" for option %s: %s",
                              value, option, message))
        , OptionError(option)
        , ValueError(value)
      {}

      /// The erroneous option
      ELLE_ATTRIBUTE_R(std::string, value);
    };

#define DAS_CLI_OPTION_ERROR(Name, Description)                         \
    class Name ## Option                                                \
      : public OptionError                                              \
    {                                                                   \
    public:                                                             \
      Name ## Option(std::string option)                                \
        : Error(elle::sprintf(Description ": %s", option))              \
        , OptionError(option)                                           \
      {}                                                                \
    }                                                                   \

#define DAS_CLI_VALUE_ERROR(Name, Description)                          \
    class Name ## Value                                                 \
      : public ValueError                                               \
    {                                                                   \
    public:                                                             \
      Name ## Value(std::string value)                                  \
        : Error(elle::sprintf(Description ": %s", value))               \
        , ValueError(value)                                             \
      {}                                                                \
    }                                                                   \

    /// Unrecognized option
    DAS_CLI_OPTION_ERROR(Unknown, "unknown option");
    /// Missing mandatory option
    DAS_CLI_OPTION_ERROR(Missing, "missing option");
    /// Duplicate option
    DAS_CLI_OPTION_ERROR(Duplicate, "duplicate option");
    /// Option used as a flag and passed arguments
    DAS_CLI_OPTION_ERROR(
      Mixed, "option can't be used both as a flag flag and with arguments");
    /// Unrecognized letfover value
    DAS_CLI_VALUE_ERROR(Unrecognized, "extra unrecognized argument");

#undef DAS_CLI_OPTION_ERROR
#undef DAS_CLI_VALUE_ERROR

    /// Base tag for cli symbols
    class CLI_Symbol
    {};

    struct Option
    {
      Option(char short_name = 0,
             std::string help = "",
             bool positional = false)
        : short_name(short_name)
        , help(std::move(help))
        , positional(positional)
      {}

      char short_name;
      std::string help;
      bool positional;
    };

    using Options = std::unordered_map<std::string, Option>;

    inline
    std::string
    option_name_from_c(std::string n)
    {
      std::replace(n.begin(), n.end(), '_', '-');
      return n;
    }

    namespace _details
    {
      class IsOption
      {
      public:
        IsOption(std::string a, Options const& opts)
          : _option(a.size() > 2 && a[0] == '-' && a[1] == '-'
                    || a.size() == 2 && a[0] == '-' && std::isalpha(a[1]))
          , _arg(std::move(a))
        {}

        template <typename T>
        bool
        is(Options const& opts)
        {
          if (this->_option)
          {
            if (this->_arg[0] == '-' && this->_arg[1] == '-')
              return this->_arg.substr(2) == option_name_from_c(T::name());
            else
            {
              using Formal = typename das::named::make_formal<T>::type;
              auto res =
                elle::meta::static_if<std::is_base_of<CLI_Symbol, Formal>::value>(
                [this] (auto&& formal)
                {
                  return this->_arg[1] == formal.short_name();
                },
                [](auto&&) { return false; })(Formal{});
              {
                auto it = opts.find(T::name());
                if (it != opts.end())
                  res = this->_arg[1] == it->second.short_name;
              }
              return res;
            }
          }
          else
            return false;
        }

        operator bool()
        {
          return this->_option;
        }

        ELLE_ATTRIBUTE(bool, option);
        ELLE_ATTRIBUTE(std::string, arg);
      };
    }

    static inline
    _details::IsOption
    is_option(std::string const& a, Options const& opts = Options())
    {
      return _details::IsOption(a, opts);
    }

    namespace _details
    {
      struct Empty
      {
        Empty(int const&)
        {}
      };

      template <typename Formal, typename Default = void>
      class Value
        : public std::conditional_t<
            std::is_same<Default, void>::value,
            Empty,
            Default>
      {
      public:
        using Super = std::conditional_t<
          std::is_same<Default, void>::value, Empty, Default>;
        static bool constexpr default_has = !std::is_same<Default, void>::value;

        Value(std::conditional_t<default_has, Default, int> const& d,
              std::string option,
              bool positional,
              std::vector<std::string>& args,
              std::vector<std::string> value,
              int& remaining)
          : Super (d)
          , _option(std::move(option))
          , _values(std::move(value))
          , _positional(positional)
          , _args(args)
          , _flag(false)
          , _remaining(remaining)
        {}

        Value(std::conditional_t<default_has, Default, int> const& d,
              std::string option,
              bool positional,
              std::vector<std::string>& args,
              bool flag,
              int& remaining)
          : Super(d)
          , _option(std::move(option))
          , _values()
          , _positional(positional)
          , _args(args)
          , _flag(flag)
          , _remaining(remaining)
        {}

        template <typename I>
        static
        std::enable_if_t<std::is_signed<I>::value, I>
        to_int(std::string const& v, std::string const& option)
        {
          std::size_t end;
          auto i = std::stoll(v, &end);
          if (end != v.size())
            throw OptionValueError(option, v, "invalid integer");
          if (i > std::numeric_limits<I>::max() ||
              i < std::numeric_limits<I>::min())
            throw OptionValueError(option, v, "integer out of range");
          return i;
        }

        template <typename I>
        static
        std::enable_if_t<!std::is_signed<I>::value, I>
        to_int(std::string const& v, std::string const& option)
        {
          // Beware: `std::stoull` underflows instead of throwing out_of_range,
          // which is UTTER BULLSHIT. Check manually for negative numbers.
          for (auto c: v)
          {
            if (c == '-')
              throw OptionValueError(option, v, "integer out of range");
            // "Discards any whitespace characters (as identified by calling
            // isspace()) until the first non-whitespace character is found"
            if (!std::isspace(c))
              break;
          }
          std::size_t end;
          auto i = std::stoull(v, &end);
          if (end != v.size())
            throw OptionValueError(option, v, "invalid integer");
          if (i > std::numeric_limits<I>::max())
            throw OptionValueError(option, v, "integer out of range");
          return i;
        }

        template <typename I>
        std::enable_if_t<std::is_same<I, bool>::value, I>
        convert(std::string const& v, int) const
        {
          if (v == "true")
            return true;
          else if (v == "false")
            return false;
          throw OptionValueError(this->_option, v, "invalid boolean ");
        }

        template <typename I>
        std::enable_if_t<std::is_same<I, std::string>::value, I>
        convert(std::string const& v, int) const
        {
          return v;
        }

        template <typename I>
        std::enable_if_t<std::is_base_of<boost::optional_detail::optional_tag, I>::value, I>
        convert(std::string const& v, int) const
        {
          if (this->_values.empty())
            return boost::none;
          if (this->_values.size() > 1)
            throw DuplicateOption(this->_option);
          return convert<typename I::value_type>(this->_values[0], 0);
        }

        template <typename I>
        std::enable_if_t<
          std::is_integral<I>::value && !std::is_same<I, bool>::value, I>
        convert(std::string const& v, int) const
        {
          try
          {
            return to_int<I>(v, this->_option);
          }
          catch (std::invalid_argument)
          {
            throw OptionValueError(
              this->_option, v, "invalid integer");
          }
          catch (std::out_of_range)
          {
            throw OptionValueError(
              this->_option, v, "integer out of range");
          }
        }

        template <typename I>
        I
        convert(std::string const& v, ...) const
        {
          elle::serialization::json::SerializerIn s(elle::json::Json(v), false);
          return s.deserialize<I>();
        }

        template <typename T>
        std::enable_if_t<!default_has, T>
        missing() const
        {
          ELLE_LOG_COMPONENT("das.cli");
          ELLE_TRACE("raise missing error");
          throw MissingOption(this->_option);
        }

        template <typename T>
        std::enable_if_t<default_has, T>
        missing() const
        {
          ELLE_LOG_COMPONENT("das.cli");
          ELLE_TRACE("use default value: %s", this->Default::value);
          return this->Default::value;
        }

        template <typename I>
        I
        convert() const
        {
          ELLE_LOG_COMPONENT("das.cli");
          if (this->_values.empty())
          {
            if (this->_positional && !this->_args.empty())
            {
              ELLE_TRACE("use next positional value: %s", *this->_args.begin());
              this->_values.emplace_back(std::move(*this->_args.begin()));
              this->_args.erase(this->_args.begin());
            }
            else
              return this->missing<I>();
          }
          if (this->_values.size() > 1)
            throw DuplicateOption(this->_option);
          return convert<I>(this->_values[0], 0);
        }

        template <typename I>
        operator I() const
        {
          static_assert(!std::is_same<std::decay_t<I>, bool>::value, "");
          ELLE_LOG_COMPONENT("das.cli");
          ELLE_TRACE_SCOPE(
            "convert %s to %s", this->_option, elle::type_info<I>());
          auto res = this->convert<I>();
          this->_check_remaining();
          return res;
        }

        operator bool() const
        {
          ELLE_LOG_COMPONENT("das.cli");
          ELLE_TRACE_SCOPE("convert %s to boolean", this->_option);
          bool res;
          if (this->_values.empty())
            res = this->_flag;
          else
            res = this->convert<bool>();
          this->_check_remaining();
          return res;
        }

        template <typename T>
        operator std::vector<T>() const
        {
          ELLE_LOG_COMPONENT("das.cli");
          std::vector<T> res;
          ELLE_TRACE_SCOPE(
            "convert %s to %s", this->_option, elle::type_info(res));
          if (this->_values.empty() && this->_positional)
          {
            std::move(this->_args.begin(), this->_args.end(),
                      std::back_inserter(this->_values));
            this->_args.clear();
          }
          for_each(
            this->_values.begin(), this->_values.end(),
            [&] (std::string const& v) { res.emplace_back(convert<T>(v, 0)); });
          this->_check_remaining();
          return res;
        }

        void
        _check_remaining() const
        {
          if (!--this->_remaining)
            if (!this->_args.empty())
            {
              if (is_option(*this->_args.begin()))
                throw UnknownOption(*this->_args.begin());
              else
                throw UnrecognizedValue(*this->_args.begin());
            }
        }

      private:
        ELLE_ATTRIBUTE_R(std::string, option);
        ELLE_ATTRIBUTE_R(std::vector<std::string>, values, mutable);
        ELLE_ATTRIBUTE(bool, positional);
        ELLE_ATTRIBUTE(std::vector<std::string>&, args);
        ELLE_ATTRIBUTE(bool, flag);
        ELLE_ATTRIBUTE(int&, remaining);
      };

      template <typename Formal, typename Default>
      std::ostream&
      operator <<(std::ostream& out, Value<Formal, Default> const& v)
      {
        elle::fprintf(out, "Value(\"%s\", %s)", v.option(), v.values());
        return out;
      }
    }

    namespace _details
    {
      template <typename ... Formals>
      struct CLI;

      template <>
      struct CLI<>
      {
        template <typename F,
                  typename D,
                  typename ... Formals,
                  typename ... Parsed>
        static
        auto
        value(das::named::Prototype<D, Formals...> const& p,
              F const& f,
              std::tuple<Parsed...> parsed,
              std::vector<std::string>& args,
              Options const& opts,
              int&)
          -> decltype(forward_tuple(f, std::move(parsed)))
        {
          ELLE_LOG_COMPONENT("das.cli");
          if (!args.empty())
            ELLE_TRACE("remaining positional arguments: %s", args);
          ELLE_TRACE("call %s%s", f, parsed)
            return forward_tuple(f, std::move(parsed));
        }
      };

      template <typename Head, typename ... Tail>
      struct CLI<Head, Tail...>
      {
        template <typename F,
                  typename D,
                  typename ... Formals,
                  typename ... Parsed>
        static
        auto
        value(das::named::Prototype<D, Formals...> const& p,
              F const& f,
              std::tuple<Parsed...> parsed,
              std::vector<std::string>& args,
              Options const& opts,
              int& counter)
          -> decltype(CLI<Tail...>::value(
            p, f,
            std::tuple_cat(
              std::move(parsed),
              std::declval<std::tuple<
                _details::Value<
                  Head,
                  std::conditional_t<
                    D::template default_for<
                      typename das::named::make_formal<Head>::type>::has,
                    typename D::template default_for<
                    typename das::named::make_formal<Head>::type>::type,
                    void>>>>()),
            args,
            opts,
            counter))
        {
          ELLE_LOG_COMPONENT("das.cli");
          using Formal = typename das::named::make_formal<Head>::type;
          bool flag = false;
          bool pos = false;
          std::vector<std::string> value;
          auto v = [&]
            {
              ELLE_TRACE_SCOPE("parsing option %s", Head::name());
              auto it = args.begin();
              while (it != args.end())
              {
                if (auto o = is_option(*it, opts))
                {
                  if (o.is<Head>(opts))
                  {
                    std::string const option = *it;
                    it = args.erase(it);
                    if (it != args.end() && !is_option(*it, opts))
                    {
                      ELLE_DEBUG(
                        "found \"%s\" on the command line with argument \"%s\"",
                        option, *it);
                      if (flag)
                        throw MixedOption(Head::name());
                      value.emplace_back(*it);
                      it = args.erase(it);
                    }
                    else
                    {
                      ELLE_DEBUG("found \"%s\" on the command line", *it);
                      if (!value.empty())
                        throw MixedOption(Head::name());
                      if (flag)
                        throw DuplicateOption(Head::name());
                      flag = true;
                    }
                  }
                  else
                  {
                    // Skip option and potential argument
                    ++it;
                    if (it != args.end() && !is_option(*it, opts))
                      ++it;
                  }
                }
                else
                  ++it;
              }
              elle::meta::static_if<std::is_base_of<CLI_Symbol, Formal>::value>(
                [] (auto& pos, auto t)
                {
                  pos = decltype(t)::type::positional();
                })(pos, elle::meta::Identity<Formal>());
              {
                auto it = opts.find(Head::name());
                if (it != opts.end())
                  pos = it->second.positional;
              }
              static bool constexpr default_has =
                D::template default_for<Formal>::has;
              using Default = std::conditional_t<
                default_has,
                typename D::template default_for<Formal >::type,
                void>;
              return elle::meta::static_if<default_has>(
                [&] (auto head, auto def)
                {
                  using Default = typename decltype(def)::type;
                  using V = Value<typename decltype(head)::type, Default>;
                  if (flag)
                    return V(p.defaults, Head::name(), pos, args,
                             flag, counter);
                  else
                  {
                    if (value.empty())
                    {
                      Default const& couille = p.defaults;
                      ELLE_DEBUG(
                        "no occurences, default value is %s", couille.value);
                    }
                    return V(p.defaults, Head::name(), pos, args,
                             std::move(value), counter);
                  }
                },
                [&] (auto head, auto)
                {
                  using V = Value<typename decltype(head)::type, void>;
                  if (flag)
                    return V(0, Head::name(), pos, args, flag, counter);
                  else
                  {
                    if (value.empty())
                      ELLE_DEBUG("no occurences and no default value");
                    return V(0, Head::name(), pos, args,
                             std::move(value), counter);
                  }
                })(elle::meta::Identity<Head>{},
                   elle::meta::Identity<Default>{});
            }();
          return CLI<Tail...>::value(
            p,
            f,
            std::tuple_cat(std::move(parsed), std::make_tuple(std::move(v))),
            args,
            opts,
            counter);
        }
      };
    }

    template <typename F, typename D, typename ... Formals>
    auto
    _call(das::named::Prototype<D, Formals...> const& p,
          F const& f,
          std::vector<std::string>& args,
          Options const& opts = Options())
      -> decltype(
        _details::CLI<Formals...>::value(
          p, f, std::tuple<>(), args, opts, std::declval<int&>()))
    {
      int counter = sizeof ... (Formals);
      return _details::CLI<Formals...>::value(
        p, f, std::tuple<>(), args, opts, counter);
    }

    template <typename F, typename D, typename ... Formals>
    auto
    call(das::named::Prototype<D, Formals...> const& p,
         F const& f,
         std::vector<std::string>& args,
         Options const& opts = Options())
      -> decltype(_call(p, f, args, opts))
    {
      return _call(p, f, args, opts);
    }

    template <typename F, typename D, typename ... Formals, typename ... Args>
    auto
    call(das::named::Prototype<D, Formals...> const& p,
         F const& f,
         std::vector<std::string> const& args,
         Options const& opts = Options())
      -> decltype(
        _call(p, f, std::declval<std::vector<std::string>&>(), opts))
    {
      std::vector<std::string> copy = args;
      return _call(p, f, copy, opts);
    }

    template <typename ... T>
    auto
    call(named::Function<T...> const& f,
         std::vector<std::string>& args,
         Options const& opts = Options())
      -> decltype(_call(f.prototype(), f, args, opts))
    {
      return _call(f.prototype(), f, args, opts);
    }

    template <typename ... T>
    auto
    call(named::Function<T...> const& f,
         std::vector<std::string> const& args,
         Options const& opts = Options())
    {
      std::vector<std::string> copy = args;
      return _call(f.prototype(), f, copy, opts);
    }

    inline
    void
    print_help(std::ostream& s,
               std::string const& name,
               char short_name,
               std::string const& help)
    {
      if (short_name)
        elle::fprintf(s, "  -%s, ", short_name);
      else
        elle::fprintf(s, "      ");
      elle::fprintf(s, "--%-15s  %s", das::cli::option_name_from_c(name), help);
    }

    template <typename Symbol, typename Defaults>
    struct help_map
    {
      using type = bool;
      static
      bool
      value(std::ostream& s, Options const& opts, Defaults const& defaults)
      {
        using Formal = typename das::named::make_formal<Symbol>::type;
        auto opt = opts.find(Symbol::name());
        if (opt != opts.end())
          print_help(
            s, Symbol::name(), opt->second.short_name, opt->second.help);
        else
          elle::meta::static_if<std::is_base_of<CLI_Symbol, Formal>::value>(
            [&s] (auto formal) {
              using formal_t = typename decltype(formal)::type;
              print_help(s,
                         formal_t::name(), formal_t::short_name(), formal_t::help());
            },
            [&s] (auto formal) {
              using formal_t = typename decltype(formal)::type;
              elle::fprintf(
                s, "      --%s", option_name_from_c(formal_t::name()));
            })(elle::meta::Identity<Formal>{});
        elle::meta::static_if<Defaults::template default_for<Formal>::has>
          ([&s] (auto const& defaults)
           {
             auto const& v = defaults.Symbol::ByConstRef::value;
             if (!std::is_same<decltype(v), bool const&>::value &&
                 !std::is_same<decltype(v), boost::none_t const&>::value)
               elle::fprintf(s, " (default: %s)", v);
           })(defaults);
        elle::fprintf(s, "\n");
        return true;
      }
    };

    template <typename D, typename ... T>
    void
    help(named::Prototype<D, T...> const& f,
         std::ostream& s,
         Options const& opts = Options())
    {
      elle::meta::List<T...>::template map<help_map, D>::value(
        s, opts, f.defaults);
    }

    template <typename F, typename ... T>
    void
    help(named::Function<F, T...> const& f,
         std::ostream& s,
         Options const& opts = Options())
    {
      elle::meta::List<T...>::
        template map<help_map, named::DefaultStore<T...>>::value(
          s, opts, f.prototype().defaults);
    }
  }
}

#define DAS_CLI_SYMBOL(Name, ...) DAS_CLI_SYMBOL_NAMED(Name, Name, __VA_ARGS__)

#define DAS_CLI_SYMBOL_NAMED(Name, CName, Short, Help, Pos)             \
  DAS_SYMBOL_TYPE_NAMED(Name, CName);                                   \
  constexpr static                                                      \
  class CLI_Symbol_##Name                                               \
    : public _Symbol_##Name<CLI_Symbol_##Name>                          \
    , public ::das::cli::CLI_Symbol                                     \
  {                                                                     \
  public:                                                               \
    using _Symbol_##Name<CLI_Symbol_##Name>::operator=;                 \
    constexpr                                                           \
      CLI_Symbol_##Name()                                               \
    {}                                                                  \
                                                                        \
    static constexpr                                                    \
    char                                                                \
    short_name()                                                        \
    {                                                                   \
      return Short;                                                     \
    }                                                                   \
                                                                        \
    static constexpr                                                    \
    char const*                                                         \
    help()                                                              \
    {                                                                   \
      return Help;                                                      \
    }                                                                   \
                                                                        \
    static constexpr                                                    \
    bool                                                                \
    positional()                                                        \
    {                                                                   \
      return Pos;                                                       \
    }                                                                   \
  } CName = {};
