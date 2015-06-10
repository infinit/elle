#ifndef  ELLE_PRINTF_HXX
# define ELLE_PRINTF_HXX

# include <iostream>
# include <typeinfo>

# include <boost/format.hpp>

# include <elle/Backtrace.hh>
# include <elle/Exception.hh>
# include <elle/assert.hh>
# include <elle/log.hh>
# include <elle/print.hh>
# include <elle/sfinae.hh>

namespace elle
{
  namespace _details
  {
    template <typename T>
    constexpr
    typename std::enable_if<
      sizeof(std::cerr << ELLE_SFINAE_INSTANCE(T)) >= 0, bool>::type
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

    template <typename T>
    static
    typename std::enable_if<is_streamable<T>(), void>::type
    feed(boost::format& fmt, T&& value)
    {
      fmt % std::forward<T>(value);
    }

    template <typename T>
    static
    typename std::enable_if<!is_streamable<T>(), void>::type
    feed(boost::format& fmt, T&& value)
    {
      static boost::format parsed("%s(%x)");
      boost::format format(parsed);
      format % demangle(typeid(T).name());
      format % reinterpret_cast<const void*>(&value);
      fmt % str(format);
    }

    template <typename ... T>
    struct Feed
    {
      static
      void
      args(boost::format& fmt, T&& ... values)
      {}
    };

    template <typename T, typename ... Rest>
    struct Feed<T, Rest ...>
    {
      static
      void
      args(boost::format& fmt, T&& value, Rest&& ... values)
      {
        feed(fmt, std::forward<T>(value));
        Feed<Rest ...>::args(fmt, std::forward<Rest>(values) ...);
      }
    };

    template<typename F, typename ... T>
    boost::format
    format(F&& fmt, T&& ... values)
    {
      boost::format format(fmt);
      Feed<T ...>::args(format, std::forward<T>(values) ...);
      return format;
    }
  }

  template<typename F, typename ... T>
  std::string
  sprintf(F&& fmt, T&& ... values)
  {
    try
    {
      auto format = _details::format(std::forward<F>(fmt),
                                     std::forward<T>(values) ...);
      return str(format);
    }
    catch (boost::io::format_error const& e)
    {
      ELLE_LOG_COMPONENT("elle.printf");
      // Don't use printf to handle printf fatal errors.
      std::stringstream ss;
      ss << "format error with \"" << fmt << "\": " << e.what();
      ELLE_ERR("%s", ss.str());
      throw elle::Exception(ss.str());
    }
  }

  template<typename F, typename ... T>
  void
  fprintf(std::ostream& stream, F&& fmt, T&& ... values)
  {
    try
    {
      boost::format format(fmt);
      _details::Feed<T ...>::args(format, std::forward<T>(values) ...);
      stream << format;
    }
    catch (boost::io::format_error const& e)
    {
      ELLE_LOG_COMPONENT("elle.printf");
      // Don't use printf to handle printf fatal errors.
      std::stringstream ss;
      ss << "format error with \"" << fmt << "\": " << e.what();
      ELLE_ERR("%s", ss.str());
      throw elle::Exception(ss.str());
    }
  }

  template<typename F, typename ... T>
  void
  printf(F&& fmt, T&& ... values)
  {
    fprintf(std::cout, std::forward<F>(fmt), std::forward<T>(values) ...);
  }

}

#endif
