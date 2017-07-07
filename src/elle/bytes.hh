#pragma once

#include <cstddef>
#include <string>

#include <boost/optional.hpp>

inline
std::size_t
operator "" _B(unsigned long long size)
{
  return size;
}

inline
std::size_t
operator "" _kB(unsigned long long size)
{
  return size * 1000;
}

inline
std::size_t
operator "" _MB(unsigned long long size)
{
  return size * 1'000'000;
}

inline
std::size_t
operator "" _GB(unsigned long long size)
{
  return size * 1000'000'000;
}

inline
std::size_t
operator "" _KiB(unsigned long long size)
{
  return size * 1024;
}

inline
std::size_t
operator "" _MiB(unsigned long long size)
{
  return size * 1024 * 1024;
}

inline
std::size_t
operator "" _GiB(unsigned long long size)
{
  return size * 1024 * 1024 * 1024;
}

namespace elle
{
  /// Pretty-print a number of bytes.
  ///
  /// @param bytes  The number of bytes.
  /// @param si     Whether to use scientific notation (KB vs KiB, etc.).
  std::string
  human_data_size(double bytes, bool si = true);

  /// Convert an optional capacity in string into an optional capacity.
  boost::optional<int64_t>
  convert_capacity(boost::optional<std::string> capacity);

  /// Parse a capacity with embeded unit.
  ///
  /// For instance "10KB" => 10000, "10KiB" => 10240.
  int64_t convert_capacity(std::string value);

  /// Parse a capacity, with its unit on the side.
  long double
  convert_capacity(long double value, std::string unit);
}
