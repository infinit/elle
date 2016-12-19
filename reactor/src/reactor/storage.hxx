#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <elle/assert.hh>

namespace reactor
{
  template <typename T>
  LocalStorage<T>::LocalStorage()
    : _content()
  {}

  template <typename T>
  LocalStorage<T>::~LocalStorage()
  {
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto const& i: this->_links)
      i.second.disconnect();
  }

  template <typename T>
  LocalStorage<T>::operator T&()
  {
    return this->Get();
  }

  // XXX: factor with Get(T const& def)
  template <typename T>
  T&
  LocalStorage<T>::Get()
  {
    Scheduler* sched = Scheduler::scheduler();
    Thread* current = sched ? sched->current() : 0;
    void* key = current ? (void*)current : (void*)sched;
    std::lock_guard<std::mutex> lock(_mutex);
    typename Content::iterator it = this->_content.find(key);
    if (it == this->_content.end())
      {
        if (current != nullptr)
        {
          auto link = current->destructed().connect(
            std::bind(&Self::_Clean, this, current)
          );
          this->_links[current] = link;
        }
        return this->_content[key];
      }
    else
      return it->second;
  }


  template <typename T>
  T&
  LocalStorage<T>::Get(T const& def)
  {
    Scheduler* sched = Scheduler::scheduler();
    Thread* current = sched ? sched->current() : 0;
    void* key = current ? (void*)current : (void*)sched;
    std::lock_guard<std::mutex> lock(_mutex);
    typename Content::iterator it = this->_content.find(key);
    if (it == this->_content.end())
      {
        this->_content[key] = def;
        if (current != nullptr)
        {
          auto link = current->destructed().connect(
            std::bind(&Self::_Clean, this, current)
          );
          this->_links[current] = link;
        }
        return this->_content[key];
      }
    else
      return it->second;
  }

  template <typename T>
  void
  LocalStorage<T>::_Clean(Thread* current)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    this->_links.erase(current);
    auto it = this->_content.find(current);
    ELLE_ASSERT_NEQ(it, this->_content.end());
    this->_content.erase(it);
  }
}
