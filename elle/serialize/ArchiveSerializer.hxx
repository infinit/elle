#ifndef ELLE_SERIALIZE_ARCHIVESERIALIZER_HXX
# define ELLE_SERIALIZE_ARCHIVESERIALIZER_HXX

namespace elle { namespace serialize {

    ///
    /// The BaseArchiveSerializer define common standard function involved in
    /// the serialization process. You might want to derive from this struct for
    /// each specialization of the template class ArchiveSerializer.
    ///
    template<typename T> struct BaseArchiveSerializer
    {
      ///
      /// Having a chance to construct yourself the object (with a placement new)
      /// can be achieved by overriding this function.
      /// The default implementation just construct the object without parameters
      /// and forward to standard Load() method.
      ///
      template<typename Archive>
      static inline void LoadConstruct(Archive&, T* ptr, unsigned int)
      {
        assert(ptr != nullptr);
        static_assert(!std::is_pointer<T>::value, "You cannot construct pointers...");
        new (ptr) T;
      }

    };

    ///
    /// This type is instanciated for any user defined type. You will have to
    /// make a specialization for each user defined type.
    ///
    template<typename T> struct ArchiveSerializer : BaseArchiveSerializer<T>
    {
      ///
      /// Generic method used by either serialization and deserialization
      /// process. You may want to use the '&' operator that do the right
      /// operation depending on the archive mode.
      ///
      template<typename Archive>
      static void Serialize(Archive&, T&, unsigned int)
      {
        static_assert(
            ArchivableClass<T>::version != 0,
            "You have to specialize ArchiveSerializer<T> for your type"
        );
      }
    };
# define ELLE_SERIALIZE_SIMPLE_SERIALIZER(T, archive, value, version)             \
namespace elle { namespace serialize {                                            \
    template<typename T> struct ArchiveSerializer<T> :                            \
      public BaseArchiveSerializer<T> {                                           \
        template<typename Archive>                                                \
          static void Serialize(Archive&, T&, unsigned int);                      \
      };                                                                          \
}}                                                                                \
    template<typename T> template<typename Archive>                               \
          static void elle::serialize::ArchiveSerializer<T>::Serialize(           \
                                                      Archive& archive,           \
                                                      T& value,                   \
                                                      unsigned int version)       \

    template<typename T, ArchiveMode mode> struct _SplittedSerializerSelectMethod;

    template<typename T>
      struct _SplittedSerializerSelectMethod<T, ArchiveMode::Input>
      {
        template<typename Archive>
          static inline void Serialize(Archive& ar, T& val, unsigned int version)
          {
            ArchiveSerializer<T>::Load(ar, val, version);
          }
      };

    template<typename T>
      struct _SplittedSerializerSelectMethod<T, ArchiveMode::Output>
      {
        template<typename Archive>
        static inline void Serialize(Archive& ar, T const& val, unsigned int version)
        {
          ArchiveSerializer<T>::Save(ar, val, version);
        }
      };

    template<typename T>
      struct SplittedSerializer : BaseArchiveSerializer<T>
      {
        template<typename Archive>
        static inline void Serialize(Archive& ar, T& val, unsigned int version)
        {
          _SplittedSerializerSelectMethod<T, Archive::mode>::Serialize(ar, val, version);
        }
      };

}}

#endif
