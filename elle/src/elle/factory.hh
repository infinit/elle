#ifndef ELLE_FACTORY_HH
 # define ELLE_FACTORY_HH

#include <elle/Error.hh>
#include <elle/memory.hh>
#include <unordered_map>

namespace elle
{
  template<typename T> class Factory
  {
  public:
    class KeyError: public Error
    {
    public:
      KeyError(std::string const& key)
        : Error("No such key: " + key)
        {}
    };
    typedef std::vector<std::string> Arguments;
    typedef std::function<std::unique_ptr<T>(Arguments const&)> Builder;
    static
    int
    register_(std::string const& name, Builder builder);
    static
    std::unique_ptr<T>
    instantiate(std::string const& name, Arguments const& args);
  private:
    typedef std::unordered_map<std::string, Builder> Items;
    static Items& _items();
  };

#define FACTORY_REGISTER(type, name, builder) \
  static int unused = elle::Factory<type>::register_(name, builder)

}

#include <elle/factory.hxx>


#endif