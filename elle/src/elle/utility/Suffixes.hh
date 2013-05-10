#ifndef ELLE_UTILITY_SUFFIXES_HH
# define ELLE_UTILITY_SUFFIXES_HH

# include <bitset>

namespace elle
{
  namespace suffix
  {
    template<char... Bits>
    constexpr
    std::bitset<sizeof...(Bits)>
    operator"" _bits() noexcept;
  } /* suffix */
} /* elle */

# include <elle/utility/Suffixes.hxx>

#endif /* end of include guard: ELLE_UTILITY_SUFFIXES_HH */
