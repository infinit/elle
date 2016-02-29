#ifndef ELLE_BYTES_HH
# define ELLE_BYTES_HH

# include <cstddef>

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
  return size * 1000000;
}

inline
std::size_t
operator "" _gB(unsigned long long size)
{
  return size * 1000000000;
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

#endif
