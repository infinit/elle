#ifndef ELLE_PRINTF_HXX
# define ELLE_PRINTF_HXX

# include <ostream>
# include <typeinfo>

# include <boost/format.hpp>

# include <elle/Backtrace.hh>
# include <elle/Exception.hh>
# include <elle/TypeInfo.hh>
# include <elle/assert.hh>
# include <elle/log.hh>
# include <elle/sfinae.hh>

// Work around Clang 3.5.0 bug where having this helper in the elle namespace
// will find a << overload for elle::serialization::Serializer::SerializerIn
// (and only that type). If such an overload existed, ADL would find it anyway
// no matter where it's called from.
namespace _elle_printf_details
{
  template <typename T>
  constexpr
  typename std::enable_if_exists<
    decltype(std::declval<std::ostream&>() << std::declval<T>()), bool>::type
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

    template <typename T>
    static
    typename std::enable_if<
      _elle_printf_details::is_streamable<T>(), void>::type
    feed(boost::format& fmt, T&& value)
    {
      fmt % std::forward<T>(value);
    }

    template <typename T>
    static
    typename std::enable_if<
      !_elle_printf_details::is_streamable<T>(), void>::type
    feed(boost::format& fmt, T&& value)
    {
      static boost::format parsed("%f(%x)");
      boost::format format(parsed);
      format % elle::type_info(value);
      format % reinterpret_cast<const void*>(&value);
      fmt % str(format);
    }

    inline
    void
    feed(boost::format& fmt, bool value)
    {
      fmt % (value ? "true" : "false");
    }

    inline
    void
    feed(boost::format& fmt, char const* value)
    {
      fmt % (value ? value : "");
    }

    inline
    void
    feed(boost::format& fmt, char* value)
    {
      fmt % (value ? value : "");
    }

    template <typename T>
    inline
    void
    feed(boost::format& fmt, std::unique_ptr<T> const& value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
    }

    template <typename T>
    inline
    void
    feed(boost::format& fmt, std::unique_ptr<T>& value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
    }

    template <typename T>
    inline
    void
    feed(boost::format& fmt, std::shared_ptr<T> const& value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
    }

    template <typename T>
    inline
    void
    feed(boost::format& fmt, std::shared_ptr<T>& value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
    }

    inline
    void
    feed(boost::format& fmt, void* value)
    {
      if (value)
        fmt % value;
      else
        fmt % "nullptr";
    }

    template <typename T>
    inline
    void
    feed(boost::format& fmt, T* value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
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
      static boost::format const f("format error with \"%s\": %s");
      auto msg = (boost::format(f) % fmt % e.what()).str();
      ELLE_ERR("%s", msg);
      elle::err(msg);
    }
  }

  template<typename F, typename ... T>
  std::ostream&
  fprintf(std::ostream& stream, F&& fmt, T&& ... values)
  {
    try
    {
      boost::format format(fmt);
      _details::Feed<T ...>::args(format, std::forward<T>(values) ...);
      return stream << format;
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
