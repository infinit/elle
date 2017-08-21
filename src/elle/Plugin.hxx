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
    auto& pm = plugins_map();
    auto it = pm.emplace(elle::type_info<T>(), nullptr);
    if (it.second)
    {
      auto v = std::make_shared<Plugins>();
      it.first->second = v;
      return *v;
    }
    else
      return *boost::any_cast<std::shared_ptr<Plugins>>(it.first->second);
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
    auto& hm = hooks_map();
    auto it = hm.emplace(elle::type_info<T>(), nullptr);
    if (it.second)
    {
      auto v = std::make_shared<Hook>();
      it.first->second = v;
      return *v;
    }
    else
      return *boost::any_cast<std::shared_ptr<Hook>>(it.first->second);
  }
}
