#pragma once

#include <cstddef>
#include <string>

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
operator "" _mB(unsigned long long size)
{
  return size * 1'000'000;
}

inline
std::size_t
operator "" _gB(unsigned long long size)
{
  return size * 1000'000'000;
}

inline
std::size_t
operator "" _kiB(unsigned long long size)
{
  return size * 1024;
}

inline
std::size_t
operator "" _miB(unsigned long long size)
{
  return size * 1024 * 1024;
}

inline
std::size_t
operator "" _giB(unsigned long long size)
{
  return size * 1024 * 1024 * 1024;
}

namespace elle
{
  /// Pretty-print a number of bytes.
  ///
  /// \param bytes  the number of bytes.
  /// \param si     whether to use KB, MB instead of KiB, MiB, etc.
  std::string
  human_data_size(double bytes, bool si = true);
}
