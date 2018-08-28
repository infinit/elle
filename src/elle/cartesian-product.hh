#pragma once

namespace elle
{
  /// Yield all possible combinations of the elements of the given ranges.
  ///
  /// Product instances must be callable and are used to combine every two
  /// values. The default MakePair build a std::pair.
  template <typename Product = void, typename LRange, typename RRange>
  auto
  cartesian_product(LRange&& lhs, RRange&& rhs);
}

#include <elle/cartesian-product.hxx>
