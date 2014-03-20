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
    static
    void
    feed(boost::format& fmt, T&& value,
           ELLE_SFINAE_IF_WORKS(std::cout << std::forward<T>(value)))
    {
      fmt % std::forward<T>(value);
    }

    template <typename T>
    static
    void
    feed(boost::format& fmt, T&& value, ELLE_SFINAE_OTHERWISE())
    {
      static boost::format parsed("%s(%x)");
      boost::format format(parsed);
      fmt % str(format % demangle(typeid(T).name()) % reinterpret_cast<const void*>(&value));
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
        feed(fmt, std::forward<T>(value), ELLE_SFINAE_TRY());
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
      auto msg = elle::sprintf("format error with \"%s\": %s", fmt, e.what());
      ELLE_ERR("%s", msg);
      throw elle::Exception(msg);
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
      auto msg = elle::sprintf("format error with \"%s\": %s", fmt, e.what());
      ELLE_ERR("%s", msg);
      throw elle::Exception(msg);
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
