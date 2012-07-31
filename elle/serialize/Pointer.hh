#ifndef  ELLE_SERIALIZE_POINTER_HH
# define ELLE_SERIALIZE_POINTER_HH

namespace elle
{
  namespace serialize
  {

    ///
    /// Handy class to serialize pointed values. This class does NOT provide a
    /// way to store memory address. It will store a boolean and the pointed
    /// value when the pointer is not null.
    ///
    /// -----------------------------------------------------------------------
    /// struct A { int* i; };
    /// ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
    /// {
    ///   archive & Pointer<int>(value.i); // i may or may not be null.
    /// }
    /// -----------------------------------------------------------------------
    ///
    ///
    /// @see elle::serialize::pointer(T*) to infer this type.
    ///
    template<typename T>
    class Pointer;

    ///
    /// Alive pointer acts the same as Pointer<T> class, except it will throw
    /// an error if the pointer is null.
    ///
    /// -----------------------------------------------------------------------
    /// struct A { int* i; };
    /// ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
    /// {
    ///   archive & AlivePointer<int>(value.i); // i should not be null.
    /// }
    /// -----------------------------------------------------------------------
    ///
    template<typename T>
    class AlivePointer;

    ///
    /// Infer the Pointer<T> type.
    ///
    /// -----------------------------------------------------------------------
    /// struct A { int* i; };
    /// ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
    /// {
    ///   archive & pointer(value.i); // i may or may not be null.
    /// }
    /// -----------------------------------------------------------------------
    ///
    template<typename T> inline
    Pointer<T>
    pointer(T*& ptr);

    ///
    /// Infer the AlivePointer<T> type.
    ///
    /// -----------------------------------------------------------------------
    /// struct A { int* i; };
    /// ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
    /// {
    ///   archive & alive_pointer(value.i); // i should not be null.
    /// }
    /// -----------------------------------------------------------------------
    ///
    template<typename T> inline
    AlivePointer<T>
    alive_pointer(T*& ptr);

  }
}

# include "Pointer.hxx"

#endif /* ! POINTER_HH */

