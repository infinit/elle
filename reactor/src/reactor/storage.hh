#pragma once

#include <mutex>
#include <unordered_map>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <reactor/fwd.hh>

namespace reactor
{
  template <typename T>
  class LocalStorage
  {
  public:
    using Self = LocalStorage<T>;
    LocalStorage();
    ~LocalStorage();
    operator T&();
    T& Get(T const& def);
    T& Get();

  private:
    void _Clean(Thread* t);
    using Content = std::unordered_map<void*, T>;
    Content _content;
    using Links = std::unordered_map<void*, boost::signals2::connection>;
    Links _links;
    std::mutex _mutex;
  };
}

#include <reactor/storage.hxx>

