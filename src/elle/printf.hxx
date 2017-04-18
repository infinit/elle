#pragma once

#include <iosfwd>
#include <typeinfo>

#include <boost/format.hpp>

#include <elle/TypeInfo.hh>
#include <elle/utils.hh> // as_const

// Work around Clang 3.5.0 bug where having this helper in the elle namespace
// will find a << overload for elle::serialization::Serializer::SerializerIn
// (and only that type). If such an overload existed, ADL would find it anyway
// no matter where it's called from.
namespace _elle_printf_details
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
  _is_streamable(...)
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
  namespace
  {
    template <typename T>
    auto
    feed(boost::format& fmt, T const& value)
      -> std::enable_if_t<_elle_printf_details::is_streamable<T>(), void>
    {
      fmt % value;
    }

    template <typename T>
    auto
    feed(boost::format& fmt, T const& value)
      -> std::enable_if_t<!_elle_printf_details::is_streamable<T>(), void>
    {
      static const auto parsed = boost::format("%f(%x)");
      auto format = parsed;
      format % elle::type_info(value);
      format % reinterpret_cast<const void*>(&value);
      fmt % format.str();
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

    /*-----------------.
    | Smart pointers.  |
    `-----------------*/

    template <typename T>
    void
    feed(boost::format& fmt, std::shared_ptr<T> const& value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
    }

    template <typename T>
    void
    feed(boost::format& fmt, std::unique_ptr<T> const& value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
    }

    template <typename T>
    void
    feed(boost::format& fmt, std::ambivalent_ptr<T> const& value)
    {
      feed(fmt, value.lock());
    }

    template <typename T>
    void
    feed(boost::format& fmt, std::weak_ptr<T> const& value)
    {
      feed(fmt, value.lock());
    }

    inline
    void
    feed(boost::format& fmt, void const* value)
    {
      if (value)
        fmt % value;
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
    void
    feed(boost::format& fmt, T* value)
    {
      if (value)
        feed(fmt, *value);
      else
        fmt % "nullptr";
    }

    template <typename R, typename ... Args>
    void
    feed(boost::format& fmt, R (*value)(Args...))
    {
      // FIXME: we could print more, but it would complexify this header quite a
      // bit.
      fmt % "<function>";
    }

    template <typename T>
    auto
    feed(boost::format& fmt, T& value)
      -> std::enable_if_t<!std::is_const<T>{}>
    {
      feed(fmt, as_const(value));
    }

    /// Create, feed and return a boost::format.
    template <typename F, typename ... T>
    boost::format
    format(F&& fmt, T&& ... values)
    {
      auto res = boost::format{fmt};
      using swallow = int[];
      (void) swallow
        {
          (feed(res, std::forward<T>(values)), 0)...
        };
      return res;
    }

    template <typename F, typename ... T>
    std::string
    sprintf(F&& fmt, T&& ... values)
    {
      try
      {
        auto f = format(std::forward<F>(fmt), std::forward<T>(values)...);
        return f.str();
      }
      catch (boost::io::format_error const& e)
      {
        format_error(fmt, e);
      }
    }

    template <typename F, typename ... T>
    std::ostream&
    fprintf(std::ostream& stream, F&& fmt, T&& ... values)
    {
      try
      {
        auto f = format(std::forward<F>(fmt), std::forward<T>(values)...);
        return stream << f;
      }
      catch (boost::io::format_error const& e)
      {
        format_error(fmt, e);
      }
    }
  }
}
