#ifndef ELLE_PLUGIN_HXX
# define ELLE_PLUGIN_HXX

# include <elle/compiler.hh>

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
    Plugin<T>::plugins().insert(std::make_pair(&typeid(*plugin),
                                                std::move(plugin)));
  }

  template <typename T>
  std::unordered_map<std::type_info const*, std::unique_ptr<T>>&
    Plugin<T>::plugins()
  {
    static std::unordered_map<std::type_info const*, std::unique_ptr<T>> res;
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
}

#endif
