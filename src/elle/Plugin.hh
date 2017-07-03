#pragma once

#include <memory>
#include <typeinfo>
#include <unordered_map>

#include <boost/signals2.hpp>

#include <elle/compiler.hh>

namespace elle
{
  /// Base class for Plugin injection.
  class ELLE_API BasePlugin
  {
  public:
    ///
    void
    load() const;
  };

  /// Create a plugin.
  ///
  /// @code{.cc}
  ///
  /// XXX[doc].
  ///
  /// @endcode
  template <typename T>
  class ELLE_API Plugin
    : public BasePlugin
  {
  public:
    using Self = Plugin;
    using Plugins
      = std::unordered_map<std::type_info const*, std::unique_ptr<T>>;

    template <typename I>
    class Register
    {
    public:
      Register();
      void
      poke() const;
    };
    template <typename ... Args>
    Plugin(Args const& ... args);
    /// Register a plugin to the global plugin storage.
    ///
    /// N.B. This triggers hook_added signal.
    static
    void
    register_plugin(std::unique_ptr<T> plugin);
    /// Get the global plugin map.
    ///
    static
    Plugins&
    plugins();
    /// XXX[doc]
    static
    boost::signals2::signal<void (T&)>&
    hook_added();
  };

  class ELLE_API PluginLoad
  {
  public:
    template <typename ... Args>
    PluginLoad(Args const& ... args);
  };
}

#include <elle/Plugin.hxx>
