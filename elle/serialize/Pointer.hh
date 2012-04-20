#ifndef ELLE_SERIALIZE_POINTER_HH
# define ELLE_SERIALIZE_POINTER_HH

namespace elle
{
  namespace serialize
  {

    template<typename T> struct ArchiveSerializer;

    ///
    /// Handy class to serialize pointed values. This class does NOT provide a
    /// way to store memory address. It will store a boolean and the pointed
    /// value when the pointer is not null.
    ///
    /// @see elle::serialize::pointer(T*) to infer this type.
    ///
    template<typename T>
      class Pointer
      {
        friend class ArchiveSerializer<Pointer<T>>;
      private:
        T*& _ptr;
      public:
        Pointer(T*& ptr) : _ptr(ptr) {}
        Pointer(Pointer&& other) : _ptr(other._ptr) {}
      };

    /// Infer the Pointer<T> type.
    template<typename T>
      Pointer<T> pointer(T*& ptr)
      {
        return Pointer<T>(ptr);
      }

  }
}


#endif /* ! POINTER_HH */


