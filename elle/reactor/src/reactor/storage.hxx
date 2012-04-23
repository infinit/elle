#ifndef INFINIT_REACTOR_STORAGE_HXX
# define INFINIT_REACTOR_STORAGE_HXX

# include <reactor/thread.hh>

namespace reactor
{
  template <typename T>
  LocalStorage<T>::LocalStorage(Scheduler& sched)
  : _sched(sched)
  {}

  template <typename T>
  LocalStorage<T>::operator T&()
  {
    return this->Get();
  }

  template <typename T>
  T&
  LocalStorage<T>::Get()
  {
    Thread* current = this->_sched.current();
    typename Content::iterator it = this->_content.find(current);
    if (it == this->_content.end())
      {
        this->_content[this->_sched.current()] = T();
        current->destructed().connect
          (boost::bind(&Self::_Clean, this, current));
        return this->_content[this->_sched.current()];
      }
    else
      return it->second;
  }

  template <typename T>
  void
  LocalStorage<T>::_Clean(Thread* current)
  {
    typename Content::iterator it = this->_content.find(current);
    assert(it != this->_content.end());
    this->_content.erase(it);
  }
}

#endif
