#ifndef ELLE_UTILITY_MOVE_HXX
# define ELLE_UTILITY_MOVE_HXX

namespace elle
{
  namespace utility
  {
    template <typename T>
    Move<T>::Move(T&& t):
      value(std::move(t))
    {}

    template <typename T>
    Move<T>::Move(Move const& other):
      value(std::move(other.value))
    {}

    template <typename T>
    Move<T>::Move(Move&& other):
        value{std::move(other.value)}
      {}

    template <typename T>
    Move<T>&
    Move<T>::operator =(Move const& other)
    {
      value = std::move(other.value);
      return *this;
    }

    template <typename T>
    Move<T>&
    Move<T>::operator =(Move&& other)
    {
      value = std::move(other.value);
      return *this;
    }

    template <typename T>
    Move<T>
    move_on_copy(T&& v)
    {
      return Move<T>(std::forward<T>(v));
    }
  }
}

#endif
