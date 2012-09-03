#ifndef  ELLE_SERIALIZE_DETAIL_BASEARCHIVEOPERATORS_HH
# define ELLE_SERIALIZE_DETAIL_BASEARCHIVEOPERATORS_HH

# include <elle/serialize/Serializable.hh>
# include <elle/serialize/Pointer.hh>

# include <elle/idiom/Close.hh>
#  include <boost/call_traits.hpp>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace serialize
  {
    namespace detail
    {

      // Desambiguate param types.
      template <typename T>
      struct CallTraits
      {
        static bool const by_reference = (
            std::is_same<
              typename boost::call_traits<T>::param_type,
                typename std::add_lvalue_reference<
                   typename std::add_const<T>::type
                >::type
            >::value ||
            (std::is_array<T>::value)
        );

        static bool const by_value = (
            std::is_same<
              typename boost::call_traits<T>::param_type,
              typename std::add_const<T>::type
            >::value &&
            !std::is_pointer<T>::value
        );
      };

# define _ELLE_SERIALIZE_BASICARCHIVEOPERATORS_CHECK_TYPE(__type)             \
      static_assert(                                                          \
          !elle::serialize::IsSerializable<__type, Archive>::value,           \
          "You have to specicify if you want polymorphic or concrete "        \
          "serialization."                                                    \
      );                                                                      \
/**/

      template <typename Archive>
      struct CommonArchiveOperators
      {
      public:
        inline
        Archive&
        self()
        {
          return static_cast<Archive&>(*this);
        }
      };

      // Provide operators << and & for output archive.
      template <typename Archive>
      struct OutputArchiveOperators:
        public CommonArchiveOperators<Archive>
      {
        // << value
        template <typename T>
        inline
        typename std::enable_if<CallTraits<T>::by_value, Archive&>::type
        operator <<(T const value)
        {
          _ELLE_SERIALIZE_BASICARCHIVEOPERATORS_CHECK_TYPE(T);
          Archive::Access::Save(this->self(), value);
          return this->self();
        }

        // & value (forward to <<)
        template <typename T>
        inline
        typename std::enable_if<CallTraits<T>::by_value, Archive&>::type
        operator &(T const value)
        {
          return *this << value;
        }

        // << const ref not serializable
        template <typename T>
        inline
        typename std::enable_if<
          CallTraits<T>::by_reference && !IsSerializable<T, Archive>::value,
          Archive&
        >::type
        operator <<(T const& value)
        {
          Archive::Access::Save(this->self(), value);
          return this->self();
        }

        // << const ref serializable
        template <typename T>
        inline
        typename std::enable_if<
          CallTraits<T>::by_reference && IsSerializable<T, Archive>::value,
          Archive&
        >::type
        operator <<(T const& value)
        {
          Archive::Access::Save(this->self(), polymorphic(value));
          return this->self();
        }

        // & const ref (forward to <<)
        template <typename T>
        inline
        typename std::enable_if<CallTraits<T>::by_reference, Archive&>::type
        operator &(T const& value)
        {
          return *this << value;
        }
      };

      template <typename T>
      struct IsWrapper {static const bool value = false;};
      template <typename T> struct IsWrapper<Polymorphic<T>> {static const bool value = true;};
      template <typename T> struct IsWrapper<Concrete<T>> {static const bool value = true;};
      template <typename T> struct IsWrapper<NamedValue<T>> {static const bool value = true;};
      template <typename T> struct IsWrapper<Pointer<T>> {static const bool value = true;};
      template <typename T> struct IsWrapper<AlivePointer<T>> {static const bool value = true;};

      // Provides operators >> and & for input archive.
      template <typename Archive>
      struct InputArchiveOperators:
        public CommonArchiveOperators<Archive>
      {
        // >> ref not serializable
        template <typename T>
        inline
        typename std::enable_if<
          !IsSerializable<T, Archive>::value,
          Archive&
        >::type
        operator >>(T& value)
        {
          _ELLE_SERIALIZE_BASICARCHIVEOPERATORS_CHECK_TYPE(T);
          Archive::Access::Load(this->self(), value);
          return this->self();
        }

        // >> ref serializable
        template <typename T>
        inline
        typename std::enable_if<
          IsSerializable<T, Archive>::value,
          Archive&
        >::type
        operator >>(T& value)
        {
          Archive::Access::Load(this->self(), polymorphic(value));
          return this->self();
        }

        // & ref (forward to >>)
        template <typename T>
        inline
        Archive&
        operator &(T& value)
        {
          return *this >> value;
        }

        // >> const move (for wrappers)
        template <typename T>
        inline
        typename std::enable_if<IsWrapper<T>::value, Archive&>::type
        operator >>(T const&& value)
        {
          _ELLE_SERIALIZE_BASICARCHIVEOPERATORS_CHECK_TYPE(T);
          Archive::Access::Load(this->self(), value);
          return this->self();
        }

        // & const move (forward to >>)
        template <typename T>
        inline
        Archive&
        operator &(T const&& value)
        {
          return *this >> value;
        }
      };

      template <ArchiveMode mode, typename Archive>
      struct BaseArchiveOperators;

      template <typename Archive>
      struct BaseArchiveOperators<ArchiveMode::input, Archive>:
        public InputArchiveOperators<Archive>
      {};

      template <typename Archive>
      struct BaseArchiveOperators<ArchiveMode::output, Archive>:
        public OutputArchiveOperators<Archive>
      {};

    }
  }
}

#endif
