#pragma once

#include <algorithm>
#include <set>
#include <vector>

#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/size.hpp>

#include <elle/meta.hh>
#include <elle/range.hh>

namespace elle
{
  /*------.
  | Set.  |
  `------*/

  /// A set of the values of `fun(e)` for e in c.
  ///
  /// @param c    A container to apply the function to.
  /// @param fun  The function to apply.
  /// @returns A set of images.
  template <typename Cont, typename Fun>
  auto
  make_set(const Cont& c, Fun&& fun)
    -> std::set<decltype(fun(*c.begin()))> // SFINAE
  {
    using Res = decltype(fun(*c.begin()));
    auto res = std::set<Res>{};
    boost::transform(c,
                     std::inserter(res, res.end()),
                     std::forward<Fun>(fun));
    return res;
  }

  /// The content of @a cont as a set.
  ///
  /// @param c  A container.
  /// @returns  A set.
  template <typename Cont>
  auto
  make_set(const Cont& c)
    -> std::set<typename Cont::value_type>
  {
    using std::begin;
    using std::end;
    return {begin(c), end(c)};
  }


  /*---------.
  | Vector.  |
  `---------*/

  /// This is really the `map` function in functional languages:
  /// a vector of the values of `fun(e)` for e in c.
  ///
  /// @param c    A container to apply the function to.
  /// @param fun  The function to apply.
  /// @returns A set containing the result of the map.
  template <typename Cont, typename Fun>
  auto
  make_vector(const Cont& c, Fun&& fun)
    -> std::vector<decltype(fun(*c.begin()))> // SFINAE
  {
    using Res = decltype(fun(*c.begin()));
    auto res = std::vector<Res>{};
    res.reserve(boost::size(c));
    boost::transform(c,
                     std::back_inserter(res),
                     std::forward<Fun>(fun));
    return res;
  }

  namespace detail
  {
    template <typename T>
    struct
    make_vector_move
    {
      static bool constexpr value = true;
    };

    template <typename T>
    struct
    make_vector_move<::elle::detail::range<T>>
    {
      static bool constexpr value = false;
    };
  }

  /// The content of @a cont as a vector.
  ///
  /// @param c   A container.
  /// @returns   A set.
  template <typename R = void,
            typename Cont>
  auto
  make_vector(Cont&& c)
    -> std::conditional_t<
        std::is_same<R, void>::value,
        std::vector<typename std::remove_reference_t<Cont>::value_type>,
        R>
  {
    using std::begin;
    using std::end;
    using Res = decltype(make_vector<R, Cont>(std::forward<Cont>(c)));
    return elle::meta::static_if<
      std::is_same<std::remove_reference_t<Cont>, Cont>::value &&
      detail::make_vector_move<std::remove_reference_t<Cont>>::value >(
        [] (auto&& c) -> Res
        {
          return {
            std::make_move_iterator(c.begin()),
            std::make_move_iterator(c.end()),
          };
        },
        [] (auto const& c) -> Res
        {
          return {begin(c), end(c)};
        })(std::forward<Cont>(c));
  }

  /// This is really the `filter` function in functional languages:
  /// a vector of the values of `c` that validate `pred`.
  ///
  /// @param c     A container to apply the function to.
  /// @param pred  A predicate.
  /// @returns A set containing the result of the map.
  template <typename Cont, typename Pred>
  auto
  make_vector_if(const Cont& c, Pred&& pred)
  {
    auto res = std::vector<typename Cont::value_type>{};
    boost::algorithm::copy_if(c,
                              std::back_inserter(res),
                              std::forward<Pred>(pred));
    return res;
  }
}
