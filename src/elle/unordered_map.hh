#pragma once

#include <unordered_map>

#include <elle/err.hh>
#include <elle/printf.hh>

namespace elle
{
  /// A unordered_map that throws a more relevant exception when querying a
  /// element that doesn't exist. The resulting exception contains... the name
  /// of the key.
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

    /// Returns a reference to the mapped value of the element with key
    /// equivalent to key. If no such element exists, an exception of type
    /// elle::Error is thrown.
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
