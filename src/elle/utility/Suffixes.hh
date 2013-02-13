#pragma once
#ifndef SUFFIXES_491WIJQ0
#define SUFFIXES_491WIJQ0

#include <bitset>

namespace elle {
  namespace suffix {
      template<char... Bits>
      constexpr std::bitset<sizeof...(Bits)>
      operator"" _bits() noexcept;
  } /* suffix */
} /* elle */

#include <elle/utility/Suffixes.hxx>

#endif /* end of include guard: SUFFIXES_491WIJQ0 */
