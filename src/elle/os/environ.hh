#pragma once

#include <string>
#include <unordered_map>

#ifdef environ // Prevent windows.h macros.
# undef environ
#endif

#include <elle/compiler.hh>
#include <elle/from-string.hh>

namespace elle
{
  namespace os ELLE_API
  {
    using Environ = std::unordered_map<std::string, std::string>;

    /// Retrieve a fresh copy of environ.
    Environ
    environ();

    /// Retrieve a fresh copy of environ keeping only the entries that
    /// validate the predicate.
    template <typename Pred>
    Environ
    environ(Pred pred)
    {
      auto res = environ();
      erase_if(res, [&pred](auto const& p) {
          return !pred(p.first, p.second);
        });
      return res;
    }

    /// Set `key=val` in the environment, unless $key is already
    /// defined and overwrite is false.
    ///
    /// @return the resulting value of `$key`.
    std::string
    setenv(std::string const& key, std::string const& val,
           bool overwrite = true);

    /// Import all the variables from env to the (real) environment.
    void
    setenv(Environ const& env, bool overwrite = true);

    /// Get a specific value from environment. If value is not found,
    /// KeyError Exception is thrown
    std::string
    getenv(std::string const& key);

    /// Get a specific value from environment. If value is not found,
    /// default_ is returned
    template <typename T>
    T
    getenv(std::string const& key, T default_)
    {
      if (auto val = ::getenv(key.c_str()))
        return from_string<T>(val);
      else
        return default_;
    }

    /// Whether a key is defined in environment.
    bool
    inenv(std::string const& key);

    /// Remove from the environment.
    void
    unsetenv(std::string const& key);
  }
}
