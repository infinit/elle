#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>

#include <elle/Error.hh>
#include <elle/Defaulted.hh>
#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/make-vector.hh>
#include <elle/serialization/json.hh>

#include <elle/das/named.hh>

namespace elle
{
  namespace das
  {
    namespace cli
    {
      /// Create a command line interface for a function using symbols.
      ///
      /// \code{.cc}
      ///
      /// ELLE_DAS_CLI_SYMBOL(prefix, 'p', "The prefix", false);
      /// ELLE_DAS_CLI_SYMBOL(body, 'b', "The body", false);
      ///
      /// ...
      ///
      /// auto func = [] (std::string const& foo, std::string const& bar)
      ///             {
      ///               return foo + "_" + bar;
      ///             };
      /// auto const proto = elle::das::named::prototype(prefix, body);
      /// assert(
      ///   elle::das::cli::call(proto, func,
      ///                        {"--prefix", "bad", "--body", "karma"})
      ///   == "bad_karma");
      ///
      /// assert(
      ///   elle::sprintf("%s", elle::das::cli::help(proto))
      ///   == "  -p, --prefix arg          The prefix\n"
      ///      "  -b, --body arg            The body\n");
      ///
      /// \endcode
      /*---------.
      | Errors.  |
      `---------*/

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

      /// Error on a value, e.g., unexpect argument.
      class ValueError
        : public virtual Error
      {
      public:
        ValueError(std::string value)
          : Error("") // Never called (virtual)
          , _value(std::move(value))
        {}

        /// The erroneous value.
        ELLE_ATTRIBUTE_R(std::string, value);
      };

      /// Error on the value given to an option, e.g., invalid type.
      class OptionValueError
        : public OptionError
        , public ValueError
      {
      public:
        OptionValueError(std::string option,
                         std::string value,
                         std::string message)
          : Error(elle::sprintf("invalid value \"%s\" for option --%s: %s",
                                value, option, message))
          , OptionError(option)
          , ValueError(value)
        {}

        /// The erroneous option
        ELLE_ATTRIBUTE_R(std::string, value);
      };

#define ELLE_DAS_CLI_OPTION_ERROR(Name, Description)                    \
      class Name ## Option                                              \
        : public OptionError                                            \
      {                                                                 \
      public:                                                           \
        Name ## Option(std::string option)                              \
          : Error(elle::sprintf(Description ": --%s", option))          \
          , OptionError(option)                                         \
        {}                                                              \
      }                                                                 \

#define ELLE_DAS_CLI_VALUE_ERROR(Name, Description)                     \
      class Name ## Value                                               \
        : public ValueError                                             \
      {                                                                 \
      public:                                                           \
        Name ## Value(std::string value)                                \
          : Error(elle::sprintf(Description ": %s", value))             \
          , ValueError(value)                                           \
        {}                                                              \
      }                                                                 \

      /// Unrecognized option
      ELLE_DAS_CLI_OPTION_ERROR(Unknown, "unknown option");
      /// Missing mandatory option
      ELLE_DAS_CLI_OPTION_ERROR(Missing, "missing option");
      /// Duplicate option
      ELLE_DAS_CLI_OPTION_ERROR(Duplicate, "duplicate option");
      /// Option used as a flag and passed arguments
      ELLE_DAS_CLI_OPTION_ERROR(
        Mixed, "option can't be used both as a flag and with arguments");
      /// Option without argument.
      ELLE_DAS_CLI_OPTION_ERROR(Valueless, "option requires an argument");
      /// Unrecognized left over value
      ELLE_DAS_CLI_VALUE_ERROR(Unrecognized, "extra unrecognized argument");

#undef ELLE_DAS_CLI_OPTION_ERROR
#undef ELLE_DAS_CLI_VALUE_ERROR

      /// Base tag for cli symbols
      class CLI_Symbol
      {};

      /*--------.
      | Options |
      `--------*/

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
        inline
        std::string
        strip_dashes(std::string const& a)
        {
          auto s = a.size();
          if (s == 2 && a[0] == '-' && std::isalpha(a[1]))
            return a.substr(1);
          else if (2 < a.size() && a[0] == '-' && a[1] == '-')
            return a.substr(2);
          else
            return a;
        }

        class IsOption
        {
        public:
          IsOption(std::string a, Options const& opts)
            : _option(a.size() > 2 && a[0] == '-' && a[1] == '-'
                      || a.size() == 2 && a[0] == '-' && std::isalpha(a[1]))
            , _arg(std::move(a))
          {}

          /// Whether this is the option corresponding to Formal F.
          template <typename F>
          bool
          is(Options const& opts)
          {
            if (this->_option)
            {
              if (this->_arg[0] == '-' && this->_arg[1] == '-')
                return this->_arg.substr(2) == option_name_from_c(F::name());
              else
              {
                using Formal = elle::das::named::make_formal<F>;
                auto res =
                  elle::meta::static_if<std::is_base_of<CLI_Symbol,
                                                        Formal>::value>(
                  [this] (auto&& formal)
                  {
                    return this->_arg[1] == formal.short_name();
                  },
                  [](auto&&) { return false; })(Formal{});
                {
                  auto it = opts.find(F::name());
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

          /// Whether is an option.
          ELLE_ATTRIBUTE(bool, option);
          /// The long option name (without the `--`).
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
        /// Type of default values for flags that don't have a default value.
        struct NoDefault
        {
          friend std::ostream& operator<<(std::ostream& os, NoDefault)
          {
            return os << "NoDefault";
          };
        };

        /// A value not found on the CLI
        template <typename Default = NoDefault>
        class Value
        {
        public:
          /// Whether a default value was specified.
          static bool constexpr default_has
             = !std::is_same<Default, NoDefault>::value;

          ELLE_LOG_COMPONENT("das.cli");

          /// An option with a value.
          ///
          /// @param set  whether the option was passed on the command line.
          Value(Default const& d,
                std::string option,
                bool positional,
                std::vector<std::string>& args,
                std::vector<std::string> value,
                int& remaining,
                bool set)
            : _def(d)
            , _option(std::move(option))
            , _values(std::move(value))
            , _flag(false)
            , _positional(positional)
            , _args(args)
            , _remaining(remaining)
            , _set(set)
          {}

          /// An option whose value was not given from the CLI, aka a flag.
          Value(Default const& d,
                std::string option,
                bool positional,
                std::vector<std::string>& args,
                int& remaining,
                bool set)
            : Value(d, std::move(option), positional, args, {},
                    remaining, set)
          {
            this->_flag = true;
          }

          template <typename I>
          static
          std::enable_if_t<std::is_signed<I>::value, I>
          to_int(std::string const& v, std::string const& option)
          {
            std::size_t end;
            auto i = std::stoll(v, &end);
            if (end != v.size())
              throw OptionValueError(option, v, "invalid integer");
            else if (i < std::numeric_limits<I>::min()
                || i > std::numeric_limits<I>::max())
              throw OptionValueError(option, v, "integer out of range");
            else
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
            else if (i > std::numeric_limits<I>::max())
              throw OptionValueError(option, v, "integer out of range");
            else
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
            else
              throw OptionValueError(this->_option, v, "invalid boolean");
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
            else if (this->_values.size() > 1)
              throw DuplicateOption(this->_option);
            else
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
              throw OptionValueError(this->_option, v, "invalid integer");
            }
            catch (std::out_of_range)
            {
              throw OptionValueError(this->_option, v, "integer out of range");
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
          std::enable_if_t<default_has, T>
          missing() const
          {
            ELLE_TRACE("use default value: %s", this->_def);
            return this->_def;
          }

          template <typename T>
          std::enable_if_t<!default_has && !std::is_same<T, bool>::value, T>
          missing() const
          {
            ELLE_TRACE("raise missing error");
            throw MissingOption(this->_option);
          }

          template <typename T>
          std::enable_if_t<!default_has && std::is_same<T, bool>::value, bool>
          missing() const
          {
            return false;
          }

          template <typename I>
          I
          convert() const
          {
            ELLE_TRACE("convert: %s", this);
            if (this->_flag)
              throw ValuelessOption(this->_option);
            if (this->_values.empty())
            {
              if (this->_positional)
              {
                auto it = this->_args.begin();
                while (it != this->_args.end() && is_option(*it))
                {
                  // Skip option and possibly argument.
                  ++it;
                  if (it != this->_args.end())
                    ++it;
                }
                if (it != this->_args.end())
                {
                  ELLE_TRACE("use next positional value: %s", *it);
                  this->_values.emplace_back(std::move(*it));
                  it = this->_args.erase(it);
                }
                else
                  return this->missing<I>();
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
            ELLE_TRACE_SCOPE(
              "convert %s to %s", this->_option, elle::type_info<I>());
            auto res = this->convert<I>();
            this->_check_remaining();
            return res;
          }

          operator bool() const
          {
            ELLE_TRACE_SCOPE("convert %s to boolean", this->_option);
            if (this->_flag)
              return true;
            else
            {
              auto res = this->convert<bool>();
              this->_check_remaining();
              return res;
            }
          }

          template <typename T>
          operator std::vector<T>() const
          {
            ELLE_TRACE_SCOPE(
              "convert %s to %s", this->_option, elle::type_info(std::vector<T>{}));
            if (this->_values.empty() && this->_positional)
              this->_values = std::move(this->_args);
            this->_check_remaining();
            return elle::make_vector(this->_values,
                                     [&] (std::string const& v)
                                     {
                                       return convert<T>(v, 0);
                                     });
          }

          /// A conversion that allows to know whether we have the
          /// option's default value, or a user defined one.
          template <typename I>
          operator elle::Defaulted<I>() const
          {
            ELLE_TRACE_SCOPE("convert %s to %s",
                             this->_option,
                             elle::type_info<elle::Defaulted<I>>());
            auto res = this->operator I();
            ELLE_TRACE_SCOPE(
              "converted %s to %s (%s)",
              this->_option, res, this->_set ? "explicit" : "implicit");
            return {res, this->_set};
          }

          void
          _check_remaining() const
          {
            if (!--this->_remaining && !this->_args.empty())
            {
              auto const& arg = *this->_args.begin();
              if (is_option(arg))
                throw UnknownOption(strip_dashes(arg));
              else
                throw UnrecognizedValue(arg);
            }
          }

          friend
          std::ostream&
          operator <<(std::ostream& out, Value<Default> const& v)
          {
            elle::fprintf(
              out, "Value(\"%s\", flag=%s, value=%s, def=%s, set=%s)",
              v.option(), v.flag(), v.values(), v.def(), v.set());
            return out;
          }

        private:
          ELLE_ATTRIBUTE_R(Default const&, def);
          ELLE_ATTRIBUTE_R(std::string, option);
          ELLE_ATTRIBUTE_R(std::vector<std::string>, values, mutable);
          /// Whether had no value given on the CLI.
          /// E.g. `--foo` => is-flag and value = true,
          /// but `--foo=true` => not-is-flag, and value = true.
          ELLE_ATTRIBUTE_R(bool, flag);
          ELLE_ATTRIBUTE(bool, positional);
          ELLE_ATTRIBUTE(std::vector<std::string>&, args);
          ELLE_ATTRIBUTE(int&, remaining);
          /// Whether the option was given on the command line.
          ELLE_ATTRIBUTE_R(bool, set);
        };

        template <typename Default, typename Formal>
        struct parse_arg
        {
          using Symbol = elle::das::named::make_formal<Formal>;
          static inline
          auto
          value(Default const& d,
                std::vector<std::string>& args,
                Options const& opts,
                int& counter)
          {
            ELLE_LOG_COMPONENT("das.cli");
            bool flag = false;
            bool pos = false;
            bool set = false;
            auto value = std::vector<std::string>{};
            auto next_option = [&](auto i) {
              return std::find_if(i, args.end(), [](auto const& arg) {
                  return is_option(arg);
                });
            };
            ELLE_TRACE_SCOPE("parsing option %s", Symbol::name());
            ELLE_DUMP("remaining arguments: %s", args);
            for (auto it = next_option(args.begin()); it != args.end();
                 it = next_option(it))
            {
              // There's a possible explicit argument after `=`.
              auto const eq = it->find('=');
              auto const option = it->substr(0, eq);
              auto o = is_option(option, opts);
              assert(o);
              if (o.template is<Formal>(opts))
              {
                // This is the option we are looking for.
                set = true;
                auto argument_set = [&](auto const& arg) {
                  ELLE_DEBUG(
                    "found \"%s\" on the command line with argument \"%s\"",
                    option, arg);
                  if (flag)
                    throw MixedOption(Symbol::name());
                  value.emplace_back(arg);
                };
                if (eq == it->npos)
                {
                  // `--foo`: no `=`.
                  it = args.erase(it);
                  if (it != args.end() && !is_option(*it, opts))
                  {
                    argument_set(*it);
                    it = args.erase(it);
                  }
                  else
                  {
                    ELLE_DEBUG("found \"%s\" on the command line", *it);
                    if (!value.empty())
                      throw MixedOption(Symbol::name());
                    if (flag)
                      throw DuplicateOption(Symbol::name());
                    flag = true;
                  }
                }
                else
                {
                  // `--foo=bar`: explicit argument.
                  argument_set(it->substr(eq + 1));
                  it = args.erase(it);
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
            elle::meta::static_if<std::is_base_of<CLI_Symbol, Symbol>::value>(
              [] (auto& pos, auto t)
              {
                pos = decltype(t)::type::positional();
              })(pos, elle::meta::Identity<Symbol>());
            {
              auto it = opts.find(Symbol::name());
              if (it != opts.end())
                pos = it->second.positional;
            }
            return elle::meta::static_if<Default::has>(
              [&] (auto const& d)
              {
                using V = Value<
                  typename std::remove_cv_reference_t<decltype(d)>::type>;
                if (flag)
                  return V(d.value, Symbol::name(), pos, args, counter, set);
                else
                {
                  if (value.empty())
                    ELLE_DEBUG("no occurences, default value is %s", d.value);
                  return V(d.value, Symbol::name(), pos, args,
                           std::move(value), counter, set);
                }
              },
              [&] (auto)
              {
                using V = Value<NoDefault>;
                if (flag)
                  return V(NoDefault{}, Symbol::name(), pos, args, counter, set);
                else
                {
                  if (value.empty())
                    ELLE_DEBUG("no occurences and no default value");
                  return V(NoDefault{}, Symbol::name(), pos, args,
                           std::move(value), counter, set);
                }
              })(d);
          }
        };
      }

      template <typename F, typename ... Formals, typename ... Raw>
      auto
      _call(elle::das::named::Prototype<Formals...> const& p,
            F const& f,
            std::vector<std::string>& args,
            Raw&& ... raw,
            Options const& opts = Options())
      {
        ELLE_LOG_COMPONENT("das.cli");
        int counter = sizeof ... (Formals);
        return f(
          _details::parse_arg<
            typename named::DefaultStore<Formals...>::
              template default_for<std::remove_cv_reference_t<Formals>>,
            std::remove_cv_reference_t<Formals>>::value(
              p.defaults, args, opts, counter)..., std::forward<Raw>(raw)...);
      }

      template <typename F,
                typename ... Formals,
                typename ... Raw>
      auto
      call(elle::das::named::Prototype<Formals...> const& p,
           F const& f,
           std::vector<std::string>& args,
           Raw&& ... raw,
           Options const& opts = Options())
      {
        return _call(p, f, args, std::forward<Raw>(raw)..., opts);
      }

      template <typename F,
                typename ... Formals,
                typename ... Args,
                typename ... Raw>
      auto
      call(elle::das::named::Prototype<Formals...> const& p,
           F const& f,
           std::vector<std::string> const& args,
           Raw&& ... raw,
           Options const& opts = Options())
      {
        auto copy = args;
        return _call(p, f, copy, std::forward<Raw>(raw)..., opts);
      }

      template <typename ... T,
                typename ... Raw>
      auto
      call(named::Function<T...> const& f,
           std::vector<std::string>& args,
           Raw&& ... raw,
           Options const& opts = Options())
      {
        return _call(f.prototype(), f.function(), args,
                     std::forward<Raw>(raw)..., opts);
      }

      template <typename ... T,
                typename ... Raw>
      auto
      call(named::Function<T...> const& f,
           std::vector<std::string> const& args,
           Raw&& ... raw,
           Options const& opts = Options())
      {
        std::vector<std::string> copy = args;
        return _call(f.prototype(), f.function(), copy,
                     std::forward<Raw>(raw)..., opts);
      }

      inline
      void
      print_help(std::ostream& s,
                 std::string const& name,
                 bool with_argument,
                 char short_name = 0,
                 std::string const& help = {})
      {
        if (short_name)
          elle::fprintf(s, "  -%s, ", short_name);
        else
          elle::fprintf(s, "      ");
        elle::fprintf(s, "--%-18s",
                      elle::das::cli::option_name_from_c(name)
                      + (with_argument ? " arg" : ""));
        if (!help.empty())
          elle::fprintf(s, "  %s", help);
      }

      template <typename Formal, typename Default>
      struct help_map
      {
        using type = bool;
        static
        bool
        value(std::ostream& s, Options const& opts, Default const& def)
        {
          using Symbol = elle::das::named::make_formal<Formal>;
          // Whether expects an argument.
          bool with_argument
            = elle::meta::static_if<Default::has>
            ([&s] (auto const& def)
             {
               auto const& v = def.value;
               return !std::is_same<decltype(v), bool const&>::value;
             },
             [] (auto const&)
             {
               return true;
             })(def);
          // Print the option's help string.
          auto opt = opts.find(Symbol::name());
          if (opt == opts.end())
            elle::meta::static_if<std::is_base_of<CLI_Symbol, Symbol>::value>(
              [&s, &with_argument] (auto formal) {
                using formal_t = typename decltype(formal)::type;
                print_help(s,
                           formal_t::name(), with_argument,
                           formal_t::short_name(), formal_t::help());
              },
              [&s, &with_argument] (auto formal) {
                using formal_t = typename decltype(formal)::type;
                print_help(s, option_name_from_c(formal_t::name()), with_argument);
              })(elle::meta::Identity<Symbol>{});
          else
            print_help(s, Symbol::name(), with_argument,
                       opt->second.short_name, opt->second.help);
          elle::meta::static_if<Default::has>
            ([&s, &with_argument] (auto const& def)
             {
               auto const& v = def.value;
               if (!std::is_same<decltype(v), bool const&>::value
                   && !std::is_same<decltype(v), boost::none_t const&>::value)
                 elle::fprintf(s, " (default: %s)", v);
             })(def);
          elle::fprintf(s, "\n");
          return true;
        }
      };

      template <typename ... T>
      class Help:
        public elle::Printable::as<Help<T ...>>
      {
      public:
        Help(named::Prototype<T...> const& p,
             Options const& opts = Options())
          : _prototype(p)
          , _options(opts)
        {}

        template <typename>
        using make_bool = bool;

        void
        print(std::ostream& s) const
        {
          std::tuple<make_bool<T>...>{
            help_map<T, typename named::Prototype<T...>::DefaultStore::template default_for<T>>::value(
              s, this->_options, this->_prototype.defaults)...
          };
        }

        ELLE_ATTRIBUTE(elle::das::named::Prototype<T...> const&, prototype);
        ELLE_ATTRIBUTE(Options, options);
      };

      template <typename ... T>
      auto
      help(named::Prototype<T...> const& p, Options const& opts = Options())
      {
        return Help<T...>(p, opts);
      }

      template <typename ... T>
      auto
      help(named::Function<T...> const& f, Options const& opts = Options())
      {
        return help(f.prototype(), opts);
      }
    }
  }
}

#define ELLE_DAS_CLI_SYMBOL(Name, ...) ELLE_DAS_CLI_SYMBOL_NAMED(Name, Name, __VA_ARGS__)

#define ELLE_DAS_CLI_SYMBOL_NAMED(Name, CName, Short, Help, Pos)        \
  ELLE_DAS_SYMBOL_TYPE_NAMED(Name, CName);                              \
  class CS_##Name                                                       \
    : public _Symbol_##Name<CS_##Name>                                  \
    , public ::elle::das::cli::CLI_Symbol                               \
  {                                                                     \
  public:                                                               \
    using Super = _Symbol_##Name<CS_##Name>;                            \
    using Super::operator=;                                             \
    constexpr                                                           \
    CS_##Name()                                                         \
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
  };                                                                    \
  constexpr static CS_##Name CName = {};
