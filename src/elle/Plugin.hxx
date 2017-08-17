#include <elle/compiler.hh>
#include <elle/log.hh>

namespace elle
{
  template <typename T>
  template <typename I>
  Plugin<T>::Register<I>::Register()
  {
    Self::register_plugin(std::make_unique<I>());
  }

  template <typename T>
  template <typename I>
  void
  Plugin<T>::Register<I>::poke() const
  {}

  template <typename T>
  template <typename ... Args>
  Plugin<T>::Plugin(Args const& ... args)
  {
    using swallow = int[];
    (void) swallow
    {
      (args.poke(), 0)...
    };
  }

  template <typename T>
  void
  Plugin<T>::register_plugin(std::unique_ptr<T> plugin)
  {
    auto& p = *plugin;
    Self::plugins().emplace(&typeid(*plugin), std::move(plugin));
    Self::hook_added()(p);
  }

  template <typename T>
  auto
  Plugin<T>::plugins()
    -> Plugins&
  {
    using SPlugins = std::shared_ptr<Plugins>;
    auto& pm = plugins_map();
    auto k = elle::type_info<T>().name();
    auto it = pm.find(k);
    if (it == pm.end())
    {
      boost::any v(std::make_shared<Plugins>());
      pm.emplace(k, std::move(v));
      return *boost::any_cast<SPlugins>(pm[k]);
    }
    return *boost::any_cast<SPlugins>(it->second);
  }

  template <typename ... Args>
  PluginLoad::PluginLoad(Args const& ... args)
  {
    using swallow = int[];
    (void) swallow
    {
      (args.load(), 0)...
    };
  }

  template <typename T>

  boost::signals2::signal<void (T&)>&
  Plugin<T>::hook_added()
  {
    using Hook = boost::signals2::signal<void (T&)>;
    using SHook = std::shared_ptr<Hook>;
    auto& hm = hooks_map();
    auto k = elle::type_info<T>().name();
    auto it = hm.find(k);
    if (it == hm.end())
    {
      boost::any v(std::make_shared<Hook>());
      hm.emplace(k, std::move(v));
      return *boost::any_cast<SHook>(hm[k]);
    }
    return *boost::any_cast<SHook>(it->second);
  }
}
