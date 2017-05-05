#pragma once

#include <cassert>
#include <random>
#include <vector>

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm_ext/iota.hpp>

namespace elle
{
  /// A unique random device.
  std::mt19937&
  random_engine();

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

  /// Random integer in [0, size-1].
  ///
  /// @return an iterator.
  template <typename Gen = std::mt19937>
  auto
  pick_one(int size, Gen& gen = random_engine())
  {
    auto random = std::uniform_int_distribution<>(0, size - 1);
    return random(gen);
  }

  /// Random selection in a range.
  ///
  /// @return an iterator.
  template <typename Range, typename Gen = std::mt19937>
  auto
  pick_one(Range& r, Gen& gen = random_engine())
    -> decltype(gen.seed(0),  // sfinae
                boost::begin(r))
  {
    if (boost::empty(r))
      return r.end();
    else
      return std::next(r.begin(), pick_one(boost::size(r), gen));
  }

  /// Random selection in a range with a filter.
  ///
  /// @return an iterator.
  template <typename Range, typename Pred, typename Gen = std::mt19937>
  auto
  pick_one(Range& r, Pred pred, Gen& gen = random_engine())
    -> decltype(pred(*boost::begin(r)),  // sfinae
                boost::begin(r))
  {
    auto&& filtered = boost::adaptors::filter(r, pred);
    return pick_one(filtered, gen).base();
  }

  /// A (sorted) range of n numbers to choose in [0, size - 1].
  ///
  /// @pre n < size(r).
  template <typename Gen = std::mt19937>
  auto
  pick_n(int count, int size, Gen& gen = random_engine())
  {
    assert(count <= size);
    auto res = std::vector<int>{};
    if (count < size)
    {
      while (res.size() < static_cast<unsigned int>(count))
      {
        auto random = std::uniform_int_distribution<>(0, size - 1 - res.size());
        int v = random(gen);
        for (auto r: res)
          if (v >= r)
            ++v;
        res.push_back(v);
        boost::sort(res);
      }
    }
    else
    {
      res.resize(size);
      boost::iota(res, 0);
    }
    return res;
  }

  /// From a list of indexes in r, make a list of iterators.
  template <typename Range, typename Indexes>
  auto
  select(Range& r, Indexes const& is)
  {
    using Iterator = decltype(boost::begin(r));
    auto res = std::vector<Iterator>{};
    res.reserve(boost::size(is));
    // Don't require direct access iterators.
    auto it = r.begin();
    auto prev = 0;
    for (auto i: is)
    {
      std::advance(it, i - prev);
      prev = i;
      res.push_back(it);
    }
    return res;
  }

  /// A (sorted) range of n iterators in a container.
  ///
  /// @pre n < size(r).
  template <typename Range, typename Gen = std::mt19937>
  auto
  pick_n(int count, Range& r, Gen& gen = random_engine())
  {
    return select(r, pick_n(count, boost::size(r), gen));
  }
}
