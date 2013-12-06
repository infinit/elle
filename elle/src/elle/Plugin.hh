#ifndef ELLE_PLUGIN_HH
# define ELLE_PLUGIN_HH

# include <memory>
# include <typeinfo>
# include <unordered_map>

namespace elle
{
  template <typename T>
  class Plugin
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
    void
    load() const;
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
