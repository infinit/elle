#ifndef  ELLE_RADIX_ARGUMENTS_SERIALIZER_HXX
# define ELLE_RADIX_ARGUMENTS_SERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/radix/Arguments.hh>

namespace elle
{
  namespace radix
  {
    namespace detail
    {

      //template<unsigned int idx, typename... T>
      //  struct OffsetOf;
      //template<typename... T>
      //  struct OffsetOf<0, T...>
      //  {
      //    static size_t const value = 0;
      //  };
      //template<unsigned int idx, typename Head, typename... Tail>
      //  struct OffsetOf
      //  {
      //    static size_t const value =
      //      sizeof(Head) + SizeSum<idx - 1, Tail...>::value;
      //  };

      // Extract the right element
      template<unsigned int index, typename T> struct ElementAt;

# define __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(n)                                 \
      template<typename T> struct ElementAt<n - 1, T>                         \
        {                                                                     \
          template<typename ElementType> static ElementType& Get(T& value)    \
          { return value.element ## n; }                                      \
        };                                                                    \

      // specializations for args 1 to 9
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(1);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(2);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(3);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(4);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(5);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(6);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(7);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(8);
      __ELLE_RADIX_ARGUMENTS_EXTRACT_EL(9);
#undef __ELLE_RADIX_ARGUMENTS_EXTRACT_EL

      template<unsigned int index, typename... T> struct TypeAt;

      template<unsigned int index, typename Head, typename... Tail>
        struct TypeAt<index, Head, Tail...>
        {
          typedef typename TypeAt<
              index - 1
            , Tail...
          >::type type;
        };

      template<typename Head, typename... Tail>
        struct TypeAt<0, Head, Tail...> { typedef Head type; };

      // n goes from sizeof(T...) to 1 (zero is the stop case)
      template<size_t const N, typename... T>
        struct ArgumentsSerializer
        {
          typedef elle::radix::Arguments< Parameters<T...> > Type;

          template<typename Archive>
            static void
            Serialize(Archive& ar, Type& value)
            {
              // its type
              typedef typename TypeAt<sizeof...(T) - N, T...>::type ElementType;

              ar & ElementAt<sizeof...(T) - N, Type>::template Get<ElementType>(value);

              // recurse
              ArgumentsSerializer<N - 1, T...>::Serialize(ar, value);
            }
        };

      // Stop serialization when n equals 0
      template<typename... T>
        struct ArgumentsSerializer<0, T...>
        {
          typedef elle::radix::Arguments< Parameters<T...> > Type;
          template<typename Archive>
            static void Serialize(Archive&, Type&) {}
        };
    }
  }
}

namespace elle
{
  namespace serialize
  {

    template<typename... T>
      struct ArchiveSerializer< elle::radix::Arguments< Parameters<T...> >>
      {
      public:
        typedef elle::radix::Arguments< Parameters<T...> > Type;

      public:
        template<typename Archive> static void
          Serialize(Archive& ar, Type& value, unsigned int version)
          {
            assert(version == 0);
            typedef elle::radix::detail::ArgumentsSerializer<sizeof...(T), T...> Serializer;
            Serializer::Serialize(ar, value);
          }
      };

  }
}

#endif
