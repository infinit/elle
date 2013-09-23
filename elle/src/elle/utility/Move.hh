#ifndef ELLE_UTILITY_MOVE_HH
# define ELLE_UTILITY_MOVE_HH

# include <algorithm>

namespace elle
{
  namespace utility
  {
    template <typename T>
    struct Move
    {
      mutable T value;

      Move(T&& t):
        value{std::move(t)}
      {}

      Move(Move const& other):
        value{std::move(other.value)}
      {}

      Move(Move&& other):
        value{std::move(other.value)}
      {}

      Move&
      operator =(Move const& other)
      {
        value = std::move(other.value);
        return *this;
      }

      Move&
      operator =(Move&& other)
      {
        value = std::move(other.value);
        return *this;
      }
    };

    template <typename T>
    Move<T>
    move_on_copy(T&& v)
    {
      return Move<T>(std::forward<T>(v));
    }
  }
}

#endif
