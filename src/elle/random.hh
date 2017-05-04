#pragma once

#include <cassert>
#include <random>

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>

namespace elle
{
  /// A unique random device.
  inline
  std::mt19937&
  random_engine()
  {
    static auto res = []
      {
        if (getenv("ELLE_SEED"))
          return std::mt19937{std::mt19937::default_seed};
        else
          {
            std::random_device rd{};
            return std::mt19937{rd()};
          }
      }();
    return res;
  }

  /// A uniform distribution in [0, size(r) - 1].
  ///
  /// @pre c is not empty.
  template <typename Range>
  auto
  uniform_index_distribution(Range const& r)
  {
    assert(!boost::empty(r));
    return std::uniform_int_distribution<>(0, boost::size(r) - 1);
  }

  /// Random selection in a range.
  ///
  /// @return an iterator.
  template <typename Range, typename Gen = std::mt19937>
  auto
  pick_one(Range& r, Gen& gen = random_engine())
  {
    if (boost::empty(r))
      return r.end();
    else
    {
      auto random = uniform_index_distribution(r);
      return std::next(r.begin(), random(gen));
    }
  }

  /// Random selection in a range with a filter.
  ///
  /// @return an iterator.
  template <typename Range, typename Pred, typename Gen = std::mt19937>
  auto
  pick_one(Range& r, Pred pred, Gen& gen = random_engine())
    -> decltype(pred(*begin(r)),  // sfinae
                begin(r))
  {
    auto&& filtered = boost::adaptors::filter(r, pred);
    return pick_one(filtered, gen).base();
  }
}
