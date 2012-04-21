#ifndef ELLE_SERIALIZE_ARCHIVESERIALIZER_HXX
# define ELLE_SERIALIZE_ARCHIVESERIALIZER_HXX

# include <type_traits>

# include "ArchivableClass.hh"
# include "ArchiveMode.hh"

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
    /// @see ELLE_SERIALIZE_SIMPLE() for an helper
    ///
    template<typename T> struct ArchiveSerializer
      : public BaseArchiveSerializer<T>
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
            "You have to specialize ArchiveSerializer<T> for your type."
        );
      }
    };


      namespace detail {

          // Call the Save() or Load() depending on the archive mode.
          template<typename T, ArchiveMode mode>
            struct _SplitSerializerSelectMethod;

          template<typename T>
            struct _SplitSerializerSelectMethod<T, ArchiveMode::Input>
            {
              template<typename Archive>
                static inline void Serialize(Archive& ar,
                                             T& val,
                                             unsigned int version)
                { ArchiveSerializer<T>::Load(ar, val, version); }
            };

          template<typename T>
            struct _SplitSerializerSelectMethod<T, ArchiveMode::Output>
            {
              template<typename Archive>
              static inline void Serialize(Archive& ar,
                                           T const& val,
                                           unsigned int version)
              { ArchiveSerializer<T>::Save(ar, val, version); }
            };

      } // !namespace detail

      /// Home made serializer specialization can inherit this class to inherit
      /// a split serializer out of the box.
      template<typename T>
        struct SplitSerializer : BaseArchiveSerializer<T>
        {
          template<typename Archive>
          static inline void Serialize(Archive& ar,
                                       T& val,
                                       unsigned int version)
          {
            typedef detail::_SplitSerializerSelectMethod<
                T
              , Archive::mode
            > Method;
            Method::Serialize(ar, val, version);
          }
        };

}} // !namespace elle::serialize

/// Define a simple serializer for the type T
# define ELLE_SERIALIZE_SIMPLE(T, archive, value, version)                      \
namespace elle { namespace serialize {                                          \
    template<> struct ArchiveSerializer<T>                                      \
      : public BaseArchiveSerializer<T>                                         \
      {                                                                         \
        template<typename Archive>                                              \
          static void Serialize(Archive&, T&, unsigned int);                    \
      };                                                                        \
}}                                                                              \
template<typename Archive>                                                      \
  void elle::serialize::ArchiveSerializer<T>::Serialize(                        \
                                              Archive& archive,                 \
                                              T& value,                         \
                                              unsigned int version)             \

/// Define a simple serializer for the type T<T1>
# define ELLE_SERIALIZE_SIMPLE_T1(T, archive, value, version)                   \
namespace elle { namespace serialize {                                          \
    template<typename T1> struct ArchiveSerializer<T<T1>>                       \
      : public BaseArchiveSerializer<T<T1>>                                     \
      {                                                                         \
        template<typename Archive>                                              \
          static void Serialize(Archive&, T<T1>&, unsigned int);                \
      };                                                                        \
}}                                                                              \
template<typename T1>                                                           \
template<typename Archive>                                                      \
  void elle::serialize::ArchiveSerializer<T<T1>>::Serialize(                    \
                                                  Archive& archive,             \
                                                  T<T1>& value,                 \
                                                  unsigned int version)         \

/// Define a simple serializer for the type T<T1, T2>
# define ELLE_SERIALIZE_SIMPLE_T2(T, archive, value, version)                   \
namespace elle { namespace serialize {                                          \
    template<typename T1, typename T2> struct ArchiveSerializer<T<T1, T2>>      \
      : public BaseArchiveSerializer<T<T1, T2>>                                 \
      {                                                                         \
        template<typename Archive>                                              \
          static void Serialize(Archive&, T<T1, T2>&, unsigned int);            \
      };                                                                        \
}}                                                                              \
template<typename T1, typename T2>                                              \
template<typename Archive>                                                      \
  void elle::serialize::ArchiveSerializer<T<T1, T2>>::Serialize(                \
                                                  Archive& archive,             \
                                                  T<T1, T2>& value,             \
                                                  unsigned int version)         \


/// Declare a split serializer for the type T
# define ELLE_SERIALIZE_SPLIT(T)                                                \
namespace elle { namespace serialize {                                          \
                                                                                \
  template<> struct ArchiveSerializer<T>                                        \
    : public BaseArchiveSerializer<T>                                           \
    {                                                                           \
    private:                                                                    \
      typedef T Type;                                                           \
                                                                                \
    public:                                                                     \
      template<typename Archive>                                                \
        static void Serialize(Archive& ar, Type& val, unsigned int version)     \
        {                                                                       \
          typedef detail::_SplitSerializerSelectMethod<                         \
              Type                                                              \
            , Archive::mode                                                     \
          > Method;                                                             \
          Method::Serialize(ar, val, version);                                  \
        }                                                                       \
        template<typename Archive>                                              \
          static void Save(Archive& ar, Type const& val, unsigned int version); \
        template<typename Archive>                                              \
          static void Load(Archive& ar, Type& val, unsigned int version);       \
    };                                                                          \
}}                                                                              \


/// Define the Save() method of the split serializer for the type T
# define ELLE_SERIALIZE_SPLIT_SAVE(T, archive, value, version)                  \
template<typename Archive>                                                      \
  void elle::serialize::ArchiveSerializer<T>::Save(                             \
                                              Archive& archive,                 \
                                              Type const& value,                \
                                              unsigned int version)             \


/// Define the Load() method of the split serializer for the type T
# define ELLE_SERIALIZE_SPLIT_LOAD(T, archive, value, version)                  \
template<typename Archive>                                                      \
  void elle::serialize::ArchiveSerializer<T>::Load(                             \
                                              Archive& archive,                 \
                                              Type& value,                      \
                                              unsigned int version)             \


/// Declare a split serializer for the type T<T1>
# define ELLE_SERIALIZE_SPLIT_T1(T)                                             \
namespace elle { namespace serialize {                                          \
                                                                                \
  template<typename T1> struct ArchiveSerializer<T<T1>>                         \
    : public BaseArchiveSerializer<T<T1>>                                       \
    {                                                                           \
    private:                                                                    \
      typedef T<T1> Type;                                                       \
                                                                                \
    public:                                                                     \
      template<typename Archive>                                                \
        static void Serialize(Archive& ar, Type& val, unsigned int version)     \
        {                                                                       \
          typedef detail::_SplitSerializerSelectMethod<                         \
              Type                                                              \
            , Archive::mode                                                     \
          > Method;                                                             \
          Method::Serialize(ar, val, version);                                  \
        }                                                                       \
        template<typename Archive>                                              \
          static void Save(Archive& ar, Type const& val, unsigned int version); \
        template<typename Archive>                                              \
          static void Load(Archive& ar, Type& val, unsigned int version);       \
    };                                                                          \
}}                                                                              \


/// Define the Save() method of the split serializer for the type T
# define ELLE_SERIALIZE_SPLIT_T1_SAVE(T, archive, value, version)               \
template<typename T1>                                                           \
template<typename Archive>                                                      \
  void elle::serialize::ArchiveSerializer<T<T1>>::Save(                         \
                                              Archive& archive,                 \
                                              Type const& value,                \
                                              unsigned int version)             \


/// Define the Load() method of the split serializer for the type T
# define ELLE_SERIALIZE_SPLIT_T1_LOAD(T, archive, value, version)               \
template<typename T1>                                                           \
template<typename Archive>                                                      \
  void elle::serialize::ArchiveSerializer<T<T1>>::Load(                         \
                                              Archive& archive,                 \
                                              Type& value,                      \
                                              unsigned int version)             \

#endif
