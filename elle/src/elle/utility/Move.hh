#ifndef ELLE_UTILITY_MOVE_HH
# define ELLE_UTILITY_MOVE_HH

# include <algorithm>

namespace elle
{
  namespace utility
  {
    template <typename T>
    struct move_on_copy
    {
      mutable T value;

      move_on_copy(T&& t):
        value{std::move(t)}
      {}

      move_on_copy(move_on_copy const& other):
        value{std::move(other.value)}
      {}

      move_on_copy(move_on_copy&& other):
        value{std::move(other.value)}
      {}

      move_on_copy&
      operator =(move_on_copy const& other)
      {
        value = std::move(other.value);
        return *this;
      }

      move_on_copy&
      operator =(move_on_copy&& other)
      {
        value = std::move(other.value);
        return *this;
      }
    };
  } /* utility */
} /* elle */

#endif
