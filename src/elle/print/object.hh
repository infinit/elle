#pragma once

#include <ostream>
#include <memory>

namespace elle
{
  namespace _details
  {
    /// Default print for non-streamable objects.
    void
    default_print(std::ostream& o, std::type_info const& info, void const* p);

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

    /*--------------------------.
    | Print individual elements |
    `--------------------------*/

    template <typename T>
    std::enable_if_t<is_streamable<T>(), void>
    print(std::ostream& o, T&& value)
    {
      o << std::forward<T>(value);
    }

    template <typename T>
    std::enable_if_t<!is_streamable<T>(), void>
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
  }
}
