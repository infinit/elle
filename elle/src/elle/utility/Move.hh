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
    public:
      Move(T&& t);
      Move(Move const& other);
      Move(Move&& other);
      Move&
      operator =(Move const& other);
      Move&
      operator =(Move&& other);

    public:
      mutable T value;
    };

    template <typename T>
    Move<T>
    move_on_copy(T&& v);
  }
}

# include <elle/utility/Move.hxx>

#endif
