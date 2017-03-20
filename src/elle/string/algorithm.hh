#ifndef ELLE_STRING_ALGORTIHM
#define ELLE_STRING_ALGORTIHM

#include <string>
#include <sstream>

namespace elle
{
  /// Create a string by joining the elements of \a range with \a given
  /// separator and \an optional transformation function.
  ///
  /// @param it, ite The range to join.
  /// @param sep The separator to insert between entries.
  /// @param t A transformation function.
  /// @returns The resulting string.
  template <typename IT>
  std::string
  join(IT it, IT ite,
       std::string const& sep = ",",
       std::function<std::string (typename IT::value_type const&)> const& t = {});

  /// Create a string by joining the elements of \a container with \a given
  /// separator and \an optional transformation function.
  ///
  /// @param container The container to join.
  /// @param sep The separator to insert between entries.
  /// @param t A transformation function.
  /// @returns The resulting string.
  template <typename C>
  std::string
  join(C const& container,
       std::string const& sep = ",",
       std::function<std::string (typename C::const_reference)> const& t = {});
}

#include <elle/string/algorithm.hxx>

#endif /* end of include guard: ELLE_STRING_ALGORTIHM */
