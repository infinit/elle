#ifndef ELLE_UTILITY_MOVE_HH
# define ELLE_UTILITY_MOVE_HH

# include <algorithm>
# include <type_traits>


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
      operator T () const;
      T&
      operator -> () const;
      T&
      operator * () const;
    public:
      mutable T value;
    };

    template <typename T>
    Move<typename std::remove_reference<T>::type>
    move_on_copy(T&& v);
  }
}

# include <elle/utility/Move.hxx>

#endif
