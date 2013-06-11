#ifndef  ELLE_SERIALIZE_POINTER_HXX
# define ELLE_SERIALIZE_POINTER_HXX

# include "Pointer.hh"
# include "Serializer.hh"

# include <elle/log.hh>

# include <memory>
# include <stdexcept>
# include <type_traits>

namespace elle
{
  namespace serialize
  {
    template <typename T>
    class Pointer
    {
    public:
      typedef typename std::remove_reference<T>::type pointer_type;
      static_assert(std::is_pointer<pointer_type>::value,
                    "Cannot use pointer with a non pointer type.");
      typedef
        typename std::remove_cv<
          typename std::remove_pointer<pointer_type>::type
        >::type
        value_type;
      friend struct Serializer<Pointer<T>>;

    protected:
      T _ptr;
    public:
      Pointer(T ptr):
        _ptr(ptr)
      {}

      Pointer(Pointer&& other):
        _ptr(other._ptr)
      {}

      Pointer(Pointer const& other):
        _ptr(other._ptr)
      {}
    };

    template <typename T>
    class AlivePointer:
      Pointer<T>
    {
      friend struct Serializer<AlivePointer<T>>;

    public:
      AlivePointer(T ptr):
        Pointer<T>(ptr)
      {}

      AlivePointer(AlivePointer&& other):
        Pointer<T>(other)
      {}

      AlivePointer(AlivePointer const& other):
        Pointer<T>(other)
      {}
    };

    template <typename T>
    inline
    Pointer<T>
    pointer(T&& ptr)
    {
      return Pointer<T>{std::forward<T>(ptr)};
    }

    template <typename T>
    inline
    AlivePointer<T>
    alive_pointer(T&& ptr)
    {
      return AlivePointer<T>{std::forward<T>(ptr)};
    }

    //
    // We do not use macros here because of the special requirements of the
    // Pointer and AlivePointer classes. As the class is a container, it can be
    // passed as a lvalue reference, but still serialize stored pointer.
    //

    template <typename T1>
    struct Serializer<Pointer<T1>>:
      public BaseSerializer<Pointer<T1>>
    {
      // Load method (note the const&)
      template <typename Archive>
      static inline
      typename std::enable_if<Archive::mode == ArchiveMode::Input>::type
      Serialize(Archive& archive,
                Pointer<T1> const& value,
                unsigned int version)
      {
        ELLE_LOG_COMPONENT("elle.serialize.Pointer");

        BaseSerializer<Pointer<T1>>::enforce(version == 0);

        if (value._ptr != nullptr)
        {
          ELLE_WARN("deleting the previous pointed value");
          delete value._ptr;
          value._ptr = nullptr;
        }

        bool not_null;
        archive >> not_null;

        typedef typename Pointer<T1>::value_type value_type;
        if (not_null)
          value._ptr = archive.template Construct<value_type>().release();
      }

      // Save
      template <typename Archive>
      static inline
      typename std::enable_if<Archive::mode == ArchiveMode::Output>::type
      Serialize(Archive& archive,
                Pointer<T1> const& value,
                unsigned int version)
      {
        BaseSerializer<Pointer<T1>>::enforce(version == 0);
        if (value._ptr != nullptr)
          archive << true << *(value._ptr);
        else
          archive << false;
      }
      template <typename Archive>
      static inline
      void
      LoadConstruct(Archive&,
                    Pointer<T1>*,
                    unsigned int)
      {
        static_assert(Archive::mode,
                      "You cannot construct a pointer container !");
      }
    };

    template <typename T1>
    struct Serializer<AlivePointer<T1>>:
      public BaseSerializer<Pointer<T1>>
    {
      // Load method (note the const&)
      template <typename Archive>
      static inline
      typename std::enable_if<Archive::mode == ArchiveMode::Input>::type
      Serialize(Archive& archive,
                AlivePointer<T1> const& value,
                unsigned int version)
      {
        ELLE_LOG_COMPONENT("elle.serialize.Pointer");

        BaseSerializer<AlivePointer<T1>>::enforce(version == 0);

        if (value._ptr != nullptr)
        {
          ELLE_WARN("deleting the previous pointed value");
          delete value._ptr;
          value._ptr = nullptr;
        }

        typedef typename AlivePointer<T1>::value_type value_type;
        value._ptr = archive.template Construct<value_type>().release();
      }

      // Save
      template<typename Archive>
      static inline
      typename std::enable_if<Archive::mode == ArchiveMode::Output>::type
      Serialize(Archive& archive,
                AlivePointer<T1> const& value,
                unsigned int version)
      {
        BaseSerializer<Pointer<T1>>::enforce(version == 0);
        if (value._ptr == nullptr)
          throw std::runtime_error("Pointer is null, cannot archive it");
        archive << *(value._ptr);
      }

      template <typename Archive>
      static inline
      void
      LoadConstruct(Archive&,
                    AlivePointer<T1>*,
                    unsigned int)
      {
        static_assert(Archive::mode,
                      "You cannot construct a pointer container !");
      }
    };


    template <typename T>
    struct StoreFormat<Pointer<T>>
    {
      static bool const value = false;
    };

    template <typename T>
    struct StoreFormat<AlivePointer<T>>
    {
      static bool const value = false;
    };
  }
}

ELLE_SERIALIZE_SPLIT_T1(std::unique_ptr);
ELLE_SERIALIZE_SPLIT_T1_LOAD(std::unique_ptr, archive, value, version)
{
  typename std::remove_cv<T1>::type* v = nullptr;
  (void)version;
  archive >> pointer(v);
  value.reset(v);
}

ELLE_SERIALIZE_SPLIT_T1_SAVE(std::unique_ptr, archive, value, version)
{
  (void)version;
  T1* p(value.get());
  archive << pointer(p);
}

#endif
