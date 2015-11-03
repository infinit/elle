#include <elle/container/set.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <typename V, typename T, typename ...OTHER>
  bool
  contains(unordered_set<T, OTHER...> const& set,
           V&& value)
  {
    return set.find(std::forward<V>(value)) != set.end();
  }
}
