#ifndef ELLE_PLUGIN_HH
# define ELLE_PLUGIN_HH

# include <memory>
# include <typeinfo>
# include <unordered_map>

namespace elle
{
  class BasePlugin
  {
  public:
    void
    load() const;
  };

  template <typename T>
  class Plugin:
    public BasePlugin
  {
  public:
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
    static
    void
    register_plugin(std::unique_ptr<T> plugin);
    static
    std::unordered_map<std::type_info const*, std::unique_ptr<T>>& plugins();
  };

  class PluginLoad
  {
  public:
    template <typename ... Args>
    PluginLoad(Args const& ... args);
  };
}

# include <elle/Plugin.hxx>

#endif
