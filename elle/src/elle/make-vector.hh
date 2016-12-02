#include <vector>
#include <algorithm>

namespace elle
{
  /// This is really the `map` function in functional languages:
  /// a vector of the values of `fun(e)` for e in c.
  template <typename Container, typename Fun>
  auto
  make_vector(Container& c, Fun&& fun)
    -> std::vector<decltype(fun(*c.begin()))> // SFINAE
  {
    using Res = decltype(fun(*c.begin()));
    auto res = std::vector<Res>{};
    res.reserve(c.size());
    std::transform(c.begin(), c.end(),
                   std::back_inserter(res),
                   std::forward<Fun>(fun));
    return res;
  }

  /// The content of \a cont as a vector.
  template <typename Cont>
  std::vector<typename Cont::value_type>
  make_vector(const Cont& cont)
  {
    using std::begin;
    using std::end;
    return {begin(cont), end(cont)};
  }
}

