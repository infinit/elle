#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>

#include <elle/Error.hh>
#include <elle/assert.hh>

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

    namespace _details
    {
      template <typename Formal, typename D, typename ... Formals>
      class Value
      {
      public:
        Value(das::named::Prototype<D, Formals...> const& p,
              std::string option,
              boost::optional<std::vector<std::string>&> positional,
              std::vector<std::string> value)
          : _prototype(p)
          , _option(std::move(option))
          , _value(std::move(value))
          , _positional(positional)
          , _flag(false)
        {}

        Value(das::named::Prototype<D, Formals...> const& p,
              std::string option,
              boost::optional<std::vector<std::string>&> positional,
              bool flag)
          : _prototype(p)
          , _option(std::move(option))
          , _value()
          , _positional(positional)
          , _flag(flag)
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
        convert(std::string const& v)
        {
          if (v == "true")
            return true;
          else if (v == "false")
            return false;
          throw OptionValueError(this->_option, v, "invalid boolean ");
        }

        template <typename I>
        std::enable_if_t<std::is_same<I, std::string>::value, I>
        convert(std::string const& v)
        {
          return v;
        }

        template <typename I>
        std::enable_if_t<std::is_base_of<boost::optional_detail::optional_tag, I>::value, I>
        convert(I const& v)
        {
          if (this->_value.empty())
            return boost::none;
          if (this->_value.size() > 1)
            throw DuplicateOption(this->_option);
          return convert<typename I::value_type>(this->_value[0]);
        }

        template <typename I>
        std::enable_if_t<
          std::is_integral<I>::value && !std::is_same<I, bool>::value, I>
        convert(std::string const& v)
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

        template <typename T>
        std::enable_if_t<
          !D::template default_for<
            typename das::named::make_formal<Formal>::type>::has,
          T>
        missing()
        {
          if (this->_positional && !this->_positional->empty())
          {
            this->_value.emplace_back(std::move(*this->_positional->begin()));
            this->_positional->erase(this->_positional->begin());
            return this->convert<T>();
          }
          else
            throw MissingOption(this->_option);
        }

        template <typename T>
        std::enable_if_t<
          D::template default_for<
            typename das::named::make_formal<Formal>::type>::has,
          T>
        missing()
        {
          using clang_workaround = Formal;
          return this->_prototype.defaults.clang_workaround::value;
        }

        operator bool()
        {
          if (this->_value.empty())
            return this->_flag;
          else
            return this->convert<bool>();
        }

        template <typename I>
        operator I()
        {
          return this->convert<I>();
        }

        template <typename I>
        I
        convert()
        {
          if (this->_value.empty())
            return missing<I>();
          if (this->_value.size() > 1)
            throw DuplicateOption(this->_option);
          return convert<I>(this->_value[0]);
        }

        operator std::vector<std::string>()
        {
          return this->_value;
        }

        template <typename T>
        operator std::vector<T>()
        {
          std::vector<T> res;
          for_each(
            this->_value.begin(), this->_value.end(),
            [&] (std::string const& v) { res.emplace_back(convert<T>(v)); });
          return res;
        }

      private:
        ELLE_ATTRIBUTE(
          (das::named::Prototype<D, Formals...> const&), prototype);
        ELLE_ATTRIBUTE(std::string, option);
        ELLE_ATTRIBUTE(std::vector<std::string>, value);
        ELLE_ATTRIBUTE(boost::optional<std::vector<std::string>&>, positional);
        ELLE_ATTRIBUTE(bool, flag);
      };

      class IsOption
      {
      public:
        IsOption(std::string a, Options const& opts)
          : _arg(std::move(a))
          , _name()
          , _option(false)
        {
          if (this->_arg.size() > 2 &&
              this->_arg[0] == '-' && this->_arg[1] == '-')
          {
            this->_option = true;
            this->_name = this->_arg.substr(2);
          }
          else if (this->_arg.size() == 2 &&
                   this->_arg[0] == '-' && std::isalpha(this->_arg[1]))
            for (auto const& opt: opts)
              if (opt.second.short_name == this->_arg[1])
              {
                this->_option = true;
                this->_name = opt.first;
                break;
              }
        }

        bool
        operator ==(std::string const& o)
        {
          if (this->_option)
          {
            auto r = o;
            std::replace(r.begin(), r.end(), '_', '-');
            return this->_name == r;
          }
          else
            return false;
        }

        operator bool()
        {
          return this->_option;
        }

        ELLE_ATTRIBUTE(std::string, arg);
        ELLE_ATTRIBUTE(std::string, name);
        ELLE_ATTRIBUTE(bool, option);
      };
    }

    static
    _details::IsOption
    is_option(std::string const& a, Options const& opts = Options())
    {
      return _details::IsOption(a, opts);
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
              Options const& opts)
          -> decltype(std::forward_tuple(f, std::move(parsed)))
        {
          if (!args.empty() && is_option(args[0], opts))
            throw UnknownOption(args[0]);
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
              Options const& opts)
          -> decltype(CLI<Tail...>::value(
            p, f,
            std::tuple_cat(
              std::move(parsed),
              std::declval<std::tuple<_details::Value<Head, D, Formals...>>>()),
            args,
            opts))
        {
          std::vector<std::string> value;
          bool flag = false;
          auto it = args.begin();
          while (it != args.end())
          {
            if (auto o = is_option(*it, opts))
              if (o == Head::name())
              {
                it = args.erase(it);
                if (it != args.end() && !is_option(*it, opts))
                {
                  if (flag)
                    throw MixedOption(Head::name());
                  value.emplace_back(*it);
                  it = args.erase(it);
                }
                else
                {
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
            else
              ++it;
          }
          bool positional = false;
          for (auto const& opt: opts)
            if (opt.first == Head::name())
            {
              positional = opt.second.positional;
              break;
            }
          boost::optional<std::vector<std::string>&> pos;
          if (positional)
            pos.reset(args);
          auto v = flag ?
            Value<Head, D, Formals...>(p, Head::name(), std::move(pos), flag) :
            Value<Head, D, Formals...>(
              p, Head::name(), std::move(pos), std::move(value));
          auto const check_extra = [&] (auto&& ... a)
            {
              if (!args.empty())
                throw UnrecognizedValue(args[0]);
              return f(std::forward<decltype(a)>(a)...);
            };
          return CLI<Tail...>::value(
            p, check_extra,
            std::tuple_cat(std::move(parsed), std::make_tuple(std::move(v))),
            args,
            opts);
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
        _details::CLI<Formals...>::value(p, f, std::tuple<>(), args, opts))
    {
      return _details::CLI<Formals...>::value(
        p, f, std::tuple<>(), args, opts);
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

    template <typename ... T>
    auto
    call(named::Function<T...> const& f,
         std::vector<std::string>& args,
         Options const& opts = Options())
      -> decltype(_call(f.prototype(), f, args, opts))
    {
      return _call(f.prototype(), f, args, opts);
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

    template <typename Symbol>
    struct help_map
    {
      using type = bool;
      static
      bool
      value(std::ostream& s, Options const& opts)
      {
        auto opt = opts.find(Symbol::name());
        if (opt != opts.end())
        {
          s << "  ";
          if (opt->second.short_name != 0)
            s << "-" << opt->second.short_name << ", ";
          else
            s << "    ";
          s << " --" << Symbol::name() << ": " << opt->second.help << "\n";
        }
        else
          s << "       --" << Symbol::name() << "\n";
        return true;
      };
    };

    template <typename D, typename ... T>
    void
    help(named::Prototype<D, T...> const& f,
         std::ostream& s,
         Options const& opts = Options())
    {
      elle::meta::List<T...>::template map<help_map>::value(s, opts);
    }

    template <typename F, typename ... T>
    void
    help(named::Function<F, T...> const& f,
         std::ostream& s,
         Options const& opts = Options())
    {
      elle::meta::List<T...>::template map<help_map>::value(s, opts);
    }
  }
}
