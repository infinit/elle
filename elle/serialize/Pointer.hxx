#ifndef  ELLE_SERIALIZE_POINTER_HXX
# define ELLE_SERIALIZE_POINTER_HXX

# include <memory>
# include <stdexcept>
# include <type_traits>

# include <elle/serialize/Serializer.hh>

# include <elle/serialize/Pointer.hh>

namespace elle
{
  namespace serialize
  {

    template<typename T>
    class Pointer
    {
      friend class Serializer<Pointer<T>>;
    public:
      typedef T pointer_type;
      static_assert(
          std::is_pointer<pointer_type>::value,
          "Cannot use alive pointer with a non pointer type."
      );
    private:
      pointer_type& _ptr;
    public:
      Pointer(pointer_type& ptr)
        : _ptr(ptr)
      {}

      Pointer(Pointer&& other)
        : _ptr(other._ptr)
      {}

      Pointer(Pointer const& other)
        : _ptr(other._ptr)
      {}
    };

    template<typename T>
    class AlivePointer
    {
      friend class Serializer<AlivePointer<T>>;
    public:
      typedef T pointer_type;
      static_assert(
          std::is_pointer<pointer_type>::value,
          "Cannot use alive pointer with a non pointer type."
      );
    private:
      pointer_type& _ptr;
    public:
      AlivePointer(pointer_type& ptr)
        : _ptr(ptr)
      {}

      AlivePointer(AlivePointer&& other)
        : _ptr(other._ptr)
      {}

      AlivePointer(AlivePointer const& other)
        : _ptr(other._ptr)
      {}
    };

    template <typename T>
    inline
    Pointer<typename std::remove_reference<T>::type>
    pointer(T&& ptr)
    {
      return Pointer<typename std::remove_reference<T>::type>{
        std::forward<T>(ptr)
      };
    }

    template
    <typename T>
    inline
    AlivePointer<typename std::remove_reference<T>::type>
    alive_pointer(T&& ptr)
    {
      return AlivePointer<typename std::remove_reference<T>::type>{
        std::forward<T>(ptr)
      };
    }

    //
    // We do not use macros here because of the special requirements of the
    // Pointer and AlivePointer classes. As the class is a container, it can be
    // passed as a lvalue reference, but still serialize stored pointer.
    //

    template<typename T1>
    struct Serializer<Pointer<T1>>:
      public BaseSerializer<Pointer<T1>>
    {
      // Load method (note the const&)
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Input
        >::type       Serialize(Archive&              archive,
                                Pointer<T1> const&    value,
                                unsigned int          version)
        {
          ELLE_LOG_COMPONENT("elle.serialize.Pointer");

          BaseSerializer<Pointer<T1>>::enforce(version == 0);

          if (value._ptr != nullptr)
            {
              ELLE_WARN("deleting the previous pointed value");
            }

          delete value._ptr;
          value._ptr = nullptr;

          bool not_null;
          archive >> not_null;

          typedef typename std::remove_pointer<T1>::type value_type;
          if (not_null)
            value._ptr = archive.template Construct<value_type>().release();
        }

      // Save
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Output
        >::type       Serialize(Archive&              archive,
                                Pointer<T1> const&    value,
                                unsigned int          version)
        {
          BaseSerializer<Pointer<T1>>::enforce(version == 0);
          if (value._ptr != nullptr)
            {
              archive << true
                      << *(value._ptr);
            }
          else
            {
              archive << false;
            }
        }
      template<typename Archive>
        static inline void LoadConstruct(Archive&, Pointer<T1>*, unsigned int)
        {
          static_assert(Archive::mode, "You cannot construct a pointer container !");
        }
    };

    template<typename T1>
    struct Serializer<AlivePointer<T1>>:
      public BaseSerializer<Pointer<T1>>
    {
      // Load method (note the const&)
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Input
        >::type       Serialize(Archive&                  archive,
                                AlivePointer<T1> const&   value,
                                unsigned int              version)
        {
          ELLE_LOG_COMPONENT("elle.serialize.Pointer");

          BaseSerializer<AlivePointer<T1>>::enforce(version == 0);

          if (value._ptr != nullptr)
            {
              ELLE_DEBUG("deleting the previous pointed value");
            }

          delete value._ptr;
          value._ptr = nullptr;

          typedef typename std::remove_pointer<T1>::type value_type;
          value._ptr = archive.template Construct<value_type>().release();
        }

      // Save
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Output
        >::type       Serialize(Archive&                  archive,
                                AlivePointer<T1> const&   value,
                                unsigned int              version)
        {
          BaseSerializer<Pointer<T1>>::enforce(version == 0);
          if (value._ptr == nullptr)
            throw std::runtime_error("Pointer is null, cannot archive it");
          archive << *(value._ptr);
        }

      template<typename Archive>
        static inline void LoadConstruct(Archive&, AlivePointer<T1>*, unsigned int)
        {
          static_assert(Archive::mode, "You cannot construct a pointer container !");
        }
    };


    template<typename T>
      struct StoreFormat<Pointer<T>>
      {
        static bool const value = false;
      };

    template<typename T>
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
  archive >> pointer(v);
  value.reset(v);
}

ELLE_SERIALIZE_SPLIT_T1_SAVE(std::unique_ptr, archive, value, version)
{
  T1* p(value.get());
  archive << pointer(p);
}

#endif
