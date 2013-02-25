#ifndef INFINIT_REACTOR_STORAGE_HH
# define INFINIT_REACTOR_STORAGE_HH

# include <boost/unordered_map.hpp>

# include <reactor/fwd.hh>

namespace reactor
{
  template <typename T>
  class LocalStorage
  {
  public:
    typedef LocalStorage<T> Self;
    LocalStorage();
    operator T&();
    T& Get(T const& def);
    T& Get();

  private:
    void _Clean(Thread* t);
    typedef boost::unordered_map<Thread*, T> Content;
    Content _content;
  };
}

# include <reactor/storage.hxx>

#endif
