#ifndef ELLE_PLUGIN_HXX
# define ELLE_PLUGIN_HXX

# include <elle/compiler.hh>
# include <elle/log.hh>

namespace elle
{
  template <typename T>
  template <typename I>
  Plugin<T>::Register<I>::Register()
  {
    Plugin<T>::register_plugin(std::unique_ptr<T>(new I()));
  }

  template <typename T>
  template <typename I>
  void
  Plugin<T>::Register<I>::poke() const
  {}

  template <typename ... Args>
  struct
  Poke
  {
    static
    void
    poke(Args const& ... args)
    {}
  };

  template <typename Arg, typename ... Args>
  struct
  Poke<Arg, Args...>
  {
    static
    void
    poke(Arg const& arg, Args const& ... args)
    {
      arg.poke();
      Poke<Args...>::poke(args...);
    }
  };

  template <typename T>
  template <typename ... Args>
  Plugin<T>::Plugin(Args const& ... args)
  {
    Poke<Args...>::poke(args...);
  }

  template <typename T>
  void
  Plugin<T>::register_plugin(std::unique_ptr<T> plugin)
  {
    auto& p = *plugin;
    Plugin<T>::plugins().emplace(&typeid(*plugin), std::move(plugin));
    Plugin<T>::hook_added()(p);
  }

  template <typename T>
  auto
    Plugin<T>::plugins()
    -> plugins_t&
  {
    static auto res = plugins_t{};
    return res;
  }

  namespace
  {
    template <typename ... Args>
    struct
    Load
    {
      static
      void
      load(Args const& ... args)
      {}
    };

    template <typename Arg, typename ... Args>
    struct
    Load<Arg, Args...>
    {
      static
      void
      load(Arg const& arg, Args const& ... args)
      {
        arg.load();
        Load<Args...>::load(args...);
      }
    };
  }

  template <typename ... Args>
  PluginLoad::PluginLoad(Args const& ... args)
  {
    Load<Args...>::load(args...);
  }

  template <typename T>

  boost::signals2::signal<void (T&)>&
  Plugin<T>::hook_added()
  {
    static boost::signals2::signal<void (T&)> res;
    return res;
  }
}

#endif
