#pragma once

#include <unordered_map>

#include <elle/err.hh>
#include <elle/printf.hh>

namespace elle
{
  template <class Key,
            class T,
            class Hash = std::hash<Key>,
            class Pred = std::equal_to<Key>,
            class Alloc = std::allocator<std::pair<const Key, T>>>
  class unordered_map
    : public std::unordered_map<Key, T, Hash, Pred, Alloc>
  {
  public:
    typedef std::unordered_map<Key, T, Hash, Pred, Alloc> Super;

    template <typename ... Args>
    unordered_map(Args&& ... args)
      : Super(std::forward<Args>(args)...)
    {}

    unordered_map(std::initializer_list<typename Super::value_type> il)
      : Super(std::move(il))
    {}

    T&
    at(Key const& k)
    {
      try
      {
        return this->Super::at(k);
      }
      catch (std::out_of_range const&)
      {
        throw elle::Error(elle::sprintf("missing key: %s", k));
      }
    }

    const T&
    at(Key const& k) const
    {
      return const_cast<unordered_map<Key, T, Hash, Pred, Alloc>*>(this)->at(k);
    }
  };
}
