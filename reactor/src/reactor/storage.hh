#ifndef INFINIT_REACTOR_STORAGE_HH
# define INFINIT_REACTOR_STORAGE_HH

# include <boost/unordered_map.hpp>

# include <reactor/scheduler.hh>

namespace reactor
{
  template <typename T>
  class LocalStorage
  {
  public:
    typedef LocalStorage<T> Self;
    LocalStorage(Scheduler& sched);
    operator T&();
    T& Get(T const& def = T());

  private:
    void _Clean(Thread* t);
    Scheduler& _sched;
    typedef boost::unordered_map<Thread*, T> Content;
    Content _content;
  };
}

# include <reactor/storage.hxx>

#endif
