#include <elle/assert.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Thread.hh>

namespace elle
{
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

    template <typename T>
    T&
    LocalStorage<T>::Get()
    {
      return _get([](auto&) {});
    }

    template <typename T>
    T&
    LocalStorage<T>::Get(T const& def)
    {
      return _get([&def](auto& storage) { storage = def; });
    }

    template <typename T>
    template <typename Fun>
    T&
    LocalStorage<T>::_get(Fun fun)
    {
      Scheduler* sched = Scheduler::scheduler();
      Thread* current = sched ? sched->current() : nullptr;
      void* key = current ? static_cast<void*>(current) : static_cast<void*>(sched);
      std::lock_guard<std::mutex> lock(_mutex);
      auto it = this->_content.find(key);
      if (it == this->_content.end())
        {
          auto& res = this->_content[key];
          fun(res);
          if (current)
          {
            auto link = current->destructed().connect(
              std::bind(&Self::_Clean, this, current)
            );
            this->_links[current] = link;
          }
          return res;
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
}
