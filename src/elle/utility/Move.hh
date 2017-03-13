#ifndef ELLE_UTILITY_MOVE_HH
# define ELLE_UTILITY_MOVE_HH

# include <algorithm>
# include <type_traits>


namespace elle
{
  namespace utility
  {
    /// A class that move the underlying value when copied.
    ///
    /// This was useful when move captures were not supported (pre C++14).
    ///
    /// \code{.cc}
    ///
    /// auto val = std::make_unique<X>();
    /// auto p = elle::utility::move_on_copy(std::move(val));
    /// assert(val.get() == nullptr);
    /// assert(p.value != nullptr);
    /// auto r = [p] { assert(p.value != nullptr); return (**p); };
    /// assert(p.value == nullptr);
    ///
    /// \endcode
    template <typename T>
    struct Move
    {
    public:
      /// Create a Move from an r-value reference of the value.
      ///
      /// @param t The value.
      Move(T&& t);
      /// Create a Move from a another Move object.
      ///
      /// N.B. The value is moved.
      ///
      /// @param other An other Move object.
      Move(Move const& other);
      /// Create a Move from an r-value of a Move object.
      ///
      /// @param other An r-value reference to a Move object.
      Move(Move&& other);
      /// Assign us the given Move value.
      ///
      /// N.B. The value is moved.
      ///
      /// @param other An other Move object.
      Move&
      operator =(Move const& other);
      /// Assign us the given Move value.
      ///
      /// N.B. The value is moved.
      ///
      /// @param other An r-value reference to a Move object.
      Move&
      operator =(Move&& other);
      /// Return the underlying value, moved.
      operator T () const;
      /// Return a reference to the underlying value.
      T&
      operator -> () const;
      /// Return a reference to the underlying value.
      T&
      operator * () const;
    public:
      mutable T value;
    };

    /// Helper to create a Move object.
    ///
    /// @param v An r-value reference of the value.
    /// @returns A Move object.
    template <typename T>
    Move<typename std::remove_reference<T>::type>
    move_on_copy(T&& v);
  }
}

# include <elle/utility/Move.hxx>

#endif
