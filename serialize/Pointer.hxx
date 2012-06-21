#ifndef  ELLE_SERIALIZE_POINTERSERIALIZER_HXX
# define ELLE_SERIALIZE_POINTERSERIALIZER_HXX

# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/serialize/Pointer.hh>

namespace elle { namespace serialize {

    //
    // We do not use macros here because of the special requirements of the
    // Pointer and AlivePointer classes. As the class is a container, it can be
    // passed as a lvalue reference, but still serialize stored pointer.
    //

    template<typename T1>
    struct ArchiveSerializer<Pointer<T1>>
    {
      // Load method (note the const&)
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Input
        >::type       Serialize(Archive&              archive,
                                Pointer<T1> const&    value,
                                unsigned int          version)
        {
          assert(version == 0);

          delete value._ptr;
          value._ptr = nullptr;

          bool not_null;
          archive >> not_null;

          if (not_null)
            value._ptr = archive.template Construct<T1>().release();
        }

      // Save
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Output
        >::type       Serialize(Archive&              archive,
                                Pointer<T1> const&    value,
                                unsigned int          version)
        {
          assert(version == 0);
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
    struct ArchiveSerializer<AlivePointer<T1>>
    {
      // Load method (note the const&)
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Input
        >::type       Serialize(Archive&                  archive,
                                AlivePointer<T1> const&   value,
                                unsigned int              version)
        {
          assert(version == 0);

          delete value._ptr;
          value._ptr = nullptr;

          value._ptr = archive.template Construct<T1>().release();
        }

      // Save
      template<typename Archive>
        static inline typename std::enable_if<
            Archive::mode == ArchiveMode::Output
        >::type       Serialize(Archive&                  archive,
                                AlivePointer<T1> const&   value,
                                unsigned int              version)
        {
          assert(version == 0);
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
      struct StoreClassVersion<Pointer<T>>
      {
        static bool const value = false;
      };

    template<typename T>
      struct StoreClassVersion<AlivePointer<T>>
      {
        static bool const value = false;
      };

  }
}

#endif
