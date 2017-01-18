#pragma once

#include <unordered_map>

#include <elle/err.hh>
#include <elle/printf.hh>

namespace elle
{
  template <typename... Args>
  class unordered_map
    : public std::unordered_map<Args...>
  {
  public:
    using Self = unordered_map;
    using Super = std::unordered_map<Args...>;
    using Super::Super;
    using key_type = typename Super::key_type;
    using mapped_type = typename Super::mapped_type;

    mapped_type&
    at(key_type const& k)
    {
      try
      {
        return this->Super::at(k);
      }
      catch (std::out_of_range const&)
      {
        elle::err("missing key: %s", k);
      }
    }

    mapped_type const&
    at(key_type const& k) const
    {
      return const_cast<Self*>(this)->at(k);
    }
  };
}
