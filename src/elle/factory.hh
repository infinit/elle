#pragma once

#include <elle/Error.hh>
#include <elle/memory.hh>
#include <unordered_map>

namespace elle
{
  namespace _details
  {
    template <typename F>
    class Factory
    {
    public:
      using Type = decltype(std::declval<F>()());

      Factory(F f)
        : _f(f)
      {}

      template <typename C>
      operator C() const
      {
        return this->_f();
      }

      ELLE_ATTRIBUTE(F, f);
    };
  }

  template <typename F>
  _details::Factory<F>
  factory(F f)
  {
    return _details::Factory<F>(std::move(f));
  }

  template <typename T>
  class Factory
  {
  public:
    class KeyError: public Error
    {
    public:
      KeyError(std::string const& key)
        : Error("No such key: " + key)
        {}
    };
    using Arguments = std::vector<std::string>;
    using Builder = std::function<std::unique_ptr<T>(Arguments const&)>;
    static
    int
    register_(std::string const& name, Builder builder);
    static
    std::unique_ptr<T>
    instantiate(std::string const& name, Arguments const& args);
  private:
    using Items = std::unordered_map<std::string, Builder>;
    static Items& _items();
  };
}

#define FACTORY_REGISTER(type, name, builder)                       \
  static int __attribute__((unused))                                \
  unused = elle::Factory<type>::register_(name, builder)

#include <elle/factory.hxx>
