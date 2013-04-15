#ifndef ELLE_UTILITY_MOVE_HH
# define ELLE_UTILITY_MOVE_HH

namespace elle
{
  namespace utility
  {

    template<typename T>
    struct move_on_copy_wrapper
    {
      mutable T value;

      move_on_copy_wrapper(T&& t):
          value(std::move(t))
      {}

      move_on_copy_wrapper(move_on_copy_wrapper const& other):
          value(std::move(other.value))
      {}

      move_on_copy_wrapper(move_on_copy_wrapper&& other):
          value(std::move(other.value))
      {}

      move_on_copy_wrapper& operator = (move_on_copy_wrapper const& other)
      {
        value = std::move(other.value);
        return *this;
      }

      move_on_copy_wrapper& operator = (move_on_copy_wrapper&& other)
      {
        value = std::move(other.value);
        return *this;
      }
    };
  } /* utility */
} /* elle */

#endif
